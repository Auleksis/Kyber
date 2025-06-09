// Kyber.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "KPKE.h"
#include <random>
#include <iomanip>


void test_compression(unsigned d) {
    std::cout << "Testing Compress/Decompress with d = " << d << ":\n";
    std::cout << " x   | Compressed | Decompressed\n";
    std::cout << "-----+------------+-------------\n";

    for (uint16_t x = 0; x <= KYBER_Q; x += KYBER_Q / 8) {  // берём 9 равномерно распределённых значений от 0 до Q
        uint8_t compressed = compressX(x, d);
        uint16_t decompressed = decompressX(compressed, d);

        std::cout << std::setw(4) << x << " | "
            << std::setw(10) << static_cast<int>(compressed) << " | "
            << std::setw(11) << decompressed << '\n';
    }

    std::cout << '\n';
}

int main()
{
    /*for (unsigned d = 3; d <= 6; ++d) {
        test_compression(d);
    }
    printf("\n\n");*/
    KPKE k;
    int8_t d[32];

    /*for (int i = 0; i < KYBER_N / 2; i++) {
        int zeta = modExp(ZETA, (reverse(i, LOG2N - 1) << 1) ^ 1, KYBER_Q);
        printf("%d\n", zeta);
    }*/

    std::random_device rd;

    /*Poly test;
    for (int i = 0; i < KYBER_N; i++) {
        test.coeffs[i] = rd() % KYBER_Q;
    }
    test.print("TEST");
    test.ntt();
    test.print("NTT TEST");
    test.invntt();
    test.print("INVNTT NTT TEST");

    for (int i = 0; i < 32; i++) {
        d[i] = static_cast<uint8_t>(1);
    }*/

    /*k.keyGen(d);*/

    std::vector<uint8_t> m(32);
    for (int i = 0; i < 32; i++) {
        m[i] = i;
    }

    /*uint16_t a = 416;
    uint16_t compressedA = compressX(a, 3);
    uint16_t decompressedA = decompressX(compressedA, 3);
    printf("\n\nCOMPRESS CHECK\n%d\n", compressedA);
    printf("DECOMPRESS CHECK\n%d\n\n\n", decompressedA);*/

    /*printf("\n\nPLAINT TEXT\n");
    for (int i = 0; i < 32; i++) {
        printf("%5d", m[i]);
    }
    printf("\n\n");

    std::vector<uint8_t> r(32);
    for (int i = 0; i < 32; i++) {
        r[i] = static_cast<uint8_t>(1);
    }
    
    std::vector<uint8_t> c = k.encrypt(m, r);
    printf("\n\nK-PKE CIPHER TEXT\n");
    for (int i = 0; i < KYBER_DU * KYBER_K + KYBER_DV; i++) {
        printf("%5d", c[i]);
    }
    printf("\n\n");

    std::vector<uint8_t> decrypred = k.decrypt(c);
    printf("\n\nK-PKE DECRYPTED TEXT\n");
    for (int i = 0; i < 32; i++) {
        printf("%5d", decrypred[i]);
    }
    printf("\n\n");*/

    Poly a;
    a.coeffs = { 99, 52, 0, 7, 0, 29, 33, 100 };
    Poly b;
    b.coeffs = { 6, 0, 45, 12, 14, 78, 65, 112 };

    a.ntt();
    b.ntt();

    a.print("NTT OF POLY A");
    b.print("NTT OF POLY B");

    Poly c = Poly::multiplyNTT(a, b);
    c.invntt();
    c.print("POLY C");
}

