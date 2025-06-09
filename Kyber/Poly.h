#pragma once

#include <cstdint>
#include "Params.h"
#include "SHAKE128.h"
#include "string.h"
#include <vector>
#include "utils.h"
#include "MathUtils.h"

class Poly
{
public:
	std::vector<uint16_t> coeffs;

	Poly();

	static void sampleNTT(Poly A[KYBER_K][KYBER_K], uint8_t rho[32], int8_t i, int8_t j);
	static Poly samplePolyCBD(int nu, uint8_t* B);

	void ntt();
	void invntt();

	static Poly multiplyNTT(Poly& f, Poly& g);
	static Poly add(Poly& f, Poly& g);
	static Poly sub(Poly& f, Poly& g);

	std::vector<uint8_t> byteEncodePoly();
	void byteDecodePoly(std::vector<uint8_t>& bytes);
	static std::vector<uint8_t> byteEncode(Poly vec[KYBER_K], int d);
	static void byteDecode(Poly vec[KYBER_K], std::vector<uint8_t>& bytes, int d);

	std::vector<uint8_t> byteEncodePoly(int d);
	void byteDecodePoly(std::vector<uint8_t>& bytes, int d);

	void compress(int d);
	void decompress(int d);

	void print(const char* msg);
};

