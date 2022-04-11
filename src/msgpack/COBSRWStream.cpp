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
		if(this->receive.bufferReadPosition < this->receive.bufferWritePosition) {
			const auto readPosition = this->receive.bufferReadPosition++;
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
	}

	//----------
	bool
	COBSRWStream::isEndOfIncomingPacket() const
	{
		return this->receive.endOfPacketReachedWithinBuffer
			&& (this->receive.bufferReadPosition == this->receive.bufferWritePosition);
	}

	//----------
	void
	COBSRWStream::decodeIncoming()
	{
		if(this->receive.skipToNextPacket) {
			// Did we see EOP?
			if(!this->receive.endOfPacketReachedWithinBuffer) {
				// Try to see the EOP
				auto data = this->stream.read();
				while(data >= 0) {
					if(data == 0x0) {
						break;
					}
				}
				if(data == -1) {
					// We reached EOF before seeing EOP
					// Nothing more we can do here
					return;
				}
			}

			// We've seen EOP and can now start again
			this->receive.bufferReadPosition = 0;
			this->receive.bufferWritePosition = 0;
			this->receive.endOfPacketReachedWithinBuffer = false;
			this->receive.skipToNextPacket = false;
			this->receive.bytesUntilNextZero = 0;
			this->receive.startOfStream = true;
		}

		// Check if we've seen an EOP
		if(this->receive.endOfPacketReachedWithinBuffer) {
			// We don't continue if we've seen an EOP until user skips to next packet
			return;
		}

		// Make as much space for writing as possible
		// This is important for cases where we are waiting for a specific number of bytes to become available
		this->realignIncoming();
		
		// Perform the decode
		{
			// Get next byte from serial
			auto incomingData = this->stream.read();

			// Whilst there's data and decoded buffer space available
			while(incomingData != -1
				&& this->receive.bufferWritePosition < MSGPACK_COBSRWSTREAM_BUFFER_SIZE) {

				if(incomingData == 0x0) {
					// EOP reached
					this->receive.endOfPacketReachedWithinBuffer = true;
				}
				else {
					// Decode into the buffer
					if(this->receive.bytesUntilNextZero == 0) {
						if(incomingData == 0xFF) {
							this->receive.bytesUntilNextZero = 0xFF;
						}
						else {
							// This is a zero
							this->receive.bytesUntilNextZero = (uint8_t) incomingData;

							if(!this->receive.startOfStream) {
								this->receive.decodedBuffer[this->receive.bufferWritePosition] = 0x0;
								this->receive.bufferWritePosition++;
							}
						}
						this->receive.startOfStream = false;
					}
					else {
						// This is a non-zero byte
						this->receive.decodedBuffer[this->receive.bufferWritePosition] = incomingData;
						this->receive.bufferWritePosition++;
					}
				}

				this->receive.bytesUntilNextZero--;
				incomingData = this->stream.read();
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
		if(data == 0x0) {
			this->writeBuffer();
		}
		else {
			this->transmit.plainTextBuffer[this->transmit.writePosition] = data;
			this->transmit.writePosition++;
		}
		

		if(this->transmit.writePosition == sizeof(this->transmit.plainTextBuffer)) {
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