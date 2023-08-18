#include "DataType.hpp"

namespace msgpack {
	const char *
	toString(const DataType& dataType) {
		switch(dataType) {
		case DataType::Unknown:
			return "Unknown";
		case DataType::Nil:
			return "Nil";
		case DataType::Map :
			return "Map";
		case DataType::Array:
			return "Array";
		case DataType::Bool:
			return "Bool";
		case DataType::UInt7:
			return "UInt7";
		case DataType::UInt8:
			return "UInt8";
		case DataType::UInt16:
			return "UInt16";
		case DataType::UInt32:
			return "UInt32";
		case DataType::UInt64:
			return "UInt64";
		case DataType::Int5:
			return "Int5";
		case DataType::Int8:
			return "Int8";
		case DataType::Int16:
			return "Int16";
		case DataType::Int32:
			return "Int32";
		case DataType::Int64:
			return "Int64";
		case DataType::Float32:
			return "Float32";
		case DataType::Float64:
			return "Float64";
		case DataType::String5:
			return "String5";
		case DataType::String8:
			return "String8";
		case DataType::String16:
			return "String16";
		case DataType::String32:
			return "String32";
		case DataType::Binary8:
			return "Binary8";
		case DataType::Binary16:
			return "Binary16";
		case DataType::Binary32:
			return "Binary32";
		case DataType::Extended:
			return "Extended";
		case DataType::FixedExtended:
			return "FixedExtended";
		default:
			return "Unknown";
		}
	}

	//----------
	bool isInt(const DataType& dataType)
	{
		return dataType == DataType::UInt7
			|| dataType == DataType::UInt8
			|| dataType == DataType::UInt16
			|| dataType == DataType::UInt32
			|| dataType == DataType::Int5
			|| dataType == DataType::Int8
			|| dataType == DataType::Int16
			|| dataType == DataType::Int32;
	}
}