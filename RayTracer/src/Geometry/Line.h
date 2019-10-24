#pragma once

#include "Geometry.h"

class Line :
	public Geometry
{
private:
	Vec3f p1;
	Vec3f p2;
	float radius;

public:
	Line(const Vec3f p1, const Vec3f p2, const float radius);
	~Line();

	bool intersect(Ray ray, HitInfo&, Vec3f& dirfrac) const;
	bool intersect(Ray ray, Vec3f& dirfrac) const { return intersect(ray, HitInfo(), dirfrac); }

	/// return the number of primitives in the geometry
	virtual size_t num_primitives() const;
	/// the AABB for the whole geometry
	virtual AABB get_bbox() const;
	/// the AABB for the primitive at index i
	virtual AABB get_bbox(unsigned int i) const;

	friend inline std::ostream& operator<<(std::ostream& os, const Line& v)
	{
		return os << "Line(" << v.p1 << "," << v.p2 << ")";
	}
};

