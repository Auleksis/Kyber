// Kyber.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

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

int main()
{
    std::vector<uint8_t> m(32);
    for (int i = 0; i < 32; i++) {
        m[i] = i;
    }

    printf("\n\nPLAINT TEXT\n");
    for (int i = 0; i < 32; i++) {
        printf("%5d", m[i]);
    }
    printf("\n\n");

    PolyRing kyberRing(3329, 256);

    kyberRing.print("CURRENT KYBER RING");

    KyberContext kyberContext = { kyberRing, 2, 2, 2, 10, 4 };

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<uint8_t> d(32);
    std::vector<uint8_t> z(32);

    for (int i = 0; i < 32; i++) {
        d[i] = gen();
        z[i] = gen();
    }

    int testCount = 10000;
    int allTime = 0;

    int temp = 0;
    for (int i = 0; i < testCount; i++) {
        temp = test(kyberContext, m, d, z);
        if (temp == -1) {
            printf("Kyber Decaps Failure\n");
            return -1;
        }

        allTime += temp;
    }

    float meanTime = (float)allTime / testCount;
    printf("Mean working time: %f microseconds\n", meanTime);

    return 0;
}

