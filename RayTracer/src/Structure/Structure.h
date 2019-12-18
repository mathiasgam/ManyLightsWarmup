#pragma once

#include <vector>

#include "RayTracer/Ray.h"
#include "RayTracer/HitInfo.h"

class Structure
{
public:
	Structure();
	virtual ~Structure();

	virtual bool closest_hit(Ray& ray, HitInfo& hit) const = 0;
	virtual bool any_hit(Ray& ray) const = 0;

	virtual void closest_hit(std::vector<Ray>& rays, std::vector<HitInfo>& hits) const = 0;
	virtual void any_hit(std::vector<Ray>& rays) const = 0;
};

