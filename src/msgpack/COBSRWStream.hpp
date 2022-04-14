#pragma once

#define MSGPACK_COBSRWSTREAM_BUFFER_SIZE 64
#include "Platform.h"
#include <stdint.h>

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
		void nextIncomingPacket();
		bool isStartOfIncomingPacket() const;
		bool isEndOfIncomingPacket() const;
		//
		//--

		//--
		// Write functions
		//--
		//
		size_t write(uint8_t) override;
		size_t write(const uint8_t *buffer, size_t size);
		int availableForWrite(void);
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
			uint8_t bufferWritePosition = 0;
			uint8_t bufferReadPosition = 0;

			// When this is true:
			//	- User can still be reading previous packet from our buffer
			//	- We will not decode any more from incoming buffer
			//	- The end of packet occurs when write position = read position
			bool incomingStreamIsAtStartOfNextPacket = false;

			// This denotes whether the user begin decoding the data
			// as if it is the start of the packet
			bool outgoingStreamIsAtStartOfNextPacket = true;

			// COBS encoding
			uint8_t bytesUntilNextZero = 0;
			uint8_t chunkLength = 0xFF;

			bool skipToNextPacket = false;
		} receive;

		void decodeIncoming();
		void realignIncoming();

		struct {
			uint8_t plainTextBuffer[254];
			uint8_t writePosition = 0;
		} transmit;

		void writeBuffer();
	};
}