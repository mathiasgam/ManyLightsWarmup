#include "MortonCode.h"

#include <intrin.h>

MortonCode3::MortonCode3()
{
	data = 0;
}

MortonCode3::MortonCode3(Vec3f p)
{
	// points must be in the range [0,1]
	assert(p[0] <= 1 && p[0] >= 0, "x-component out of range [0,1]");
	assert(p[1] <= 1 && p[1] >= 0, "y-component out of range [0,1]");
	assert(p[2] <= 1 && p[2] >= 0, "z-component out of range [0,1]");

	// project the normalized values onto the range of 21 bits
	uint64_t x = SplitBy3(0x1fffff * p[0]);
	uint64_t y = SplitBy3(0x1fffff * p[1]);
	uint64_t z = SplitBy3(0x1fffff * p[2]);

	// interleave the bits from the 3 dimensions
	data = x | y << 1 | z << 2;
}

bool MortonCode3::operator<(const MortonCode3& other) const
{
	return data < other.data;
}

bool MortonCode3::operator>(const MortonCode3& other) const
{
	return data > other.data;
}

unsigned int clz(uint64_t code)
{
	return __lzcnt64(code);
}

uint64_t MortonCode3::SplitBy3(uint64_t x)
{
	x &= 0x1fffff; // we only look at the first 21 bits
	x = (x | x << 32) & 0x1f00000000ffff;
	x = (x | x << 16) & 0x1f0000ff0000ff;
	x = (x | x << 8) & 0x100f00f00f00f00f;
	x = (x | x << 4) & 0x10c30c30c30c30c3;
	x = (x | x << 2) & 0x1249249249249249;

	return x;
}
