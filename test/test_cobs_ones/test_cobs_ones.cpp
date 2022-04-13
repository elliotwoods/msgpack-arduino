#include <unity.h>
#include "../LoopbackStream.h"
#include <msgpack.hpp>

LoopbackStream loopbackStream;
msgpack::COBSRWStream cobsStream(loopbackStream);

void singleOneTest()
{
	// write to encoded stream
	{
		cobsStream.write(0);
		cobsStream.flush();
	}

	// check encoded stream contents
	{
		const auto & data = loopbackStream.getData();
		TEST_ASSERT_EQUAL(3, data.size());
		TEST_ASSERT_EQUAL(1, data[0]);
		TEST_ASSERT_EQUAL(1, data[1]);
		TEST_ASSERT_EQUAL(0, data[2]);
	}

	// check decoded stream
	{
		auto available = cobsStream.available();
		TEST_ASSERT_EQUAL(1, available);
		
		auto readback = cobsStream.read();
		TEST_ASSERT_EQUAL(0, readback);
	}
}

void NOneTest(size_t count)
{
	// write to encoded stream
	{
		for(int i=0; i<count; i++) {
			cobsStream.write(1);
		}
		cobsStream.flush();
	}

	// check encodedstream
	{
		const auto & data = loopbackStream.getData();

		// test length
		const auto expectedLength = count/254 + 1 + count + 1;
		TEST_ASSERT_EQUAL(expectedLength, data.size());
	}

	// check decoded stream
	{
		auto available = cobsStream.available();
		//TEST_ASSERT_EQUAL(count, available); <-- this will actually return the buffer length
		
		for(uint32_t i=0; i<count; i++) {
			auto readback = cobsStream.read();
			TEST_ASSERT_EQUAL(1, readback);
		}

		// EOP
		TEST_ASSERT_EQUAL(0, cobsStream.available());
	}
}

void test2()
{
	NOneTest(2);
}

void test4()
{
	NOneTest(4);
}

void test10()
{
	NOneTest(10);
}

void test100()
{
	NOneTest(100);
}

void test300()
{
	NOneTest(300);
}

void test1000()
{
	NOneTest(1000);
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
	}
	UNITY_END();
}