#pragma once

#include "Arduino.h"
#include "DataType.hpp"
#include <float.h>

namespace msgpack {
	void writeNil(Stream &);
	
	void writeMapSize4(Stream &, const uint8_t &);
	void writeMapSize16(Stream &, const uint16_t &);
	void writeMapSize32(Stream &, const uint32_t &);
	template<typename IntType>
	void writeMapSize(Stream & stream, const IntType & value) {
		if(value == value & 0x0f) {
			writeMapSize4(stream, (uint8_t) value);
		}
		else if(value == value & 0xffff) {
			writeMapSize16(stream, (uint16_t) value);
		}
		else {
			writeMapSize32(stream, value);
		}
	}

	void writeArraySize4(Stream &, const uint8_t &);
	void writeArraySize16(Stream &, const uint16_t &);
	void writeArraySize32(Stream &, const uint32_t &);
	template<typename IntType>
	void writeArraySize(Stream & stream, const IntType & value) {
		if(value == value & 0x0f) {
			writeArraySize4(stream, (uint8_t) value);
		}
		else if(value == value & 0xffff) {
			writeArraySize16(stream, (uint16_t) value);
		}
		else {
			writeArraySize32(stream, value);
		}
	}

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
	void writeInt(Stream & stream, const IntType & value) {
		  //NB : We don't use the 5 and 7 bit types here.
		// If you want to write them then please do so
		// explicitly with the functions above.

		if(value > 0) {
			if (value < 1 << 7) {
				writeIntU7(stream, value);
			}
			else if (value < 1 << 8) {
				writeIntU8(stream, (uint8_t)value);
			}
			else if (value < 1 << 16) {
				writeIntU16(stream, (uint16_t)value);
			}
			else {
				writeIntU32(stream, (uint32_t) value);
			}
		} 
		else {
			if (value > -(1 << 6)) {
				writeInt5(stream, value);
			}
			else if (value > -(1 << 7)) {
				writeInt8(stream, (int8_t) value);
			}
			else if (value > -(1 << 15)) {
				writeInt16(stream, (int16_t) value);
			}
			else {
				writeInt32(stream, (int32_t) value);
			}
		} 
	}

	void writeFloat32(Stream &, const float &);
	void writeFloat64(Stream &, const double &);
	template<typename FloatType>
	void writeFloat(Stream & stream, const FloatType & value) {
		if(value >= FLT_MIN && value <= FLT_MAX) {
			writeFloat32(stream, (float) value);
		}
		else {
			writeFloat64(stream, (double) value);
		}
	}

	void writeBool(Stream & stream, bool);

	void writeString5(Stream &, const char *, uint8_t size);
	void writeString8(Stream &, const char *, uint8_t size);
	void writeString16(Stream &, const char *, uint16_t size);
	void writeString32(Stream &, const char *, uint32_t size);
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
		else if(size == (SizeType) (uint32_t) size) {
			writeString32(stream, value, (uint32_t) size);
		}
	}
	void writeString(Stream & stream, const char * value);

	void writeBinary5(Stream &, const char *, uint8_t size);
	void writeBinary8(Stream &, const char *, uint8_t size);
	void writeBinary16(Stream &, const char *, uint16_t size);
	void writeBinary32(Stream &, const char *, uint32_t size);
	template<typename SizeType>
	void writeBinary(Stream & stream, const char * value, SizeType size) {
		if(size == (SizeType) (uint8_t) size) {
			writeBinary8(stream, value, (uint8_t) size);
		}
		else if(size == (SizeType) (uint16_t) size) {
			writeBinary16(stream, value, (uint16_t) size);
		}
		else if(size == (SizeType) (uint32_t) size) {
			writeBinary32(stream, value, (uint32_t) size);
		}
	}
	
	template<typename DataType>
	void writeRawReversed(Stream & stream, const DataType & value) {
		const auto size = sizeof(DataType);
		for(uint8_t i=0; i<size; i++) {
			auto address = (size_t) (size - 1 - i);
			stream.write(( (const char *) & value )[address]);
		}
	}

	template<typename DataType>
	void writeRaw(Stream & stream, const DataType & value) {
		const auto size = sizeof(DataType);
		stream.write((char * ) & value, sizeof(DataType));
	}

	void writeRawByte(Stream & stream, const uint8_t & value);
	void writeRaw(Stream & stream, const char * value, size_t size);
}