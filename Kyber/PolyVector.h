#pragma once

#include "Poly.h"

class PolyVector
{
private:
	PolyRing& ring;
	std::vector<Poly> vector;
	int size;
public:
	PolyVector(PolyRing& ring, int size);

	int getSize() const;
	PolyRing& getRing() const;

	Poly& operator[] (int i);
	const Poly& operator[] (int i) const;

	void ntt();
	void invntt();
	void compress(int d);
	void decompress(int d);
	void print(const char* prefix);

	friend const PolyVector operator+ (const PolyVector& left, const PolyVector& right);
	PolyVector& operator+= (const PolyVector& right);
	friend const PolyVector operator- (const PolyVector& left, const PolyVector& right);
	PolyVector& operator-= (const PolyVector& right);
	const PolyVector& operator= (const PolyVector& right);
	friend bool operator== (const PolyVector& left, const PolyVector& right);
};


