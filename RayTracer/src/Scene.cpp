#include "Scene.h"

#include <iostream>
#include <fstream>

#include "OBJModel.h"
#include "Geometry/Point.h"

Scene::~Scene()
{
	for (OBJModel* model : models)
		delete model;

	for (PointLight* light : lights)
		delete light;

	for (const Material* mat : materials)
		delete mat;
}

void Scene::AddMesh(const TriMesh* trimesh, const Vec3f color, Vec3f transform)
{
	std::cerr << "TriMesh Direct Loading not supported!" << std::endl;
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

void Scene::LoadMTL(const std::string_view filename, const std::string_view basepath)
{
	std::string fullPath = basepath.data();
	fullPath += filename.data();
	std::ifstream file(fullPath);
	if (!file) {
		std::cerr << "Could not open file: " << filename << "!";
	}

	Material* p_current_material = nullptr;

	std::string buf;
	while (file >> buf) {
		switch (buf[0])
		{
		case 'n':
			if (buf == "newmtl") {
				// get name of the new material
				std::string name;
				file >> name;

				// push a new material to the vector
				p_current_material = new Material();
				p_current_material->name = name;
				materials.push_back(p_current_material);
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'K':
			if (buf == "Ka") {
				float r, g, b;
				file >> r >> g >> b;
				p_current_material->ambient = Vec3f(r, g, b);
			}
			else if (buf == "Kd") {
				float r, g, b;
				file >> r >> g >> b;
				p_current_material->diffuse = Vec3f(r, g, b);
			}
			else if (buf == "Ks") {
				float r, g, b;
				file >> r >> g >> b;
				p_current_material->specular = Vec3f(r, g, b);
			}
			else if (buf == "Ke") {
				float r, g, b;
				file >> r >> g >> b;
				p_current_material->emissive = Vec3f(r, g, b);
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'N':
			if (buf == "Ns") {
				float exponent;
				file >> exponent;
				p_current_material->shininess = exponent;
			}
			else if (buf == "Ni") {
				float ior;
				file >> ior;
				p_current_material->ior = ior;
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'd':
			if (buf == "d") {
				float d;
				file >> d;
				p_current_material->transmission = Vec3f(d);
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'T':
			if (buf == "Tr") {
				float d;
				file >> d;
				p_current_material->transmission = Vec3f(1.0f - d);
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		default:
			file.ignore(1024, '\n');
			break;
		}
	}
	std::cout << "Loaded file: " << filename << std::endl;
}

void Scene::LoadOBJFile(const std::string_view filename, const std::string_view basepath)
{
	std::string full_path = basepath.data();
	full_path += filename.data();
	std::ifstream file(full_path);
	if (!file) {
		std::cerr << "Could not open file: " << filename << "!";
	}

	//TriMesh* CurrentMesh = nullptr;
	TriMesh* CurrentMesh = new TriMesh();
	meshes.push_back(CurrentMesh);
	unsigned int current_material = 0;

	std::string buf;
	while (file >> buf) {
		switch (buf[0])
		{
		case 'v':
			if (buf == "v") {
				float x, y, z;
				file >> x >> y >> z;
				CurrentMesh->geometry.addVertex(Vec3f(x, y, z));
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'f':
			if (buf == "f") {
				unsigned int f0, f1, f2;
				file >> f0 >> f1 >> f2;
				CurrentMesh->geometry.addFace(Vec3ui(f0 - 1, f1 - 1, f2 - 1));
				CurrentMesh->material_indecies.push_back(current_material);
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'u':
			if (buf == "usemtl") {
				std::string name;
				file >> name;
				bool found = false;
				for (int i = 0; i < materials.size(); i++) {
					if (materials[i]->name == name) {
						current_material = i;
						found = true;
					}
				}
				if (!found) {
					std::cerr << "Failed to find material with name: " << name << std::endl;
					current_material = 0;
				}
			}
			file.ignore(1024, '\n');
			break;
		case 'm':
			if (buf == "mtllib") {
				std::string libname;
				file >> libname;
				std::cout << "Loading Material: " << libname << std::endl;
				LoadMTL(libname, basepath);
			}
			break;
		case 'o':
			if (buf == "o") {
				std::string name;
				file >> name;
				std::cout << "Creating new mesh: " << name << std::endl;
				//CurrentMesh = new TriMesh();
				//meshes.push_back(CurrentMesh);
			}
		default:
			file.ignore(1024, '\n');
			break;
		}
	}
	std::cout << "Loaded file: " << filename << std::endl;
}

void Scene::LoadScene(const std::string_view filename, const std::string_view basepath)
{
	std::cout << "Loading scene: \"" << filename << "\", from: \"" << basepath << "\"" << std::endl;
	std::string fullPath = basepath.data();
	fullPath += filename.data();
	std::ifstream file(fullPath);
	if (!file) {
		std::cerr << "Could not open file: \"" << filename << "\"!";
	}

	std::string buf;
	while (file >> buf) {
		switch (buf[0])
		{
		case 'N':
			if (buf == "NAME") {
				std::string name;
				file >> name;
				this->name = name;
				std::cout << "- Scene name: " << name << std::endl;
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'O':
			if (buf == "OBJ") {
				std::string name;
				file >> name;
				this->LoadOBJFile(name, basepath);
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'C':
			if (buf == "CAMPOS") {
				float x, y, z;
				file >> x >> y >> z;
				this->cam.SetPosition(Vec3f(x, y, z));
				std::cout << "- Camera position: " << Vec3f(x, y, z) << std::endl;
			}
			else if (buf == "CAMLOOK") {
				float x, y, z;
				file >> x >> y >> z;
				this->cam.LookAt(Vec3f(x, y, z));
				std::cout << "- Camera looking at: " << Vec3f(x, y, z) << std::endl;
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'S':
			if (buf == "STRUC") {
				float t;
				file >> t;
				this->threshold = t;
				std::cout << "- Threshold: " << t << std::endl;
			}
			else {
				file.ignore(1024, '\n');
			}
		default:
			file.ignore(1024, '\n');
			break;
		}
	}
}

void Scene::prepare()
{
	std::cout << "Preparing scene:\n";
	std::cout << "- Models: " << models.size() << std::endl;
	std::cout << "- Planes: " << planes.size() << std::endl;
	std::cout << "- Lights: " << lights.size() << std::endl;

	BVHLights.init(lights);

	for (PointLight* light : lights) {
		AddPoint(light->position, 0.08f);
	}

#if USE_LIGHT_TREE
	float sum_length = 0;
	unsigned int edges = 0;
	for (Line& line : BVHLights.GetTreeEdges()) {
		AddLine(line.p1, line.p2, 0.02f, line.color);
		edges++;
		sum_length += (line.p1 - line.p2).length();
	}

	std::cout << "num edges: " << edges << std::endl;
	std::cout << "avg edge length: " << sum_length / edges << std::endl;
#endif

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

