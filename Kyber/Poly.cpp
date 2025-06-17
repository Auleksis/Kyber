#include "Poly.h"

Poly::Poly(PolyRing& ring) : ring(ring)
{
	int n = ring.getN();
	coeffs = std::vector<uint16_t>(n);
	for (int16_t i = 0; i < n; i++) {
		coeffs[i] = 0;
	}
}

void Poly::setCoeffs(const std::vector<int>& coeffs)
{
	if (ring.getN() < coeffs.size()) {
		throw std::invalid_argument("coeffs count must be less or equal to PolyRing N parameter");
	}
	int q = ring.getQ();
	for (int i = 0; i < ring.getN(); i++) {
		this->coeffs[i] = mod(coeffs[i], q);
	}
}

Poly& Poly::operator+=(const Poly& right)
{
	if (ring != right.ring) {
		throw std::invalid_argument("Operands must have the same ring");
	}

	int n = ring.getN();
	int q = ring.getQ();

	for (int i = 0; i < n; i++) {
		coeffs[i] = mod(coeffs[i] + right.coeffs[i], q);
	}

	return *this;
}

Poly& Poly::operator-=(const Poly& right)
{
	if (ring != right.ring) {
		throw std::invalid_argument("Operands must have the same ring");
	}

	int n = ring.getN();
	int q = ring.getQ();

	for (int i = 0; i < n; i++) {
		coeffs[i] = mod(coeffs[i] - right.coeffs[i], q);
	}

	return *this;
}

const Poly& Poly::operator=(const Poly& right)
{
	if (this == &right) {
		return right;
	}

	coeffs = right.coeffs;
	return *this;
}

uint16_t& Poly::operator[](int i)
{
	if (i < 0 || i > coeffs.size()) {
		throw std::invalid_argument("Index is out of range");
	}

	return coeffs[i];
}

const uint16_t& Poly::operator[](int i) const
{
	if (i < 0 || i > coeffs.size()) {
		throw std::invalid_argument("Index is out of range");
	}

	return coeffs[i];
}

void Poly::ntt()
{
	int n = ring.getN();
	int q = ring.getQ();
	int zeta0 = ring.getZeta();
	int bitLength = log2(n) - 1;

	int i = 1;
	int zeta = 0;
	int t = 0;

	int temp1 = 0;
	int temp2 = 0;

	for (int len = n / 2; len >= 2; len >>= 1) {
		for (int start = 0; start < n; start += 2 * len) {
			zeta = modExp(zeta0, reverse(i++, bitLength), q);
			for (int j = start; j < start + len; j++) {
				t = mod(zeta * coeffs[j + len], q);
				temp1 = coeffs[j];
				temp2 = coeffs[j + len];
				coeffs[j + len] = mod(temp1 - t, q);
				coeffs[j] = mod(temp1 + t, q);
			}
		}
	}
}

void Poly::invntt()
{
	int n = ring.getN();
	int q = ring.getQ();
	int zeta0 = ring.getZeta();
	int invHalfN = ring.getInvHalfN();
	int bitLength = log2(n) - 1;

	int i = n / 2 - 1;
	int zeta = 0;
	int t = 0;

	int temp1 = 0;
	int temp2 = 0;

	for (int len = 2; len <= n / 2; len *= 2) {
		for (int start = 0; start < n; start += 2 * len) {
			zeta = modExp(zeta0, reverse(i--, bitLength), q);
			for (int j = start; j < start + len; j++) {
				t = coeffs[j];
				temp1 = coeffs[j];
				temp2 = coeffs[j + len];
				coeffs[j] = mod(t + temp2, q);
				coeffs[j + len] = mod(zeta * (temp2 - t), q);
			}
		}
	}

	for (int i = 0; i < n; i++) {
		coeffs[i] = mod(coeffs[i] * invHalfN, q);
	}
}

void Poly::compress(int d)
{
	for (int i = 0; i < ring.getN(); i++) {
		coeffs[i] = compressX(coeffs[i], d, ring.getQ());
	}
}

void Poly::decompress(int d)
{
	for (int i = 0; i < ring.getN(); i++) {
		coeffs[i] = decompressX(coeffs[i], d, ring.getQ());
	}
}

void Poly::print(const char* msg) const
{
	printf("\n%s\n", msg);
	printf("[");
	for (int i = 0; i < ring.getN(); i++) {
		printf("%5d,", coeffs[i]);
	}
	printf("]");
	printf("\nEND OF POLYNOM\n\n");
}

PolyRing& Poly::getRing()
{
	return ring;
}

//Requires left and right be in NTT form
const Poly operator*(const Poly& left, const Poly& right)
{
	if (!(left.ring == right.ring)) {
		throw std::invalid_argument("Operands must have the same ring");
	}

	Poly c(left.ring);
	int q = left.ring.getQ();
	int zeta0 = left.ring.getZeta();
	int n = left.ring.getN();

	int zeta = 0;
	int bitLength = log2(n) - 1;

	int a = 0;
	int b = 0;

	//Poly: a + bx

	for (int i = 0; i < c.ring.getN() / 2; i++) {
		int leftA = left.coeffs[2 * i];
		int leftB = left.coeffs[2 * i + 1];

		int rightA = right.coeffs[2 * i];
		int rightB = right.coeffs[2 * i + 1];

		zeta = modExp(zeta0, 2 * reverse(i, bitLength) + 1, q);
		// printf("%5d", zeta);


		a = mod(mod(leftA * rightA, q) + mod(mod(leftB * rightB, q) * zeta, q), q);
		b = mod(mod(leftA * rightB, q) + mod(rightA * leftB, q), q);
		c.coeffs[2 * i] = a;
		c.coeffs[2 * i + 1] = b;
	}

	return c;
}

const Poly operator+(const Poly& left, const Poly& right)
{
	if (!(left.ring == right.ring)) {
		throw std::invalid_argument("Operands must have the same ring");
	}

	Poly c(left.ring);
	int q = left.ring.getQ();
	int n = left.ring.getN();

	for (int i = 0; i < n; i++) {
		c.coeffs[i] = mod(left.coeffs[i] + right.coeffs[i], q);
	}

	return c;
}

const Poly operator-(const Poly& left, const Poly& right)
{
	if (!(left.ring == right.ring)) {
		throw std::invalid_argument("Operands must have the same ring");
	}

	Poly c(left.ring);
	int q = left.ring.getQ();
	int n = left.ring.getN();

	for (int i = 0; i < n; i++) {
		c.coeffs[i] = mod(left.coeffs[i] - right.coeffs[i], q);
	}

	return c;
}

bool operator==(const Poly& left, const Poly& right)
{
	if (left.ring != right.ring) {
		return false;
	}

	for (int i = 0; i < left.ring.getN(); i++) {
		if (left[i] != right[i]) {
			return false;
		}
	}

	return true;
}
