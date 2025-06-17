#pragma once
#include "PolyVector.h"

class PolyMatrix
{
private:
	PolyRing& ring;
	std::vector<std::vector<Poly>> matrix;
	int size;
public:
	PolyMatrix(PolyRing& ring, int size);

	void ntt();
	void invntt();
	void transpose();
	void print(const char* prefix);

	Poly& operator() (int i, int j);

	const Poly& operator() (int i, int j) const;

	int getSize() const;
	PolyRing& getRing() const;
};

PolyVector operator* (const PolyMatrix& left, const PolyVector& right);
