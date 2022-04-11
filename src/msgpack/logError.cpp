#include "logError.hpp"
#include "serialize.hpp"

namespace msgpack {
	//---------
	ErrorSerializer::ErrorSerializer(Print & stream)
	: Serializer(stream)
	{

	}

	//---------
	ErrorSerializer::~ErrorSerializer()
	{
		this->stream.flush();
	}

	//---------
	void
	logError(Print & stream, const char * functionName, const char * errorMessage)
	{
		writeMapSize4(stream, 1);
		{
			writeString5(stream, "error", 5);

			writeMapSize4(stream, 2);
			{
				writeString5(stream, "function", 8);
				writeString(stream, functionName);

				writeString5(stream, "message", 7);
				writeString(stream, errorMessage);
			}
			
		}

		stream.flush();
	}

	//---------
	ErrorSerializer
	logErrorWithData(Print & stream, const char * functionName, const char * errorMessage)
	{
		writeMapSize4(stream, 1);
		{
			writeString5(stream, "error", 5);

			writeMapSize4(stream, 3);
			{
				writeString5(stream, "function", 8);
				writeString(stream, functionName);

				writeString5(stream, "message", 7);
				writeString(stream, errorMessage);

				writeString5(stream, "data", 4);
				return ErrorSerializer(stream);
			}
			
		}
	}
}