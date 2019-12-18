#pragma once

#include "Ray.h"

class RayGeneartor
{
public:

	virtual unsigned int size() = 0;
	virtual void generate(Ray* target) = 0;

};
