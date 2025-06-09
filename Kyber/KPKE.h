#pragma once

#include "Params.h"
#include "Poly.h"
#include <cstdint>
#include "utils.h"
#include "openssl/evp.h"
#include <span>
#include <iterator>

class KPKE
{
private:
	std::vector<uint8_t> dkPke;

public:
	std::vector<uint8_t> ekPke;

	int keyGen(int8_t* d);
	std::vector<uint8_t> encrypt(std::vector<uint8_t>& m, std::vector<uint8_t>& r);
	std::vector<uint8_t> decrypt(std::vector<uint8_t>& c);
};

