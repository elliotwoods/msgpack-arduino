#pragma once

#include "Arduino.h"
#include "utilities.hpp"
#include "DataType.hpp"

namespace msgpack {
    void writeNil(Stream &);
    
    void writeMapSize4(Stream &, const uint8_t &);
    void writeMapSize16(Stream &, const uint16_t &);
    void writeMapSize32(Stream &, const uint32_t &);
    template<typename IntType>
    void writeMapSize(Stream &, const IntType &);

    void writeArraySize4(Stream &, const uint8_t &);
    void writeArraySize16(Stream &, const uint16_t &);
    void writeArraySize32(Stream &, const uint32_t &);
    template<typename IntType>
    void writeArraySize(Stream &, const IntType &);

    void writeIntU7(Stream &, const uint8_t &);
    void writeIntU8(Stream &, const uint8_t &);
    void writeIntU16(Stream &, const uint16_t &);
    void writeIntU32(Stream &, const uint32_t &);
    void writeIntU64(Stream &, const uint64_t &);
    void writeInt5(Stream &, const int8_t &);
    void writeInt8(Stream &, const int8_t &);
    void writeInt16(Stream &, const int16_t &);
    void writeInt32(Stream &, const int32_t &);
    void writeInt64(Stream &, const int64_t &);
    template<typename IntType>
    void writeInt(Stream & stream, const IntType &);

    void writeFloat32(Stream &, const float &);
    void writeFloat64(Stream &, const double &);
    template<typename IntType>
    void writeFloat(Stream & stream, const IntType &);

    void writeString5(Stream &, const char *, uint8_t size);
    void writeString8(Stream &, const char *, uint8_t size);
    void writeString16(Stream &, const char *, uint16_t size);
    void writeString32(Stream &, const char *, uint32_t size);
    template<typename SizeType>
    void writeString(Stream & stream, const char * value, SizeType size);

    void writeBinary5(Stream &, const char *, uint8_t size);
    void writeBinary8(Stream &, const char *, uint8_t size);
    void writeBinary16(Stream &, const char *, uint16_t size);
    void writeBinary32(Stream &, const char *, uint32_t size);
    template<typename SizeType>
    void writeBinary(Stream & stream, const char * value, SizeType size);
    
}