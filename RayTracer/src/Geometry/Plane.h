#pragma once
#include "Vec3f.h"
#include "Ray.h"
#include "HitInfo.h"

class Plane
{
protected:
	Vec3f position;
	Vec3f normal;
	Vec3f tangent;
	Vec3f binormal;
	float d;

public:
	Plane(Vec3f position, Vec3f normal);
	~Plane();



	bool intersect(Ray, HitInfo&, Vec3f& dirfrac) const;

	friend inline std::ostream& operator<<(std::ostream&os, const Plane& v)
	{
		return os << "Plane(" << v.position << "," << v.normal << ")";
	}
};



