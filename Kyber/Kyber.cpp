// Kyber.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "KPKE.h"
#include <random>
#include <iomanip>

int main()
{
    PolyRing kyberRing(3329, 256);

    kyberRing.print("CURRENT KYBER RING");

    KyberContext kyberContext = { kyberRing, 2, 3, 2, 10, 4 };

    KPKE k(kyberContext);

    int8_t d[32];

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < 32; i++) {
        d[i] = gen();
    }

    k.keyGen(d);

    std::vector<uint8_t> m(32);
    for (int i = 0; i < 32; i++) {
        m[i] = i;
    }

    printf("\n\nPLAINT TEXT\n");
    for (int i = 0; i < 32; i++) {
        printf("%5d", m[i]);
    }
    printf("\n\n");

    std::vector<uint8_t> r(32);
    for (int i = 0; i < 32; i++) {
        r[i] = static_cast<uint8_t>(gen());
    }
    
    std::vector<uint8_t> c = k.encrypt(m, r);
    printf("\n\nK-PKE CIPHER TEXT\n");
    for (int i = 0; i < kyberContext.du * kyberContext.k + kyberContext.dv; i++) {
        printf("%5d", c[i]);
    }
    printf("\n\n");

    std::vector<uint8_t> decrypred = k.decrypt(c);
    printf("\n\nK-PKE DECRYPTED TEXT\n");
    for (int i = 0; i < 32; i++) {
        printf("%5d", decrypred[i]);
    }
    printf("\n\n");
}

