#include "LightArray.h"
#include "HitInfo.h"

void LightArray::init(std::vector<PointLight*> _lights)
{
	lights.clear();
	for (PointLight* l : _lights)
		lights.push_back(l);
}

std::vector<PointLight*> LightArray::GetLights(const HitInfo& hit, float threshold) const
{
	return lights;
}
