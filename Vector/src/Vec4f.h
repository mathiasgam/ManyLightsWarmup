#pragma once
#include "API.h"
#include "ArithVec.h"

class API Vec4f :
	public ArithVec<float, Vec4f, 4>
{

public:

	Vec4f() :ArithVec<float, Vec4f, 4>() {}
	Vec4f(const float _data[4]) :ArithVec<float, Vec4f, 4>(_data) {}
	Vec4f(float x, float y, float z, float w) :ArithVec<float, Vec4f, 4>(x, y, z, w) {}

	Vec4f operator+(Vec4f const &) const;
	Vec4f operator-(Vec4f const &) const;
	Vec4f operator*(float) const;
	Vec4f operator/(float) const;

	Vec4f cross(Vec4f const &) const;
	Vec4f normalized() const;
	float length() const;
};


