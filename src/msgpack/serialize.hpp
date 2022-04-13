#pragma once

#include "Platform.h"
#include "DataType.hpp"
#include <float.h>

namespace msgpack {
	//explicit functions
	void writeNil(Print &);

	void writeMapSize4(Print &, const uint8_t &);
	void writeMapSize16(Print &, const uint16_t &);
	void writeMapSize32(Print &, const uint32_t &);

	void writeArraySize4(Print &, const uint8_t &);
	void writeArraySize16(Print &, const uint16_t &);
	void writeArraySize32(Print &, const uint32_t &);

	void writeIntU7(Print &, const uint8_t &);
	void writeIntU8(Print &, const uint8_t &);
	void writeIntU16(Print &, const uint16_t &);
	void writeIntU32(Print &, const uint32_t &);
	void writeIntU64(Print &, const uint64_t &);
	void writeInt5(Print &, const int8_t &);
	void writeInt8(Print &, const int8_t &);
	void writeInt16(Print &, const int16_t &);
	void writeInt32(Print &, const int32_t &);
	void writeInt64(Print &, const int64_t &);

	void writeFloat32(Print &, const float &);
	void writeFloat64(Print &, const double &);

	void writeBool(Print &, bool);

	void writeString5(Print &, const char *, uint8_t size);
	void writeString8(Print &, const char *, uint8_t size);
	void writeString16(Print &, const char *, uint16_t size);
	void writeString32(Print &, const char *, uint32_t size);

	void writeBinary5(Print &, const char *, uint8_t size);
	void writeBinary8(Print &, const char *, uint8_t size);
	void writeBinary16(Print &, const char *, uint16_t size);
	void writeBinary32(Print &, const char *, uint32_t size);

	//implicit functions
	inline void writeMapSize(Print & stream, const uint8_t & value) { value < 1 << 4 ? writeMapSize4(stream, value) : writeMapSize16(stream, value); }
	inline void writeMapSize(Print & stream, const uint16_t & value) { writeMapSize16(stream, value); }
	inline void writeMapSize(Print & stream, const uint32_t & value) { writeMapSize32(stream, value); }
	inline void writeArraySize(Print & stream, const uint16_t & value) { writeArraySize16(stream, value); }
	inline void writeArraySize(Print & stream, const uint32_t & value) { writeArraySize32(stream, value); }
	inline void writeInt(Print & stream, const uint8_t & value) { writeIntU8(stream, value); }
	inline void writeInt(Print & stream, const uint16_t & value) { writeIntU16(stream, value); }
	inline void writeInt(Print & stream, const uint32_t & value) { writeIntU32(stream, value); }
	inline void writeInt(Print & stream, const uint64_t & value) { writeIntU64(stream, value); }
	inline void writeInt(Print & stream, const int8_t & value) { writeInt8(stream, value); }
	inline void writeInt(Print & stream, const int16_t & value) { writeInt16(stream, value); }
	inline void writeInt(Print & stream, const int32_t & value) { writeInt32(stream, value); }
	inline void writeInt(Print & stream, const int64_t & value) { writeInt64(stream, value); }
	inline void writeFloat(Print & stream, const float & value) { writeFloat32(stream, value); }
	inline void writeFloat(Print & stream, const double & value) { writeFloat64(stream, value); }
	inline void writeString(Print & stream, const char * value, const uint16_t & size) { writeString16(stream, value, size); }
	inline void writeString(Print & stream, const char * value, const uint32_t & size) { writeString32(stream, value, size); }
	inline void writeString(Print & stream, const char * value) { writeString32(stream, value, strlen(value)); }
	inline void writeBinary(Print & stream, const char * value, const uint32_t & size) { writeBinary16(stream, value, size); };
	inline void writeBinary(Print & stream, const char * value, const uint64_t & size) { writeBinary32(stream, value, size); };
	
	//utilities
	template<typename DataType>
	void writeRawReversed(Print & stream, const DataType & value) {
		const auto size = sizeof(DataType);
		for(uint8_t i=0; i<size; i++) {
			auto address = (size_t) (size - 1 - i);
			stream.write(( (const char *) & value )[address]);
		}
	}

	template<typename DataType>
	void writeRaw(Print & stream, const DataType & value) {
		stream.write((char * ) & value, sizeof(DataType));
	}

	void writeRawByte(Print &, const uint8_t & value);
	void writeRaw(Print &, const char * value, size_t size);
}