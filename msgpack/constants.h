#pragma once

#define MSGPACK_SAFETY_CHECK(ACTION) \
if(safely) { \
    if(!ACTION) { \
        return false; \
    } \
}

#define MSGPACK_SAFELY_RUN(ACTION) \
MSGPACK_SAFETY_CHECK(ACTION) \
else ACTION

#define MSGPACK_SAFETY_FORMAT_CHECK(FORMAT) MSGPACK_SAFETY_CHECK(nextDataTypeIs(stream, FORMAT))
#define MSGPACK_SAFETY_LENGTH_CHECK(LENGTH) MSGPACK_SAFETY_CHECK(waitForData(stream, LENGTH))