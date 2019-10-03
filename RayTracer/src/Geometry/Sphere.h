#pragma once

#include "Geometry.h"

#include "Vec3f.h"
#include "Ray.h"
#include "HitInfo.h"
#include "Structure/AABB.h"

class Sphere: Geometry
{
public:

	Vec3f center;
	float radius;

	Sphere() :center(0, 0, 0), radius(1) {}
	Sphere(Vec3f center, float radius);
	~Sphere();

	/// intersect primitive inside the geometry at index i
	bool intersect(Ray& ray, HitInfo& hit, unsigned int i) const;
	/// return the number of primitives in the geometry
	size_t num_primitives() const { return 1; }
	/// the AABB for the whole geometry
	AABB get_bbox() const;
	/// the AABB for the primitive at index i
	AABB get_bbox(unsigned int i) const { return get_bbox(); }
};

