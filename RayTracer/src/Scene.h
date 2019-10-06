#pragma once

#include <vector>
#include <string>

#include "Geometry/TriMesh.h"
#include "Geometry/Plane.h"
#include "PointLight.h"
#include "OBJModel.h"

#include "Structure/Structure.h"
#include "Structure/LBVHStructure.h"

#include "LightTree.h"

class Scene
{
private:
	std::vector<OBJModel*> models;
	std::vector<const Geometry*> meshes;
	std::vector<const Plane*> planes;

	std::vector<PointLight> lights;

	LBVHStructure BVHMesh;
	LightTree BVHLights;

public:
	Scene() : meshes(0), planes(0), lights(0), BVHMesh(), BVHLights() {}
	~Scene();

	void AddMesh(const char* filename, Vec3f transform);
	void AddPlane(const Vec3f position, const Vec3f normal);
	void AddLight(const Vec3f position, const Vec3f color);

	void prepare();

	bool closest_hit(Ray& ray, HitInfo& hit) const { return BVHMesh.closest_hit(ray, hit); }
	bool any_hit(Ray& ray, HitInfo& hit) const { return BVHMesh.any_hit(ray, hit); }

	std::vector<PointLight> GetLights(Vec3f position, Vec3f normal, float threshold) const { return BVHLights.cut(position, normal, threshold); }
};

