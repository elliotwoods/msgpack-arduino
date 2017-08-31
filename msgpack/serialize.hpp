#pragma once

#include "Arduino.h"
#include "DataType.hpp"
#include <float.h>

namespace msgpack {
	//explicit functions
	void writeNil(Stream &);

	void writeMapSize4(Stream &, const uint8_t &);
	void writeMapSize16(Stream &, const uint16_t &);
	void writeMapSize32(Stream &, const uint32_t &);

	void writeArraySize4(Stream &, const uint8_t &);
	void writeArraySize16(Stream &, const uint16_t &);
	void writeArraySize32(Stream &, const uint32_t &);

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

	void writeFloat32(Stream &, const float &);
	void writeFloat64(Stream &, const double &);

	void writeBool(Stream & stream, bool);

	void writeString5(Stream &, const char *, uint8_t size);
	void writeString8(Stream &, const char *, uint8_t size);
	void writeString16(Stream &, const char *, uint16_t size);
	void writeString32(Stream &, const char *, uint32_t size);

	void writeBinary5(Stream &, const char *, uint8_t size);
	void writeBinary8(Stream &, const char *, uint8_t size);
	void writeBinary16(Stream &, const char *, uint16_t size);
	void writeBinary32(Stream &, const char *, uint32_t size);

	//implicit functions
	inline void writeMapSize(Stream & stream, const uint8_t & value) { value < 1 << 4 ? writeMapSize4(stream, value) : writeMapSize16(stream, value); }
	inline void writeMapSize(Stream & stream, const uint16_t & value) { writeMapSize16(stream, value); }
	inline void writeMapSize(Stream & stream, const uint32_t & value) { writeMapSize32(stream, value); }
	inline void writeArraySize(Stream & stream, const uint16_t & value) { writeArraySize16(stream, value); }
	inline void writeArraySize(Stream & stream, const uint32_t & value) { writeArraySize32(stream, value); }
	inline void writeInt(Stream & stream, const uint8_t & value) { writeIntU8(stream, value); }
	inline void writeInt(Stream & stream, const uint16_t & value) { writeIntU16(stream, value); }
	inline void writeInt(Stream & stream, const uint32_t & value) { writeIntU32(stream, value); }
	inline void writeInt(Stream & stream, const uint64_t & value) { writeIntU64(stream, value); }
	inline void writeInt(Stream & stream, const int8_t & value) { writeInt8(stream, value); }
	inline void writeInt(Stream & stream, const int16_t & value) { writeInt16(stream, value); }
	inline void writeInt(Stream & stream, const int32_t & value) { writeInt32(stream, value); }
	inline void writeInt(Stream & stream, const int64_t & value) { writeInt64(stream, value); }
	inline void writeFloat(Stream & stream, const float & value) { writeFloat32(stream, value); }
	inline void writeFloat(Stream & stream, const double & value) { writeFloat64(stream, value); }
	inline void writeString(Stream & stream, const char * value, const uint16_t & size) { writeString16(stream, value, size); }
	inline void writeString(Stream & stream, const char * value, const uint32_t & size) { writeString32(stream, value, size); }
	inline void writeString(Stream & stream, const char * value) { writeString32(stream, value, strlen(value)); }
	inline void writeBinary(Stream & stream, const char * value, const uint32_t & size) { writeBinary16(stream, value, size); };
	inline void writeBinary(Stream & stream, const char * value, const uint64_t & size) { writeBinary32(stream, value, size); };
	
	//utilities
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
		stream.write((char * ) & value, sizeof(DataType));
	}

	void writeRawByte(Stream & stream, const uint8_t & value);
	void writeRaw(Stream & stream, const char * value, size_t size);
}