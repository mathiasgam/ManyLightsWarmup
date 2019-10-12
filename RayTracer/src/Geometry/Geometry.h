#pragma once
#include "Ray.h"
#include "HitInfo.h"

#include "Structure/AABB.h"

class Geometry
{
public:
	Geometry();
  ~Geometry();

	/// intersect primitive inside the geometry at index i
	virtual bool intersect(Ray& ray, HitInfo& hit, unsigned int i) const = 0;
	virtual bool intersect(Ray& ray, HitInfo& hit) const { return intersect(ray, hit); };
	virtual bool intersect(Ray& ray, unsigned int i) const { return intersect(ray, HitInfo(), i); };
	/// return the number of primitives in the geometry
	virtual size_t num_primitives() const = 0;
	/// the AABB for the whole geometry
	virtual AABB get_bbox() const = 0;
	/// the AABB for the primitive at index i
	virtual AABB get_bbox(unsigned int i) const = 0;

	Vec3f color = Vec3f(1.0f, 1.0f, 1.0f);

};

