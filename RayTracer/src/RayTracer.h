#pragma once

#include "Structure/Structure.h"

class RayTracer
{
protected:
	std::shared_ptr<Structure> search_structure;

public:
	RayTracer(std::shared_ptr<Structure> search_structure):search_structure(search_structure){}
	~RayTracer();
};

