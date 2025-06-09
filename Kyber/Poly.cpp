#include "Poly.h"

Poly::Poly()
{
	coeffs = std::vector<uint16_t>(KYBER_N);
	for (int16_t i = 0; i < KYBER_N; i++) {
		coeffs[i] = 1;
	}
}

void Poly::sampleNTT(Poly A[KYBER_K][KYBER_K], uint8_t rho[32], int8_t i, int8_t j)
{
	SHAKE128 ctx;

	uint8_t seed[34];
	memcpy(seed, rho, 32);
	seed[32] = i;
	seed[33] = j;

	ctx.absorb(seed, 34);

	short counter = 0;

	while (counter < KYBER_N) {
		uint8_t C[3];
		ctx.squeeze(C, 3);
		short d1 = C[0] + 256 * (C[1] % 16);
		short d2 = C[1] / 16 + 16 * C[2];

		if (d1 < KYBER_Q) {
			A[i][j].coeffs[counter] = d1;
			counter++;
		}

		if (d2 < KYBER_Q && counter < KYBER_N) {
			A[i][j].coeffs[counter] = d2;
			counter++;
		}
	}
}

Poly Poly::samplePolyCBD(int nu, uint8_t* b)
{
	Poly result;
	std::vector<uint8_t> bits = bytesToBits(b, 64 * nu);
	for (int i = 0; i < KYBER_N; i++) {
		int8_t x = 0;
		int8_t y = 0;
		
		for (int j = 0; j < nu; j++) {
			x += bits[2 * i * nu + j];
			y += bits[2 * i * nu + nu + j];
		}

		result.coeffs[i] = ((x - y) % KYBER_Q + KYBER_Q) % KYBER_Q;
	}
	return result;
}

void Poly::ntt()
{
	int i = 1;
	int zeta = 0;
	int t = 0;
	for (int len = KYBER_N / 2; len >= 2; len /= 2) {
		for (int start = 0; start < KYBER_N; start += 2 * len) {
			zeta = modExp(ZETA, reverse(i++, LOG2N - 1), KYBER_Q); 
			for (int j = start; j < start + len; j++) {
				t = ((zeta * coeffs[j + len]) % KYBER_Q + KYBER_Q) % KYBER_Q;
				coeffs[j + len] = ((coeffs[j] - t) % KYBER_Q + KYBER_Q) % KYBER_Q;
				coeffs[j] = ((coeffs[j] + t) % KYBER_Q + KYBER_Q) % KYBER_Q;
			}
		}
	}
}

void Poly::invntt()
{
	int i = KYBER_N / 2 - 1;
	int zeta = 0;
	int t = 0;
	for (int len = 2; len <= KYBER_N / 2; len *= 2) {
		for (int start = 0; start < KYBER_N; start += 2 * len) {
			zeta = modExp(ZETA, reverse(i--, LOG2N - 1), KYBER_Q);
			for (int j = start; j < start + len; j++) {
				t = coeffs[j];
				coeffs[j] = ((t + coeffs[j + len]) % KYBER_Q + KYBER_Q) % KYBER_Q;
				coeffs[j + len] = ((zeta * (coeffs[j + len] - t)) % KYBER_Q + KYBER_Q) % KYBER_Q;
			}
		}
	}

	for (int i = 0; i < KYBER_N; i++) {
		coeffs[i] = ((coeffs[i] * INV_N_DIV2) % KYBER_Q + KYBER_Q) % KYBER_Q;
	}
}

Poly Poly::multiplyNTT(Poly& fNtt, Poly& gNtt)
{
	Poly result;

	for (int i = 0; i < KYBER_N / 2; i++) {
		int a0 = fNtt.coeffs[2 * i];
		int a1 = fNtt.coeffs[2 * i + 1];
		int b0 = gNtt.coeffs[2 * i];
		int b1 = gNtt.coeffs[2 * i + 1];
		int zeta = modExp(ZETA, 2 * reverse(i, LOG2N - 1) + 1, KYBER_Q);
		int zeta1 = zetas[i];

		int c0 = mod(a0 * b0 + a1 * b1 * zeta);
		int c01 = mod(a0 * b0 + a1 * b1 * zeta1);
		int c1 = mod(a0 * b1 + a1 * b0);

		result.coeffs[2 * i] = c0;
		result.coeffs[2 * i + 1] = c1;
	}

	return result;
}

Poly Poly::add(Poly& f, Poly& g)
{
	Poly result;
	for (int i = 0; i < KYBER_N; i++) {
		result.coeffs[i] = mod(f.coeffs[i] + g.coeffs[i]);
	}
	return result;
}

Poly Poly::sub(Poly& f, Poly& g)
{
	Poly result;
	for (int i = 0; i < KYBER_N; i++) {
		result.coeffs[i] = mod(f.coeffs[i] - g.coeffs[i]);
	}
	return result;
}

std::vector<uint8_t> Poly::byteEncodePoly()
{
	std::vector<uint8_t> result(KYBER_POLYBYTES);

	for (int i = 0, j = 0; i < KYBER_N; i += 2, j += 3) {
		uint16_t t0 = coeffs[i];
		uint16_t t1 = coeffs[i + 1];

		t0 = t0 % KYBER_Q;
		t1 = t1 % KYBER_Q;

		result[j] = t0 & 0xFF;
		result[j + 1] = ((t0 >> 8) | ((t1 & 0x0F) << 4)) & 0xFF;
		result[j + 2] = (t1 >> 4) & 0xFF;
	}

	return result;
}

void Poly::byteDecodePoly(std::vector<uint8_t>& bytes)
{
	for (int i = 0, j = 0; i < KYBER_N; i += 2, j += 3) {
		uint16_t t0 = bytes[j] | ((bytes[j + 1] & 0x0F) << 8);
		uint16_t t1 = (bytes[j + 1] >> 4) | (bytes[j + 2] << 4);

		coeffs[i] = t0;
		coeffs[i + 1] = t1;
	}
}

std::vector<uint8_t> Poly::byteEncode(Poly vec[KYBER_K], int d)
{
	std::vector<uint8_t> result;
	for (int i = 0; i < KYBER_K; i++) {
		std::vector<uint8_t> encoded = vec[i].byteEncodePoly(d);
		result.insert(result.end(), encoded.begin(), encoded.end());
	}
	return result;
}

void Poly::byteDecode(Poly vec[KYBER_K], std::vector<uint8_t>& bytes, int d)
{
	for (int v = 0; v < KYBER_K; v++) {
		std::vector<uint8_t> slice(bytes.begin() + v * d * 32, bytes.begin() + (v + 1) * d * 32);
		vec[v].byteDecodePoly(slice, d);
	}
}

std::vector<uint8_t> Poly::byteEncodePoly(int d)
{
	std::vector<uint8_t> bits(256 * d);

	uint16_t a;

	for (int i = 0; i < KYBER_N; i++) {
		a = coeffs[i];
		for (int j = 0; j < d; j++) {
			bits[i * d + j] = a & 0x1;
			a = (a - bits[i * d + j]) >> 1;
		}
	}

	return bitsToBytes(bits.data(), 256 * d, d);
}

void Poly::byteDecodePoly(std::vector<uint8_t>& bytes, int d)
{
	int m = d < 12 ? (1 << d) : KYBER_Q;

	std::vector<uint8_t> bits = bytesToBits(bytes.data(), 32 * d);

	for (int i = 0; i < KYBER_N; i++) {
		coeffs[i] = 0;
		for (int j = 0; j < d; j++) {
			coeffs[i] += (bits[i * d + j] * (1 << j)) % m;
		}
	}
}

void Poly::compress(int d)
{
	for (int i = 0; i < KYBER_N; i++) {
		coeffs[i] = compressX(coeffs[i], d);
	}
}

void Poly::decompress(int d)
{
	for (int i = 0; i < KYBER_N; i++) {
		coeffs[i] = decompressX(coeffs[i], d);
	}
}

void Poly::print(const char* msg)
{
	printf("\n%s\n", msg);
	printf("[");
	for (int i = 0; i < KYBER_N; i++) {
		printf("%5d,", coeffs[i]);
	}
	printf("]");
	printf("\nEND OF POLYNOM\n\n");
}

