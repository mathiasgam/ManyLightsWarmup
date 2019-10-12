#pragma once

#include "Util/Image.h"
#include "Scene.h"

#include "Vec3f.h"

struct TraceResult
{
	Vec3f color;
	HitInfo hit;
	unsigned int num_lights;
};

class RayTracer
{
protected:
	const Scene* p_scene = nullptr;
	float LightThreshold = 0.01f;

public:
	RayTracer(Scene* scene) : p_scene(scene) {}
	~RayTracer(){}

	TraceResult trace(Ray ray);
	void SetLightThreshold(float t) { LightThreshold = t; }

	/// Determines if there's a clear line of sight between point A and point B
	bool Occluded(Ray& ray) const;
	//RayTracer(std::shared_ptr<Structure> search_structure):search_structure(search_structure){}
};

