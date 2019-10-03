#pragma once

#include "Vec3f.h"

#include <limits>
#define MAX_DIST std::numeric_limits<float>::max()


class HitInfo
{
public:

	float t;
	Vec3f position;
	Vec3f normal;
	int trace_depth;
	
	HitInfo();
	~HitInfo();
};


