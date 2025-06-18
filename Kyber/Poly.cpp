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
	
}

void Poly::invntt()
{
	
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

	int n = left.ring.getN();
	int q = left.ring.getQ();
	
	std::vector<uint16_t>newCoeffs(2 * n - 1);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			newCoeffs[i + j] = mod(newCoeffs[i + j] + mod(left[i] * right[j], q), q);
		}
	}

	Poly c(left.ring);
	for (int i = 0; i < n; i++) {
		c[i] = newCoeffs[i];
	}

	for (int i = n; i < 2 * n - 1; i++) {
		c[i - n] = mod(c[i - n] - newCoeffs[i], q);
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
