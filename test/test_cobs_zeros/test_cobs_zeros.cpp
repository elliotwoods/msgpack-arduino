#include <unity.h>
#include "../LoopbackStream.h"
#include <msgpack.hpp>

LoopbackStream loopbackStream;
msgpack::COBSRWStream cobsStream(loopbackStream);

void singleByteTest()
{
	cobsStream.write(1);
	cobsStream.flush();

	auto available = cobsStream.available();
	TEST_ASSERT_EQUAL(1, available);
	
	auto readback = cobsStream.read();
	TEST_ASSERT_EQUAL(1, readback);
}

void singleZeroTest()
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

void NZeroTest(uint32_t count)
{
	// write to encoded stream
	{
		for(int i=0; i<count; i++) {
			cobsStream.write(0);
		}
		cobsStream.flush();
	}

	// check encoded stream contents
	{
		const auto & data = loopbackStream.getData();

		// length
		TEST_ASSERT_EQUAL(count + 2, data.size());

		// first byte
		TEST_ASSERT_EQUAL(1, data[0]);
		
		// center
		for(uint32_t i=0; i<count; i++) {
			TEST_ASSERT_EQUAL(1, data[i + 1]);
		}

		// EOP
		TEST_ASSERT_EQUAL(0, data.back());
	}

	// check decoded stream
	{
		auto available = cobsStream.available();
		//TEST_ASSERT_EQUAL(count, available); <-- this will actually return the buffer length
		
		for(uint32_t i=0; i<count; i++) {
			auto readback = cobsStream.read();
			TEST_ASSERT_EQUAL(0, readback);
		}

		// EOP
		TEST_ASSERT_EQUAL(0, cobsStream.available());
	}
}

void zeroTest2()
{
	NZeroTest(2);
}

void zeroTest4()
{
	NZeroTest(4);
}

void zeroTest10()
{
	NZeroTest(10);
}

void zeroTest100()
{
	NZeroTest(100);
}

void zeroTest1000()
{
	NZeroTest(1000);
}

int main() {
	UNITY_BEGIN();
	{
		RUN_TEST(singleByteTest);

		cobsStream.nextIncomingPacket();

		RUN_TEST(singleZeroTest);

		cobsStream.nextIncomingPacket();

		RUN_TEST(zeroTest2);

		cobsStream.nextIncomingPacket();

		RUN_TEST(zeroTest4);

		cobsStream.nextIncomingPacket();

		RUN_TEST(zeroTest10);

		cobsStream.nextIncomingPacket();

		RUN_TEST(zeroTest100);

		cobsStream.nextIncomingPacket();

		RUN_TEST(zeroTest1000);

		cobsStream.nextIncomingPacket();

	}
	UNITY_END();
}