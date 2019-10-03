#pragma once
#include "API.h"
#include "ArithVec.h"

class API Vec3i :
	public ArithVec<int, Vec3i, 3>
{
public:
	Vec3i() : ArithVec<int, Vec3i, 3>(0) {}
	Vec3i(int c) : ArithVec<int, Vec3i, 3>(c) {}
	Vec3i(int x, int y, int z) : ArithVec<int, Vec3i, 3>(x, y, z) {}

};

