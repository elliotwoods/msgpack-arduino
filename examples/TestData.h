#pragma once
struct TestData {
    struct Ints {
        uint8_t zero = 0;
        uint8_t one = 1;
        uint8_t seven_bit = (1 << 7) - 1;
        uint8_t eight_bit = UINT8_MAX;
        uint16_t sixteen_bit = UINT16_MAX;
        uint32_t thirty_two_bit = UINT32_MAX;
        uint64_t sixty_four_bit = UINT64_MAX;

        uint8_t minus_one = -1;
        uint8_t minus_six_bit = - (1 << 5);
        uint8_t minus_eight_bit = INT8_MIN;
        uint16_t minus_sixteen_bit = INT16_min;
        uint32_t minus_thirty_two_bit = - INT32_MIN;
        uint64_t minus_sixty_four_bit = INT64_MIN;
    };

    struct Floats {
        float thirty_two_bit = FLT_MAX;
        double sixty_four_bit = DBL_MAX;
    }

    struct Bools {
        bool negative = false;
        bool positive = true;
    }

    struct Strings {
        const char empty[1] = "\0";
        const char five_bit[32] = "1234567890123\0";
        const char eight_bit[256] = "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345\0";
        const char sixteen_bit[257] = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678900123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456\0";
    }
    
    uint8_t Array = {0, 1, 2, 3, 4, 5, 6, 7, 8};
}