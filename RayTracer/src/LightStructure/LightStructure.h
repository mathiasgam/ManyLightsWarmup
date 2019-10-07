#pragma once

#include <vector>

#include "PointLight.h"
#include "Vec3f.h"


/// interface for lights structures, for easy swapping between the two solutions
class LightStructure
{
public:
	virtual void init(std::vector<PointLight*> lights) = 0;
	virtual std::vector<PointLight*> GetLights(Vec3f position, Vec3f normal, float threshold) const = 0;
};
