#include "Messaging.hpp"
#include "deserialize.hpp"

#include "serialize.hpp"
#include "Serializer.hpp"


namespace msgpack {
	//----------
	bool Messaging::processIncoming(Stream & stream) {
		const bool safely = true;

#ifdef MESSENGER_DEBUG_INCOMING
		msgpack::writeString(stream, "try rx map");
#endif

		size_t mapSize;
		MSGPACK_SAFELY_RUN(msgpack::readMapSize(stream, mapSize));

		
#ifdef MESSENGER_DEBUG_INCOMING
		msgpack::writeMapSize4(stream, 1);
		msgpack::writeString(stream, "map size");
		msgpack::writeInt(stream, mapSize);
#endif

		char key[100];
		for(size_t i=0; i<mapSize; i++) {
			size_t stringSize;
			MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, 100, stringSize));

#ifdef MESSENGER_DEBUG_INCOMING
			msgpack::writeMapSize4(stream, 1);
			msgpack::writeString(stream, "key");
			msgpack::writeString(stream, key);
#endif

			if (!this->processIncomingByKey(key, stream)) {
				return false;
			}
		}

		return true;
	}
}