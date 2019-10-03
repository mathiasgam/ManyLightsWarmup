#pragma once
#include "Structure.h"

#include "Geometry/Sphere.h"
#include "HitInfo.h"

#include <vector>


class ArraySphereStructure :
	public Structure
{
public:
	ArraySphereStructure(std::vector<Sphere>);
	~ArraySphereStructure();

	bool trace(Ray& ray, HitInfo& hit);

private:
	std::vector<float> cx;
	std::vector<float> cy;
	std::vector<float> cz;
	std::vector<float> r;
};

