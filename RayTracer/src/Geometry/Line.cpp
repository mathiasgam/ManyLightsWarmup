#include "Line.h"

Line::Line(const Vec3f _p1, const Vec3f _p2, const float _radius, Vec3f _color)
{
	p1 = _p1;
	p2 = _p2;
	radius = _radius;
	color = _color;
}

Line::~Line()
{
}

bool Line::intersect(Ray& ray, HitInfo& hit, unsigned int i) const
{
	const Vec3f line = p2 - p1;
	const Vec3f diff = p1 - ray.center;
	const Vec3f x = cross(ray.direction, line).normalized();

	float t = dot(x, diff);
	if (fabsf(t) < radius) {

		hit.t = ray.t_min;
		hit.position = ray.center + ray.direction * ray.t_min;
		//hit.t = t;
		//hit.position = p;
		hit.normal = -ray.direction;
		hit.color = color;
		return true;
	}
	return false;
}

size_t Line::num_primitives() const
{
	return static_cast<size_t>(1);
}

AABB Line::get_bbox() const
{
	AABB bbox = AABB();
	bbox.add_point(p1 - radius);
	bbox.add_point(p1 + radius);
	bbox.add_point(p2 - radius);
	bbox.add_point(p2 + radius);
	return bbox;
}

AABB Line::get_bbox(unsigned int i) const
{
	return get_bbox();
}
