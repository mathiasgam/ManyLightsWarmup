#include "AABB.h"

#include <limits>
#include <vector>


#include <iostream>


bool AABB::intersect(const Ray& ray)const
{
	Vec3f dirfrac = Vec3f(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);
	
	return intersect(ray, dirfrac);
}

void swap(float& a, float& b) {
	float tmp = a;
	a = b;
	b = tmp;
}

bool AABB::intersect(const Ray& ray, Vec3f& dirfrac) const
{
	float tmin = ray.t_min;
	float tmax = ray.t_max;

	for (int i = 0; i < 3; ++i) {
		float t1 = (p_min[i] - ray.center[i])*dirfrac[i];
		float t2 = (p_max[i] - ray.center[i])*dirfrac[i];

		tmin = fmaxf(tmin, fminf(t1, t2));
		tmax = fminf(tmax, fmaxf(t1, t2));
	}

	return tmax > fmaxf(tmin, 0.0);
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
	for (int i = 0; i < 3; i++) {
		p_min[i] = fminf(p_min[i], p[i]);
		p_max[i] = fmaxf(p_max[i], p[i]);
	}
	
}

void AABB::add_bbox(const AABB& aabb)
{
	for (int i = 0; i < 3; i++) {
		p_min[i] = fminf(p_min[i], aabb.p_min[i]);
		p_max[i] = fmaxf(p_max[i], aabb.p_max[i]);
	}
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

AABB::AABB(AABB a, AABB b)
{
	for (int i = 0; i < 3; i++) {
		p_min[i] = fminf(a.p_min[i], b.p_min[i]);
		p_max[i] = fmaxf(a.p_max[i], b.p_max[i]);
	}
}


AABB::~AABB()
{
}
