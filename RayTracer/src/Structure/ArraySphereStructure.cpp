#include "ArraySphereStructure.h"

#include <iostream>


ArraySphereStructure::ArraySphereStructure(std::vector<Sphere> spheres)
{
	cx = std::vector<float>();
	cy = std::vector<float>();
	cz = std::vector<float>();
	r = std::vector<float>();
	const size_t size = spheres.size();
	cx.reserve(size);
	cy.reserve(size);
	cz.reserve(size);
	r.reserve(size);

	for (int i = 0; i < size; i++) {
		Sphere s = spheres[i];
		cx.push_back(s.center[0]);
		cy.push_back(s.center[1]);
		cz.push_back(s.center[2]);
		r.push_back(s.radius);
	}
}

ArraySphereStructure::~ArraySphereStructure()
{
}

bool ArraySphereStructure::trace(Ray& ray, HitInfo& hit)
{
	bool hasHit = false;
	const int size = static_cast<int>(r.size());
	for (int i = 0; i < size; i++) {
		Sphere s = Sphere(Vec3f(cx[i], cy[i], cz[i]), r[i]);
		if (s.intersect(ray, hit,0)) {
			hasHit = true;
		}
		
	}
	return hasHit;
}
