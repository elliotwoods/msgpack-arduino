#pragma once

#define MSGPACK_COBSRWSTREAM_BUFFER_SIZE 64
#include <Arduino.h>
namespace msgpack {
	class COBSRWStream : public Stream {
	public:
		COBSRWStream(Stream& stream);
		~COBSRWStream();

		//--
		// Read functions
		//

		/// Returns the amount of decoded bytes available. This should always be called before read
		/// It will return 0 at either end of stream or end of packet. Use isEndOfPacket to check which.
		int available() override;
		int read() override;
		int peek() override;

		// Custom read functions
		void nextPacket();
		bool isEndOfPacket() const;

		//
		//--

		//--
		// Write functions
		//--
		//
		size_t write(uint8_t) override;
		size_t write(const uint8_t *buffer, size_t size);
		int availableForWrite(void) override;
		void flush() override; // This also writes an EOP

		// Custom write functions
		void writeEOP();
		//
		//--

	protected:
		Stream& stream;

		struct {
			uint8_t * decodedBuffer;
			uint8_t * decodedBufferBack;
			size_t bufferWritePosition = 0;
			size_t bufferReadPosition = 0;

			// When this is true:
			//	- We will not decode any more
			//	- The end of packet occurs when write position = read position
			bool endOfPacketReachedWithinBuffer = false;

			// COBS encoding
			uint8_t bytesUntilNextZero = 0;
			bool startOfStream = true;

			bool skipToNextPacket = false;
		} incoming;

		void decodeIncoming();
		void realignIncoming();

		void resetIncoming();
	};
}