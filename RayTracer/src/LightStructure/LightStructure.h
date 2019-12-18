#pragma once

#include <vector>

#include "PointLight.h"
#include "Vec3f.h"
#include "RayTracer/HitInfo.h"


/// interface for lights structures, for easy swapping between the two solutions
class LightStructure
{
public:
	virtual void init(std::vector<PointLight*> lights) = 0;
	virtual std::vector<PointLight*> GetLights(const HitInfo& hit, float threshold) const = 0;
};
