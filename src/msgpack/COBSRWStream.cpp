#include "COBSRWStream.hpp"
#include <string.h>
#include <assert.h>

namespace msgpack {
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
	}
}