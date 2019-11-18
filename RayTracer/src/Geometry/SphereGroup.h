#pragma once
#include "Geometry/Geometry.h"
#include "Geometry/Sphere.h"
#include <vector>

class SphereGroup :
	public Geometry
{
protected:
	std::vector<Sphere> spheres;

public:
	SphereGroup(): spheres(0){}
	~SphereGroup();

	void addSphere(const Vec3f& center, float radius) { 
		Sphere s;
		s.center = center;
		s.radius = radius;
		spheres.push_back(s);
	}
	void clear() { spheres.clear(); }

	bool intersect(Ray& ray, HitInfo& hit, unsigned int i) const { return spheres[i].intersect(ray, hit, 0); }
	
	/// return the number of faces in the mesh
	unsigned int num_primitives() const { return static_cast<unsigned int>(spheres.size()); }

	/// returns the bbox for the whole mesh
	AABB get_bbox() const;
	/// returns the bbox for a single triangle face
	AABB get_bbox(unsigned int i) const;
};

