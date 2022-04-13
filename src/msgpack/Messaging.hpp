#pragma once

#include "Platform.h"
#include "constants.h"

namespace msgpack {
	class Serializer;
	class Messaging {
	public:
		virtual const char * getTypeName() const = 0;
	
		virtual void reportStatus(Serializer &) = 0;
		bool processIncoming(Stream &);
		bool matchesKey(const char * key);
	protected:
		virtual bool processIncomingByKey(const char * key, Stream &) = 0;
	};
}
