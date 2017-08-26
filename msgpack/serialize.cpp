#include "serialize.hpp"

namespace msgpack {
#pragma mark Nil
    //----------
    void writeNil(Stream & stream) {
        stream.write(0xc0);
    }
    
#pragma mark Map
    //----------
    void writeMapSize4(Stream & stream, const uint8_t & value) {
        value &= 0x0f;
        value |= 0x80;
        stream.write(value);
    }

    //----------
    void writeMapSize16(Stream & stream, const uint16_t & value) {
        stream.write(0xde);
        writeFlipped(stream, value);
    }

    //----------
    void writeMapSize32(Stream & stream, const uint32_t & value) {
        stream.write(0xdf);
        writeFlipped(stream, value);
    }

    //----------
    template<typename IntType>
    void writeMapSize(Stream & stream, const IntType & value) {
        if(value == value & 0x0f) {
            writeMapSize4(stream, (uint8_t) value);
        }
        else if(value == value & 0xffff) {
            writeMapSize16(stream, (uint16_t) value);
        }
        else {
            writeMapSize32(value);
        }
    }
    template void writeMapSize(Stream &, uint8_t &);
    template void writeMapSize(Stream &, uint16_t &);
    template void writeMapSize(Stream &, uint32_t &);
    template void writeMapSize(Stream &, uint64_t &);
    template void writeMapSize(Stream &, int8_t &);
    template void writeMapSize(Stream &, int16_t &);
    template void writeMapSize(Stream &, int32_t &);
    template void writeMapSize(Stream &, int64_t &);

#pragma mark Array
    //----------
    void writeArraySize4(Stream & stream, const uint8_t & value) {
        value &= 0x0f;
        value |= 0x90;
        stream.write(value);
    }

    //----------
    void writeArraySize16(Stream & stream, const uint16_t & value) {
        stream.write(0xdc);
        writeFlipped(stream, value);
    }

    //----------
    void writeArraySize32(Stream & stream, const uint32_t & value) {
        stream.write(0xdd);
        writeFlipped(stream, value);
    }

    //----------
    template<typename IntType>
    void writeArraySize(Stream & stream, const IntType & value) {
        if(value == value & 0x0f) {
            writeArraySize4(stream, (uint8_t) value);
        }
        else if(value == value & 0xffff) {
            writeArraySize16(stream, (uint16_t) value);
        }
        else {
            writeArraySize32(value);
        }
    }
    template void writeArraySize(Stream &, const uint8_t &);
    template void writeArraySize(Stream &, const uint16_t &);
    template void writeArraySize(Stream &, const uint32_t &);
    template void writeArraySize(Stream &, const uint64_t &);
    template void writeArraySize(Stream &, const int8_t &);
    template void writeArraySize(Stream &, const int16_t &);
    template void writeArraySize(Stream &, const int32_t &);
    template void writeArraySize(Stream &, const int64_t &);

#pragma mark Int
    //----------
    void writeIntU8(Stream & stream, const uint8_t & value) {
        value &= 0x7f;
        writeRaw(stream, value);
    }

    //----------
    void writeIntU8(Stream & stream, const uint8_t & value) {
        stream.write(0xcc);
        writeRaw(stream, value);
    }

    //----------
    void writeIntU16(Stream & stream, const uint16_t & value) {
        stream.write(0xcd);
        writeRawReversed(stream, value);
    }

    //----------
    void writeIntU32(Stream & stream, const uint32_t & value) {
        stream.write(0xce);
        writeRawReversed(stream, value);
    }

    //----------
    void writeIntU64(Stream & stream, const uint64_t & value) {
        stream.write(0xcf);
        writeRawReversed(stream, value);
    }

    //----------
    void writeInt5(Stream & stream, const int8_t & value) {
        auto valueRaw = (uint8_t) value;
        valueRaw &= 0x1f;
        writeRaw(stream, valueRaw);
    }

    //----------
    void writeInt8(Stream & stream, const int8_t & value) {
        stream.write(0xd0);
        writeRaw(stream, value);
    }

    //----------
    void writeInt16(Stream & stream, const int16_t & value) {
        stream.write(0xd1);
        writeRawReversed(stream, value);
    }

    //----------
    void writeInt32(Stream & stream, const int32_t & value) {
        stream.write(0xd2);
        writeRawReversed(stream, value);
    }

    //----------
    void writeInt64(Stream & stream, const int64_t & value) {
        stream.write(0xd3);
        writeRawReversed(stream, value);
    }

    //----------
    template<typename IntType>
    void writeInt(Stream & stream, const IntType & value) {
        //NB : We don't use the 5 and 7 bit types here.
        // If you want to write them then please do so
        // explicitly with the functions above.

        if(value == (IntType) (uint8_t) value) {
            writeIntU8(stream, value);
        } 
        else if(value == (IntType) (uint16_t) value) {
            writeIntU16(stream, (uint16_t) value);
        } 
        else if(value == (IntType) (uint32_t) value) {
            writeIntU32(stream, (uint32_t) value);
        } 
        else if(value == (IntType) (uint64_t) value) {
            writeIntU64(stream, (uint64_t) value);
        } 
        else if(value == (IntType) (int8_t) value) {
            writeInt8(stream, (int8_t) value);
        } 
        else if(value == (IntType) (int16_t) value) {
            writeInt16(stream, (int16_t) value);
        } 
        else if(value == (IntType) (int32_t) value) {
            writeInt32(stream, (int32_t) value);
        } 
        else if(value == (IntType) (int64_t) value) {
            writeInt64(stream, (int64_t) value);
        }
        //we should never be able to arrive here
    }
    template void writeInt(Stream & stream, const uint8_t &);
    template void writeInt(Stream & stream, const uint16_t &);
    template void writeInt(Stream & stream, const uint32_t &);
    template void writeInt(Stream & stream, const uint64_t &);
    template void writeInt(Stream & stream, const int8_t &);
    template void writeInt(Stream & stream, const int16_t &);
    template void writeInt(Stream & stream, const int32_t &);
    template void writeInt(Stream & stream, const int64_t &);
    
#pragma mark Float
    //----------
    void writeFloat32(Stream & stream, const float & value) {
        stream.write(0xca);
        writeRaw(stream, value);
    }

    //----------
    void writeFloat64(Stream & stream, const double & value) {
        stream.write(0xcb);
        writeRaw(stream, value);
    }

    //----------
    template<typename FloatType>
    void writeFloat(Stream & stream, const FloatType & value) {
        if(value == (FloatType) (float) value) {
            writeFloat32(stream, (float) value);
        }
        else if(value == (FloatType) (double) value) {
            writeFloat64(stream, (double) value);
        }
    }
    template void writeFloat(Stream & stream, const float &);
    template void writeFloat(Stream & stream, const double &);

#pragma mark String
    //----------
    void writeString5(Stream & stream, const char * value, uint8_t size) {
        size &= 0x1f;
        writeRaw(stream, size | 0xA0);
        writeRaw(stream, value, size);
    }

    //----------
    void writeString8(Stream & stream, const char * value, uint8_t size) {
        writeRaw(stream, 0xd9);
        writeRaw(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    void writeString16(Stream & stream, const char * value, uint16_t size) {
        writeRaw(stream, 0xda);
        writeRawReversed(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    void writeString32(Stream & stream, const char * value, uint32_t size) {
        writeRaw(stream, 0xdb);
        writeRawReversed(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    template<typename SizeType>
    void writeString(Stream & stream, const char * value, SizeType size) {
        if(size == size & 0x1f) {
            writeString5(stream, value, (uint8_t) size);
        }
        else if(size == (SizeType) (uint8_t) size) {
            writeString8(stream, value, (uint8_t) size);
        }
        else if(size == (SizeType) (uint16_t) size) {
            writeString16(stream, value, (uint16_t) size);
        }
        if(size == (SizeType) (uint32_t) size) {
            writeString32(stream, value, (uint32_t) size);
        }
    }
    template void writeString(Stream & stream, const char *, const uint8_t);
    template void writeString(Stream & stream, const char *, const uint16_t);
    template void writeString(Stream & stream, const char *, const uint32_t);
    template void writeString(Stream & stream, const char *, const uint64_t);
    template void writeString(Stream & stream, const char *, const int8_t);
    template void writeString(Stream & stream, const char *, const int16_t);
    template void writeString(Stream & stream, const char *, const int32_t);
    template void writeString(Stream & stream, const char *, const int64_t);

#pragma mark Binary
    //----------
    void writeBinary8(Stream & stream, const char * value, uint8_t size) {
        writeRaw(stream, 0xc4);
        writeRaw(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    void writeBinary16(Stream & stream, const char * value, uint16_t size) {
        writeRaw(stream, 0xc5);
        writeRawReversed(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    void writeBinary32(Stream & stream, const char * value, uint32_t size) {
        writeRaw(stream, 0xc6);
        writeRawReversed(stream, size);
        writeRaw(stream, value, size);
    }

    //----------
    template<typename SizeType>
    void writeBinary(Stream & stream, const char * value, SizeType size) {
        if(size == (SizeType) (uint8_t) size) {
            writeBinary8(stream, value, (uint8_t) size);
        }
        else if(size == (SizeType) (uint16_t) size) {
            writeBinary16(stream, value, (uint16_t) size);
        }
        if(size == (SizeType) (uint32_t) size) {
            writeBinary32(stream, value, (uint32_t) size);
        }
    }
    template void writeBinary(Stream & stream, const char *, const uint8_t);
    template void writeBinary(Stream & stream, const char *, const uint16_t);
    template void writeBinary(Stream & stream, const char *, const uint32_t);
    template void writeBinary(Stream & stream, const char *, const uint64_t);
    template void writeBinary(Stream & stream, const char *, const int8_t);
    template void writeBinary(Stream & stream, const char *, const int16_t);
    template void writeBinary(Stream & stream, const char *, const int32_t);
    template void writeBinary(Stream & stream, const char *, const int64_t);
}

