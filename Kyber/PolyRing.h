#pragma once
#include <cmath>
#include <stdexcept>
#include "MathUtils.h"

class PolyRing
{
private:
	int q; // Coeffs modulo
	int n; // defines poly modulo X^n + 1
	int zeta; // first nth root of unity
	int invHalfN; // (N/2)^(-1) in q ring
	int invN; // N^(-1) in q ring
	int qBitLength; // bit length of q

	bool isPrime(int number);
	void findFirstPrimitiveRootOfUnity();
	int gcdExtended(int a, int b, int* x, int* y);
	void findInvHalfN();

public:
	PolyRing(int q, int n);
	void print(const char* prefix = "");
	int getQ();
	int getQBitLength();
	int getN();
	int getZeta();
	int getInvHalfN();
	int getInvN();
	friend bool operator== (const PolyRing& left, const PolyRing& right);
};

