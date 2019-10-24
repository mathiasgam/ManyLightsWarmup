#include "AABB.h"

#include <limits>
#include <vector>


#include <iostream>


bool AABB::intersect(const Ray& ray)const
{
	//Vec3f dirfrac = Vec3f(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);
	Vec3f dirfrac = 1.0f / ray.direction;
	
	return intersect(ray, dirfrac);
}

void swap(float& a, float& b) {
	float tmp = a;
	a = b;
	b = tmp;
}

bool AABB::intersect(const Ray& ray, const Vec3f& dirfrac) const
{
	Vec3f t1 = (p_min - ray.center) * dirfrac;
	Vec3f t2 = (p_max - ray.center) * dirfrac;

	float tmin = min(t1, t2).max_componont();
	tmin = max(tmin, ray.t_min);

	float tmax = max(t1, t2).min_componont();
	tmax = min(tmax, ray.t_max);

	tmin = tmin > 0.0f ? tmin : 0.0f;

	return tmax > tmin;
	
}

Vec3f AABB::center()const
{
	return (p_min + p_max) / 2;
}

Vec3f AABB::size()const
{
	return p_max - p_min;
}

float AABB::area() const
{
	const Vec3f size = p_max - p_min;
	return dot(size,size);
}

void AABB::add_point(const Vec3f p)
{
	p_min = min(p_min, p);
	p_max = max(p_max, p);
}

void AABB::add_bbox(const AABB& aabb)
{
	p_min = min(p_min, aabb.p_min);
	p_max = max(p_max, aabb.p_max);
}


AABB::AABB()
{
	p_min = Vec3f(std::numeric_limits<float>::max());
	p_max = Vec3f(-std::numeric_limits<float>::max());
}

AABB::AABB(std::vector<AABB> const &bboxes)
{
	p_min = Vec3f(std::numeric_limits<float>::max());
	p_max = Vec3f(-std::numeric_limits<float>::max());

	for (int i = 0; i < 3; i++) {
		for (auto box : bboxes) {
			if (box.p_min[i] < p_min[i])
				p_min[i] = box.p_min[i];

			if (box.p_max[i] > p_max[i])
				p_max[i] = box.p_max[i];
		}
	}
}

AABB::AABB(Vec3f min, Vec3f max):
	p_min(min), p_max(max)
{
}

AABB::AABB(Vec3f p): 
	p_min(p), p_max(p)
{
}

AABB::AABB(AABB a, AABB b)
{
	p_min = min(a.p_min, b.p_min);
	p_max = max(a.p_max, b.p_max);
}


AABB::~AABB()
{
}
