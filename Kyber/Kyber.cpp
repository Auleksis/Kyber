#include <iostream>
#include "MLKEM.h"
#include <random>
#include <iomanip>
#include <chrono>

int test(KyberContext& kyberContext, std::vector<uint8_t>& m, std::vector<uint8_t>& d, std::vector<uint8_t>& z) {
    auto start_time = std::chrono::high_resolution_clock::now();

    int qBitLen = kyberContext.ring.getQBitLength();
    
    MLKEM mlkem(kyberContext);
    std::vector<uint8_t> keys1 = mlkem.keyGen(d, z);
    std::vector<uint8_t> ek1(keys1.begin(), keys1.begin() + 32 * qBitLen * kyberContext.k + 32);
    std::vector<uint8_t> dk1(keys1.begin() + 32 * qBitLen * kyberContext.k + 32, keys1.end());

    std::vector<uint8_t> encapsResult = mlkem.encaps(ek1, m);
    std::vector<uint8_t> K(encapsResult.begin(), encapsResult.begin() + 32);
    std::vector<uint8_t> c(encapsResult.begin() + 32, encapsResult.end());

    std::vector<uint8_t> decapsedK = mlkem.decaps(dk1, c);

    for (int i = 0; i < K.size(); i++) {
        if (K[i] != decapsedK[i]) {
            return -1;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = duration_cast<std::chrono::microseconds>(end_time - start_time);

    return duration.count();
}

std::vector<uint8_t> nextByte(KyberContext& kyberContext, std::vector<uint8_t>& m, std::vector<uint8_t>& d, std::vector<uint8_t>& z) {
    int qBitLen = kyberContext.ring.getQBitLength();

    MLKEM mlkem(kyberContext);
    std::vector<uint8_t> keys1 = mlkem.keyGen(d, z);
    std::vector<uint8_t> ek1(keys1.begin(), keys1.begin() + 32 * qBitLen * kyberContext.k + 32);
    std::vector<uint8_t> dk1(keys1.begin() + 32 * qBitLen * kyberContext.k + 32, keys1.end());

    std::vector<uint8_t> encapsResult = mlkem.encaps(ek1, m);
    std::vector<uint8_t> result(encapsResult.begin(), encapsResult.begin() + 64);
    return result;
}

void computeStats(std::vector<int>& stats, std::vector<uint8_t>& v) {
    int t = 0;
    for (int i = 0; i < v.size(); i++) {
        t = v[i];
        for(int j = 0; j < 8; j++) {
            int r = t & 1;
            t >>= 1;
            if (r) {
                stats[1]++;
            }
            else {
                stats[0]++;
            }
        }
    }
}

void printVector(std::vector<uint8_t>& v) {
    for (int i = 0; i < v.size(); i++) {
        printf("%5d", v[i]);
    }
    printf("\n");
}

void printVector(std::vector<int>& v) {
    for (int i = 0; i < v.size(); i++) {
        printf("%20d", v[i]);
    }
    printf("\n");
}

int main()
{
    std::vector<int> stats(2, 0);

    PolyRing kyberRing(3329, 256);

    KyberContext kyberContext = { kyberRing, 4, 2, 2, 11, 5 };

    kyberContext.print();

    std::vector<uint8_t> seed(32, 0);
    seed[0] = 2;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 10000; i++) {

        std::vector<uint8_t> extendedSeed(64);

        if (!EVP_Digest(seed.data(), seed.size(), extendedSeed.data(), nullptr, EVP_sha3_512(), nullptr)) {
            printf("Error while extending seed\n");
            return -1;
        }

        std::vector<uint8_t> d(32);
        memcpy(d.data(), extendedSeed.data(), 32);
        std::vector<uint8_t> z(32);
        memcpy(z.data(), &extendedSeed[32], 32);

        std::vector<uint8_t> encapsResult = nextByte(kyberContext, seed, d, z);
        std::vector<uint8_t> K(encapsResult.begin(), encapsResult.begin() + 32);
        std::vector<uint8_t> c(encapsResult.begin() + 32, encapsResult.end());

        seed = K;

        //printVector(K);
        //std::vector<uint8_t> cStats(c.begin(), c.begin() + 1);

        //computeStats(stats, cStats);
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = duration_cast<std::chrono::seconds>(end_time - start_time);

    //printVector(stats);
    printf("%d seconds", duration.count());

    return 0;
}
