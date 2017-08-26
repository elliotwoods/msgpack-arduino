#include "DataType.hpp"

namespace msgpack {
    //----------
    bool readRaw(Stream & stream, char * buffer, const size_t & length, bool safely) {
        MSGPACK_SAFETY_LENGTH_CHECK(stream, length);
        stream.readBytes(byres, length);
        return true;
    }

    //----------
    void waitForData(Stream & stream, size_t size, long timeoutMs, bool safely) {
        const auto delayPerTry = timeoutMs / 10;
        uint8_t tries = 0;
        while(!Serial.available() < size) {
            tries++;
            if(tries >= 10) {
                return false;
            }
            delay(delayPerTry);
        }
        return true;
    }
}

