#pragma once

#include "serialize.hpp"

namespace msgpack {
    class Serializer {
    public:
        Serializer(Stream &);

        template<typename IntType>
        void beginMap(const IntType & size) {
            writeMapSize(this->stream, size);
        }

        Serializer & operator<<(const char * value) {
            writeString(this->stream, value);
            return * this;
        }

        template<typename IntType>
        Serializer & operator<<(const IntType & value) {
            writeInt(this->stream, value);
            return * this;
        }

        Serializer & operator<<(const float & value) {
            writeFloat32(this->stream, value);
            return * this;
        }

        Serializer & operator<<(const double & value) {
            writeFloat64(this->stream, value);
            return * this;
        }
        
        Serializer & operator<<(const bool & value) {
            writeBool(this->stream, value);
            return * this;
        }
    protected:
        Stream & stream;
    };
}

