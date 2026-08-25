// Two packets arriving back to back must decode as two packets.
//
// They did not. `decodeIncoming()` only stopped at a delimiter once the reader had consumed a byte
// of the current packet, so calling `available()` twice before the first `read()` decoded straight
// through and appended the following packet to the current one. `nextIncomingPacket()` then threw
// the merged tail away, and the second packet was gone with no error raised anywhere.
//
// That is reachable from ordinary use rather than from misuse. Every `waitForData()` inside every
// read helper calls `available()`, so the usual shape
//
//     while(stream.isStartOfIncomingPacket() && stream.available()) { parse(); ...; next(); }
//
// merges on its second call -- the loop asks "is there work?", then the parser asks again before
// reading anything. On a bus where frames arrive back to back that silently drops most of them:
// three queued commands decoded as one.
//
// These tests fail against the old behaviour and pass against the new.
//
// They build on the **non-Arduino** path, so they need no ArduinoFake -- which is also why they
// run: the `test` environment's ArduinoFake dependency no longer compiles under current clang, and
// a regression test that cannot be run is not a regression test.

#include <unity.h>
#include <msgpack.hpp>

#include <deque>
#include <stdint.h>

namespace {
	/// A plain byte queue, exactly as unhelpful as a UART ring: no notion of frames, and bytes from
	/// several packets sitting in it at once.
	class ByteQueue : public msgpack::Stream {
	public:
		int available() override { return (int) this->data.size(); }

		int read() override
		{
			if(this->data.empty()) {
				return -1;
			}
			const uint8_t value = this->data.front();
			this->data.pop_front();
			return (int) value;
		}

		int peek() override
		{
			return this->data.empty() ? -1 : (int) this->data.front();
		}

		size_t write(uint8_t value) override
		{
			this->data.push_back(value);
			return 1;
		}

		size_t write(const uint8_t * buffer, size_t size) override
		{
			for(size_t index = 0; index < size; index++) {
				this->data.push_back(buffer[index]);
			}
			return size;
		}

		void flush() override {}

		std::deque<uint8_t> data;
	};

	/// Encode `count` packets into one contiguous stream, the way a host sending without pausing
	/// leaves them in a receive buffer. Packet *n* is `length` copies of `payload + n`, so a merge
	/// is visible as the wrong byte value rather than merely the wrong count.
	void fillWithPackets(ByteQueue & queue, uint8_t count, uint8_t payload, size_t length)
	{
		msgpack::COBSRWStream encoder(queue);
		for(uint8_t packet = 0; packet < count; packet++) {
			for(size_t index = 0; index < length; index++) {
				encoder.write((uint8_t) (payload + packet));
			}
			encoder.flush();
		}
	}
}

/// The exact sequence that used to merge: poll, then parse.
void twoAvailableCallsBeforeAReadMustNotMergePackets()
{
	ByteQueue queue;
	fillWithPackets(queue, 3, 0x10, 4);

	msgpack::COBSRWStream cobs(queue);

	// A caller checking for work, then a parser checking again before its first read. Neither may
	// pull the following packet into this one.
	const int first = cobs.available();
	const int second = cobs.available();
	const int third = cobs.available();
	TEST_ASSERT_EQUAL(4, first);
	TEST_ASSERT_EQUAL_MESSAGE(first, second, "a second available() merged the next packet");
	TEST_ASSERT_EQUAL_MESSAGE(first, third, "a third available() merged another packet");
}

/// All three packets must be readable, in order, with their own contents.
void everyPacketInABurstIsDelivered()
{
	ByteQueue queue;
	fillWithPackets(queue, 3, 0x10, 4);

	msgpack::COBSRWStream cobs(queue);

	for(uint8_t packet = 0; packet < 3; packet++) {
		TEST_ASSERT_TRUE_MESSAGE(cobs.isStartOfIncomingPacket(), "not at a packet boundary");
		TEST_ASSERT_EQUAL_MESSAGE(4, cobs.available(), "packet has the wrong length");

		for(size_t index = 0; index < 4; index++) {
			TEST_ASSERT_EQUAL_MESSAGE(0x10 + packet, cobs.read(), "wrong packet contents");
		}

		// End of this packet, and not the start of the next one's bytes.
		TEST_ASSERT_EQUAL_MESSAGE(0, cobs.available(), "packet did not end at its delimiter");
		cobs.nextIncomingPacket();
	}
}

/// The interleaving a real parser produces: available(), read a byte, available() again.
void availableCallsInterleavedWithReadsStayInsideOnePacket()
{
	ByteQueue queue;
	fillWithPackets(queue, 2, 0x40, 6);

	msgpack::COBSRWStream cobs(queue);

	size_t read = 0;
	while(cobs.available() > 0) {
		TEST_ASSERT_EQUAL_MESSAGE(0x40, cobs.read(), "the second packet leaked into the first");
		read++;
		TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(6, read, "read past the first packet's delimiter");
	}
	TEST_ASSERT_EQUAL(6, read);
}

/// A packet still arriving must still arrive intact, and must not be joined to the next one.
void aPacketSplitAcrossArrivalsIsStillOnePacket()
{
	ByteQueue source;
	fillWithPackets(source, 1, 0x77, 5);
	const std::deque<uint8_t> encoded = source.data;

	ByteQueue queue;
	for(size_t index = 0; index + 1 < encoded.size(); index++) {
		queue.write(encoded[index]);
	}

	msgpack::COBSRWStream cobs(queue);
	// Decoded bytes may legitimately appear before the delimiter -- this is a streaming decoder,
	// and that property is what lets a parser start work early. What matters is that they are the
	// right bytes and that nothing follows them.
	TEST_ASSERT_LESS_OR_EQUAL(5, cobs.available());

	queue.write(encoded[encoded.size() - 1]);
	TEST_ASSERT_EQUAL(5, cobs.available());
	for(size_t index = 0; index < 5; index++) {
		TEST_ASSERT_EQUAL(0x77, cobs.read());
	}
	TEST_ASSERT_EQUAL(0, cobs.available());
}

/// A whole msgpack message per packet, parsed the way a real receive loop parses one.
///
/// The closest thing here to what actually broke: `readArraySize` calls `waitForData`, which calls
/// `available()`, after the loop condition has already called it once.
void aBurstOfMessagesIsParsedAsSeparateMessages()
{
	ByteQueue queue;
	{
		msgpack::COBSRWStream encoder(queue);
		for(int32_t value = 1; value <= 3; value++) {
			msgpack::writeArraySize4(encoder, 3);
			msgpack::writeInt8(encoder, 1);
			msgpack::writeInt8(encoder, 0);
			msgpack::writeInt32(encoder, value * 1111);
			encoder.flush();
		}
	}

	msgpack::COBSRWStream cobs(queue);
	int32_t seen = 0;
	while(cobs.isStartOfIncomingPacket() && cobs.available()) {
		size_t arraySize = 0;
		TEST_ASSERT_TRUE(msgpack::readArraySize(cobs, arraySize));
		TEST_ASSERT_EQUAL(3, arraySize);

		int32_t target = 0;
		int32_t source = 0;
		int32_t value = 0;
		TEST_ASSERT_TRUE(msgpack::readInt<int32_t>(cobs, target));
		TEST_ASSERT_TRUE(msgpack::readInt<int32_t>(cobs, source));
		TEST_ASSERT_TRUE(msgpack::readInt<int32_t>(cobs, value));

		seen++;
		TEST_ASSERT_EQUAL_MESSAGE(seen * 1111, value, "messages were merged");
		cobs.nextIncomingPacket();
	}
	TEST_ASSERT_EQUAL_MESSAGE(3, seen, "a burst of three messages was not delivered as three");
}

int main() {
	UNITY_BEGIN();
	RUN_TEST(twoAvailableCallsBeforeAReadMustNotMergePackets);
	RUN_TEST(everyPacketInABurstIsDelivered);
	RUN_TEST(availableCallsInterleavedWithReadsStayInsideOnePacket);
	RUN_TEST(aPacketSplitAcrossArrivalsIsStillOnePacket);
	RUN_TEST(aBurstOfMessagesIsParsedAsSeparateMessages);
	return UNITY_END();
}
