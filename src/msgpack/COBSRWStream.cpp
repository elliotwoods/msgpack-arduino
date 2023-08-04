#include "COBSRWStream.hpp"

namespace msgpack {
	//----------
	COBSRWStream::COBSRWStream(Stream& stream)
	: stream(stream)
	{
		this->receive.decodedBuffer = new uint8_t[MSGPACK_COBSRWSTREAM_BUFFER_SIZE];
		this->receive.decodedBufferBack = new uint8_t[MSGPACK_COBSRWSTREAM_BUFFER_SIZE];
	}

	//----------
	COBSRWStream::~COBSRWStream()
	{
		delete[] this->receive.decodedBuffer;
		delete[] this->receive.decodedBufferBack;
	}

	//----------
	int
	COBSRWStream::available()
	{
		this->decodeIncoming();
		
		return this->receive.bufferWritePosition - this->receive.bufferReadPosition;
	}

	//----------
	int
	COBSRWStream::read()
	{
		if(this->receive.bufferReadPosition == this->receive.bufferWritePosition) {
			this->decodeIncoming();
		}

		if(this->receive.bufferReadPosition < this->receive.bufferWritePosition) {
			const auto readPosition = this->receive.bufferReadPosition++;
			this->receive.outgoingStreamIsAtStartOfNextPacket = false;
			return this->receive.decodedBuffer[readPosition];
		}
		else {
			return -1; // EOF
		}
	}

	//----------
	int
	COBSRWStream::peek()
	{
		if(this->receive.bufferReadPosition == this->receive.bufferWritePosition) {
			this->decodeIncoming();
		}

		if(this->receive.bufferReadPosition < this->receive.bufferWritePosition) {
			return this->receive.decodedBuffer[this->receive.bufferReadPosition];
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
			&& (this->receive.bufferReadPosition == this->receive.bufferWritePosition);
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
				this->receive.bufferReadPosition = 0;
				this->receive.bufferWritePosition = 0;
				this->receive.incomingStreamIsAtStartOfNextPacket = false;
				this->receive.outgoingStreamIsAtStartOfNextPacket = true;
				this->receive.skipToNextPacket = false;
				this->receive.bytesUntilNextZero = 0;
				this->receive.chunkLength = 0xFF;
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

		// Make as much space for writing as possible
		// This is important for cases where we are waiting for a specific number of bytes to become available
		this->realignIncoming();
		
		// Perform the decode
		{
			// Whilst there's data and decoded buffer space available
			while(this->stream.available() > 0
				&& this->receive.bufferWritePosition < MSGPACK_COBSRWSTREAM_BUFFER_SIZE) {

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
							this->receive.decodedBuffer[this->receive.bufferWritePosition] = 0x0;
							this->receive.bufferWritePosition++;
						}

						this->receive.bytesUntilNextZero = (uint8_t) incomingData;
						this->receive.chunkLength = (uint8_t) incomingData;
					}
					// This is a non-zero byte
					else {
						this->receive.decodedBuffer[this->receive.bufferWritePosition] = incomingData;
						this->receive.bufferWritePosition++;
					}

					this->receive.bytesUntilNextZero--;
				}
			}
		}
	}

	//----------
	void
	COBSRWStream::realignIncoming()
	{
		if(this->receive.bufferReadPosition > 0) {
			const auto decodedSize = this->receive.bufferWritePosition - this->receive.bufferReadPosition;

			// Move the data to the start of buffer
			if (decodedSize > 0) {
				// Copy into the other buffer
				memcpy(this->receive.decodedBufferBack
					, this->receive.decodedBuffer + this->receive.bufferReadPosition
					, decodedSize);

				// Swap buffers
				{
					auto temp = this->receive.decodedBuffer;
					this->receive.decodedBuffer = this->receive.decodedBufferBack;
					this->receive.decodedBufferBack = temp;
				}
			}

			// Change the positions
			{
				this->receive.bufferWritePosition -= this->receive.bufferReadPosition;
				this->receive.bufferReadPosition = 0;
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