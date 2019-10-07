#pragma once

#include "PointLight.h"
#include "LightStructure/LightStructure.h"

#include <vector>

class LightArray : public LightStructure
{
private:
	std::vector<PointLight*> lights;

public:
	LightArray() : lights(0) {}
	~LightArray(){}

	virtual void init(std::vector<PointLight*> lights);
	virtual std::vector<PointLight*> GetLights(Vec3f position, Vec3f normal, float threshold) const;

private:

};