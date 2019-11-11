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
	Vec4f diffuse;
	/// Ambient reflection
	Vec4f ambient;
	/// Specular reflection
	Vec4f specular;
	/// Specular exponent
	float shininess;
	/// Index of refraction
	float ior;
	/// Transmission filter
	Vec3f transmission;

	int illum;

public:
	Material() : name("default") {
		ior = 1.5f;
		shininess = 0.0f;
		diffuse = Vec4f(0.8f, 0.8f, 0.8f, 1.0f);
		ambient = Vec4f(0.2f, 0.2f, 0.2f, 1.0f);
		specular = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		transmission = Vec3f(0.0f, 0.0f, 0.0f);
		illum = 2;
	}
};

