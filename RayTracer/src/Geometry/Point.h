#pragma once

#include "Geometry.h"
#include "Vec3f.h"

/// A disc which is always pointing towards the ray direction
class Point :
	public Geometry
{
private:
	Vec3f center;
	float radius;

public:
	Point(const Vec3f center, const float radius);
	~Point();

	bool intersect(Ray& ray, HitInfo& hit, unsigned int i) const;

	/// return the number of primitives in the geometry
	virtual size_t num_primitives() const;
	/// the AABB for the whole geometry
	virtual AABB get_bbox() const;
	/// the AABB for the primitive at index i
	virtual AABB get_bbox(unsigned int i) const;

	friend inline std::ostream& operator<<(std::ostream& os, const Point& v)
	{
		return os << "Point(" << v.center << "," << v.radius << ")";
	}
};

