#include "Point.h"

Point::Point(const Vec3f center, const float radius):
	center(center), radius(radius)
{
	color = Vec3f(1.0f, 1.0f, 0.0f);
}

Point::~Point()
{
}

bool Point::intersect(Ray& ray, HitInfo& hit, unsigned int i) const
{
	float t = dot((center - ray.center), ray.direction);

	if (t < ray.t_min || t > ray.t_max) {
		return false;
	}

	Vec3f p = ray.center + ray.direction * t;
	Vec3f v = p - center;
	float d2 = dot(v, v);

	if (d2 < radius * radius) {
		hit.t = t;
		hit.position = p;
		hit.normal = -ray.direction;
		hit.color = Vec3f(10.0f, 10.0f, 10.0f);
		return true;
	}

	return false;
}

unsigned int Point::num_primitives() const
{
	return 1;
}

AABB Point::get_bbox() const
{
	return AABB(center - radius, center + radius);
}

AABB Point::get_bbox(unsigned int i) const
{
	return get_bbox();
}
