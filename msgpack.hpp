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

#define MSGPACK_SAFETY_FORMAT_CHECK(FORMAT) MSGPACK_SAFETY_CHECK(nextDataFormatIs(stream, FORMAT))
#define MSGPACK_SAFETY_LENGTH_CHECK(LENGTH) MSGPACK_SAFETY_CHECK(waitForData(stream, LENGTH))

namespace msgpack {
    enum DataFormat : uint8_t {
        Unknown = 0
        , Nil
        , Map 
        , Array
        , Bool
        , UInt8
        , UInt16
        , UInt32
        , UInt64
        , Int8
        , Int16
        , Int32
        , Int64
        , IntPositive
        , IntNegative
        , Float32
        , Float64
        , Binary
        , String
        , Extended
        , FixedExtended
    };

    inline DataFormat getNextDataFormatUnsafely(Stream &);

    //We inline this function to shrink the functions in actual use (most compilers won't need the `inline`)
    inline bool getNextDataFormat(Stream &, DataFormat &, bool safely = true);
    inline bool nextDataFormatIs(Stream &, const DataFormat &, bool safely = true);

    //--
    // Deserialize
    //--
    //
    bool readNil(Stream &, bool safely = true);

    bool readMapSize(Stream &, size_t &, bool safely = true);
    bool readArraySize(Stream &, size_t &, bool safely = true);

    //use these when the encoded int value is of a specific known type
    bool readSpecificInt(Stream &, uint8_t &, bool safely = true);
    bool readSpecificInt(Stream &, uint16_t &, bool safely = true);
    bool readSpecificInt(Stream &, uint32_t &, bool safely = true);
    bool readSpecificInt(Stream &, uint64_t &, bool safely = true);
    bool readSpecificInt(Stream &, int8_t &, bool safely = true);
    bool readSpecificInt(Stream &, int16_t &, bool safely = true);
    bool readSpecificInt(Stream &, int32_t &, bool safely = true);
    bool readSpecificInt(Stream &, int64_t &, bool safely = true);

    //use this if when you don't care what type the encoded int is
    template<typename OutputType>
    bool readGeneralInt(Stream &, OutputType &, bool safely = true);

    bool readSpecificFloat(Stream &, float, bool safely = true);
    bool readSpecificFloat(Stream &, double, bool safely = true);
    
    template<typename OutputType>
    bool readGeneralFloat(Stream &, OutputType &, bool safely = true);
    //
    //--

    //utility functions
    bool waitForData(Stream & stream, size_t size, long timeoutMs = 100);

    template<typename DataType>
    bool readRaw(Stream & stream, DataType & value, bool safely = true) {
        const auto size = sizeof(DataType);
        MSGPACK_SAFETY_LENGTH_CHECK(size);
        stream.readBytes((char *) & value, size);
        return true;
    }
    
    template<typename DataType>
    bool readRawFlipped(Stream & stream, DataType & value, bool safely = true) {
        const auto size = sizeof(DataType);
        MSGPACK_SAFETY_LENGTH_CHECK(size);
        for(uint8_t i=0; i<size; i++) {
            auto address = (size_t) (size - 1 - i);
            ( (char*) & value )[address] = (char) stream.read();
        }
        return true;
    }
}