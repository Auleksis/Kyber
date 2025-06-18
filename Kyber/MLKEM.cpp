#include "MLKEM.h"

std::vector<uint8_t> MLKEM::keyGenInternal(std::vector<uint8_t>& d, std::vector<uint8_t>& z)
{
	std::vector<uint8_t> keys = kpke.keyGen(d);

	int byteLen = 32 * context.ring.getQBitLength();

	std::vector<uint8_t> ekPke(keys.begin(), keys.begin() + byteLen * context.k + 32);
	std::vector<uint8_t> dkPke(keys.begin() + byteLen * context.k + 32, keys.begin() + 2 * byteLen * context.k + 32);

	std::vector<uint8_t> ek;
	ek.reserve(byteLen * context.k + 32);
	std::vector<uint8_t> dk;
	dk.reserve(2 * byteLen * context.k + 96);

	ek = ekPke;
	dk.insert(dk.end(), dkPke.begin(), dkPke.end());
	dk.insert(dk.end(), ek.begin(), ek.end());

	std::vector<uint8_t> hHash(32);
	std::vector<uint8_t> hHashInput = ek;

	if (!EVP_Digest(hHashInput.data(), hHashInput.size(), hHash.data(), nullptr, EVP_sha3_256(), nullptr)) {
		printf("Error while H hash calculation in ML-KEM Internal KeyGen\n");
		return {};
	}

	dk.insert(dk.end(), hHash.begin(), hHash.end());
	dk.insert(dk.end(), z.begin(), z.end());

	std::vector<uint8_t> outputKeys;
	outputKeys.reserve(ek.size() + dk.size());
	outputKeys.insert(outputKeys.end(), ek.begin(), ek.end());
	outputKeys.insert(outputKeys.end(), dk.begin(), dk.end());

	return outputKeys;
}

std::vector<uint8_t> MLKEM::encapsInternal(std::vector<uint8_t>& ek, std::vector<uint8_t>& m)
{
	std::vector<uint8_t> hHash(32);
	std::vector<uint8_t> hHashInput = ek;

	if (!EVP_Digest(hHashInput.data(), hHashInput.size(), hHash.data(), nullptr, EVP_sha3_256(), nullptr)) {
		printf("Error while H hash calculation in ML-KEM Internal KeyGen\n");
		return {};
	}

	std::vector<uint8_t> gHash(64);
	std::vector<uint8_t> gHashInput;
	gHashInput.reserve(64);
	gHashInput.insert(gHashInput.end(), m.begin(), m.end());
	gHashInput.insert(gHashInput.end(), hHash.begin(), hHash.end());

	if (!EVP_Digest(gHashInput.data(), gHashInput.size(), gHash.data(), nullptr, EVP_sha3_512(), nullptr)) {
		printf("Error while G hash calculation in K-PKE KeyGen\n");
		return {};
	}

	std::vector<uint8_t> K(gHash.begin(), gHash.begin() + 32);
	std::vector<uint8_t> r(gHash.begin() + 32, gHash.end());

	std::vector<uint8_t> c = kpke.encrypt(m, r, ek);

	std::vector<uint8_t> output;
	output.reserve((K.size() + c.size()));
	output.insert(output.end(), K.begin(), K.end());
	output.insert(output.end(), c.begin(), c.end());

	return output;
}

std::vector<uint8_t> MLKEM::decapsInternal(std::vector<uint8_t>& dk, std::vector<uint8_t>& c)
{
	int byteLen = 32 * context.ring.getQBitLength();

	std::vector<uint8_t> dkPke(dk.begin(), dk.begin() + byteLen * context.k);
	std::vector<uint8_t> ekPke(dk.begin() + byteLen * context.k, dk.begin() + 2 * byteLen * context.k + 32);
	std::vector<uint8_t> h(dk.begin() + 2 * byteLen * context.k + 32, dk.begin() + 2 * byteLen * context.k + 64);
	std::vector<uint8_t> z(dk.begin() + 2 * byteLen * context.k + 64, dk.end());

	std::vector<uint8_t> m = kpke.decrypt(c, dkPke);

	std::vector<uint8_t> gHash(64);
	std::vector<uint8_t> gHashInput;
	gHashInput.reserve(64);
	gHashInput.insert(gHashInput.end(), m.begin(), m.end());
	gHashInput.insert(gHashInput.end(), h.begin(), h.end());

	if (!EVP_Digest(gHashInput.data(), gHashInput.size(), gHash.data(), nullptr, EVP_sha3_512(), nullptr)) {
		printf("Error while G hash calculation in K-PKE KeyGen\n");
		return {};
	}

	std::vector<uint8_t> K(gHash.begin(), gHash.begin() + 32);
	std::vector<uint8_t> r(gHash.begin() + 32, gHash.end());

	std::vector<uint8_t> jK(32);
	std::vector<uint8_t> jHashInput;
	jHashInput.reserve((z.size() + c.size()));
	jHashInput.insert(jHashInput.end(), z.begin(), z.end());
	jHashInput.insert(jHashInput.end(), c.begin(), c.end());
	shake256_hash(jHashInput.data(), jHashInput.size(), jK.data(), 32);

	std::vector<uint8_t> newC = kpke.encrypt(m, r, ekPke);

	if (newC.size() != c.size()) {
		return jK;
	}

	for (int i = 0; i < c.size(); i++) {
		if (c[i] != newC[i]) {
			return jK;
		}
	}

	return K;
}

MLKEM::MLKEM(const KyberContext& context): context(context), kpke(context)
{
	
}

std::vector<uint8_t> MLKEM::keyGen(std::vector<uint8_t>& d, std::vector<uint8_t>& z)
{
	return keyGenInternal(d, z);
}

std::vector<uint8_t> MLKEM::encaps(std::vector<uint8_t>& ek, std::vector<uint8_t>& m)
{
	return encapsInternal(ek, m);
}

std::vector<uint8_t> MLKEM::decaps(std::vector<uint8_t>& dk, std::vector<uint8_t>& c)
{
	return decapsInternal(dk, c);
}
