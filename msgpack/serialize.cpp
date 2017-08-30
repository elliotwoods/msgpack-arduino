#include "serialize.hpp"
#include <string.h>

namespace msgpack {
// Nil
    //----------
    void writeNil(Stream & stream) {
        writeRawByte(stream, 0xc0);
    }
    
// Map
    //----------
    void writeMapSize4(Stream & stream, const uint8_t & value) {
        uint8_t bits = value;
        bits &= 0x0f;
        bits |= 0x80;
        writeRawByte(stream, bits);
    }

    //----------
    void writeMapSize16(Stream & stream, const uint16_t & value) {
        writeRawByte(stream, 0xde);
        writeRawReversed(stream, value);
    }

    //----------
    void writeMapSize32(Stream & stream, const uint32_t & value) {
        writeRawByte(stream, 0xdf);
        writeRawReversed(stream, value);
    }

// Array
    //----------
    void writeArraySize4(Stream & stream, const uint8_t & value) {
        uint8_t bits = value;
        bits &= 0x0f;
        bits |= 0x90;
        writeRawByte(stream, bits);
    }

    //----------
    void writeArraySize16(Stream & stream, const uint16_t & value) {
        writeRawByte(stream, 0xdc);
        writeRawReversed(stream, value);
    }

    //----------
    void writeArraySize32(Stream & stream, const uint32_t & value) {
        writeRawByte(stream, 0xdd);
        writeRawReversed(stream, value);
    }

// Int
    //----------
    void writeIntU7(Stream & stream, const uint8_t & value) {
        uint8_t bits = value;
        bits &= 0x7f;
        writeRawByte(stream, bits);
    }

    //----------
    void writeIntU8(Stream & stream, const uint8_t & value) {
        writeRawByte(stream, 0xcc);
        writeRawByte(stream, value);
    }

    //----------
    void writeIntU16(Stream & stream, const uint16_t & value) {
        writeRawByte(stream, 0xcd);
        writeRawReversed(stream, value);
    }

    //----------
    void writeIntU32(Stream & stream, const uint32_t & value) {
        writeRawByte(stream, 0xce);
        writeRawReversed(stream, value);
    }

    //----------
    void writeIntU64(Stream & stream, const uint64_t & value) {
        writeRawByte(stream, 0xcf);
        writeRawReversed(stream, value);
    }

    //----------
    void writeInt5(Stream & stream, const int8_t & value) {
        auto valueRaw = (uint8_t) value;
        valueRaw &= 0x1f;
        writeRawByte(stream, valueRaw);
    }

    //----------
    void writeInt8(Stream & stream, const int8_t & value) {
        writeRawByte(stream, 0xd0);
        writeRawByte(stream, value);
    }

    //----------
    void writeInt16(Stream & stream, const int16_t & value) {
        writeRawByte(stream, 0xd1);
        writeRawReversed(stream, value);
    }

    //----------
    void writeInt32(Stream & stream, const int32_t & value) {
        writeRawByte(stream, 0xd2);
        writeRawReversed(stream, value);
    }

    //----------
    void writeInt64(Stream & stream, const int64_t & value) {
        writeRawByte(stream, 0xd3);
        writeRawReversed(stream, value);
    }

// Float
    //----------
    void writeFloat32(Stream & stream, const float & value) {
        writeRawByte(stream, 0xca);
        writeRawReversed(stream, value);
    }

    //----------
    void writeFloat64(Stream & stream, const double & value) {
        writeRawByte(stream, 0xcb);
        writeRawReversed(stream, value);
    }

// Bool
    //----------
    void writeBool(Stream & stream, bool value) {
        if(value) {
            writeRawByte(stream,  0xc3);
        }
        else {
            writeRawByte(stream,  0xc2);
        }
    }
    
// String
    //----------
    void writeString5(Stream & stream, const char * value, uint8_t size) {
        size &= 0x1f;
        writeRawByte(stream, size | 0xA0);
        writeRaw(stream, value, size);
    }

    //----------
    void writeString8(Stream & stream, const char * value, uint8_t size) {
        writeRawByte(stream, 0xd9);
        writeRawByte(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    void writeString16(Stream & stream, const char * value, uint16_t size) {
        writeRawByte(stream, 0xda);
        writeRawReversed(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    void writeString32(Stream & stream, const char * value, uint32_t size) {
        writeRawByte(stream, 0xdb);
        writeRawReversed(stream, size);
        writeRaw(stream, value, size);
    }
        
// Binary
    //----------
    void writeBinary8(Stream & stream, const char * value, uint8_t size) {
        writeRawByte(stream, 0xc4);
        writeRawByte(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    void writeBinary16(Stream & stream, const char * value, uint16_t size) {
        writeRawByte(stream, 0xc5);
        writeRawReversed(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    void writeBinary32(Stream & stream, const char * value, uint32_t size) {
        writeRawByte(stream, 0xc6);
        writeRawReversed(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    void writeRawByte(Stream & stream, const uint8_t & value) {
        stream.write( (char &) value);
    }

    //----------
    void writeRaw(Stream & stream, const char * value, size_t size) {
        stream.write(value, size);
    }
}

