#pragma once

#include "Geometry/TriMesh.h"

class OBJModel
{
public:
	OBJModel(const char* filename, const Vec3f color);
	~OBJModel();

	std::vector<TriMesh> meshes;
	Vec3f color;
};

