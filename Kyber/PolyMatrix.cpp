#include "PolyMatrix.h"

PolyMatrix::PolyMatrix(PolyRing& ring, int size): ring(ring), size(size)
{
	matrix = std::vector<std::vector<Poly>>(size, std::vector<Poly>(size, Poly(ring)));
}

void PolyMatrix::ntt()
{
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrix[i][j].ntt();
		}
	}
}

void PolyMatrix::invntt()
{
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrix[i][j].invntt();
		}
	}
}

void PolyMatrix::transpose()
{
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < i; j++) {
			Poly t = matrix[i][j];
			matrix[i][j] = matrix[j][i];
			matrix[j][i] = t;
		}
	}
}

void PolyMatrix::print(const char* prefix)
{
	printf("\n%s\n", prefix);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < ring.getN(); k++) {
				printf("%5d", matrix[i][j][k]);
			}
			printf("\t");
		}
		printf("\n");
	}
}

Poly& PolyMatrix::operator()(int i, int j)
{
	if (i < 0 || j < 0 || i >= size || j >= size) {
		throw std::invalid_argument("Index is out of range");
	}

	return matrix[i][j];
}

const Poly& PolyMatrix::operator()(int i, int j) const
{
	if (i < 0 || j < 0 || i >= size || j >= size) {
		throw std::invalid_argument("Index is out of range");
	}

	return matrix[i][j];
}

int PolyMatrix::getSize() const
{
	return size;
}

PolyRing& PolyMatrix::getRing() const
{
	return ring;
}

PolyVector operator*(const PolyMatrix& left, const PolyVector& right)
{
	if (left.getSize() != right.getSize()) {
		throw std::invalid_argument("Operands must have the same size");
	}

	if (left.getRing() != right.getRing()) {
		throw std::invalid_argument("Operands must have the same ring");
	}

	PolyRing& ring = left.getRing();
	int size = left.getSize();

	PolyVector c(ring, size);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			c[i] += left(i, j) * right[j];
		}
	}

	return c;
}
