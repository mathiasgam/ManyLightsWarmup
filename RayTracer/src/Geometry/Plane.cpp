#include "Plane.h"

Plane::Plane(Vec3f _position, Vec3f _normal)
{
	position = _position;
	normal = _normal.normalized();
	if (_normal[0] == 0 && _normal[1] == 1 && _normal[2] == 0) {
		tangent = Vec3f(1.0f, 0.0f, 0.0f);
	}
	else {
		tangent = cross(normal, Vec3f(0, 1, 0));
	}
	binormal = cross(normal, tangent);
	d = dot(normal, -position);
}

Plane::~Plane()
{
}

bool Plane::intersect(Ray ray, HitInfo& hit, Vec3f& dirfrac) const
{
	Vec3f o = ray.center;
	Vec3f d = ray.direction;

	float denom = dot(normal, d);
	if (fabsf(denom) < 0.00001f) return false;

	float t = dot((position - o), normal) / denom;

	if (t > ray.t_min && t < ray.t_max) {
		Vec3f position = o + d * t;
		hit.t = t;
		hit.position = position;
		hit.normal = normal;
		return true;
	}
	return false;
	/*
	float cos_theta = dot(normal, dirfrac);
	float t = -(dot(normal, ray.center) + d) * cos_theta;
	if (fabsf(cos_theta) < 1.0e-12f)
		return false;
	if (t < ray.t_min || t > ray.t_max)
		return false;
	hit.t = t;
	hit.normal = normal;
	hit.position = ray.center + ray.direction * t;
	return true;
	*/
}
