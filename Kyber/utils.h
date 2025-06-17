#pragma once
#include <vector>

inline std::vector<uint8_t> bytesToBits(const uint8_t* b, int bLen) {
    std::vector<uint8_t> result(bLen * 8);
    for (size_t i = 0; i < bLen; i++) {
        for (int8_t j = 0; j < 8; j++) {
            result[8 * i + j] = (b[i] >> j) & 0x1;
        }
    }

    return result;
}

inline std::vector<uint8_t> bitsToBytes(uint8_t* b, int bLen) {
    std::vector<uint8_t> result(bLen / 8);

    for (int i = 0; i < bLen; i++) {
        result[i / 8] = result[i / 8] + b[i] * (1 << (i % 8));
    }

    return result;
}

