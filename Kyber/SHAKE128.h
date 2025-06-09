#pragma once

#include <openssl/evp.h>


class SHAKE128
{
private:
    EVP_MD_CTX* ctx;

public:
    SHAKE128() {
        ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_shake128(), nullptr);
    }

    ~SHAKE128() {
        EVP_MD_CTX_free(ctx);
    }

    void absorb(const uint8_t* input, size_t input_len) {
        EVP_DigestUpdate(ctx, input, input_len);
    }

    void squeeze(uint8_t* output, size_t output_len) {
        EVP_DigestSqueeze(ctx, output, output_len);
    }

    void reset() {
        EVP_DigestInit_ex(ctx, EVP_shake128(), nullptr);
    }
};

