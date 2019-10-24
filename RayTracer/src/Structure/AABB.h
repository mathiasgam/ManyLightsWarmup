#pragma once

#include "Vec3f.h"
#include "Ray.h"

#include <vector>

class AABB
{
private:

public:

	Vec3f p_min, p_max;

	bool intersect(const Ray&)const;
	bool intersect(const Ray& ray, const Vec3f& dirfrac)const;
	Vec3f center()const;
	Vec3f size()const;
	float area()const;

	void add_point(const Vec3f p);
	void add_bbox(const AABB& aabb);

	AABB();
	AABB(const AABB& bbox) { p_min = bbox.p_min; p_max = bbox.p_max; }
	AABB(std::vector<AABB> const&);
	AABB(Vec3f min, Vec3f max);
	AABB(Vec3f p);
	AABB(AABB, AABB);
	~AABB();

	
};

inline std::ostream& operator<<(std::ostream&os, const AABB& bbox)
{
	return os << "[ " << bbox.p_min << "," << bbox.p_max << " ]";
}



