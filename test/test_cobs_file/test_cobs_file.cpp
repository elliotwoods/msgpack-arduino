#include <unity.h>
#include "../LoopbackStream.h"
#include <msgpack.hpp>

#include "raw.h"
#include "encoded.h"

LoopbackStream loopbackStream;
msgpack::COBSRWStream cobsStream(loopbackStream);

void testEncode()
{
	// write to stream
	cobsStream.write(raw.data(), raw.size());
	cobsStream.flush();

	// check matches encoding
	{
		const auto & encodedHere = loopbackStream.getData();
		TEST_ASSERT_EQUAL(encoded.size() + 1, encodedHere.size()); // note that we also have the 0 at the end

		for(size_t i=0; i<encoded.size(); i++) {
			TEST_ASSERT_EQUAL(encoded[i], encodedHere[i]);
		}
	}
}

void testDecode()
{
	// stream in encoded
	loopbackStream.write(encoded.data(), encoded.size());

	// check out decoded matches
	{
		size_t i = 0;
		while(cobsStream.available()) {
			auto decodedHere = cobsStream.read();
			auto originalData = raw[i++];
			TEST_ASSERT_EQUAL(originalData, decodedHere);
		}
	}
}

int main() {
	UNITY_BEGIN();
	{
		RUN_TEST(testEncode);
		cobsStream.nextIncomingPacket();

		RUN_TEST(testDecode);
		cobsStream.nextIncomingPacket();
	}
	UNITY_END();
}