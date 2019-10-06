#pragma once
#include "Structure/Structure.h"
#include "MortonCode.h"

#include "Geometry/Geometry.h"
#include "Geometry/Plane.h"

#include "Geometry/TriMesh.h"

#include <vector>

class LBVHStructure :
	public Structure
{
private:

	enum NodeType {
		Internal,
		Leaf
	};

	struct Node {
		int type;
		AABB bbox;
		Vec3f center;
		Node* ChildA; // double as start index in primitives if leaf
		Node* ChildB; // double as end index in primitives if leaf
		int primitive;
	};

	struct Primitive {
		const Geometry* Object;
		unsigned int index;
		MortonCode3 code;

		bool operator<(const Primitive& other) const
		{
			return code < other.code;
		}
	};

	//std::vector<const Geometry*> objects;
	std::vector<const Plane*> planes;

	std::vector<Primitive> primitives;

	Node* root = nullptr;

public:

	LBVHStructure() : planes(0), primitives(0) {}
	~LBVHStructure();
	virtual void init(const std::vector<const Geometry*>& geometry, const std::vector<const Plane*>& planes);
	virtual bool closest_hit(Ray& ray, HitInfo& hit) const;
	virtual bool any_hit(Ray& ray, HitInfo& hit) const;

private:

	bool closest_hit_recurse(Ray&, HitInfo&, Node& node, const Vec3f& dirfrac) const;
	bool any_hit_recurse(Ray&, HitInfo&, Node& node, const Vec3f& dirfrac) const;

	bool closest_plane(Ray&, HitInfo&, Vec3f& dirfrac) const;
	bool any_plane(Ray&, HitInfo&, Vec3f& dirfrac) const;

	Node* generateHierarchy(std::vector<Primitive>& sortedPrimitives, int first, int last);
	int findSplit(std::vector<Primitive>& sortedPrimitives, int first, int last);

	static void DestroyNode(Node* node) {
		return;
		static int count = 0;
		if (node->type == NodeType::Internal) {
			assert(node->ChildA != nullptr);
			assert(node->ChildB != nullptr);
			DestroyNode(node->ChildA);
			DestroyNode(node->ChildB);
		}
		//std::cout << "count: " << count++ << std::endl;
		delete node;
	}

	void calculateBBox(Node* node);

};

