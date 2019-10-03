#pragma once

#include "Ray.h"
#include "HitInfo.h"

class Structure
{
public:
	Structure();
	virtual ~Structure();

	virtual bool trace(Ray& ray, HitInfo& hit) = 0;
};

