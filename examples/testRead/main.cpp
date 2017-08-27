#include <Arduino.h>
#include "msgpack.hpp"
#include "../TestData.h"

void setup() {
    Serial.begin(115200);
}

TestData readData;
TestData correctValues;

#define CHECK_STRING(A, B) (strcmp(A, B) == -1)

bool checkData(Stream & stream) {
    //zero out the data before checking
    memset(& readData, sizeof(readData));

    size_t mapSize
    MSGPACK_SAFELY_RUN(msgpack::readMapSize(stream, mapSize, true));
    MSGPACK_SAFETY_CHECK(mapSize == 1);

    checkRoot(stream);
}

void checkRoot(Stream & stream) {
    {
        char key[100]
        size_t keyLength;
        MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
        MSGPACK_SAFETY_CHECK(CHECK_STRING(key, "root"));
    }

    {
        size_t mapSize;
        MSGPACK_SAFELY_RUN(msgpack::readMapSize(stream, mapSize, true));
        MSGPACK_SAFETY_CHECK(mapSize == 4);
    }

    MSGPACK_SAFETY_CHECK(checkInts(stream));
    MSGPACK_SAFETY_CHECK(checkFloats(stream));
    MSGPACK_SAFETY_CHECK(checkBools(stream));
    MSGPACK_SAFETY_CHECK(checkStrings(stream));
    MSGPACK_SAFETY_CHECK(checkArray(stream));
}

void checkInts(Stream & stream) {
    {
        char key[100];
        size_t keyLength;
        MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
        MSGPACK_SAFETY_CHECK(CHECK_STRING(key, "Ints"));
    }

    {
        size_t size;
        MSGPACK_SAFELY_RUN(msgpack::readMapSize(stream, size, true));
        MSGPACK_SAFETY_CHECK(size == 13);
    }

    {
        char key[100];
        size_t keyLength;
        {
            {
                uint8_t data;

                //zero
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readIntU8(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == 0);

                //one
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readIntU8(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == 1);

                //seven_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readIntU7(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.seven_bit);

                //eight_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readIntU8(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.eight_bit);
            }

            {
                uint16_t data;

                //sixteen_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readIntU16(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.sixteen_bit);
            }

            {
                uint32_t data;

                //thirty_two_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readIntU32(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.thirty_two_bit);
            }

            {
                uint64_t data;

                //sixty_four_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readIntU32(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.sixty_four_bit);
            }

            {
                int8_t data;

                //minus_one
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readInt8(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.minus_one);

                //minus_six_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readInt5(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.minus_six_bit);

                //minus_eight_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readInt8(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.minus_eight_bit);
            }

            {
                int16_t data;

                //minus_sixteen_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readInt16(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.minus_sixteen_bit);
            }

            {
                int32_t data;

                //minus_thirty_two_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readInt32(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.minus_thirty_two_bit);
            }

            {
                int64_t data;

                //minus_thirty_two_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readInt64(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Ints.minus_sixty_four_bit);
            }
        }
    }

    return true;
}

void checkFloats() {
    {
        char key[100];
        size_t keyLength;
        MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
        MSGPACK_SAFETY_CHECK(CHECK_STRING(key, "Floats"));
    }

    {
        size_t size;
        MSGPACK_SAFELY_RUN(msgpack::readMapSize(stream, size, true));
        MSGPACK_SAFETY_CHECK(size == 2);
    }

    {
        char key[100];
        size_t keyLength;
        {
            {
                float data;

                //thirty_two_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readFloat32(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Floats.thirty_two_bit);
            }

            {
                double data;

                //sixty_four_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readFloat64(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Floats.sixty_four_bit);
            }
        }
    }
    
    return true;
}

void checkBools() {
    {
        char key[100];
        MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
        MSGPACK_SAFETY_CHECK(CHECK_STRING(key, "Bools"));
    }

    {
        size_t size;
        MSGPACK_SAFELY_RUN(msgpack::readMapSize(stream, size, true));
        MSGPACK_SAFETY_CHECK(size == 2);
    }

    {
        char key[100];
        size_t keyLength;
        {
            {
                bool data;

                //negative
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readBool(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Bools.negative);
            }

            {
                bool data;

                //positive
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
                MSGPACK_SAFELY_RUN(msgpack::readBool(stream, data, true));
                MSGPACK_SAFETY_CHECK(data == checkData.Bools.positive);
            }
        }
    }

    return true;
}

void checkStrings() {
    {
        char key[100];
        MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
        MSGPACK_SAFETY_CHECK(CHECK_STRING(key, "Strings"));
    }

    {
        size_t size;
        MSGPACK_SAFELY_RUN(msgpack::readMapSize(stream, size, true));
        MSGPACK_SAFETY_CHECK(size == 4);
    }

    {
        char key[100];
        size_t keyLength;
        {
            {
                char data[257];
                size_t dataLength

                //empty
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), keyLength, true));
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, data, sizeof(data), dataLength, true));
                MSGPACK_SAFETY_CHECK(CHECKSTRING(data, checkData.Strings.empty));

                //five_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), keyLength, true));
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, data, sizeof(data), dataLength, true));
                MSGPACK_SAFETY_CHECK(CHECKSTRING(data, checkData.Strings.five_bit);

                //eight_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), keyLength, true));
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, data, sizeof(data), dataLength, true));
                MSGPACK_SAFETY_CHECK(CHECKSTRING(data, checkData.Strings.eight_bit);

                //sixteen_bit
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), keyLength, true));
                MSGPACK_SAFELY_RUN(msgpack::readString(stream, data, sizeof(data), dataLength, true));
                MSGPACK_SAFETY_CHECK(CHECKSTRING(data, checkData.Strings.sixteen_bit);
            }
        }
    }

    return true;
}

bool checkArray() {
    {
        char key[100];
        MSGPACK_SAFELY_RUN(msgpack::readString(stream, key, keyLength, sizeof(key), true));
        MSGPACK_SAFETY_CHECK(CHECK_STRING(key, "Array"));
    }

    {
        size_t size;
        MSGPACK_SAFELY_RUN(msgpack::readArraySize(stream, size, true));
        MSGPACK_SAFETY_CHECK(size == 8);
    }

    for(int i=0; i<9; i++) {
        int value;
        MSGPACK_SAFELY_RUN(msgpack::readInt(stream, value, true));
    }
}

void loop() {
    // ../exampleData.json
    while(!Serial.available()) {
        Serial.print('.');
    }
    Serial.println();

    checkData(Serial);
}