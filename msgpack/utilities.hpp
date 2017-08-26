#pragma once

#include "Arduino.h"
#include <stdint.h>

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

namespace msgpack {
    bool waitForData(Stream & stream, size_t size, long timeoutMs = 100);

    template<typename DataType>
    bool readRaw(Stream & stream, DataType & value, bool safely = true) {
        const auto size = sizeof(DataType);
        MSGPACK_SAFETY_LENGTH_CHECK(size);
        stream.readBytes((char *) & value, size);
        return true;
    }
    
    template<typename DataType>
    bool readRawReversed(Stream & stream, DataType & value, bool safely = true) {
        const auto size = sizeof(DataType);
        MSGPACK_SAFETY_LENGTH_CHECK(size);
        for(uint8_t i=0; i<size; i++) {
            auto address = (size_t) (size - 1 - i);
            ( (char*) & value )[address] = (char) stream.read();
        }
        return true;
    }

    bool readRaw(Stream & stream, char *, const size_t & length);

    template<typename DataType>
    bool writeRawReversed(Stream & stream, const DataType & value) {
        const auto size = sizeof(DataType);
        MSGPACK_SAFETY_LENGTH_CHECK(size);
        for(uint8_t i=0; i<size; i++) {
            auto address = (size_t) (size - 1 - i);
            stream.write(( (const char *) & value )[address]);
        }
    }
}