#include "constants.h"
#include "Serializer.hpp"

namespace msgpack {
	class ErrorSerializer : public Serializer
	{
	public:
		ErrorSerializer(Print &);
		~ErrorSerializer();
	};

	void logError(Print &, const char * functionName, const char * errorMessage);
	ErrorSerializer logErrorWithData(Print &, const char * functionName, const char * errorMessage);
}