#pragma once
#include "Structure/Structure.h"
#include "Geometry/Geometry.h"
#include "Geometry/Plane.h"
#include <vector>

class SAHBVHStructure :
	public Structure
{
public:
	SAHBVHStructure() : objects(0), nodes(0), primitives(0) {}
	~SAHBVHStructure();

	virtual void init(const std::vector<const Geometry*>& geometry, const std::vector<const Plane*>& planes);
	virtual bool closest_hit(Ray& ray, HitInfo& hit) const;
	virtual bool any_hit(Ray& ray) const;

	virtual bool trace(Ray& ray, HitInfo& hit) { return closest_hit(ray, hit); }

	void print();

private:

	enum NodeType
	{
		axisX,
		axisY,
		axisZ,
		leaf
	};

	struct BVHNode {
		NodeType type;
		AABB bbox;
		unsigned int leftChild; // double as start index of primitives
		unsigned int rightChild; // double as end index of primitives
	};

	struct Primitive
	{
		unsigned int object;
		unsigned int index;
	};

	/// primitive struct for constructing the BVH structure
	struct Primitive_Fat {
		unsigned int object;
		unsigned int index;
		AABB bbox;
		Vec3f center;
	};

	unsigned int root_index;

	std::vector<const Geometry*> objects;
	std::vector<const Plane*> planes;

	std::vector<BVHNode> nodes;
	std::vector<Vec3f> centers;

	std::vector<Primitive> primitives;

	bool closest_hit_recurse(Ray&, HitInfo&, unsigned int i, const Vec3f& dirfrac) const;
	bool any_hit_recurse(Ray&, unsigned int i, const Vec3f& dirfrac) const;

	bool closest_plane(Ray&, HitInfo&, Vec3f& dirfrac) const;
	bool any_plane(Ray&, Vec3f& dirfrac) const;

	AABB recalculate_bboxes(unsigned int index);

	void swap(Primitive_Fat& left, Primitive_Fat& right);
	unsigned int build(std::vector<Primitive_Fat>& primitives, unsigned int start, unsigned int end);
};

