#pragma once

#include "Vec3f.h"
#include "RayTracer/Ray.h"
#include "RayTracer/HitInfo.h"
#include "RayTracer/Tracer.h"

class shader
{
public:
	virtual Vec3f shade(Ray ray, HitInfo hit, Tracer& tracer) = 0;
};