#pragma once
#include "Vec3f.h"
#include "RayTracer/Ray.h"
#include "RayTracer/HitInfo.h"

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

	Vec3f color = Vec3f(1.0f, 1.0f, 1.0f);

	bool intersect(Ray ray, HitInfo&, Vec3f& dirfrac) const;
	bool intersect(Ray ray, Vec3f& dirfrac) const { return intersect(ray, HitInfo(), dirfrac); }

	friend inline std::ostream& operator<<(std::ostream&os, const Plane& v)
	{
		return os << "Plane(" << v.position << "," << v.normal << ")";
	}
};



