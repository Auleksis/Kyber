#include "PolyVector.h"

PolyVector::PolyVector(PolyRing& ring, int size): ring(ring), size(size)
{
	vector = std::vector<Poly>(size, Poly(ring));
}

int PolyVector::getSize() const
{
	return size;
}

PolyRing& PolyVector::getRing() const
{
	return ring;
}

Poly& PolyVector::operator[](int i)
{
	if (i < 0 || i >= size) {
		throw std::invalid_argument("Index is out of range");
	}

	return vector[i];
}

const Poly& PolyVector::operator[](int i) const
{
	if (i < 0 || i >= size) {
		throw std::invalid_argument("Index is out of range");
	}

	return vector[i];
}

void PolyVector::ntt()
{
	//std::vector<std::thread> threads(size);

	//for (int i = 0; i < size; i++) {
	//	threads[i] = std::thread(&Poly::ntt, &vector[i]);
	//}

	//for (auto& t : threads) {
	//	t.join();
	//}

	for (int i = 0; i < size; i++) {
		vector[i].ntt();
	}
}

void PolyVector::invntt()
{
	for (int i = 0; i < size; i++) {
		vector[i].invntt();
	}
}

void PolyVector::compress(int d)
{
	for (int i = 0; i < size; i++) {
		vector[i].compress(d);
	}
}

void PolyVector::decompress(int d)
{
	for (int i = 0; i < size; i++) {
		vector[i].decompress(d);
	}
}

void PolyVector::print(const char* prefix)
{
	printf("\n%s\n", prefix);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < ring.getN(); j++) {
			printf("%5d", vector[i][j]);
		}
		printf("\n");
	}
}

PolyVector& PolyVector::operator+=(const PolyVector& right)
{
	if (size != right.size) {
		throw std::invalid_argument("Operands must have the same size");
	}

	if (ring != right.ring) {
		throw std::invalid_argument("Operands must have the same ring");
	}

	for (int i = 0; i < size; i++) {
		vector[i] += right[i];
	}

	return *this;
}

PolyVector& PolyVector::operator-=(const PolyVector& right)
{
	if (size != right.size) {
		throw std::invalid_argument("Operands must have the same size");
	}

	if (ring != right.ring) {
		throw std::invalid_argument("Operands must have the same ring");
	}

	for (int i = 0; i < size; i++) {
		vector[i] -= right[i];
	}

	return *this;
}

const PolyVector& PolyVector::operator=(const PolyVector& right)
{
	if (this == &right) {
		return right;
	}

	vector = right.vector;
	return *this;
}

const PolyVector operator+(const PolyVector& left, const PolyVector& right)
{
	if (left.size != right.size) {
		throw std::invalid_argument("Operands must have the same size");
	}

	if (left.ring != right.ring) {
		throw std::invalid_argument("Operands must have the same ring");
	}

	PolyVector c(left.ring, left.size);
	for (int i = 0; i < left.size; i++) {
		c[i] = left[i] + right[i];
	}

	return c;
}

const PolyVector operator-(const PolyVector& left, const PolyVector& right)
{
	if (left.size != right.size) {
		throw std::invalid_argument("left and right operands must have the same size");
	}

	if (left.ring != right.ring) {
		throw std::invalid_argument("left and right operands must have the same ring");
	}

	PolyVector c(left.ring, left.size);
	for (int i = 0; i < left.size; i++) {
		c[i] = left[i] - right[i];
	}

	return c;
}

bool operator==(const PolyVector& left, const PolyVector& right)
{
	if (left.size != right.size) {
		return false;
	}

	for (int i = 0; i < left.size; i++) {
		if (left[i] != right[i]) {
			return false;
		}
	}

	return true;
}
