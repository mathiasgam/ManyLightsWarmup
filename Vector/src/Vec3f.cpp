#include "Vec3f.h"

#include <memory>
#include <math.h>



Vec3f Vec3f::normalized() const
{
	return *this / this->length();
}

float Vec3f::length() const
{
	return sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
}

float Vec3f::sqr_length() const
{
	return data[0] * data[0] + data[1] * data[1] + data[2] * data[2];
}

float Vec3f::min_componont() const
{
	float t;
	t = data[0] < data[1] ? data[0] : data[1];
	t = t < data[2] ? t : data[2];
	return t;
}

float Vec3f::max_componont() const
{
	float t;
	t = data[0] > data[1] ? data[0] : data[1];
	t = t > data[2] ? t : data[2];
	return t;
}

inline Vec3f cross(const Vec3f & v0, const Vec3f & v1)
{
	return Vec3f(
		(v0[1] * v1[2]) - (v0[2] * v1[1]),
		(v0[2] * v1[0]) - (v0[0] * v1[2]),
		(v0[0] * v1[1]) - (v0[1] * v1[0])
	);
}
