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
	bool contained(const Vec3f& p) const;
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

inline float CosineBound(const Vec3f& position, const Vec3f& normal, const AABB& bbox)
{
#ifdef DEBUG
	float length = normal.length();
	if (length < 0.999999f || length > 1.000001f) {
		std::cerr << "Normal not nomalized: " << length << std::endl;
		assert(false);
	}
#endif // DEBUG

	Vec3f t1 = (bbox.p_max - position);
	Vec3f t2 = (bbox.p_min - position);

	Vec3f z = max(t1 * normal, t2 * normal);
	Vec3f xy = min(t1 * (1.0f - normal), t2 * (1.0f - normal));

	return dot(normal, (z+xy).normalized());

	
}

