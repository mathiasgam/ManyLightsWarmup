#pragma once

#include "Util/Image.h"
#include "Scene.h"

#include "Vec3f.h"

struct TraceResult
{
	Vec3f color;
	Vec3f visualColor;
	HitInfo hit;
	unsigned int num_lights;
	unsigned int num_rays;
	unsigned int num_occlusion_rays;
};

class RayTracer
{
protected:
	const Scene* p_scene = nullptr;
	float LightThreshold = 0.02f;

public:
	RayTracer(Scene* scene) : p_scene(scene) {}
	~RayTracer(){}

	TraceResult trace(Ray ray);
	void SetLightThreshold(float t) { LightThreshold = t; }

	/// Determines if there's a clear line of sight between point A and point B
	bool Occluded(Ray& ray) const;
	//RayTracer(std::shared_ptr<Structure> search_structure):search_structure(search_structure){}

private:
	Vec3f sample(Ray ray, int bounces) const;
	Vec3f SampleLights(const HitInfo& hit, const Vec3f& material, int& numLights, const float threshold) const;
};

