#include "Vec4f.h"
#include "inverse_sqrt.h"

#include <memory>


Vec4f Vec4f::operator+(Vec4f const &other) const
{
	return Vec4f(
		data[0] + other[0],
		data[1] + other[1],
		data[2] + other[2],
		data[3] + other[3]
	);
}

Vec4f Vec4f::operator-(Vec4f const &other) const
{
	return Vec4f(
		data[0] - other[0],
		data[1] - other[1],
		data[2] - other[2],
		data[3] - other[3]
	);
}

Vec4f Vec4f::operator*(float c) const
{
	return Vec4f(
		data[0] * c,
		data[1] * c,
		data[2] * c,
		data[3] * c
	);
}

Vec4f Vec4f::operator/(float c) const
{
	return Vec4f(
		data[0] / c,
		data[1] / c,
		data[2] / c,
		data[3] / c
	);
}

Vec4f Vec4f::cross(Vec4f const &) const
{
	return Vec4f();
}

Vec4f Vec4f::normalized() const
{
	return *this / this->length();
}

float Vec4f::length() const
{
	return sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3]);
}


