#pragma once

#include "config.h"

#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Geometry/TriMesh.h"
#include "Geometry/Plane.h"
#include "PointLight.h"
#include "OBJModel.h"
#include "Material.h"
#include "Camera/PinHoleCamera.h"

#include "Structure/Structure.h"
#include "Structure/LBVHStructure.h"

#include "LightStructure/LightStructure.h"
#include "LightStructure/LightArray.h"
#include "LightStructure/LightTree.h"
#include "LightStructure/BULightTree.h"

class Scene
{
private:

	std::string name;
	std::vector<const Material*> materials;

	std::vector<OBJModel*> models;
	std::vector<const Geometry*> meshes;
	std::vector<const Plane*> planes;

	std::vector<PointLight*> lights;

	std::vector<const Geometry*> visuals;

	LBVHStructure BVHMesh;
	LBVHStructure BVHVisuals;

#if USE_LIGHT_TREE
	BULightTree BVHLights;
#else
	LightArray BVHLights;
#endif
	float threshold = 0.02f;

	//LightTree BVHLights;

	Vec3f ambient = Vec3f(0.1f, 0.1f, 0.1f);

	PinHoleCamera cam = PinHoleCamera();

public: /// Public variables
	const Vec3f background_color = Vec3f(0.1f, 0.2f, 0.15f);
	const float scene_epsilon = 0.0001f;

public: /// Public Functions
	Scene() : name("default"), materials(0), meshes(0), planes(0), lights(0), visuals(0), BVHMesh(), BVHVisuals(), BVHLights() {
		Material* default_material = new Material();
		materials.push_back(default_material);
	}

	~Scene();

	void AddMesh(const TriMesh* trimesh, const Vec3f color, Vec3f transform);
	void AddPoint(const Vec3f position, const float radius);
	void AddLine(const Vec3f p1, const Vec3f p2, const float radius, Vec3f color);
	void AddPlane(const Vec3f position, const Vec3f color, const Vec3f normal);
	void AddLight(const Vec3f position, const Vec3f color);

	void LoadMTL(const std::string_view filename, const std::string_view basepath);
	void LoadOBJFile(const std::string_view filename, const std::string_view basepath);

	void LoadScene(const std::string_view filename, const std::string_view basepath);

	unsigned int GetNumModels() const { return static_cast<unsigned int>(models.size()); }
	unsigned int GetNumMeshes() const { return static_cast<unsigned int>(meshes.size()); }
	unsigned int GetNumLights() const { return static_cast<unsigned int>(lights.size()); }
	unsigned int GetNumPlanes() const { return static_cast<unsigned int>(planes.size()); }
	float GetThreshold() const { return threshold; }
	const PinHoleCamera& GetCamera() const { return cam; }

	const Material* GetMaterial(unsigned int index) const { return materials[index]; }

	void prepare();

	bool closest_hit(Ray& ray, HitInfo& hit) const { return BVHMesh.closest_hit(ray, hit); }
	bool any_hit(Ray& ray) const { return BVHMesh.any_hit(ray); }
	bool trace_visuals(Ray& ray, HitInfo& hit) const { return BVHVisuals.closest_hit(ray, hit); }

	std::vector<PointLight*> GetLights(const HitInfo& hit, float threshold) const { return BVHLights.GetLights(hit, threshold); }
	void SetAmbient(const Vec3f& color);
	Vec3f GetAmbient(const Vec3f& position) const;
	Vec3f GetBackground(const Vec3f& direction) const;
};

