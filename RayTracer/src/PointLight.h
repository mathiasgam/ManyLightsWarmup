#pragma once

#include "Vec3f.h"

class PointLight
{
private:

public:
	const Vec3f position;
	const Vec3f color;

	PointLight();
	PointLight(Vec3f position, Vec3f color);
	~PointLight();

};

