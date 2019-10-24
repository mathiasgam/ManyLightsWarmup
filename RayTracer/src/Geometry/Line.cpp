#include "Line.h"

Line::Line(const Vec3f p1, const Vec3f p2, const float radius):
	p1(p1), p2(p2), radius(radius)
{
}

Line::~Line()
{
}

bool Line::intersect(Ray ray, HitInfo&, Vec3f& dirfrac) const
{
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
