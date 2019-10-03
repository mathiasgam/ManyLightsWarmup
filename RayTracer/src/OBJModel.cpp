#include "OBJModel.h"

//#define TINYOBJLOADER_IMPLEMENTATION
//#include "tinyobjloader.h"

#include <iostream>
#include <fstream>
#include <string>


OBJModel::OBJModel(const char * filename)
{
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Could not open file: " << filename << "!";
	}

	meshes.push_back(TriMesh());
	TriMesh& mesh = meshes[0];
	std::string buf;
	while (file >> buf) {
		switch (buf[0])
		{
		case 'v':
			if (buf == "v") {
				float x, y, z;
				file >> x >> y >> z;
				mesh.geometry.addVertex(Vec3f(x, y, z));
			}
			else {
				file.ignore(1024, '\n');
			}
			break;
		case 'f':
			if (buf == "f") {
				unsigned int f0, f1, f2;
				file >> f0 >> f1 >> f2;
				mesh.geometry.addFace(Vec3ui(f0-1, f1-1, f2-1));
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
	std::cout << "vertices: " << mesh.geometry.num_vertices()<< "\n";
	std::cout << "faces: " << mesh.geometry.num_faces() << "\n";

	/*
	for (auto& v : mesh.geometry.vertices) {
		std::cout << "v: " << v << "\n";
	}
	for (auto& f : mesh.geometry.faces) {
		std::cout << "f: " << f << "\n";
	}
	*/
	
}

OBJModel::~OBJModel()
{
}
