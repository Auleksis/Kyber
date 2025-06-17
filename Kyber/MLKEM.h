#pragma once

#include "KPKE.h"

class MLKEM
{
private:
	const KyberContext& context;
	KPKE kpke;
	std::vector<uint8_t> keyGenInternal(std::vector<uint8_t>& d, std::vector<uint8_t>& z);
	std::vector<uint8_t> encapsInternal(std::vector<uint8_t>& ek, std::vector<uint8_t>& m);
	std::vector<uint8_t> decapsInternal(std::vector<uint8_t>& dk, std::vector<uint8_t>& c);

public:
	MLKEM(const KyberContext& context);

	std::vector<uint8_t> keyGen(std::vector<uint8_t>& d, std::vector<uint8_t>& z);
	std::vector<uint8_t> encaps(std::vector<uint8_t>& ek, std::vector<uint8_t>& m);
	std::vector<uint8_t> decaps(std::vector<uint8_t>& dk, std::vector<uint8_t>& c);
};

