#pragma once

#include "PolyRing.h"

struct KyberContext {
	PolyRing& ring;
	int k;
	int nu1;
	int nu2;
	int du;
	int dv;

	void print() {
		printf("CURRENT KYBER CONTEXT:\n");
		ring.print();
		printf("k = %d\nnu1 = %d\nnu2 = %d\ndu = %d\ndv = %d\n\n", k, nu1, nu2, du, dv);
	}
};
