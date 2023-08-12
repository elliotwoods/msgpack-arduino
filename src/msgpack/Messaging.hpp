#pragma once

#include "constants.h"
#include "Platform.hpp"

namespace msgpack {
	class Serializer;
	class Messaging {
	public:
		virtual const char * getTypeName() const = 0;
	
		virtual void reportStatus(Serializer &) = 0;

		// If you do not override this, then you need to override processIncomingByKey below
		virtual bool processIncoming(Stream &);
		bool matchesKey(const char * key);
	protected:
		virtual bool processIncomingByKey(const char * key, Stream &);
	};
}
