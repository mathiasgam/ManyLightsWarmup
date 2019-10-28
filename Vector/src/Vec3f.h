#pragma once
#include "API.h"
#include "ArithVec.h"

class API Vec3f:
	public ArithVec<float, Vec3f, 3>
{

public:

	Vec3f() : ArithVec<float, Vec3f, 3>(0.0f) {}
	Vec3f(float c) : ArithVec<float, Vec3f, 3>(c) {}
	Vec3f(float x, float y, float z) : ArithVec<float, Vec3f, 3>(x, y, z) {}
	Vec3f(const float _data[3]) : ArithVec<float, Vec3f, 3>(_data) {}
	
	//Vec3f operator+(Vec3f const &) const;
	//Vec3f operator-(Vec3f const &) const;
	//Vec3f operator/(Vec3f const &) const;
	//Vec3f operator*(float) const;
	//Vec3f operator/(float) const;

	inline Vec3f normalized() const;
	inline float length() const;

	inline float min_componont() const;
	inline float max_componont() const;

};

inline API Vec3f cross(const Vec3f& v0, const Vec3f& v1);