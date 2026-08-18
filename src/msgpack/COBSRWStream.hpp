#pragma once

// This can be smaller, e.g. 64 bytes if needed
#define MSGPACK_COBSRWSTREAM_BUFFER_SIZE 256

#include "Platform.hpp"
#include <stdint.h>
#include "lwrb.h"

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

		// A running CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF, no reflection, xorout
		// 0x0000) folded over every logical byte written this frame (including zeros, before
		// COBS elimination), reset at the end of writeEOP(). Exposed so a sender can append it
		// as a trailer field -- see RS485.cpp's finishFrame().
		uint16_t getTxRunningCRC() const { return this->transmit.runningCRC; }

		// The RX-side running CRC, folded over every byte returned by read() so far this
		// packet (see read()). Always available (not Arduino-gated) so it's testable on the
		// host; checkChecksum() below is the Arduino-only convenience that also reads and
		// compares the trailer fields.
		uint16_t getRxRunningCRC() const { return this->receive.runningCRC; }
		//
		//--

#ifdef ARDUINO
		// Verifies a trailing `[..., seq_u8, crc16_u16]` pair against the running CRC
		// accumulated over every byte read() so far this packet (see the `receive.runningCRC`
		// fold in read()) -- i.e. everything before the trailer, which is exactly what the
		// sender's getTxRunningCRC() snapshot covers at the same point. Call this as the LAST
		// thing a handler does before any side effect, not at frame start: unlike gating the
		// whole packet before parsing, this keeps the existing streaming read API completely
		// unchanged for callers that don't care about the trailer.
		//
		// Blocks up to a few milliseconds if the trailer hasn't fully arrived yet (RS485 at
		// 115200 baud is ~87 us/byte, so for these small frames it normally already has); on
		// timeout, on a malformed trailer, or on a genuine mismatch, returns false and `seq` is
		// left unchanged -- treat exactly like a CRC failure, i.e. do not commit.
		bool checkChecksum(uint8_t & seq);
#endif

	protected:
		Stream& stream;

		struct {
			lwrb decodedRingBuffer;
			uint8_t decodedRingBufferData[MSGPACK_COBSRWSTREAM_BUFFER_SIZE];

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

			// Running CRC over every byte returned by read() this packet (see read()), reset
			// whenever a new packet starts being decoded. NOT advanced by peek() -- only bytes
			// actually consumed by the parser are covered, matching what a handler has read by
			// the time it calls checkChecksum().
			uint16_t runningCRC = 0xFFFF;
		} receive;

		void decodeIncoming();

		struct {
			uint8_t plainTextBuffer[254];
			uint8_t writePosition = 0;
			uint16_t runningCRC = 0xFFFF;
		} transmit;

		void writeBuffer();
	};
}