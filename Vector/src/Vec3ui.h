#pragma once
#include "API.h"
#include "ArithVec.h"

class API Vec3ui :
	public ArithVec<unsigned int, Vec3ui, 3>
{
public:
	Vec3ui() : ArithVec<unsigned int, Vec3ui, 3>(0U) {}
	Vec3ui(unsigned int c) : ArithVec<unsigned int, Vec3ui, 3>(c) {}
	Vec3ui(unsigned int x, unsigned int y, unsigned int z) : ArithVec<unsigned int, Vec3ui, 3>(x, y, z) {}

};

