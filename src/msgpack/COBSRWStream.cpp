#include "COBSRWStream.hpp"
#include <string.h>
#include <assert.h>

#ifdef ARDUINO
#include "deserialize.hpp"
#endif

namespace msgpack {
	namespace {
		//----------
		// CRC-16/CCITT-FALSE: poly 0x1021, init 0xFFFF, no reflection, xorout 0x0000.
		// Known vector: crc16Update(0xFFFF, "123456789", 9) == 0x29B1.
		uint16_t crc16Update(uint16_t crc, uint8_t byte) {
			crc ^= (uint16_t) byte << 8;
			for(int i = 0; i < 8; i++) {
				crc = (crc & 0x8000) ? (uint16_t) ((crc << 1) ^ 0x1021) : (uint16_t) (crc << 1);
			}
			return crc;
		}
	}

	//----------
	COBSRWStream::COBSRWStream(Stream& stream)
	: stream(stream)
	{
		lwrb_init(&this->receive.decodedRingBuffer
			, this->receive.decodedRingBufferData
			, MSGPACK_COBSRWSTREAM_BUFFER_SIZE);
	}

	//----------
	COBSRWStream::~COBSRWStream()
	{
		lwrb_free(&this->receive.decodedRingBuffer);
	}

	//----------
	int
	COBSRWStream::available()
	{
		this->decodeIncoming();
		
		return lwrb_get_full(&this->receive.decodedRingBuffer);
	}

	//----------
	int
	COBSRWStream::read()
	{
		if(this->available() > 0) {
			this->receive.outgoingStreamIsAtStartOfNextPacket = false;
			uint8_t data;
			lwrb_read(&this->receive.decodedRingBuffer, &data, 1);
			this->receive.runningCRC = crc16Update(this->receive.runningCRC, data);
			return (int) data;
		}
		else {
			return -1; // EOF
		}
	}

	//----------
	int
	COBSRWStream::peek()
	{
		if(this->available() > 0) {
			uint8_t data;
			lwrb_peek(&this->receive.decodedRingBuffer, 0, &data, 1);
			return (int) data;
		}
		else {
			return -1; // EOF
		}
	}

	//----------
	void
	COBSRWStream::nextIncomingPacket()
	{
		this->receive.skipToNextPacket = true;
		this->receive.outgoingStreamIsAtStartOfNextPacket = false;
		this->decodeIncoming();
	}

	//----------
	bool
	COBSRWStream::isStartOfIncomingPacket() const
	{
		return this->receive.outgoingStreamIsAtStartOfNextPacket;
	}

	//----------
	bool
	COBSRWStream::isEndOfIncomingPacket() const
	{
		return this->receive.incomingStreamIsAtStartOfNextPacket
			&& lwrb_get_full(&this->receive.decodedRingBuffer) == 0;
	}

	//----------
	void
	COBSRWStream::decodeIncoming()
	{
		if(this->receive.skipToNextPacket) {
			// Did we see EOP?
			if(!this->receive.incomingStreamIsAtStartOfNextPacket) {
				// Try to see the EOP
				while(stream.available() > 0) {
					auto data = this->stream.read();
					if(data == 0x0) {
						this->receive.incomingStreamIsAtStartOfNextPacket = true;
						break;
					}
				}
			}

			// We've seen EOP and can now start again
			if(this->receive.incomingStreamIsAtStartOfNextPacket) {
				this->receive.incomingStreamIsAtStartOfNextPacket = false;
				this->receive.outgoingStreamIsAtStartOfNextPacket = true;
				this->receive.skipToNextPacket = false;
				this->receive.bytesUntilNextZero = 0;
				this->receive.chunkLength = 0xFF;
				this->receive.runningCRC = 0xFFFF;

				// clear any bytes in output buffer
				{
					auto bytesInBuffer = lwrb_get_full(&this->receive.decodedRingBuffer);
					if(bytesInBuffer > 0) {
						lwrb_skip(&this->receive.decodedRingBuffer, bytesInBuffer);
					}
				}
			}

			// We haven't seen EOP and user wants next packet, so will try again next time
			else {
				return;
			}
		}

		// Check if we've seen an EOP
		if(this->receive.incomingStreamIsAtStartOfNextPacket
			&& !this->receive.outgoingStreamIsAtStartOfNextPacket) {
			// We don't continue if we've seen an EOP until user skips to next packet
			return;
		}

		// Perform the decode
		{
			// Whilst there's data and decoded buffer space available
			while(this->stream.available() > 0
				&& lwrb_get_free(&this->receive.decodedRingBuffer) > 0) {

				// Get next byte from serial
				const auto incomingData = this->stream.read();

				if(incomingData == 0x0) {
					// EOP reached
					this->receive.incomingStreamIsAtStartOfNextPacket = true;

					// We don't want to decode any more
					break;
				}
				else {
					// This is a pointer (zero, start of stream, or start of chunk)
					if (this->receive.bytesUntilNextZero == 0) {
						// Also it's a zero byte
						if(this->receive.chunkLength != 0xFF) {
							uint8_t data = 0x0;
							lwrb_write(&this->receive.decodedRingBuffer, &data, 1);
						}

						this->receive.bytesUntilNextZero = (uint8_t) incomingData;
						this->receive.chunkLength = (uint8_t) incomingData;
					}
					// This is a non-zero byte
					else {
						lwrb_write(&this->receive.decodedRingBuffer, &incomingData, 1);
					}

					this->receive.bytesUntilNextZero--;
				}
			}
		}
	}

	//----------
	size_t
	COBSRWStream::write(uint8_t data)
	{
		// Fold every logical byte -- including zeros -- before the COBS zero-elimination
		// branch below, so the CRC covers exactly what the caller wrote, not the on-wire
		// (zero-eliminated) encoding.
		this->transmit.runningCRC = crc16Update(this->transmit.runningCRC, data);

		if(data != 0x0) {
			this->transmit.plainTextBuffer[this->transmit.writePosition] = data;
			this->transmit.writePosition++;

			// check if buffer is full
			if(this->transmit.writePosition == sizeof(this->transmit.plainTextBuffer)) {
				// Writing the buffer with length+1 of 0xFF doesn't count as a zero
				this->writeBuffer();
			}
		}
		else {
			this->writeBuffer();
		}

		return 1;
	}

	//----------
	size_t
	COBSRWStream::write(const uint8_t *buffer, size_t size)
	{
		for(size_t i=0; i<size; i++) {
			this->write(buffer[i]);
		}
		return size;
	}

	//----------
	void
	COBSRWStream::writeBuffer()
	{
		// Write distance to next buffer start
		this->stream.write((uint8_t) this->transmit.writePosition + 1);

		// Write all the non-zero bytes
		this->stream.write(this->transmit.plainTextBuffer, this->transmit.writePosition);

		this->transmit.writePosition = 0;
	}

	//----------
	int
	COBSRWStream::availableForWrite()
	{
		return sizeof(this->transmit.plainTextBuffer) - this->transmit.writePosition;
	}

	//----------
	void
	COBSRWStream::flush()
	{
		this->writeEOP();
		this->stream.flush();
	}

	//----------
	void
	COBSRWStream::writeEOP()
	{
		this->writeBuffer();
		this->stream.write((uint8_t) 0x0);
		this->transmit.runningCRC = 0xFFFF;
	}

#ifdef ARDUINO
	//----------
	bool
	COBSRWStream::checkChecksum(uint8_t & seq)
	{
		// The trailer is 5 decoded bytes: seq as a forced uint8 (0xCC + 1 byte) and crc16 as a
		// forced uint16 (0xCD + 2 bytes) -- see RS485.cpp's finishFrame(), which writes them
		// with writeIntU8/writeIntU16 specifically so the width here is never ambiguous.
		// Bounded wait: at 115200 baud this has almost always already arrived by the time a
		// handler finishes reading the body, but don't hang forever if the bus has gone quiet
		// mid-frame.
		const uint32_t deadline = millis() + 5;
		while(this->available() < 5) {
			if(millis() > deadline) {
				return false;
			}
		}

		if(!msgpack::readInt<uint8_t>(*this, seq, true)) {
			return false;
		}
		// Snapshot here: everything up to and including seq is covered by the CRC the sender
		// computed; the crc16 field read next must not fold into its own check.
		const uint16_t crcSoFar = this->receive.runningCRC;

		uint16_t transmittedCRC;
		if(!msgpack::readInt<uint16_t>(*this, transmittedCRC, true)) {
			return false;
		}

		return crcSoFar == transmittedCRC;
	}
#endif
}