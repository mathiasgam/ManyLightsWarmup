#pragma once

#include "Geometry/TriMesh.h"

class OBJModel
{
public:
	OBJModel(const char* filename);
	~OBJModel();

	std::vector<TriMesh> meshes;
};

