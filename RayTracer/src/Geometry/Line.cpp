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
	const Vec3f diff = ray.center - p1;
	//const Vec3f tangent = cross(line, ray.direction).normalized();
	//const Vec3f x = cross(ray.direction, line).normalized();

	float t = dot(diff, line.normalized());

	// Find vectors for edges sharing v0
	Vec3f e0 = p2 - p1;
	Vec3f e1 = cross(line, diff).normalized() * radius;

	// Find face normal
	Vec3f n = cross(e0, e1);

	// Compute ray-plane intersection
	float q = dot(ray.direction, n);
	if (fabsf(q) < 1.0e-12f)
		return false;
	q = 1.0f / q;
	Vec3f o_to_v0 = p1 - ray.center;
	t = dot(o_to_v0, n) * q;

	// Check distance to intersection
	if (t < ray.t_min || t > ray.t_max)
		return false;

	// Find barycentric coordinates
	Vec3f n_tmp = cross(o_to_v0, ray.direction);
	float v = dot(n_tmp, e1) * q;
	if (v < 0.0f || v > 1.0f)
		return false;
	float w = fabsf(dot(n_tmp, e0)) * q;
	if (w < 0.0f || w > 1.0f)
		return false;

	// Let the counterclockwise wound side face forward
	if (q > 0)
		n = -n;
	return true;
	
	/*
	Vec3f diff = p1 - ray.center;

	// Find vectors for edges sharing v0
	Vec3f line = p2 - p1;
	float length = line.length();

	float t1 = dot(line.normalized(), diff);
	if (t1 < 0.0f || t1 > length) {
		return false;
	}
	return true;

	// Find face normal
	Vec3f tangent = cross(line, -ray.direction).normalized();
	Vec3f n = cross(line, tangent).normalized();

	Vec3f o_to_v0 = p1 - ray.center;
	float t = dot(o_to_v0, n);

	// Check distance to intersection
	//if (t < ray.t_min || t > ray.t_max)
	//	return false;

	// Find barycentric coordinates
	//Vec3f n_tmp = cross(e0, o_to_v0);
	float v = fabsf(dot(o_to_v0, line.normalized()));
	if (v < 0.0f || v > length) 
		return false;
	float w = fabsf(dot(o_to_v0, tangent));
	if (w < 0.0f || w > radius)
		return false;

	return true;
	*/
}

unsigned int Line::num_primitives() const
{
	return 1;
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
