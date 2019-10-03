#pragma once
#include "ArithVec.h"

class Vec2ui :
	public ArithVec<unsigned int, Vec2ui, 2>
{
public:
	Vec2ui() : ArithVec<unsigned int, Vec2ui, 2>(0U) {};
	Vec2ui(unsigned int c) : ArithVec<unsigned int, Vec2ui, 2>(c) {};
	Vec2ui(unsigned int x, unsigned int y) : ArithVec<unsigned int, Vec2ui, 2>(x, y) {};
};

