#pragma once

#include <vector>
#include <string>

#include "Geometry/TriMesh.h"
#include "Geometry/Plane.h"
#include "PointLight.h"
#include "OBJModel.h"

#include "Structure/Structure.h"
#include "Structure/LBVHStructure.h"

#include "LightStructure/LightStructure.h"
#include "LightStructure/LightArray.h"
#include "LightStructure/LightTree.h"
#include "LightStructure/BULightTree.h"

class Scene
{
private:
	std::vector<OBJModel*> models;
	std::vector<const Geometry*> meshes;
	std::vector<const Plane*> planes;

	std::vector<PointLight*> lights;

	std::vector<const Geometry*> visuals;

	LBVHStructure BVHMesh;
	LBVHStructure BVHVisuals;
	BULightTree BVHLights;
	//LightTree BVHLights;

	Vec3f ambient = Vec3f(0.1f, 0.1f, 0.1f);

public: /// Public variables
	const Vec3f background_color = Vec3f(0.1f, 0.2f, 0.15f);
	const float scene_epsilon = 0.0001f;

public: /// Public Functions
	Scene() : meshes(0), planes(0), lights(0), visuals(0), BVHMesh(), BVHVisuals(), BVHLights() {}
	~Scene();

	void AddMesh(const char* filename, const Vec3f color, Vec3f transform);
	void AddPoint(const Vec3f position, const float radius);
	void AddLine(const Vec3f p1, const Vec3f p2, const float radius, Vec3f color);
	void AddPlane(const Vec3f position, const Vec3f color, const Vec3f normal);
	void AddLight(const Vec3f position, const Vec3f color);

	unsigned int GetNumModels() { return models.size(); }
	unsigned int GetNumMeshes() { return meshes.size(); }
	unsigned int GetNumLights() { return lights.size(); }
	unsigned int GetNumPlanes() { return planes.size(); }

	void prepare();

	bool closest_hit(Ray& ray, HitInfo& hit) const { return BVHMesh.closest_hit(ray, hit); }
	bool any_hit(Ray& ray) const { return BVHMesh.any_hit(ray); }
	bool trace_visuals(Ray& ray, HitInfo& hit) const { return BVHVisuals.closest_hit(ray, hit); }

	std::vector<PointLight*> GetLights(Vec3f position, Vec3f normal, float threshold) const { return BVHLights.GetLights(position, normal, threshold); }
	void SetAmbient(const Vec3f& color);
	Vec3f GetAmbient(const Vec3f& position) const;
	Vec3f GetBackground(const Vec3f& direction) const;
};

