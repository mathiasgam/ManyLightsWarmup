#include "Sphere.h"


Sphere::Sphere(Vec3f center, float radius):
	center(center), radius(radius)
{
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(Ray & ray, HitInfo & hit, unsigned int i) const
{
	float t0, t1;
	float radius2 = radius*radius;
	// geometric solution
	Vec3f L = center - ray.center;
	float tca = dot(L, ray.direction);

	// if (tca < 0) return false;
	float d2 = dot(L, L) - tca * tca;
	if (d2 > radius2) return false;
	float thc = sqrt(radius2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	if (t0 > t1) {
		float tmp = t0;
		t0 = t1;
		t1 = tmp;
	}

	if (t0 < 0) {
		t0 = t1; // if t0 is negative, let's use t1 instead 
		if (t0 < 0) return false; // both t0 and t1 are negative 
	}

	if (t0 < hit.t) {
		hit.t = t0;
		hit.position = ray.center + ray.direction * t0;
		hit.normal = (hit.position - center).normalized();
		return true;
	}

	return false;
}

AABB Sphere::get_bbox() const
{
	Vec3f _min = center - radius;
	Vec3f _max = center + radius;
	return AABB(_min, _max);
}
