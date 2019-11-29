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

	Vec3f p_min = bbox.p_min;
	Vec3f p_max = bbox.p_max;

	Vec3f p[8];

	p[0] = Vec3f(p_min[0], p_min[1], p_min[2]);
	p[1] = Vec3f(p_max[0], p_min[1], p_min[2]);
	p[2] = Vec3f(p_max[0], p_max[1], p_min[2]);
	p[3] = Vec3f(p_min[0], p_max[1], p_min[2]);
	p[4] = Vec3f(p_min[0], p_min[1], p_max[2]);
	p[5] = Vec3f(p_max[0], p_min[1], p_max[2]);
	p[6] = Vec3f(p_max[0], p_max[1], p_max[2]);
	p[7] = Vec3f(p_min[0], p_max[1], p_max[2]);

	float max_cos_theta = 0.0f;
	for (int i = 0; i < 8; i++) {
		Vec3f diff = p[i] - position;
		float cos_theta = dot(normal, diff.normalized());
		max_cos_theta = max(cos_theta, max_cos_theta);
	}

	return max_cos_theta;

	/*
	Vec3f t1 = (bbox.p_max - position);
	Vec3f t2 = (bbox.p_min - position);

	//Vec3f z = max(t1 * normal, t2 * normal);
	float zmax = max(dot(t1, normal), dot(t2, normal));

	Vec3f tangent = cross(Vec3f(0, 1, 0), normal);
	Vec3f cotangent = cross(normal, tangent);

	if (z.max_componont() >= 0.0f) {
		float x = max(0.0f, min(dot(t1, tangent), dot(t2, tangent)));
		float y = max(0.0f, min(dot(t1, cotangent), dot(t2, cotangent)));
		//Vec3f xy = min(t1 * (1.0f - normal), t2 * (1.0f - normal));
		//xy = max(xy, Vec3f(0.0f));
		return zmax / sqrt(sqr(x) + sqr(y) + sqr(zmax));
		//return dot(normal, (z + xy).normalized());
	}
	else {
		//Vec3f xy = max(t1 * (1.0f - normal), t2 * (1.0f - normal));
		float xsqr = max(sqr(dot(t1, tangent)), sqr(dot(t2, tangent)));
		float ysqr = max(sqr(dot(t1, tangent)), sqr(dot(t2, tangent)));
		//return dot(normal, (z + xy).normalized());
		return zmax / sqrt(xsqr + ysqr + sqr(zmax));
	}
	*/
	
}

inline float minSqrDist(const Vec3f& position, const AABB& bbox) {

	Vec3f d = max(bbox.p_min - position, Vec3f(0.0f));
	d = max(d, position - bbox.p_max);
	return d.sqr_length();
	//Vec3f diff = bbox.center() - position;
	//diff = abs(diff) - (bbox.size() / 2.0f);
	//diff = max(diff, Vec3f(0.0f));
	//return (diff).sqr_length();

}

inline float maxSqrDist(const Vec3f& position, const AABB& bbox) {

	Vec3f d1 = abs(bbox.p_min - position);
	Vec3f d2 = abs(bbox.p_max - position);

	return max(d1,d2).sqr_length();
	//Vec3f diff = bbox.center() - position;
	//diff = abs(diff) - (bbox.size() / 2.0f);
	//diff = max(diff, Vec3f(0.0f));
	//return (diff).sqr_length();

}
