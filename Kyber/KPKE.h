#pragma once

#include "Params.h"
#include <cstdint>
#include "utils.h"
#include "openssl/evp.h"
#include <span>
#include <iterator>
#include "KyberContext.h"
#include "PolyMatrix.h"
#include "PolyCoder.h"

class KPKE
{
private:
	const KyberContext& context;
	PolyCoder coder;

	void sampleNTT(PolyMatrix& A, uint8_t rho[32], int8_t i, int8_t j);
	Poly samplePolyCBD(int nu, uint8_t* b);

public:
	KPKE(const KyberContext& context) : context(context), coder(PolyCoder()) {}

	std::vector<uint8_t> keyGen(std::vector<uint8_t>& d);
	std::vector<uint8_t> encrypt(std::vector<uint8_t>& m, std::vector<uint8_t>& r, std::vector<uint8_t>& ekPke);
	std::vector<uint8_t> decrypt(std::vector<uint8_t>& c, std::vector<uint8_t>& dkPke);
};

