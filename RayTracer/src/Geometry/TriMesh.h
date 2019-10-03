#pragma once
#include "Geometry/Geometry.h"
#include <vector>

#include "IndexedFaceSet.h"
#include "Ray.h"
#include "HitInfo.h"
#include "Structure/AABB.h"

class TriMesh :
	public Geometry
{
protected:

public:
	IndexedFaceSet geometry;
	IndexedFaceSet normals;


	TriMesh();
	~TriMesh();

	/// tests if the ray has hit the triangle found at index i
	bool intersect(Ray& ray, HitInfo& hit, unsigned int i) const;

	/// return the number of faces in the mesh
	size_t num_primitives() const { return geometry.num_faces(); }

	/// returns the bbox for the whole mesh
	AABB get_bbox() const;
	/// returns the bbox for a single triangle face
	AABB get_bbox(unsigned int i) const;

	void normalize();
	void translate(const Vec3f& diff);
};

