#pragma once

#include "PolyRing.h"

struct KyberContext {
	PolyRing& ring;
	int k;
	int nu1;
	int nu2;
	int du;
	int dv;
};
