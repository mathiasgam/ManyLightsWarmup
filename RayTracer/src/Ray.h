#pragma once

#include "Vec3f.h"

class Ray
{
public:

	Vec3f center;
	Vec3f direction;
	float t_min;
	float t_max;

	Ray();
	Ray(Vec3f center, Vec3f direction, float t_min, float t_max);
	~Ray();


private:
	
};


