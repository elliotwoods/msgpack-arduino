#include <unity.h>
#include "../LoopbackStream.h"

LoopbackStream loopbackStream;

int main() {
	UNITY_BEGIN();
	{
		loopbackStream.write(0);
		TEST_ASSERT_EQUAL(loopbackStream.read(), 0);
	}
	UNITY_END();
}