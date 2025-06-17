#include "KPKE.h"

void KPKE::sampleNTT(PolyMatrix& A, uint8_t rho[32], int8_t i, int8_t j)
{
	SHAKE128 ctx;

	uint8_t seed[34];
	memcpy(seed, rho, 32);
	seed[32] = i;
	seed[33] = j;

	ctx.absorb(seed, 34);

	short counter = 0;

	while (counter < context.ring.getN()) {
		uint8_t C[3];
		ctx.squeeze(C, 3);
		short d1 = C[0] + 256 * (C[1] % 16);
		short d2 = C[1] / 16 + 16 * C[2];

		if (d1 < context.ring.getQ()) {
			A(i, j)[counter] = d1;
			counter++;
		}

		if (d2 < context.ring.getQ() && counter < context.ring.getN()) {
			A(i, j)[counter] = d2;
			counter++;
		}
	}
}

Poly KPKE::samplePolyCBD(int nu, uint8_t* b)
{
	Poly result(context.ring);
	std::vector<uint8_t> bits = bytesToBits(b, 64 * nu);
	for (int i = 0; i < context.ring.getN(); i++) {
		int x = 0;
		int y = 0;

		for (int j = 0; j < nu; j++) {
			x += bits[2 * i * nu + j];
			y += bits[2 * i * nu + nu + j];
		}

		result[i] = mod((x - y), context.ring.getQ());
	}
	return result;
}

int KPKE::keyGen(int8_t* d)
{
	uint8_t gHash[64];
	int8_t gHashInput[33];
	memcpy(gHashInput, d, 32);
	gHashInput[32] = context.k;


	if (!EVP_Digest(gHashInput, 33, gHash, nullptr, EVP_sha3_512(), nullptr)) {
		printf("Error while G hash calculation in K-PKE KeyGen\n");
		return -1;
	}

	uint8_t rho[32];
	memcpy(rho, gHash, 32);
	uint8_t sigma[32];
	memcpy(sigma, &gHash[32], 32);

	uint8_t N = 0;

	PolyMatrix A(context.ring, context.k);

	for (int8_t i = 0; i < context.k; i++) {
		for (int8_t j = 0; j < context.k; j++) {
			sampleNTT(A, rho, i, j);
		}
	}

	PolyVector s(context.ring, context.k);
	uint8_t prfShakeInput[33];
	memcpy(prfShakeInput, sigma, 32);

	std::vector<uint8_t> B(64 * context.nu1);
	for (int8_t i = 0; i < context.k; i++) {
		prfShakeInput[32] = N;
		shake256_hash(prfShakeInput, 33, B.data(), 64 * (size_t)context.nu1);
		s[i] = samplePolyCBD(context.nu1, B.data());
		N++;
	}

	PolyVector e(context.ring, context.k);
	for (int8_t i = 0; i < context.k; i++) {
		prfShakeInput[32] = N;
		shake256_hash(prfShakeInput, 33, B.data(), 64 * (size_t)context.nu1);
		e[i] = samplePolyCBD(context.nu1, B.data());
		N++;
	}

	s.ntt();
	e.ntt();

	PolyVector t = A * s + e;

	ekPke = coder.byteEncode(t, 12);
	ekPke.insert(ekPke.end(), std::begin(rho), std::end(rho));

	dkPke = coder.byteEncode(s, 12);

	return 0;
}

std::vector<uint8_t> KPKE::encrypt(std::vector<uint8_t>& m, std::vector<uint8_t>& r)
{
	PolyVector t = coder.byteDecode(ekPke, context.ring, context.k, 12);

	uint8_t rho[32];
	memcpy(rho, ekPke.data() + 384 * context.k, 32);

	uint8_t N = 0;

	PolyMatrix A(context.ring, context.k);

	for (int i = 0; i < context.k; i++) {
		for (int j = 0; j < context.k; j++) {
			sampleNTT(A, rho, i, j);
		}
	}

	PolyVector y(context.ring, context.k);
	uint8_t prfShakeInput[33];
	memcpy(prfShakeInput, r.data(), 32);

	std::vector<uint8_t> B(64 * context.nu1);
	for (int i = 0; i < context.k; i++) {
		prfShakeInput[32] = N;
		shake256_hash(prfShakeInput, 33, B.data(), 64 * (size_t)context.nu1);
		y[i] = samplePolyCBD(context.nu1, B.data());
		N++;
	}

	PolyVector e1(context.ring, context.k);
	for (int i = 0; i < context.k; i++) {
		prfShakeInput[32] = N;
		shake256_hash(prfShakeInput, 33, B.data(), 64 * (size_t)context.nu2);
		e1[i] = samplePolyCBD(context.nu2, B.data());
		N++;
	}

	e1.ntt();
	
	Poly e2(context.ring);
	prfShakeInput[32] = N;
	shake256_hash(prfShakeInput, 33, B.data(), 64 * (size_t)context.nu2);
	e2 = samplePolyCBD(context.nu2, B.data());

	y.ntt();
	A.transpose();

	PolyVector u = A * y + e1; // = A^T * y
	u.invntt();

	Poly mu = coder.byteDecode(m, context.ring, 1, 1)[0];
	mu.decompress(1);

	Poly v(context.ring);
	for (int i = 0; i < context.k; i++) {
		v += t[i] * y[i]; // = (s*A)^T * y
	}
	v.invntt();
	v += e2 + mu; // = s^T * A^T * y + m
	
	u.compress(context.du);
	std::vector<uint8_t> c1 = coder.byteEncode(u, context.du);

	v.compress(context.dv);
	PolyVector vWrapper(context.ring, 1);
	vWrapper[0] = v;
	std::vector<uint8_t> c2 = coder.byteEncode(vWrapper, context.dv);

	std::vector<uint8_t> c;
	c.reserve(c1.size() + c2.size());
	c.insert(c.end(), c1.begin(), c1.end());
	c.insert(c.end(), c2.begin(), c2.end());

	return c;
}

std::vector<uint8_t> KPKE::decrypt(std::vector<uint8_t>& c)
{
	std::vector<uint8_t> c1(c.begin(), c.begin() + 32 * context.du * context.k);
	std::vector<uint8_t> c2(c.begin() + 32 * context.du * context.k, c.end());

	PolyVector u = coder.byteDecode(c1, context.ring, context.k, context.du);
	u.decompress(context.du);

	PolyVector vWrapper = coder.byteDecode(c2, context.ring, 1, context.dv);
	vWrapper.decompress(context.dv);
	Poly v = vWrapper[0];

	PolyVector s = coder.byteDecode(dkPke, context.ring, context.k, 12);

	u.ntt();

	Poly temp(context.ring);
	for (int i = 0; i < context.k; i++) {
		temp += s[i] * u[i];
	}
	temp.invntt();
	Poly omega = v - temp;

	omega.compress(1);

	PolyVector omegaWrapper(context.ring, 1);
	omegaWrapper[0] = omega;

	std::vector<uint8_t> m = coder.byteEncode(omegaWrapper, 1);
	return m;
}
