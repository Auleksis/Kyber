#include "PolyCoder.h"

std::vector<uint8_t> PolyCoder::byteEncode(PolyVector& v, int d)
{
	int size = v.getSize();

	int n = v.getRing().getN();

	int step = 256 * d;

	std::vector<uint8_t> bits(step * size);

	uint16_t a;

	for (int k = 0; k < size; k++) {
		for (int i = 0; i < n; i++) {
			a = v[k][i];
			for (int j = 0; j < d; j++) {
				bits[i * d + j + step * k] = a & 0x1;
				a = (a - bits[i * d + j + step * k]) >> 1;
			}
		}
	}

	return bitsToBytes(bits.data(), step * size);
}

PolyVector PolyCoder::byteDecode(std::vector<uint8_t>& bytes, PolyRing& ring, int size, int d)
{
	PolyVector decoded(ring, size);

	int m = d < 12 ? (1 << d) : ring.getQ();

	int n = ring.getN();

	for (int i = 0; i < size; i++) {
		std::vector<uint8_t> slice(bytes.begin() + i * d * 32, bytes.begin() + (i + 1) * d * 32);
		std::vector<uint8_t> bits = bytesToBits(slice.data(), 32 * d);

		for (int j = 0; j < n; j++) {
			decoded[i][j] = 0;
			for (int k = 0; k < d; k++) {
				decoded[i][j] += mod((bits[j * d + k] * (1 << k)), m);
			}
		}
	}

    return decoded;
}
