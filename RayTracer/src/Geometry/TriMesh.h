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
	//IndexedFaceSet normals;
	std::vector<unsigned int> material_indecies;

	TriMesh();
	~TriMesh();

	/// tests if the ray has hit the triangle found at index i
	bool intersect(Ray& ray, HitInfo& hit, unsigned int i) const;

	/// return the number of faces in the mesh
	unsigned int num_primitives() const { return static_cast<unsigned int>(geometry.num_faces()); }

	/// returns the bbox for the whole mesh
	AABB get_bbox() const;
	/// returns the bbox for a single triangle face
	AABB get_bbox(unsigned int i) const;

	void normalize();
	void translate(const Vec3f& diff);

	void addVertex(const Vec3f& vertex);
	//void addNormal(const Vec3f& normal);
	void addFace(const Vec3ui& vertices, const Vec3ui& normals, const unsigned int material_index);
};

