#include "Messaging.hpp"
#include "deserialize.hpp"

#include "serialize.hpp"
#include "Serializer.hpp"
#include "logError.hpp"

#include <stdio.h>
#include <string.h>

namespace msgpack {
	//----------
	bool Messaging::processIncoming(Stream & stream) {
		const bool safely = true;

#ifdef MSGPACK_DEBUG_INCOMING
		msgpack::writeMapSize4(stream, 1);
		{
			msgpack::writeString(stream, "processIncoming for");
			{
				msgpack::writeString(stream, this->getTypeName());
			}
		}
		stream.flush();
#endif

		size_t mapSize;
		MSGPACK_SAFELY_RUN(msgpack::readMapSize(stream, mapSize));

		
#ifdef MSGPACK_DEBUG_INCOMING
		msgpack::writeMapSize4(stream, 1);
		{
			msgpack::writeString(stream, "map size");
			{
				msgpack::writeInt(stream, (uint32_t) mapSize);
			}
		}
		stream.flush();
#endif

		char key[100];
		for(size_t i=0; i<mapSize; i++) {
			size_t stringSize;
			MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, 100, stringSize));

#ifdef MSGPACK_DEBUG_INCOMING
			msgpack::writeMapSize4(stream, 1);
			{
				msgpack::writeString(stream, "key");
				{
					msgpack::writeString(stream, key);
				}
			}
			stream.flush();
#endif

			if (!this->processIncomingByKey(key, stream)) {
				char data[150];
				sprintf(data, "Module [%s] : Key [%s]", this->getTypeName(), key);
				msgpack::logErrorWithData(stream, "Messaging::processIncoming", "couldn't deserialise with key") << data;
				return false;
			}
		}

		return true;
	}

	//----------
	bool Messaging::matchesKey(const char * key) {
		return strcmp(key, this->getTypeName()) == 0;
	}

	//----------
	bool Messaging::processIncomingByKey(const char* key, Stream&) {
		return false;
	}
}


