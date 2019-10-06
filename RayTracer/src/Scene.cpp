#include "Scene.h"

#include "OBJModel.h"

Scene::~Scene()
{
	for (OBJModel* model : models) {
		delete model;
	}
}

void Scene::AddMesh(const char* filename, Vec3f transform)
{
	OBJModel* model = new OBJModel(filename);
	models.push_back(model);
	model->meshes[0].normalize();
	model->meshes[0].translate(transform);
	meshes.push_back(&model->meshes[0]);
}

void Scene::AddPlane(const Vec3f position, const Vec3f normal)
{
	planes.push_back(new Plane(position, normal));
}

void Scene::AddLight(const Vec3f position, const Vec3f color)
{
	lights.emplace_back(position, color);
}

void Scene::prepare()
{
	BVHMesh.init(meshes, planes);
	BVHLights.build(lights);
}


