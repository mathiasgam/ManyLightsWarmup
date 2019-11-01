#include "LightTree.h"
#include "MortonCode.h"
#include "Structure/AABB.h"
#include <vector>
#include <algorithm>
#include <limits>

float randomf(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

void LightTree::init(std::vector<PointLight*> lights)
{

	std::vector<BuildPrimitive> primitives = std::vector<BuildPrimitive>();
	// find the bounding volume of the scenes lights
	AABB scene_bbox = AABB();
	for (PointLight* light : lights) {
		scene_bbox.add_point(light->position);
	}

	std::cout << "Collecting primitives\n";
	// Collect all the primitives in the objects
	for (PointLight* light : lights) {

		BuildPrimitive p = {};
		p.light = light;

		Vec3f center = light->position;
		center -= scene_bbox.p_min;
		center /= scene_bbox.p_max - scene_bbox.p_min;
		p.code = MortonCode3(center);
		//std::cout << "object: " << object.get_bbox(j) << " bbox: " << p.bbox << "\n";
		primitives.push_back(p);
	}

	std::cout << "Sorting primitives\n";
	std::sort(primitives.begin(), primitives.end());

	std::cout << "Building BVH structure\n";
	// recursively construct the binary search tree
	root = generateHierarchy(primitives, 0, primitives.size() - 1);

	assert(root != nullptr);
	CalculateReprecentativeLights(root);
}

std::vector<PointLight*> LightTree::GetLights(Vec3f sample_position, Vec3f sample_normal, float threshold) const
{
	std::vector<PointLight*> lights = std::vector<PointLight*>();
	SearchLights(lights, root, sample_position, sample_normal, threshold);

	return lights;
}

std::vector<Line> LightTree::GetTreeEdges() const
{
	std::vector<Line> lines = std::vector<Line>();
	GetTreeEdgesRecurse(lines, root, Vec3f(1, 0, 0));
	return lines;
}

void LightTree::SearchLights(std::vector<PointLight*>& out, LightNode* node, Vec3f pos, Vec3f normal, float threshold) const
{
	if (node->type == NodeType::Leaf) {
		out.push_back(node->reprecentative);
		return;
	}
	float dist = (pos - node->bbox.center()).length();
	float area = node->bbox.area();
	//std::cout << area << std::endl;
	if (area / (dist * dist) < threshold) {
		out.push_back(node->reprecentative);
	}
	else {
		SearchLights(out, node->ChildA, pos, normal, threshold);
		SearchLights(out, node->ChildB, pos, normal, threshold);
	}
}

LightTree::LightNode* LightTree::generateHierarchy(std::vector<BuildPrimitive>& sortedPrimitives, int first, int last)
{
	LightNode* node = new LightNode();

	if (first == last) {
		node->type = NodeType::Leaf;
		node->ChildA = nullptr;
		node->ChildB = nullptr;
		node->reprecentative = sortedPrimitives[first].light;
		assert(node != nullptr, "created node is nullptr");
		return node;
	}

	node->type = NodeType::Internal;

	int split = findSplit(sortedPrimitives, first, last);

	node->ChildA = generateHierarchy(sortedPrimitives, first, split);
	node->ChildB = generateHierarchy(sortedPrimitives, split + 1, last);

	node->reprecentative == nullptr;

	assert(node->ChildA != nullptr, "created node is nullptr");
	assert(node->ChildB != nullptr, "created node is nullptr");
	assert(node != nullptr, "created node is nullptr");

	return node;
}

int LightTree::findSplit(std::vector<BuildPrimitive>& sortedPrimitives, int first, int last)
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

void LightTree::CalculateReprecentativeLights(LightNode* node)
{
	assert(node != nullptr, "Node need to be initialized");
	//std::cout << "Nodetype: " << node->type << "\n";
	if (node->type == NodeType::Leaf) {
		Vec3f pos = node->reprecentative->position;
		node->bbox = AABB(pos, pos);
		return;
	}
	else if (node->type == NodeType::Internal) {
		assert(node->ChildA != nullptr);
		assert(node->ChildB != nullptr);
		CalculateReprecentativeLights(node->ChildA);
		CalculateReprecentativeLights(node->ChildB);
		node->reprecentative = MergeLights(node->ChildA->reprecentative, node->ChildB->reprecentative);
		node->bbox = AABB(node->ChildA->bbox, node->ChildB->bbox);
		return;
	}
	assert(false, "node type not recognized");
}

PointLight* LightTree::MergeLights(PointLight* A, PointLight* B)
{
	float sumA = A->color.element_sum();
	float sumB = B->color.element_sum();
	float sum = sumA + sumB;
	Vec3f pos = randomf(0, 1) < sumA / sum ? A->position : B->position;
	Vec3f color = A->color + B->color;
	PointLight* light = new PointLight(pos, color);
	ReprecentativeLights.push_back(light);
	return light;
}

void LightTree::GetTreeEdgesRecurse(std::vector<Line>& lines, const LightNode* node, Vec3f color) const
{
	if (node->type == NodeType::Leaf) {
		return;
	}
	else if (node->type == NodeType::Internal) {
		lines.emplace_back(node->reprecentative->position, node->ChildA->reprecentative->position, 0.005f, color);
		lines.emplace_back(node->reprecentative->position, node->ChildB->reprecentative->position, 0.005f, color);
		GetTreeEdgesRecurse(lines, node->ChildA, color * 0.9f);
		GetTreeEdgesRecurse(lines, node->ChildB, color * 0.9f);
		return;
	}
	assert(false);
}

void LightTree::deleteNode(LightTree::LightNode* node)
{
	if (node == nullptr)
		return;
	if (node->type == NodeType::Internal) {
		deleteNode(node->ChildA);
		deleteNode(node->ChildB);
	}
	delete node;
}
