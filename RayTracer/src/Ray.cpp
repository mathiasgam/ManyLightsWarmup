#include "Ray.h"


Ray::Ray() :
	center(), direction()
{
}

Ray::Ray(Vec3f center, Vec3f direction, float t_min, float t_max) :
	center(center), direction(direction), t_min(t_min), t_max(t_max)
{
}


Ray::~Ray()
{
}
