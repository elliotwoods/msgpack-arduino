#include "deserialize.hpp"
#include "serialize.hpp"
#include "logError.hpp"

namespace msgpack {
// Header
	//----------
	DataType getNextDataTypeUnsafely(Stream & stream) {
		auto nextByte = stream.peek();
		switch(nextByte) {
		case -1:
			return DataType::Unknown;
		case 0xc0:
			return DataType::Nil;
		case 0xc2:
		case 0xc3:
			return DataType::Bool;
		case 0xc4:
			return DataType::Binary8;
		case 0xc5:
			return DataType::Binary16;
		case 0xc6:
			return DataType::Binary32;
		case 0xc7:
		case 0xc8:
		case 0xc9:
			return DataType::Extended;
		case 0xca:
			return DataType::Float32;
		case 0xcb:
			return DataType::Float64;
		case 0xcc:
			return DataType::UInt8;
		case 0xcd:
			return DataType::UInt16;
		case 0xce:
			return DataType::UInt32;
		case 0xcf:
			return DataType::UInt64;
		case 0xd0:
			return DataType::Int8;
		case 0xd1:
			return DataType::Int16;
		case 0xd2:
			return DataType::Int32;
		case 0xd3:
			return DataType::Int64;
		case 0xd4:
		case 0xd5:
		case 0xd6:
		case 0xd7:
		case 0xd8:
			return DataType::FixedExtended;
		case 0xd9:
			return DataType::String8;
		case 0xda:
			return DataType::String16;
		case 0xdb:
			return DataType::String32;
		case 0xdc:
		case 0xdd:
			return DataType::Array;
		case 0xde:
		case 0xdf:
			return DataType::Map;
		default:
			break;
		}

		if(nextByte >= 0x00 && nextByte <= 0x7f) {
			return DataType::UInt7;
		}
		if(nextByte >= 0x80 && nextByte <= 0x8f) {
			return DataType::Map;
		}
		if(nextByte >= 0x90 && nextByte <= 0x9d) {
			return DataType::Array;
		}
		if(nextByte >= 0xa0 && nextByte <= 0xbf) {
			return DataType::String5;
		}
		if(nextByte >= 0xe0 && nextByte <= 0xff) {
			return DataType::Int5;
		}

		return DataType::Unknown;
	}

	//----------
	bool getNextDataType(Stream & stream, DataType & dataFormat, bool safely) {
		MSGPACK_SAFETY_LENGTH_CHECK(stream, 1);
		dataFormat = getNextDataTypeUnsafely(stream);
		return true;
	}

	//----------
	bool nextDataTypeIs(Stream & stream, const DataType & dataType, bool safely) {
		DataType actualDataType;
		MSGPACK_SAFELY_RUN(getNextDataType(stream, actualDataType, safely));
		return actualDataType == dataType;
	}

// Nil
	//----------
	bool readNil(Stream & stream, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Nil);

		//this is already safe to perform if the format checks out
		stream.read();

		return true;
	}

// Map
	//----------
	bool readMapSize(Stream & stream, size_t & size, bool safely) {
#ifdef MSGPACK_DEBUG_INCOMING
		msgpack::writeMapSize4(stream, 2);
		{
			msgpack::writeString(stream, "next type");
			{
				msgpack::writeString(stream, toString(getNextDataTypeUnsafely(stream)));
			}

			msgpack::writeString(stream, "is map");
			{
				msgpack::writeBool(stream, nextDataTypeIs(stream, DataType::Map));
			}
		}
		stream.flush();
#endif

		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Map);

		uint8_t header;
		MSGPACK_SAFELY_RUN(readRaw(stream, header, safely));

#ifdef MSGPACK_DEBUG_INCOMING
		msgpack::writeMapSize4(stream, 1);
		{
			msgpack::writeString(stream, "Map header");
			{
				msgpack::writeIntU8(stream, header);
			}
		}
		stream.flush();
#endif

		switch(header) {
		case 0xde:
			//map 16:
			uint16_t size16;
			MSGPACK_SAFELY_RUN(readRawReversed(stream, size16, safely));
			size = (size_t) size16;
			break;
		case 0xdf:
			//map 32
			uint32_t size32;
			MSGPACK_SAFELY_RUN(readRawReversed(stream, size32, safely));
			size = (size_t) size32;
			break;
		default:
			//fixmap
			size = header & 0x0f;
			break;
		}

		return true;
	}

// Array
	//----------
	bool readArraySize(Stream & stream, size_t & size, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Array);

		uint8_t header;
		MSGPACK_SAFELY_RUN(readRawReversed(stream, header));

		switch(header) {
		case 0xdc:
			//array 16:
			uint16_t size16;
			MSGPACK_SAFELY_RUN(readRawReversed(stream, size16));
			size = (size_t) size16;
			break;
		case 0xdd:
			//array 32
			uint32_t size32;
			MSGPACK_SAFELY_RUN(readRawReversed(stream, size32));
			size = (size_t) size32;
			break;
		default:
			//fixarray
			size = (size_t) (header & 0x0f);
		}

#ifdef MSGPACK_DEBUG_INCOMING
		writeMapSize4(stream, 1);
		{
			writeString(stream, "Array size");
			{
				writeInt(stream, (uint32_t) size);
			}
		}
		stream.flush();
#endif

		return true;
	}

// Int
	//----------
	bool readIntU7(Stream & stream, uint8_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::UInt7);
		// since the first bits are 0XXXXXXX, we don't need to do any maths
		return readRaw(stream, value, safely);
	}

	//----------
	bool readIntU8(Stream & stream, uint8_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::UInt8);
		stream.read();
		return readRaw(stream, value, safely);
	}

	//----------
	bool readIntU16(Stream & stream, uint16_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::UInt16);
		stream.read();
		return readRawReversed(stream, value, safely);
	}

	//----------
	bool readIntU32(Stream & stream, uint32_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::UInt32);
		stream.read();
		return readRawReversed(stream, value, safely);
	}


	//----------
	bool readIntU64(Stream & stream, uint64_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Int64);
		stream.read();
		return readRawReversed(stream, value, safely);
	}

	//----------
	bool readInt5(Stream & stream, int8_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Int5);
		// since the first bits are 111XXXXX, we don't need to do any maths
		return readRaw(stream, value, safely);
	}

	//----------
	bool readInt8(Stream & stream, int8_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Int8);
		stream.read();
		return readRaw(stream, value, safely);
	}

	//----------
	bool readInt16(Stream & stream, int16_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Int16);
		stream.read();
		return readRawReversed(stream, value, safely);
	}

	//----------
	bool readInt32(Stream & stream, int32_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Int32);
		stream.read();
		return readRawReversed(stream, value, safely);
	}

	//----------
	bool readInt64(Stream & stream, int64_t & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Int64);
		stream.read();
		return readRawReversed(stream, value, safely);
	}

	//----------
	template<typename OutputType>
	bool readInt(Stream & stream, OutputType & value, bool safely) {
		DataType dataFormat;
		MSGPACK_SAFELY_RUN(getNextDataType(stream, dataFormat, safely));

		#ifdef MSGPACK_DEBUG_INCOMING
		msgpack::writeMapSize4(stream, 1);
		{
			msgpack::writeString(stream, "Int type");
			{
				msgpack::writeString(stream, toString(getNextDataTypeUnsafely(stream)));
			}
		}
		stream.flush();
		#endif

		switch (dataFormat)
		{
			case DataType::UInt7:
			{
				uint8_t specificValue;
				MSGPACK_SAFELY_RUN(readIntU7(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}

			case DataType::UInt8:
			{
				uint8_t specificValue;
				MSGPACK_SAFELY_RUN(readIntU8(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}
			case DataType::UInt16:
			{
				uint16_t specificValue;
				MSGPACK_SAFELY_RUN(readIntU16(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}
			case DataType::UInt32:
			{
				uint32_t specificValue;
				MSGPACK_SAFELY_RUN(readIntU32(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}
			case DataType::UInt64:
			{
				uint64_t specificValue;
				MSGPACK_SAFELY_RUN(readIntU64(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}
			case DataType::Int5:
			{
				int8_t specificValue;
				MSGPACK_SAFELY_RUN(readInt5(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}
			case DataType::Int8:
			{
				int8_t specificValue;
				MSGPACK_SAFELY_RUN(readInt8(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}
			case DataType::Int16:
			{
				int16_t specificValue;
				MSGPACK_SAFELY_RUN(readInt16(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}
			case DataType::Int32:
			{
				int32_t specificValue;
				MSGPACK_SAFELY_RUN(readInt32(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}
			case DataType::Int64:
			{
				int64_t specificValue;
				MSGPACK_SAFELY_RUN(readInt64(stream, specificValue, safely));
				value = (OutputType) specificValue;
				return true;
			}
			default:
			{
#ifdef MSGPACK_DEBUG_INCOMING
				logErrorWithData(stream, "readInt", "dataFormat") << (uint8_t) dataFormat;
#endif
				return false;
			}
		}
	}
	template bool readInt<uint8_t>(Stream &, uint8_t &, bool);
	template bool readInt<uint16_t>(Stream &, uint16_t &, bool);
	template bool readInt<uint32_t>(Stream &, uint32_t &, bool);
	template bool readInt<uint64_t>(Stream &, uint64_t &, bool);
	template bool readInt<int8_t>(Stream &, int8_t &, bool);
	template bool readInt<int16_t>(Stream &, int16_t &, bool);
	template bool readInt<int32_t>(Stream &, int32_t &, bool);
	template bool readInt<int64_t>(Stream &, int64_t &, bool);

// Float
	//----------
	bool readFloat32(Stream & stream, float & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Float32);
		stream.read();
		return readRawReversed(stream, value);
	}

	//----------
	bool readFloat64(Stream & stream, double & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Float64);
		stream.read();
		return readRawReversed(stream, value);
	}

	//----------
	template<typename OutputType>
	bool readFloat(Stream & stream, OutputType & value, bool safely) {
		DataType dataFormat;
		getNextDataType(stream, dataFormat, safely);

#ifdef MSGPACK_DEBUG_INCOMING
		msgpack::writeMapSize4(stream, 1);
		{
			msgpack::writeString(stream, "Float type");
			{
				msgpack::writeString(stream, toString(getNextDataTypeUnsafely(stream)));
			}
		}
		stream.flush();
#endif

		switch (dataFormat)
		{
			case DataType::Float32:
			{
				float specificValue;
				MSGPACK_SAFELY_RUN(readFloat32(stream, specificValue, safely));
				value = (OutputType)specificValue;
				return true;
			}
			case DataType::Float64:
			{
				double specificValue;
				MSGPACK_SAFELY_RUN(readFloat64(stream, specificValue, safely));
				value = (OutputType)specificValue;
				return true;
			}
			default:
			{
				// Try to deserialize as an int if we couldn't find a float type
				{
					int32_t specificValue;
					if(readInt(stream, specificValue)) {
						value = (OutputType) specificValue;
						return true;
					}
				}
#ifdef MSGPACK_DEBUG_INCOMING
				logErrorWithData(stream, "readFloat", "dataFormat") << (uint8_t) dataFormat;
#endif
				return false;
			}
		}
	}
	template bool readFloat<float>(Stream &, float &, bool);
	template bool readFloat<double>(Stream &, double &, bool);

// Bool
	//----------
	bool readBool(Stream & stream, bool & value, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Bool);

		uint8_t bits;
		MSGPACK_SAFELY_RUN(readRaw(stream, bits, safely));

		value = (bits == 0xc3); // 0xc2 = false

		return true;
	}

// String
	//----------
	bool readString5(Stream & stream, char * value, const uint8_t & allocation, uint8_t & outputSize, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::String5);
		MSGPACK_SAFELY_RUN(readRaw(stream, outputSize, safely));
		outputSize &= 0x1f;
		MSGPACK_SAFETY_CHECK(allocation >= outputSize + 1);
		MSGPACK_SAFELY_RUN(readRaw(stream, value, outputSize, safely));
		value[outputSize] = '\0';
		return true;
	}

	//----------
	bool readString8(Stream & stream, char * value, const uint8_t & allocation, uint8_t & outputSize, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::String8);
		stream.read();
		MSGPACK_SAFELY_RUN(readRaw(stream, outputSize, safely));
		MSGPACK_SAFETY_CHECK(allocation >= outputSize + 1);
		MSGPACK_SAFELY_RUN(readRaw(stream, value, outputSize, safely));
		value[outputSize] = '\0';
		return true;
	}

	//----------
	bool readString16(Stream & stream, char * value, const uint16_t & allocation, uint16_t & outputSize, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::String16);
		stream.read();
		MSGPACK_SAFELY_RUN(readRawReversed(stream, outputSize, safely));
		MSGPACK_SAFETY_CHECK(allocation >= outputSize + 1);
		MSGPACK_SAFELY_RUN(readRaw(stream, value, outputSize, safely));
		value[outputSize] = '\0';
		return true;
	}

	//----------
	bool readString32(Stream & stream, char * value, const uint32_t & allocation, uint32_t & outputSize, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::String32);
		stream.read();
		MSGPACK_SAFELY_RUN(readRawReversed(stream, outputSize, safely));
		MSGPACK_SAFETY_CHECK(allocation >= outputSize + 1);
		MSGPACK_SAFELY_RUN(readRaw(stream, value, outputSize, safely));
		value[outputSize] = '\0';
		return true;
	}

	//----------
	bool readString(Stream & stream, char * value, const size_t & allocation, size_t & outputSize, bool safely) {
		DataType dataFormat;
		getNextDataType(stream, dataFormat, safely);

		switch(dataFormat) {
			case DataType::String5:
			{
				uint8_t specificOutputSize;
				MSGPACK_SAFELY_RUN(readString5(stream, value, (uint8_t) allocation, specificOutputSize, safely));
				outputSize = (size_t) specificOutputSize;
				return true;
			}
			case DataType::String8:
			{
				uint8_t specificOutputSize;
				MSGPACK_SAFELY_RUN(readString8(stream, value, (uint8_t) allocation, specificOutputSize, safely));
				outputSize = (size_t) specificOutputSize;
				return true;
			}
			case DataType::String16:
			{
				uint16_t specificOutputSize;
				MSGPACK_SAFELY_RUN(readString16(stream, value, (uint16_t) allocation, specificOutputSize, safely));
				outputSize = (size_t) specificOutputSize;
				return true;
			}
			case DataType::String32:
			{
				uint32_t specificOutputSize;
				MSGPACK_SAFELY_RUN(readString32(stream, value, (uint32_t) allocation, specificOutputSize, safely));
				outputSize = (size_t) specificOutputSize;
				return true;
			}
			default:
#ifdef MSGPACK_DEBUG_INCOMING
				logErrorWithData(stream, "readString", "dataFormat") << (uint8_t) dataFormat;
#endif
				return false;
		}
	}

	//----------
	String readStringNew(Stream &stream, bool safely) {
		char* buffer = nullptr;
		if(readStringNewC(stream, buffer, safely)) {
			String result(buffer);
			delete buffer;
			return result;
		}
		else {
			// We return an empty string if deserialisation fails
			return String();
		}
	}

	//----------
	bool readStringNewC(Stream &stream, char * & string, bool safely) {
		DataType	dataFormat;
		char*			value = nullptr;
		getNextDataType(stream, dataFormat, safely);
		switch(dataFormat) {
			case DataType::String5: {
				MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::String5);
				uint8_t outputSize;
				MSGPACK_SAFELY_RUN(readRaw(stream, outputSize, safely));
				outputSize &= 0x1f;
				value = new char[outputSize + 1];
				MSGPACK_SAFELY_RUN(readRaw(stream, value, outputSize, safely));
				value[outputSize] = '\0';
				string = value;
				return true;
			}
			case DataType::String8: {
				MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::String8);
				uint8_t outputSize;
				stream.read();
				MSGPACK_SAFELY_RUN(readRaw(stream, outputSize, safely));
				value = new char[outputSize + 1];
				MSGPACK_SAFELY_RUN(readRaw(stream, value, outputSize, safely));
				value[outputSize] = '\0';
				string = value;
				return true;
			}
			case DataType::String16: {
				MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::String16);
				uint16_t outputSize;
				stream.read();
				MSGPACK_SAFELY_RUN(readRawReversed(stream, outputSize, safely));
				value = new char[outputSize + 1];
				MSGPACK_SAFELY_RUN(readRaw(stream, value, outputSize, safely));
				value[outputSize] = '\0';
				string = value;
				return true;
			}
			case DataType::String32: {
				MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::String32);
				uint32_t outputSize;
				stream.read();
				MSGPACK_SAFELY_RUN(readRawReversed(stream, outputSize, safely));
				value = new char[outputSize + 1];
				MSGPACK_SAFELY_RUN(readRaw(stream, value, outputSize, safely));
				value[outputSize] = '\0';
				string = value;
				return true;
			}
			default:
#ifdef MSGPACK_DEBUG_INCOMING
				logErrorWithData(stream, "readStringNewC", "dataFormat") << (uint8_t) dataFormat;
#endif
				return false;
		}
	}

// Binary
	//----------
	bool readBinary8(Stream & stream, char * value, const uint8_t & allocation, uint8_t & outputSize, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Binary8);
		stream.read();
		MSGPACK_SAFELY_RUN(readRaw(stream, outputSize, safely));
		MSGPACK_SAFETY_CHECK(allocation >= outputSize);
		return readRaw(stream, value, outputSize, safely);
	}

	//----------
	bool readBinary16(Stream & stream, char * value, const uint16_t & allocation, uint16_t & outputSize, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Binary16);
		stream.read();
		MSGPACK_SAFELY_RUN(readRawReversed(stream, outputSize, safely));
		MSGPACK_SAFETY_CHECK(allocation >= outputSize);
		return readRaw(stream, value, outputSize, safely);
	}

	//----------
	bool readBinary32(Stream & stream, char * value, const uint32_t & allocation, uint32_t & outputSize, bool safely) {
		MSGPACK_SAFETY_FORMAT_CHECK(stream, DataType::Binary32);
		stream.read();
		MSGPACK_SAFELY_RUN(readRawReversed(stream, outputSize, safely));
		MSGPACK_SAFETY_CHECK(allocation >= outputSize);
		return readRaw(stream, value, outputSize, safely);
	}

	//----------
	bool readBinary(Stream & stream, char * value, const size_t & allocation, size_t & outputSize, bool safely) {
		DataType dataFormat;
		getNextDataType(stream, dataFormat, safely);

		switch(dataFormat) {
			case DataType::Binary8:
			{
				uint8_t specificOutputSize;
				MSGPACK_SAFELY_RUN(readBinary8(stream, value, (uint8_t) allocation, specificOutputSize, safely));
				outputSize = (size_t) specificOutputSize;
				return true;
			}
			case DataType::Binary16:
			{
				uint16_t specificOutputSize;
				MSGPACK_SAFELY_RUN(readBinary16(stream, value, (uint16_t) allocation, specificOutputSize, safely));
				outputSize = (size_t) specificOutputSize;
				return true;
			}
			case DataType::Binary32:
			{
				uint32_t specificOutputSize;
				MSGPACK_SAFELY_RUN(readBinary32(stream, value, (uint32_t) allocation, specificOutputSize, safely));
				outputSize = (size_t) specificOutputSize;
				return true;
			}
			default:
			{
#ifdef MSGPACK_DEBUG_INCOMING
				logErrorWithData(stream, "readBinary", "dataFormat") << (uint8_t) dataFormat;
#endif
				return false;
			}
		}
	}

	//----------
	bool waitForData(Stream & stream, size_t size, long timeoutMs) {
		const uint8_t triesMax = 10;
		const auto delayPerTry = timeoutMs / (long) triesMax;
		uint8_t tryCount = 0;
		while(stream.available() < (int) size) {
#ifdef MSGPACK_DEBUG_INCOMING
			writeMapSize4(stream, 1);
			{
				writeString(stream, "Waiting for data try");
				{
					writeIntU8(stream, tryCount);
				}
			}
#endif			
			tryCount++;
			if(tryCount >= triesMax) {
#ifdef MSGPACK_DEBUG_INCOMING
				logError(stream, "waitForData", "timeout waiting for data");
#endif
				return false;
			}
			delay(delayPerTry);
		}
		return true;
	}

	//----------
	bool readRaw(Stream & stream, char * data, const size_t & length, bool safely) {
		if(safely) {
			if (!waitForData(stream, length)) {
				return false;
			}
		}
		stream.readBytes(data, length);
		return true;
	}

	//----------
	bool readRaw(Stream & stream, uint8_t & value, bool safely) {
		MSGPACK_SAFETY_LENGTH_CHECK(stream, 1);
		value = (uint8_t) stream.read();
		return true;
	}
}
