#include "msgpack.hpp"

namespace msgpack {
    //----------
    DataFormat getNextDataFormatUnsafely(Stream & stream) {
        auto nextByte = stream.peek();
        switch(nextByte) {
        case -1:
            return DataFormat::Unknown;
        case 0xc0:
            return DataFormat::Nil;
        case 0xc2:
        case 0xc3:
            return DataFormat::Bool;
        case 0xc4:
        case 0xc5:
        case 0xc6:
            return DataFormat::Binary;
        case 0xc7:
        case 0xc8:
        case 0xc9:
            return DataFormat::Extended;
        case 0xca:
            return DataFormat::Float32;
        case 0xcb:
            return DataFormat::Float64;
        case 0xcc:
            return DataFormat::UInt8;
        case 0xcd:
            return DataFormat::UInt16;
        case 0xce:
            return DataFormat::UInt32;
        case 0xcf:
            return DataFormat::UInt64;
        case 0xd0:
            return DataFormat::Int8;
        case 0xd1:
            return DataFormat::Int16;
        case 0xd2:
            return DataFormat::Int32;
        case 0xd3:
            return DataFormat::Int64;
        case 0xd4:
        case 0xd5:
        case 0xd6:
        case 0xd7:
        case 0xd8:
            return DataFormat::FixedExtended;
        case 0xd9:
        case 0xda:
        case 0xdb:
            return DataFormat::String;
        case 0xdc:
        case 0xdd:
            return DataFormat::Array;
        case 0xde:
        case 0xdf:
            return DataFormat::Map;
        default:
            break;
        }

        if(nextByte >= 0x00 && nextByte <= 0x7f) {
            return DataFormat::IntPositive;
        }
        if(nextByte >= 0x80 && nextByte <= 0x8f) {
            return DataFormat::Map;
        }
        if(nextByte >= 0x90 && nextByte <= 0x9d) {
            return DataFormat::Array;
        }
        if(nextByte >= 0xa0 && nextByte <= 0xbf) {
            return DataFormat::String;
        }
        if(nextByte >= 0xe0 && nextByte <= 0xff) {
            return DataFormat::IntNegative;
        }

        return DataFormat::Unknown;
    }
    
    //----------
    bool getNextDataFormat(Stream & stream, DataFormat & dataFormat, bool safely) {
        MSGPACK_SAFETY_LENGTH_CHECK(1);
        dataFormat = getNextDataFormatUnsafely(stream);
        return true;
    }

    //----------
    bool nextDataFormatIs(Stream & stream, const DataFormat & dataFormat, bool safely) {
        DataFormat actualDataFormat;
        MSGPACK_SAFELY_RUN(getNextDataFormat(stream, actualDataFormat, safely));
        return actualDataFormat == dataFormat;
    }

    //----------
    bool readNil(Stream & stream, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Nil);
        
        //this is already safe to perform if the format checks out
        stream.read();
        
        return true;
    }

    //----------
    bool readMapSize(Stream & stream, size_t & size, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Map);
        
        uint8_t header;
        MSGPACK_SAFELY_RUN(readRawFlipped(stream, header));
        
        switch(header) {
        case 0xde:
            //map 16:
            uint16_t size16;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, size16, safely));
            size = (size_t) size16;
            break;
        case 0xdf:
            //map 32
            uint32_t size32;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, size32));
            size = (size_t) size32;
            break;
        default:
            //fixmap
            size = header & 0x0f;
            break;
        }

        return true;
    }

    //----------
    bool readArraySize(Stream & stream, size_t & size, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Array);

        uint8_t header;
        MSGPACK_SAFELY_RUN(readRawFlipped(stream, header));

        switch(header) {
        case 0xde:
            //array 16:
            uint16_t size16;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, size16));
            size = (size_t) size16;
            break;
        case 0xdf:
            //array 32
            uint32_t size32;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, size32));
            size = (size_t) size32;
            break;
        default:
            //fixarray
            size = (size_t) (header & 0x0f);
        }

        return true;
    }

    //----------
    bool readSpecificInt(Stream & stream, uint8_t & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::UInt8);
        stream.read();
        return readRawFlipped(stream, value);
    }

    //----------
    bool readSpecificInt(Stream & stream, uint16_t & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::UInt16);
        stream.read();
        return readRawFlipped(stream, value);
    }

    //----------
    bool readSpecificInt(Stream & stream, uint32_t & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::UInt32);
        stream.read();
        return readRawFlipped(stream, value);
    }


    //----------
    bool readSpecificInt(Stream & stream, uint64_t & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Int64);
        stream.read();
        return readRawFlipped(stream, value);
    }

    //----------
    bool readSpecificInt(Stream & stream, int8_t & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Int8);
        stream.read();
        return readRawFlipped(stream, value);
    }

    //----------
    bool readSpecificInt(Stream & stream, int16_t & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Int16);
        stream.read();
        return readRawFlipped(stream, value);
    }

    //----------
    bool readSpecificInt(Stream & stream, int32_t & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Int32);
        stream.read();
        return readRawFlipped(stream, value);
    }

    //----------
    bool readSpecificInt(Stream & stream, int64_t & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Int64);
        stream.read();
        return readRawFlipped(stream, value);
    }

    //----------
    template<typename OutputType>
    bool readGeneralInt(Stream & stream, OutputType & value, bool safely) {
        DataFormat dataFormat;
        getNextDataFormat(stream, dataFormat, safely);

        //discard the header byte
        stream.read();

        switch (dataFormat)
        {
        case DataFormat::IntPositive:
        case DataFormat::UInt8:
        {
            uint8_t specificValue;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        case DataFormat::UInt16:
        {
            uint16_t specificValue;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        case DataFormat::UInt32:
        {
            uint32_t specificValue;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        case DataFormat::UInt64:
        {
            uint64_t specificValue;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        case DataFormat::IntNegative:
        case DataFormat::Int8:
        {
            int8_t specificValue;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        case DataFormat::Int16:
        {
            int16_t specificValue;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        case DataFormat::Int32:
        {
            int32_t specificValue;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        case DataFormat::Int64:
        {
            int64_t specificValue;
            MSGPACK_SAFELY_RUN(readRawFlipped(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        default:
            return false;
        }

        return true;
    }
    template bool readGeneralInt<uint8_t>(Stream &, uint8_t &, bool);
    template bool readGeneralInt<uint16_t>(Stream &, uint16_t &, bool);
    template bool readGeneralInt<uint32_t>(Stream &, uint32_t &, bool);
    template bool readGeneralInt<uint64_t>(Stream &, uint64_t &, bool);
    template bool readGeneralInt<int8_t>(Stream &, int8_t &, bool);
    template bool readGeneralInt<int16_t>(Stream &, int16_t &, bool);
    template bool readGeneralInt<int32_t>(Stream &, int32_t &, bool);
    template bool readGeneralInt<int64_t>(Stream &, int64_t &, bool);

    //----------
    bool readSpecificFloat(Stream & stream, float & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Float32);
        stream.read();
        return readRaw(stream, value);
    }

    //----------
    bool readSpecificFloat(Stream & stream, double & value, bool safely) {
        MSGPACK_SAFETY_FORMAT_CHECK(DataFormat::Float64);
        stream.read();
        return readRaw(stream, value);
    }

    //----------
    template<typename OutputType>
    bool readGeneralFloat(Stream & stream, OutputType & value, bool safely) {
        DataFormat dataFormat;
        getNextDataFormat(stream, dataFormat, safely);

        //discard the header byte
        stream.read();

        switch (dataFormat)
        {
        case DataFormat::Float32:
        {
            float specificValue;
            MSGPACK_SAFELY_RUN(readRaw(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        case DataFormat::Float64:
        {
            double specificValue;
            MSGPACK_SAFELY_RUN(readRaw(stream, specificValue, safely));
            value = (OutputType)specificValue;
            break;
        }
        default:
            return false;
        }

        return true;
    }
    template bool readGeneralFloat<float>(Stream &, float &, bool);
    template bool readGeneralFloat<double>(Stream &, double &, bool);

    //----------
    void waitForData(Stream & stream, size_t size, long timeoutMs, bool safely) {
        const auto delayPerTry = timeoutMs / 10;
        uint8_t tries = 0;
        while(!Serial.available() < size) {
            tries++;
            if(tries >= 10) {
                return false;
            }
            delay(delayPerTry);
        }
        return true;
    }
}

