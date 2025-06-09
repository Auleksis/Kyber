#include "KPKE.h"

int KPKE::keyGen(int8_t* d)
{
	uint8_t gHash[64];
	int8_t gHashInput[33];
	memcpy(gHashInput, d, 32);
	gHashInput[32] = KYBER_K;


	if (!EVP_Digest(gHashInput, 33, gHash, nullptr, EVP_sha3_512(), nullptr)) {
		printf("Error while G hash calculation in K-PKE KeyGen\n");
		return -1;
	}

	uint8_t rho[32];
	memcpy(rho, gHash, 32);
	uint8_t sigma[32];
	memcpy(sigma, &gHash[32], 32);

	uint8_t N = 0;

	Poly A[KYBER_K][KYBER_K];

	for (int8_t i = 0; i < KYBER_K; i++) {
		for (int8_t j = 0; j < KYBER_K; j++) {
			Poly::sampleNTT(A, rho, i, j);
		}
	}

	Poly s[KYBER_K];
	uint8_t prfShakeInput[33];
	memcpy(prfShakeInput, sigma, 32);

	uint8_t B[64 * KYBER_NU1];
	for (int8_t i = 0; i < KYBER_K; i++) {
		prfShakeInput[32] = N;
		shake256_hash(prfShakeInput, 33, B, 64 * (size_t)KYBER_NU1);
		s[i] = Poly::samplePolyCBD(KYBER_NU1, B);
		N++;
	}

	Poly e[KYBER_K];
	for (int8_t i = 0; i < KYBER_K; i++) {
		prfShakeInput[32] = N;
		shake256_hash(prfShakeInput, 33, B, 64 * (size_t)KYBER_NU1);
		e[i] = Poly::samplePolyCBD(KYBER_NU1, B);
		N++;
	}

	s[0].print("s[0]");

	for (int i = 0; i < KYBER_K; i++) {
		s[i].ntt();
		e[i].ntt();
	}

	A[0][0].invntt();
	A[0][0].print("A[0][0]");
	A[0][0].ntt();

	Poly ch = Poly::multiplyNTT(A[0][0], s[0]);
	ch.invntt();
	ch.print("A[0][0] * s[0]");

	Poly t[KYBER_K];
	for (int i = 0; i < KYBER_K; i++) {
		for (int j = 0; j < KYBER_K; j++) {
			Poly as = Poly::multiplyNTT(A[i][j], s[j]);
			t[i] = Poly::add(t[i], as);
		}

		t[i] = Poly::add(t[i], e[i]);
	}

	ekPke = Poly::byteEncode(t, 12);
	ekPke.insert(ekPke.end(), std::begin(rho), std::end(rho));

	dkPke = Poly::byteEncode(s, 12);

	return 0;
}

std::vector<uint8_t> KPKE::encrypt(std::vector<uint8_t>& m, std::vector<uint8_t>& r)
{
	Poly t[KYBER_K];
	Poly::byteDecode(t, ekPke, 12);

	uint8_t rho[32];
	memcpy(rho, ekPke.data() + KYBER_POLYBYTES * KYBER_K, 32);

	uint8_t N = 0;

	Poly A[KYBER_K][KYBER_K];

	for (int i = 0; i < KYBER_K; i++) {
		for (int j = 0; j < KYBER_K; j++) {
			Poly::sampleNTT(A, rho, i, j);
		}
	}

	Poly y[KYBER_K];
	uint8_t prfShakeInput[33];
	memcpy(prfShakeInput, r.data(), 32);

	uint8_t B[64 * KYBER_NU1];
	for (int i = 0; i < KYBER_K; i++) {
		prfShakeInput[32] = N;
		shake256_hash(prfShakeInput, 33, B, 64 * (size_t)KYBER_NU1);
		y[i] = Poly::samplePolyCBD(KYBER_NU1, B);
		N++;
	}

	Poly e1[KYBER_K];
	for (int i = 0; i < KYBER_K; i++) {
		prfShakeInput[32] = N;
		shake256_hash(prfShakeInput, 33, B, 64 * (size_t)KYBER_NU2);
		e1[i] = Poly::samplePolyCBD(KYBER_NU2, B);
		N++;
	}
	
	Poly e2;
	prfShakeInput[32] = N;
	shake256_hash(prfShakeInput, 33, B, 64 * (size_t)KYBER_NU2);
	e2 = Poly::samplePolyCBD(KYBER_NU2, B);

	for (int i = 0; i < KYBER_K; i++) {
		y[i].ntt();
	}

	Poly u[KYBER_K];
	for (int i = 0; i < KYBER_K; i++) {
		for (int j = 0; j < KYBER_K; j++) {
			// Матрица А транспонирована
			Poly ay = Poly::multiplyNTT(A[j][i], y[j]);
			u[i] = Poly::add(u[i], ay);
		}

		u[i].invntt();
		u[i] = Poly::add(u[i], e1[i]);
	}

	Poly mu;
	mu.byteDecodePoly(m, 1);

	Poly v;
	for (int i = 0; i < KYBER_K; i++) {
		Poly ty = Poly::multiplyNTT(t[i], y[i]);
		v = Poly::add(v, ty);
	}
	v.invntt();

	v = Poly::add(v, e2);
	v = Poly::add(v, mu);

	for (int i = 0; i < KYBER_K; i++) {
		u[i].compress(KYBER_DU);
	}
	std::vector<uint8_t> c1 = Poly::byteEncode(u, KYBER_DU);

	v.compress(KYBER_DV);
	std::vector<uint8_t> c2 = v.byteEncodePoly(KYBER_DV);

	std::vector<uint8_t> c;
	c.reserve(c1.size() + c2.size());
	c.insert(c.end(), c1.begin(), c1.end());
	c.insert(c.end(), c2.begin(), c2.end());

	return c;
}

std::vector<uint8_t> KPKE::decrypt(std::vector<uint8_t>& c)
{
	std::vector<uint8_t> c1(c.begin(), c.begin() + 32 * KYBER_DU * KYBER_K);
	std::vector<uint8_t> c2(c.begin() + 32 * KYBER_DU * KYBER_K, c.end());

	Poly u[KYBER_K];
	Poly::byteDecode(u, c1, KYBER_DU);
	for (int i = 0; i < KYBER_K; i++) {
		u[i].decompress(KYBER_DU);
	}

	Poly v;
	v.byteDecodePoly(c2, KYBER_DV);
	v.decompress(KYBER_DV);

	Poly s[KYBER_K];
	Poly::byteDecode(s, dkPke, 12);

	Poly omega;
	Poly temp;
	for (int i = 0; i < KYBER_K; i++) {
		u[i].ntt();
		Poly su = Poly::multiplyNTT(s[i], u[i]);
		temp = Poly::add(temp, su);
	}
	temp.invntt();
	omega = Poly::sub(v, temp);

	std::vector<uint8_t> m = omega.byteEncodePoly(1);
	return m;
}
