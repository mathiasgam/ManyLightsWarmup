#pragma once

#include "Vec3f.h"
#include "Material.h"

#include <limits>
#define MAX_DIST std::numeric_limits<float>::max()


class HitInfo
{
public:

	float t;
	Vec3f position;
	Vec3f normal;
	Vec3f color;
	Vec3f incomming;
	int trace_depth;
	unsigned int material_index;
	const Material* p_material;
	
	HitInfo();
	~HitInfo();
};


