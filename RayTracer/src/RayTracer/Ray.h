#pragma once

#include "Vec3f.h"


class Ray
{
public:

	Vec3f center;
	Vec3f direction;
	float t_min;
	float t_max;

	unsigned int depth;
	unsigned int pixel;

	Vec3f throughput;
	bool hashit = false;

	Ray();
	Ray(Vec3f center, Vec3f direction, float t_min, float t_max, unsigned int depth, unsigned int pixel, Vec3f throughput);
	~Ray();


private:
	
};







