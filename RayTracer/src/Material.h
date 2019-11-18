#pragma once

#include <string>

#include "Vec3f.h"
#include "Vec4f.h"

class Material
{

public:
	/// Name of material
	std::string name;

	/// Diffuse reflection
	Vec3f diffuse;
	/// Ambient reflection
	Vec3f ambient;
	/// Specular reflection
	Vec3f specular;
	/// Emissive
	Vec3f emissive;
	/// Specular exponent
	float shininess;
	/// Index of refraction
	float ior;
	/// Transmission filter
	Vec3f transmission;

	int illum;

public:
	Material();
};

