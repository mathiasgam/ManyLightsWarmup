#pragma once

#include "Vec3f.h"

#include <vector>
#include <cassert>
#include <bitset>

class MortonCode
{

};

class MortonCode3 : MortonCode {
private:

public:
	uint64_t data;
	MortonCode3();
	MortonCode3(Vec3f p);

	bool operator < (const MortonCode3& other) const;
	bool operator > (const MortonCode3& other) const;

private:
	static inline uint64_t SplitBy3(uint64_t);
};

unsigned int clz(uint64_t code);

inline std::ostream& operator<<(std::ostream& os, const MortonCode3& code)
{
	os << "(" << std::bitset<64>(code.data) << ")";
	return os;
}

