#pragma once

#include "Ray.h"
#include "HitInfo.h"

class Structure
{
public:
	Structure();
	virtual ~Structure();

	virtual bool closest_hit(Ray& ray, HitInfo& hit) const = 0;
	virtual bool any_hit(Ray& ray, HitInfo& hit) const = 0;
};

