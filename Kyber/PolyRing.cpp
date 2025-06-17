#include "PolyRing.h"

bool PolyRing::isPrime(int number)
{
	for (int i = 2; i < sqrt(number); i++) {
		if (number % i == 0) {
			return false;
		}
	}
	return true;
}

void PolyRing::findFirstPrimitiveRootOfUnity()
{
	bool foundRoot;
	for (int candidate = 2; candidate < q; candidate++) {
		foundRoot = true;
		for (int i = 1; i < n; i++) {
			if (modExp(candidate, i, q) == 1) {
				foundRoot = false;
				break;
			}

		}
		if (foundRoot && modExp(candidate, n, q) == 1) {
			zeta = candidate;
			return;
		}
	}
}

int PolyRing::gcdExtended(int a, int b, int* x, int* y)
{
	if (a == 0) {
		*x = 0, * y = 1;
		return b;
	}

	int x1, y1;
	int gcd = gcdExtended(b % a, a, &x1, &y1);

	*x = y1 - (b / a) * x1;
	*y = x1;

	return gcd;
}

void PolyRing::findInvHalfN()
{
	int x, y;
	int g = gcdExtended(n / 2, q, &x, &y);
	
	invHalfN = mod(x, q);
}

PolyRing::PolyRing(int q, int n): q(q), n(n)
{
	if (!isPrime(q)) {
		throw std::invalid_argument("q must be prime");
	}

	if ((q - 1) % n != 0) {
		throw std::invalid_argument("n must divide q - 1");
	}

	findFirstPrimitiveRootOfUnity();
	findInvHalfN();
}

void PolyRing::print(const char* prefix)
{
	printf("%s\nPolyRing info:\n\tQ = %d\n\tN = %d\n\tZeta = %d\n\t(N / 2)^(-1) = %d\n\n", prefix, q, n, zeta, invHalfN);
}

int PolyRing::getQ()
{
	return q;
}

int PolyRing::getN()
{
	return n;
}

int PolyRing::getZeta()
{
	return zeta;
}

int PolyRing::getInvHalfN()
{
	return invHalfN;
}

bool operator==(const PolyRing& left, const PolyRing& right)
{
	return left.n == right.n && left.q == right.q;
}
