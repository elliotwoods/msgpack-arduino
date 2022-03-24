#pragma once

// Add this define when you need to debug your deserialisation routines.
// It will result in many msgpack debug messages being sent over serial (in no particular format)
// NB : suggest you disable any of your other outgoing messsages whilst this flag is defined
// #define MESSENGER_DEBUG_INCOMING

// Note that the 'safely' here is generally in the function arguments

#define MSGPACK_SAFETY_CHECK(ACTION) \
if(safely) { \
	if(!(ACTION)) { \
		return false; \
	} \
}

#define MSGPACK_SAFELY_RUN(ACTION) \
MSGPACK_SAFETY_CHECK(ACTION) \
else ACTION

#define MSGPACK_SAFETY_FORMAT_CHECK(STREAM, FORMAT) MSGPACK_SAFETY_CHECK(nextDataTypeIs(STREAM, FORMAT))
#define MSGPACK_SAFETY_LENGTH_CHECK(STREAM, LENGTH) MSGPACK_SAFETY_CHECK(waitForData(STREAM, LENGTH))