#pragma once
#include "PolyVector.h"
#include "PolyRing.h"

class PolyCoder
{
public:
	PolyCoder() {};
	std::vector<uint8_t> byteEncode(PolyVector& v, int d);
	PolyVector byteDecode(std::vector<uint8_t>& bytes, PolyRing& ring, int k, int d);
};

