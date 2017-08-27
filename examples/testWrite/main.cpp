#include <Arduino.h>
#include "msgpack.hpp"
#include "../TestData.h"

TestData testData;
void setup() {
    Serial.begin(115200);

    msgpack::writeMapSize(stream, 1);
    msgpack::writeString(stream, "TestData");
    {
        msgpack::writeMapSize(5);
        msgpack::writeString(stream, "Ints");
        {
            msgpack::writeMapSize(13);
            msgpack::writeIntU8(stream, testData.Ints.zero);
            msgpack::writeIntU8(stream, testData.Ints.one);
            msgpack::writeIntU7(stream, testData.Ints.seven_bit);
            msgpack::writeIntU8(stream, testData.Ints.eight_bit);
            msgpack::writeIntU16(stream, testData.Ints.sixteen_bit);
            msgpack::writeIntU32(stream, testData.Ints.thirty_two_bit);
            msgpack::writeIntU64(stream, testData.Ints.sixty_four_bit);
            msgpack::writeInt8(stream, testData.Ints.minus_one);
            msgpack::writeInt5(stream, testData.Ints.minus_six_bit);
            msgpack::writeInt8(stream, testData.Ints.minus_eight_bit);
            msgpack::writeInt16(stream, testData.Ints.minus_sixteen_bit);
            msgpack::writeInt32(stream, testData.Ints.minus_thirty_two_bit);
            msgpack::writeInt64(stream, testData.Ints.minus_sixty_four_bit);
        }

        msgpack::writeString(stream, "Floats");
        {
            msgpack::writeMapSize(2);
            msgpack::writeFloat32(stream, testData.Floats.thirty_two_bit);
            msgpack::writeFloat64(stream, testData.Floats.sixty_four_bit);
        }

        msgpack::writeString(stream, "Bools");
        {
            msgpack::writeMapSize(2);
            msgpack::writeBool(stream, testData.Bools.negative);
            msgpack::writeBool(stream, testData.Bools.positive);
        }
        
        msgpack::writeString(stream, "Strings");
        {
            msgpack::writeMapSize(4);
            msgpack::writeString(stream, testData.Strings.empty);
            msgpack::writeString(stream, testData.Strings.five_bit);
            msgpack::writeString(stream, testData.Strings.eight_bit);
            msgpack::writeString(stream, testData.Strings.sixteen_bit);
        }

        msgpack::writeString(stream, "Array");
        {
            msgpack::writeArraySize(9);
            for(int i=0; i<9; i++) {
                msgpack::writeIntU8(testData.Array[i]);
            }
        }
    }
}



void loop() {
}