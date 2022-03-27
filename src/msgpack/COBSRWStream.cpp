#include "COBSRWStream.hpp"

namespace msgpack {
	//----------
	COBSRWStream::COBSRWStream(Stream& stream)
	: stream(stream)
	{
		this->incoming.decodedBuffer = new uint8_t[MSGPACK_COBSRWSTREAM_BUFFER_SIZE];
		this->incoming.decodedBufferBack = new uint8_t[MSGPACK_COBSRWSTREAM_BUFFER_SIZE];
	}

	//----------
	COBSRWStream::~COBSRWStream()
	{
		delete[] this->incoming.decodedBuffer;
		delete[] this->incoming.decodedBufferBack;
	}

	//----------
	int
	COBSRWStream::available()
	{
		this->decodeIncoming();
		return this->incoming.bufferWritePosition - this->incoming.bufferReadPosition;
	}

	//----------
	int
	COBSRWStream::read()
	{
		if(this->incoming.bufferReadPosition < this->incoming.bufferWritePosition) {
			const auto readPosition = this->incoming.bufferReadPosition++;
			return this->incoming.decodedBuffer[readPosition];
		}
		else {
			return -1; // EOF
		}
	}

	//----------
	int
	COBSRWStream::peek()
	{
		if(this->incoming.bufferReadPosition < this->incoming.bufferWritePosition) {
			return this->incoming.decodedBuffer[this->incoming.bufferReadPosition];
		}
		else {
			return -1; // EOF
		}
	}

	//----------
	void
	COBSRWStream::nextPacket()
	{
		this->incoming.skipToNextPacket = true;
	}

	//----------
	bool
	COBSRWStream::isEndOfPacket() const
	{
		return this->incoming.endOfPacketReachedWithinBuffer
			&& (this->incoming.bufferReadPosition == this->incoming.bufferWritePosition);
	}

	//----------
	void
	COBSRWStream::decodeIncoming()
	{
		if(this->incoming.skipToNextPacket) {
			// Did we see EOP?
			if(!this->incoming.endOfPacketReachedWithinBuffer) {
				// Try to see the EOP
				auto data = this->stream.read();
				while(data != -1) {
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
				this->incoming.bufferReadPosition = 0;
				this->incoming.bufferWritePosition = 0;
				this->incoming.endOfPacketReachedWithinBuffer = false;
				this->incoming.skipToNextPacket = false;
				this->incoming.bytesUntilNextZero = 0;
				this->incoming.startOfStream = true;
		}

		// Check if we've seen an EOP
		if(this->incoming.endOfPacketReachedWithinBuffer) {
			// We don't continue if we've seen an EOP until user skips to next packet
			return;
		}

		// Make as much space for writing as possible
		// This is important for cases where we are waiting for a specific number of bytes to become available
		//this->realignIncoming();
		
		// Perform the decode
		{
			// Get next byte from serial
			auto incomingData = this->stream.read();

			// Whilst there's data and decoded buffer space available
			while(incomingData != -1
				&& this->incoming.bufferWritePosition < MSGPACK_COBSRWSTREAM_BUFFER_SIZE) {
					
				if(incomingData == 0x0) {
					// EOP reached
					this->incoming.endOfPacketReachedWithinBuffer = true;
				}
				else {
					// Decode into the buffer
					if(this->incoming.bytesUntilNextZero == 0) {
						if(incomingData == 0xFF) {
							this->incoming.bytesUntilNextZero = 0xFF;
						}
						else {
							// This is a zero
							this->incoming.bytesUntilNextZero = (uint8_t) incomingData;

							if(!this->incoming.startOfStream) {
								this->incoming.decodedBuffer[this->incoming.bufferWritePosition] = 0x0;
								this->incoming.bufferWritePosition++;
							}
						}
						this->incoming.startOfStream = false;
					}
					else {
						// This is a non-zero byte
						this->incoming.decodedBuffer[this->incoming.bufferWritePosition] = incomingData;
						this->incoming.bufferWritePosition++;
					}
				}

				this->incoming.bytesUntilNextZero--;
				incomingData = this->stream.read();
			}
		}
	}

	//----------
	void
	COBSRWStream::realignIncoming()
	{
		if(this->incoming.bufferReadPosition > 0) {
			const auto decodedSize = this->incoming.bufferWritePosition > this->incoming.bufferReadPosition;

			// Move the data to the start of buffer
			if (decodedSize > 0) {
				// Copy into the other buffer
				memcpy(this->incoming.decodedBufferBack
					, this->incoming.decodedBuffer + this->incoming.bufferReadPosition
					, decodedSize);

				// Swap buffers
				{
					auto temp = this->incoming.decodedBuffer;
					this->incoming.decodedBuffer = this->incoming.decodedBufferBack;
					this->incoming.decodedBufferBack = temp;
				}
			}

			// Change the positions
			{
				this->incoming.bufferWritePosition -= this->incoming.bufferReadPosition;
				this->incoming.bufferReadPosition = 0;
			}
		}
	}

	//----------
	size_t
	COBSRWStream::write(uint8_t)
	{

		return 0;
	}

	//----------
	size_t
	COBSRWStream::write(const uint8_t *buffer, size_t size)
	{
		
		return 0;
	}

	//----------
	int
	COBSRWStream::availableForWrite()
	{
		
		return 0;
	}

	//----------
	void
	COBSRWStream::flush()
	{

	}

	//----------
	void
	COBSRWStream::writeEOP()
	{

	}
}