#pragma once

#include "serialize.hpp"
#include "Messaging.hpp"

namespace msgpack {
    class Serializer {
    public:
        Serializer(Stream &);

        inline Serializer & operator<<(Messaging & messaging) {
            writeString(this->stream, messaging.getTypeName());
            messaging.reportStatus(* this);
            return * this;
        }

        template<typename IntType>
        inline void beginMap(const IntType & size) {
            writeMapSize(this->stream, size);
        }

        template<typename IntType>
        inline void beginArray(const IntType & size) {
            writeArraySize(this->stream, size);
        }

        inline Serializer & operator<<(const char * value) {
            writeString(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const uint8_t & value) {
            writeIntU8(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const uint16_t & value) {
            writeIntU16(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const uint32_t & value) {
            writeIntU32(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const uint64_t & value) {
            writeIntU64(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const int8_t & value) {
            writeInt8(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const int16_t & value) {
            writeInt16(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const int32_t & value) {
            writeInt32(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const int64_t & value) {
            writeInt64(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const float & value) {
            writeFloat32(this->stream, value);
            return * this;
        }

        inline Serializer & operator<<(const double & value) {
            writeFloat64(this->stream, value);
            return * this;
        }
        
        inline Serializer & operator<<(const bool & value) {
            writeBool(this->stream, value);
            return * this;
        }

        inline Stream & operator()() {
            return this->stream;
        }
    protected:
        Stream & stream;
    };
}

