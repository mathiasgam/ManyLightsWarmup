#pragma once

#include "Vec3f.h"
#include "Vec3ui.h"

#include <vector>
class IndexedFaceSet
{
protected:

public:
	std::vector<Vec3f> vertices;
	std::vector<Vec3ui> faces;
	/// initialize the arrays of the set to zero
	IndexedFaceSet() :vertices(0), faces(0) {}
	~IndexedFaceSet() {}

	/// adds the vertex v to the set, and returns the index
	inline unsigned int addVertex(const Vec3f& v) { vertices.push_back(v); return vertices.size() - 1; }
	/// adds the face f to the set, and returns the index
	inline unsigned int addFace(const Vec3ui& f) { faces.push_back(f); return faces.size() - 1; }

	/// returns the number of vertices in the set
	inline size_t num_vertices() const { return vertices.size(); }
	/// returns the number of faces in the set
	inline size_t num_faces() const { return faces.size(); }

	/// returns the vertex at index i in the set
	inline const Vec3f& vertex(unsigned int i) const { return vertices[i]; }
	/// returns the face at index i in the set
	inline const Vec3ui& face(unsigned int i) const { return faces[i]; }
};

