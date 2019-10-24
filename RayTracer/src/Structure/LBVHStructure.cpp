#include "LBVHStructure.h"

#include "MortonCode.h"

#include <algorithm>
#include <cassert>
#include <iostream>

LBVHStructure::~LBVHStructure()
{
	DestroyNode(root);
}

void LBVHStructure::init(const std::vector<const Geometry*>& _geometry, const std::vector<const Plane*>& _planes)
{
	primitives.clear();
	//objects.clear();
	planes.clear();
	
	for (auto& plane : _planes) {
		planes.push_back(plane);
	}

	//std::cout << "" << objects.size() << " loaded\n";
	AABB scene_bbox = AABB();
	for (auto o : _geometry) {
		scene_bbox.add_bbox(o->get_bbox());
	}

	std::cout << "Collecting primitives\n";
	// Collect all the primitives in the objects
	for (unsigned int i = 0; i < _geometry.size(); i++) {
		const Geometry* object = _geometry[i];
		const unsigned int object_size = object->num_primitives();

		for (unsigned int j = 0; j < object_size; j++) {
			Primitive p = {};
			p.Object = object;
			p.index = j;
			Vec3f center = object->get_bbox(j).center();
			center -= scene_bbox.p_min;
			center /= scene_bbox.p_max - scene_bbox.p_min;
			p.code = MortonCode3(center);
			//std::cout << "object: " << object.get_bbox(j) << " bbox: " << p.bbox << "\n";
			primitives.push_back(p);
		}
	}

	std::cout << "Sorting primitives\n";
	std::sort(primitives.begin(), primitives.end());

	std::cout << "Building BVH structure\n";
	// recursively construct the binary search tree
	root = generateHierarchy(primitives, 0, primitives.size() - 1);

	assert(root != nullptr);
	calculateBBox(root);
}

bool LBVHStructure::closest_hit(Ray& ray, HitInfo& hit) const
{
	Vec3f dirfrac = Vec3f(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);
	bool hashit = closest_plane(ray, hit, dirfrac);
	if (closest_hit_recurse(ray, hit, *root, dirfrac))
		hashit = true;
	return hashit;
}

bool LBVHStructure::any_hit(Ray& ray) const
{
	Vec3f dirfrac = Vec3f(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);
	if (any_plane(ray, dirfrac))
		return true;
	return any_hit_recurse(ray, *root, dirfrac);
}

bool LBVHStructure::closest_hit_recurse(Ray& ray, HitInfo& hit, Node& node, const Vec3f& dirfrac) const
{
	hit.trace_depth++;

	//std::cout << "node: " << i << ", type: " << node.type << ", depth: " << hit.trace_depth << "\n";
	bool hashit = false;
	if (node.bbox.intersect(ray, dirfrac)) {
		if (node.type == NodeType::Leaf) {
			const Primitive p = primitives[node.primitive];
			if (p.Object->intersect(ray, hit, p.index)) {
				hashit = true;
				//hit.color = p.Object->color;
				ray.t_max = hit.t;
			}
		}
		else {
			// find the approximate distance to the left and right bbox
			float left_dist = dot(node.ChildA->center - ray.center, ray.direction);
			float right_dist = dot(node.ChildB->center - ray.center, ray.direction);

			// start search in the closest one
			if (left_dist < right_dist) {
				if (closest_hit_recurse(ray, hit, *node.ChildA, dirfrac))
					hashit = true;
				if (closest_hit_recurse(ray, hit, *node.ChildB, dirfrac))
					hashit = true;
			}
			else {
				if (closest_hit_recurse(ray, hit, *node.ChildB, dirfrac))
					hashit = true;
				if (closest_hit_recurse(ray, hit, *node.ChildA, dirfrac))
					hashit = true;
			}

		}
	}
	return hashit;
}

bool LBVHStructure::any_hit_recurse(Ray&ray, Node& node, const Vec3f& dirfrac) const
{
	if (node.bbox.intersect(ray, dirfrac)) {
		if (node.type == NodeType::Leaf) {
			const Primitive& p = primitives[node.primitive];
			if (p.Object->intersect(ray, p.index)) {
				return true;
			}
		}
		else {
			if (any_hit_recurse(ray, *node.ChildA, dirfrac))
				return true;
			if (any_hit_recurse(ray, *node.ChildB, dirfrac))
				return true;
		}
	}
	return false;
}

bool LBVHStructure::closest_plane(Ray& ray, HitInfo& hit, Vec3f& dirfrac) const
{
	bool hashit = false;
	for (int i = 0; i < planes.size(); i++) {
		if (planes[i]->intersect(ray, hit, dirfrac)) {
			hashit = true;
			hit.color = planes[i]->color;
			ray.t_max = hit.t;
		}
	}
	return hashit;
}

bool LBVHStructure::any_plane(Ray&ray, Vec3f& dirfrac) const
{
	for (auto& plane : planes) {
		if (plane->intersect(ray, dirfrac)) {
			return true;
		}
	}
	return false;
}

LBVHStructure::Node* LBVHStructure::generateHierarchy(std::vector<Primitive>& sortedPrimitives, int first, int last)
{
	Node* node = new Node();

	if (first == last) {
		node->type = NodeType::Leaf;
		node->ChildA = nullptr;
		node->ChildB = nullptr;
		node->primitive = first;
		assert(node != nullptr, "created node is nullptr");
		return node;
	}

	node->type = NodeType::Internal;

	int split = findSplit(sortedPrimitives, first, last);

	node->ChildA = generateHierarchy(sortedPrimitives, first, split);
	node->ChildB = generateHierarchy(sortedPrimitives, split + 1, last);

	node->primitive = -1;

	assert(node->ChildA != nullptr, "created node is nullptr");
	assert(node->ChildB != nullptr, "created node is nullptr");
	assert(node != nullptr, "created node is nullptr");

	return node;
}

int LBVHStructure::findSplit(std::vector<Primitive>& sortedPrimitives, int first, int last)
{
	MortonCode3 firstCode = sortedPrimitives[first].code;
	MortonCode3 lastCode = sortedPrimitives[last].code;

	if (firstCode.data == lastCode.data)
		return (first + last) >> 1;

	// Calculate the number of highest bits that are the same
	// for all objects, using the count-leading-zeros intrinsic.

	int commonPrefix = clz(firstCode.data ^ lastCode.data);
	//int commonPrefix = __clz(firstCode ^ lastCode);

	// Use binary search to find where the next bit differs.
	// Specifically, we are looking for the highest object that
	// shares more than commonPrefix bits with the first one.

	int split = first; // initial guess
	int step = last - first;

	do
	{
		step = (step + 1) >> 1; // exponential decrease
		int newSplit = split + step; // proposed new position

		if (newSplit < last)
		{
			MortonCode3 splitCode = sortedPrimitives[newSplit].code;
			int splitPrefix = clz(firstCode.data ^ splitCode.data);
			if (splitPrefix > commonPrefix)
				split = newSplit; // accept proposal
		}
	} while (step > 1);

	return split;
}

void LBVHStructure::calculateBBox(Node* node)
{
	assert(node != nullptr, "Node need to be initialized");
	//std::cout << "Nodetype: " << node->type << "\n";
	if (node->type == NodeType::Leaf) {
		Primitive& p = primitives[node->primitive];
		node->bbox = p.Object->get_bbox(p.index);
		node->center = node->bbox.center();
		return;
	}
	else if (node->type == NodeType::Internal){
		assert(node->ChildA != nullptr);
		assert(node->ChildB != nullptr);
		calculateBBox(node->ChildA);
		calculateBBox(node->ChildB);
		node->bbox = AABB(node->ChildA->bbox, node->ChildB->bbox);
		node->center = node->bbox.center();
		return;
	}
	assert(false, "node type not recognized");
}


