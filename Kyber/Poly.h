#pragma once

#include <cstdint>
#include "Params.h"
#include "SHAKE128.h"
#include "string.h"
#include <vector>
#include "utils.h"
#include "MathUtils.h"
#include "PolyRing.h"

class Poly
{
private:
	PolyRing& ring;
	std::vector<uint16_t> coeffs;
public:
	Poly(PolyRing& ring);

	void setCoeffs(const std::vector<int>& coeffs);
	void ntt();
	void invntt();
	void print(const char* msg) const;
	
	PolyRing& getRing();

	friend const Poly operator* (const Poly& left, const Poly& right);
	friend const Poly operator+ (const Poly& left, const Poly& right);
	Poly& operator+= (const Poly& right);
	friend const Poly operator- (const Poly& left, const Poly& right);
	Poly& operator-= (const Poly& right);
	const Poly& operator= (const Poly& right);
	uint16_t& operator[] (int i);
	const uint16_t& operator[] (int i) const;
	friend bool operator== (const Poly& left, const Poly& right);

	void compress(int d);
	void decompress(int d);
};

