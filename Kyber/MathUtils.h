#pragma once

#include <openssl/evp.h>
#include <cmath>
#include "Params.h"

inline void shake256_hash(uint8_t* input, size_t input_len, uint8_t* output, size_t output_len) {
    for (int i = 0; i < output_len; i++) {
        output[i] = 0;
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_shake256();

    EVP_DigestInit_ex(mdctx, md, nullptr);
    EVP_DigestUpdate(mdctx, input, input_len);
    EVP_DigestFinalXOF(mdctx, output, output_len);

    EVP_MD_CTX_free(mdctx);
}

inline int mod(int x, int q) {
    return (x % q + q) % q;
}

inline int modExp(int base, int exp, int modulo) {
    int result = 1;
    base = mod(base, modulo);
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = mod((result * base), modulo);
        }
        base = mod((base * base), modulo);
        exp >>= 1;
    }

    return result;
}

// Сжимает значение x ∈ [0, Q) до d бит
inline uint16_t compressX(uint16_t x, int d, int q) {
    // Умножаем x на 2^d, прибавляем Q/2 (для округления вверх), делим на Q и берём по модулю 2^d
    return static_cast<uint16_t>(mod(((static_cast<uint32_t>(x) << d) + q / 2) / q, (1 << d)));
}

// Восстанавливает приближённое значение из d-битного y
inline uint16_t decompressX(uint16_t y, int d, int q) {
    // Умножаем y на Q, прибавляем 2^{d-1} (для округления вверх), делим на 2^d
    return static_cast<uint16_t>((q * static_cast<uint32_t>(y) + (1 << (d - 1))) >> d);
}

inline int reverse(int x, int bits) {
    int result = 0;
    for (int i = bits - 1; i >= 0; i--) {
        result |= ((x >> i) & 0x1) << (bits - 1 - i);
    }
    return result;
}
