#include "BVHSphereStructure.h"

#include <algorithm>
#include <math.h>

#include <random>

#include <iostream>



BVHSphereStructure::BVHSphereStructure(std::vector<Sphere>spheres)
{
	const size_t size = spheres.size();
	primitives = std::vector<Primitive>();
	for (int i = 0; i < size;i++) {
		Primitive p = {};
		p.aabb = spheres[i].get_bbox();
		p.GeometryIndex = i;
		primitives.push_back(p);
	}


	root = build(primitives, 0, size);

	
	//printTree(root, "");
	 

	cx = std::vector<float>();
	cy = std::vector<float>();
	cz = std::vector<float>();
	r = std::vector<float>();
	
	cx.reserve(size);
	cy.reserve(size);
	cz.reserve(size);
	r.reserve(size);

	for (int i = 0; i < size; i++) {
		Sphere s = spheres[i];
		cx.push_back(s.center[0]);
		cy.push_back(s.center[1]);
		cz.push_back(s.center[2]);
		r.push_back(s.radius);
	}
}


BVHSphereStructure::~BVHSphereStructure()
{
}

bool BVHSphereStructure::trace(Ray & ray, HitInfo & hit)
{
	Vec3f dirfrac = Vec3f(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);
	
	return traceBVHNode(ray, hit, root);
}

bool BVHSphereStructure::tracePrimitive(Ray& ray, HitInfo& hit, int i) {
	Sphere s = Sphere(Vec3f(cx[i], cy[i], cz[i]), r[i]);
	if (s.intersect(ray, hit,0)) {
		return true;
	}
	return false;
}

BVHSphereStructure::BVHNode * BVHSphereStructure::build(std::vector<Primitive>& primitives, int left, int right)
{
	BVHNode* node = new BVHNode();
	AABB aabb;
	for (int i = left; i < right; i++) {
		aabb = AABB(aabb, primitives[i].aabb);
	}
	node->aabb = aabb;

	const int num = right - left;

	if (num <= 2) {
		
		node->axis = leaf;
		node->id_left = primitives[left].GeometryIndex;
		node->id_right = primitives[right-1].GeometryIndex;
		return node;
	}

	// skip SAH for now
	int best_axis;
	Vec3f size = node->aabb.size();
	if (size[0] > size[1]) {
		if (size[2] > size[0]) {
			best_axis = 2;
		}
		else {
			best_axis = 0;
		}
	}
	else {
		best_axis = 1;
	}

	int best_index = left + num / 2;

	//std::cout << "left:" << left << ", right:" << right << "\n";
	//std::cout << "num: " << num << "best index: " << best_index << "\n";

	// split primatives on axis
	std::sort(primitives.begin() + left, primitives.begin() + right-1, [best_axis](const Primitive & a, const Primitive & b) -> bool {
		return a.aabb.center()[best_axis] < b.aabb.center()[best_axis];
	});

	node->leftChild = build(primitives, left, best_index);
	node->rightChild = build(primitives, best_index, right);
	return node;
}

void BVHSphereStructure::printTree(BVHNode * p_node, std::string prefix)
{
	
		if (p_node->axis == leaf) {
			std::cout << prefix << "Leaf(" << p_node->id_left << "," << p_node->id_right << ")";
		}
		else {
			std::cout << prefix << "Node(\n";
			printTree(p_node->leftChild, prefix+"  ");
			std::cout << prefix << ",\n";
			printTree(p_node->rightChild, prefix + "  ");
			std::cout << prefix << "\n)";
			
		}
	
}

bool BVHSphereStructure::traceBVHNode(Ray& ray, HitInfo& hit, BVHNode* p_node)
{
	BVHNode& node = *p_node;
	
	hit.trace_depth++;

	if (node.axis == leaf) {
		if (node.id_left == node.id_right) {
			return tracePrimitive(ray, hit, node.id_left);
		}
		else {
			bool hasHit = false;
			hasHit |= tracePrimitive(ray, hit, node.id_left);
			hasHit |= tracePrimitive(ray, hit, node.id_right);
			return hasHit;
		}
	}
	if (node.aabb.intersect(ray)) {
		return (traceBVHNode(ray, hit, node.leftChild) || traceBVHNode(ray, hit, node.rightChild));
	}
	else {
		return false;
	}
}

