// Kyber.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "MLKEM.h"
#include <random>
#include <iomanip>

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

    KyberContext kyberContext = { kyberRing, 2, 3, 2, 10, 4 };

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<uint8_t> d(32);
    std::vector<uint8_t> z(32);

    for (int i = 0; i < 32; i++) {
        d[i] = gen();
        z[i] = gen();
    }

    MLKEM mlkem(kyberContext);
    std::vector<uint8_t> keys1 = mlkem.keyGen(d, z);
    std::vector<uint8_t> ek1(keys1.begin(), keys1.begin() + 384 * kyberContext.k + 32);
    std::vector<uint8_t> dk1(keys1.begin() + 384 * kyberContext.k + 32, keys1.end());

    for (int i = 0; i < 32; i++) {
        d[i] = gen();
        z[i] = gen();
    }

    std::vector<uint8_t> keys2 = mlkem.keyGen(d, z);
    std::vector<uint8_t> ek2(keys2.begin(), keys2.begin() + 384 * kyberContext.k + 32);
    std::vector<uint8_t> dk2(keys2.begin() + 384 * kyberContext.k + 32, keys2.end());

    std::vector<uint8_t> encapsResult = mlkem.encaps(ek1, m);
    std::vector<uint8_t> K(encapsResult.begin(), encapsResult.begin() + 32);
    std::vector<uint8_t> c(encapsResult.begin() + 32, encapsResult.end());

    printf("ENCAPSED K\n");
    for (int i = 0; i < 32; i++) {
        printf("%5d", K[i]);
    }
    printf("\n\n");

    std::vector<uint8_t> decapsedK = mlkem.decaps(dk1, c);
    printf("DECAPSED K\n");
    for (int i = 0; i < 32; i++) {
        printf("%5d", decapsedK[i]);
    }
    printf("\n\n");

    return 0;
}

