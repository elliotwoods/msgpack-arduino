#include <unity.h>
#include "../LoopbackStream.h"
#include <msgpack.hpp>
#include <vector>

LoopbackStream loopbackStream;
msgpack::COBSRWStream cobsStream(loopbackStream);

void NRandomTest(size_t count)
{
	// create random data
	std::vector<uint8_t> plainText;
	{
		for(size_t i=0; i<count; i++) {
			plainText.push_back(rand() % 255);
		}
	}

	// write to encoded stream
	{
		for(const auto & byte : plainText) {
			cobsStream.write(byte);
		}
		cobsStream.flush();
	}

	// check decoded stream
	{
		for(uint32_t i=0; i<count; i++) {
			auto readback = cobsStream.read();
			TEST_ASSERT_EQUAL(plainText[i], readback);
		}

		// EOP
		TEST_ASSERT_EQUAL(0, cobsStream.available());
	}
}

void test2()
{
	NRandomTest(2);
}

void test4()
{
	NRandomTest(4);
}

void test10()
{
	NRandomTest(10);
}

void test100()
{
	NRandomTest(100);
}

void test300()
{
	NRandomTest(300);
}

void test1000()
{
	NRandomTest(1000);
}

void test10000()
{
	NRandomTest(10000);
}

int main() {
	UNITY_BEGIN();
	{
		RUN_TEST(test2);

		cobsStream.nextIncomingPacket();

		RUN_TEST(test4);

		cobsStream.nextIncomingPacket();

		RUN_TEST(test10);

		cobsStream.nextIncomingPacket();

		RUN_TEST(test100);

		cobsStream.nextIncomingPacket();

		RUN_TEST(test300);

		cobsStream.nextIncomingPacket();

		RUN_TEST(test1000);

		cobsStream.nextIncomingPacket();
		
		RUN_TEST(test10000);

		cobsStream.nextIncomingPacket();
	}
	UNITY_END();
}