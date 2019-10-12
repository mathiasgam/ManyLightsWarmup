#include "SAHBVHStructure.h"

#include <limits>
#include <iostream>

SAHBVHStructure::~SAHBVHStructure()
{
}

void SAHBVHStructure::init(const std::vector<const Geometry*>& geometry, const std::vector<const Plane*>& _planes)
{
	nodes.clear();
	primitives.clear();
	objects.clear();

	std::cout << "Loading objects: ";
	std::vector<Primitive_Fat> primitives_build(0);

	for (auto& object : geometry) {
		objects.push_back(object);
	}

	for (auto& plane : _planes) {
		planes.push_back(plane);
	}

	std::cout << "" << objects.size() << " loaded\n";

	std::cout << "Collecting primitives\n";
	// Collect all the primitives in the objects
	for (unsigned int i = 0; i < objects.size(); i++) {
		const Geometry& object = *objects[i];
		const unsigned int object_size = object.num_primitives();

		for (unsigned int j = 0; j < object_size; j++) {
			Primitive_Fat p = {};
			p.object = i;
			p.index = j;
			p.bbox = object.get_bbox(j);
			p.center = object.get_bbox(j).center();

			//std::cout << "object: " << object.get_bbox(j) << " bbox: " << p.bbox << "\n";
			primitives_build.push_back(p);
		}
	}

	std::cout << "Building BVH structure\n";
	// recursively construct the binary search tree
	root_index = build(primitives_build, 0, primitives_build.size());


	std::cout << "Precomputing bbox centers\n";
	auto num_nodes = nodes.size();
	centers.resize(num_nodes);
	for (int i = 0; i < num_nodes; i++) {
		centers[i] = nodes[i].bbox.center();
	}
	

	std::cout << "Reducing primitive size\n";
	// reduce the build primitives to search primitives
	primitives.resize(primitives_build.size());
	for (int i = 0; i < primitives_build.size(); i++) {
		Primitive_Fat& p = primitives_build[i];
		primitives[i].index = p.index;
		primitives[i].object = p.object;
	}

	recalculate_bboxes(root_index);
}

bool SAHBVHStructure::closest_hit(Ray& ray, HitInfo& hit) const
{
	Vec3f dirfrac = Vec3f(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);
	bool hashit = closest_plane(ray, hit, dirfrac);
	if (closest_hit_recurse(ray, hit, root_index, dirfrac))
		hashit = true;
	return hashit;
}

bool SAHBVHStructure::any_hit(Ray& ray) const
{
	Vec3f dirfrac = Vec3f(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);
	if (any_plane(ray, dirfrac))
		return true;
	return any_hit_recurse(ray, root_index, dirfrac);
}

void SAHBVHStructure::print()
{
	for (int i = 0; i < nodes.size(); i++) {
		BVHNode& node = nodes[i];
		if (node.type != NodeType::leaf) {
			std::cout << "node: " << i << ", type: " << node.type << ", (" << node.leftChild << "," << node.rightChild << ") " << node.bbox << "\n";
		}
		else {
			std::cout << "node: " << i << ", type: leaf(" << node.leftChild << "," << node.rightChild << ") " << node.bbox << "\n";
		}
		
	}
}




bool SAHBVHStructure::closest_hit_recurse(Ray &ray, HitInfo &hit, unsigned int i, const Vec3f& dirfrac) const
{
	hit.trace_depth++;
	const BVHNode& node = nodes[i];

	//std::cout << "node: " << i << ", type: " << node.type << ", depth: " << hit.trace_depth << "\n";
	bool hashit = false;
	if (node.bbox.intersect(ray)) {
		if (node.type == leaf) {
			for (int j = node.leftChild; j < node.rightChild; j++) {
				const Primitive p = primitives[j];
				if (objects[p.object]->intersect(ray, hit, p.index)) {
					hashit = true;
					ray.t_max = hit.t;
				}
			}
		}
		else {
			// find the approximate distance to the left and right bbox
			float left_dist = dot(centers[node.leftChild] - ray.center, ray.direction);
			float right_dist = dot(centers[node.rightChild] - ray.center, ray.direction);

			// start search in the closest one
			if (left_dist < right_dist) {
				if (closest_hit_recurse(ray, hit, node.leftChild, dirfrac))
					hashit = true;
				if (closest_hit_recurse(ray, hit, node.rightChild, dirfrac))
					hashit = true;
			}
			else {
				if (closest_hit_recurse(ray, hit, node.rightChild, dirfrac))
					hashit = true;
				if (closest_hit_recurse(ray, hit, node.leftChild, dirfrac))
					hashit = true;
			}
			
		}
	}
	return hashit;
}

bool SAHBVHStructure::any_hit_recurse(Ray &ray, unsigned int i, const Vec3f& dirfrac) const
{
	const BVHNode& node = nodes[i];
	if (node.bbox.intersect(ray)) {
		if (node.type == leaf) {
			for (int j = node.leftChild; j < node.rightChild; j++) {
				const Primitive& p = primitives[j];
				if (objects[p.object]->intersect(ray, p.index)) {
					return true;
				}
			}
		}
		else {
			if (any_hit_recurse(ray, node.leftChild, dirfrac))
				return true;
			if (any_hit_recurse(ray, node.rightChild, dirfrac))
				return true;
		}
	}
	return false;
}

bool SAHBVHStructure::closest_plane(Ray &ray, HitInfo &hit, Vec3f& dirfrac) const
{
	bool hashit = false;
	for (int i = 0; i < planes.size(); i++){
		if (planes[i]->intersect(ray, hit, dirfrac)) {
			hashit = true;
			ray.t_max = hit.t;
		}
	}
	return hashit;
}

bool SAHBVHStructure::any_plane(Ray &ray, Vec3f& dirfrac) const
{
	for (auto& plane : planes) {
		if (plane->intersect(ray, dirfrac)) {
			return true;
		}
	}
	return false;
}

AABB SAHBVHStructure::recalculate_bboxes(unsigned int index)
{
	AABB bbox = AABB();
	const BVHNode& node = nodes[index];
	if (node.type == leaf) {
		for (int i = node.leftChild; i < node.rightChild; i++) {
			Primitive& primitive = primitives[i];
			bbox.add_bbox(objects[primitive.object]->get_bbox(primitive.index));
		}
	}
	else {
		bbox.add_bbox(recalculate_bboxes(node.leftChild));
		bbox.add_bbox(recalculate_bboxes(node.rightChild));
	}
	nodes[index].bbox = bbox;
	return bbox;
}

void SAHBVHStructure::swap(Primitive_Fat & left, Primitive_Fat & right)
{
	Primitive_Fat tmp = left;
	left = right;
	right = tmp;
}

unsigned int SAHBVHStructure::build(std::vector<Primitive_Fat>& fat_primitives, unsigned int start, unsigned int end)
{
	// create node in the vector
	unsigned int num = end - start;
	unsigned int index = nodes.size();
	{
		BVHNode n;
		nodes.push_back(n);
	}
	nodes[index].bbox = AABB();
	AABB centroid_box = AABB();
	for (unsigned int i = start; i < end; i++) {
		nodes[index].bbox.add_bbox(fat_primitives[i].bbox);
		centroid_box.add_point(fat_primitives[i].center);
	}

	if (num <= 4) {
		nodes[index].type = leaf;
		nodes[index].leftChild = start;
		nodes[index].rightChild = end;
		return index;
	}
	
	float min_cost = std::numeric_limits<float>::max();
	float min_split;
	int min_axis = 0;
	int left_num = 0, right_num = 0;

	const int TESTS = 8;

	// find best combo of axis and split.
	for (int axis = 0; axis < 3; axis++) {
		for (int j = 0; j < TESTS; j++) {

			AABB _left_bbox;
			AABB _right_bbox;

			float max_corner = centroid_box.p_max[axis];
			float min_corner = centroid_box.p_min[axis];
			float split = (max_corner - min_corner)*j / static_cast<float>(TESTS) + min_corner;
			//float split = centroid_box.center()[axis];

			_left_bbox.p_max[axis] = split;
			_right_bbox.p_min[axis] = split;

			int num_left = 0;
			int num_right = 0;

			for (int k = start; k < end; k++) {
				if (fat_primitives[k].center[axis] <= split) {
					_left_bbox.add_bbox(fat_primitives[k].bbox);
					num_left++;
				}
				else {
					_right_bbox.add_bbox(fat_primitives[k].bbox);
					num_right++;
				}
			}

			//std::cout << "left: " << num_left << " right: " << num_right << "\n";

			if (num_left < 1 || num_right < 1) {
				continue;
			}

			float p = (split - min_corner) / (max_corner - min_corner);
			float cost = p * num_left * _left_bbox.area() + (1.0f-p) * num_right * _right_bbox.area();
			
			if (cost < min_cost) {
				
				min_cost = cost;
				min_split = split;
				min_axis = axis;
				left_num = num_left;
				right_num = num_right;
				nodes[index].type = static_cast<NodeType>(axis);
			}

		}
	}

	//std::cout << "min_split: " << min_split << " left: " << left_num << " right: " << right_num << "\n";
	//std::cout << "index: " << index << " num: " << num << " start: " << start << " split: " << min_split << "\n";
	//std::cout << "min cost: " << min_cost << ", split: " << min_split << "\n";

	//std::cout << "sorting\n";
	// swap elements on the left and right to the split
	unsigned int x = start, y = end-1;
	while (x < y)
	{
		//std::cout << "1x,y: " << x << "," << y << "\n";
		if (fat_primitives[x].center[min_axis] <= min_split) {
			x++;
		}
		else {
			while (x < y)
			{
				//std::cout << "2x,y: " << x << "," << y << "\n";
				if (fat_primitives[y].center[min_axis] <= min_split) {
					//std::cout << "swap\n";
					swap(fat_primitives[x], fat_primitives[y]);
					break;
				}
				y--;
			}
		}
	}
	//std::cout << "x: " << x << " left: " << (x - start) << " right: " << (end - x) << "\n";

	nodes[index].leftChild = build(fat_primitives, start, x);
	nodes[index].rightChild = build(fat_primitives, x, end);

	// std::cout << "(" << node.leftChild << "," << node.rightChild << ")";

	return index;
}
