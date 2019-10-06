#include "PointLight.h"

PointLight::PointLight():
	position(0,0,0), color(0,0,0)
{
}

PointLight::PointLight(Vec3f position, Vec3f color):
	position(position), color(color)
{
}

PointLight::~PointLight()
{
}
