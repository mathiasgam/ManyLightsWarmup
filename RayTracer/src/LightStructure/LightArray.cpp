#include "LightArray.h"

void LightArray::init(std::vector<PointLight*> _lights)
{
	lights.clear();
	for (PointLight* l : _lights)
		lights.push_back(l);
}

std::vector<PointLight*> LightArray::GetLights(Vec3f position, Vec3f normal, float threshold) const
{
	return lights;
}
