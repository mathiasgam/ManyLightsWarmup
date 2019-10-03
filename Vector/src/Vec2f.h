#pragma once
#include "API.h"
#include "ArithVec.h"

class API Vec2f :
	public ArithVec<float, Vec2f, 2>
{
public:

	/// Construct a 2D float vector.
	Vec2f() : ArithVec<float, Vec2f, 2>(0.0f) {}

	/// Construct a 2D float vector.
	Vec2f(float a, float b) : ArithVec<float, Vec2f, 2>(a, b) {}


};

