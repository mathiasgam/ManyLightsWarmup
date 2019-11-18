#include "Material.h"

Material::Material() : name("default") {
	ior = 1.5f;
	shininess = 0.0f;
	diffuse = Vec3f(0.8f, 0.8f, 0.8f);
	ambient = Vec3f(0.2f, 0.2f, 0.2f);
	specular = Vec3f(0.0f, 0.0f, 0.0f);
	emissive = Vec3f(0.0f, 0.0f, 0.0f);
	transmission = Vec3f(0.0f, 0.0f, 0.0f);
	illum = 2;
}