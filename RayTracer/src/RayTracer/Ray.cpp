#include "Ray.h"


Ray::Ray() :
	center(), direction()
{
}

Ray::Ray(Vec3f center, Vec3f direction, float t_min, float t_max, unsigned int depth, unsigned int pixel, Vec3f throughput) :
	center(center), direction(direction), t_min(t_min), t_max(t_max), depth(depth), pixel(pixel), throughput(throughput)
{
}


Ray::~Ray()
{
}
