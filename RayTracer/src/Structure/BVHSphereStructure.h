#pragma once
#include "Structure.h"
#include "Geometry/Sphere.h"

#include <vector>
#include <string>

class BVHSphereStructure
{
public:
	BVHSphereStructure(std::vector<Sphere>);
	~BVHSphereStructure();

	bool trace(Ray& ray, HitInfo& hit);

private:

	struct Primitive {
		AABB aabb;
		int GeometryIndex;
	};

	std::vector<Primitive> primitives;

	std::vector<float> cx;
	std::vector<float> cy;
	std::vector<float> cz;
	std::vector<float> r;

	enum BVHAxis {
		axisX,
		axisY,
		axisZ,
		leaf
	};

	struct BVHNode {
		BVHAxis axis;
		AABB aabb;
		BVHNode* leftChild;
		BVHNode* rightChild;
		int id_left;
		int id_right;

		~BVHNode() {
			delete leftChild;
			delete rightChild;
		}
	};

	


	BVHNode* root;

	bool traceBVHNode(Ray& ray, HitInfo& hit, BVHNode* node);
	bool tracePrimitive(Ray& ray, HitInfo& hit, int index);

	BVHNode* build(std::vector<Primitive>& primitives, int left, int right);

	void printTree(BVHNode* p_node, std::string prefix);

};

