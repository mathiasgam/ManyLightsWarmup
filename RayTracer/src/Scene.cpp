#include "Scene.h"

#include "OBJModel.h"
#include "Geometry/Point.h"

Scene::~Scene()
{
	for (OBJModel* model : models)
		delete model;

	for (PointLight* light : lights)
		delete light;
}

void Scene::AddMesh(const char* filename, const Vec3f color, Vec3f transform)
{
	OBJModel* model = new OBJModel(filename, color);
	models.push_back(model);
	//model->meshes[0].normalize();
	model->meshes[0].translate(transform);
	meshes.push_back(&model->meshes[0]);
}

void Scene::AddPoint(const Vec3f position, const float radius)
{
	Point* p = new Point(position, radius);
	visuals.push_back(p);
}

void Scene::AddLine(const Vec3f p1, const Vec3f p2, const float radius, Vec3f color) {
	Line* l = new Line(p1, p2, radius, color);
	visuals.push_back(l);
}

void Scene::AddPlane(const Vec3f position, const Vec3f color, const Vec3f normal)
{
	Plane* plane = new Plane(position, normal);
	plane->color = color;
	planes.push_back(plane);
}

void Scene::AddLight(const Vec3f position, const Vec3f color)
{
	PointLight* light = new PointLight(position, color);
	lights.push_back(light);
}

void Scene::prepare()
{
	std::cout << "Preparing scene:\n";
	std::cout << "- Models: " << models.size() << std::endl;
	std::cout << "- Planes: " << planes.size() << std::endl;
	std::cout << "- Lights: " << lights.size() << std::endl;

	BVHLights.init(lights);

	for (PointLight* light : lights) {
		AddPoint(light->position, 0.05f);
	}

	unsigned int edges = 0;
	for (Line& line : BVHLights.GetTreeEdges()) {
		AddLine(line.p1, line.p2, 0.01f, line.color);
		edges++;
	}

	std::cout << "num edges: " << edges << std::endl;

	BVHMesh.init(meshes, planes);
	std::vector<const Plane*> tmp = std::vector<const Plane*>();
	BVHVisuals.init(visuals, tmp);
}

void Scene::SetAmbient(const Vec3f& color)
{
	ambient = color;
}

Vec3f Scene::GetAmbient(const Vec3f& position) const
{
	return ambient;
}

Vec3f Scene::GetBackground(const Vec3f& direction) const
{
	return Vec3f(0.2f, 0.3f, 0.4f);
}

