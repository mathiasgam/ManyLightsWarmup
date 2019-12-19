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
	root = generateHierarchy(primitives, 0, static_cast<int>(primitives.size()) - 1);

	assert(root != nullptr);
	CalculateReprecentativeLights(root);
}

std::vector<PointLight*> LightTree::GetLights(const HitInfo& hit, float threshold) const
{
	std::vector<PointLight*> lights = std::vector<PointLight*>();
	Vec3f total_radiance = Vec3f(0.0f);
	std::priority_queue<NodeCut, std::vector<NodeCut>, NodeCutCompare> queue = std::priority_queue<NodeCut, std::vector<NodeCut>, NodeCutCompare>();

	// push the root node to the queue
	{
		NodeCut root_cut = CreateNodeCut(root, hit);
		queue.push(root_cut);
		total_radiance += root_cut.radiance;
	}

	while (queue.size() < 1000) {
		NodeCut cut = queue.top();
		if (cut.node->type == NodeType::Leaf)
			break;

		//std::cout << "Error: " << cut.error << std::endl;
#if RANDOM_THRESHOLD
		if ((cut.error - total_radiance * threshold * random(0.9f, 1.1)).max_componont() > 0.0f) {
#else
		if ((cut.error - total_radiance * threshold).max_componont() > 0.0f) {
#endif
			queue.pop();
			total_radiance -= cut.radiance;

			NodeCut A = CreateNodeCut(cut.node->ChildA, hit);
			NodeCut B = CreateNodeCut(cut.node->ChildB, hit);
			total_radiance += A.radiance + B.radiance;

			//std::cout << "Prev error: " << cut.errorsum << ", new error: " << A.errorsum + B.errorsum << std::endl;

			queue.push(A);
			queue.push(B);
		}
		else {
			// if the worst error is less than the permitted error, theres no reason to refine more clusters
			//std::cout << "threshold reached: " << cut.errorsum << std::endl;
			break;
		}

		}
	size_t N = queue.size();
	lights.resize(N);
	//std::cout << "Num lights: " << N << ", max error: " << queue.top().errorsum << std::endl;
	for (int i = 0; i < N; i++) {
		lights[i] = queue.top().node->reprecentative;
		queue.pop();
	}

	return lights;
}

std::vector<Line> LightTree::GetTreeEdges() const
{
	std::vector<Line> lines = std::vector<Line>();
	GetTreeEdgesRecurse(lines, root, Vec3f(1, 0, 0));
	return lines;
}
/*
void LightTree::SearchLights(std::vector<PointLight*>& out, LightNode* node, const HitInfo& hit, float threshold) const
{
	if (node->type == NodeType::Leaf) {
		out.push_back(node->reprecentative);
		return;
	}

	// Test if the bounding box is behind the surface.
	Vec3f t1 = (node->bbox.p_min - hit.position) / hit.normal;
	Vec3f t2 = (node->bbox.p_max - hit.position) / hit.normal;
	if (max(t1, t2).max_componont() < 0.0f)
		return;


	const float dist = (hit.position - node->reprecentative->position).length();
	const float radius = (node->bbox.p_max - node->bbox.p_min).length() / 2.0f;

	const float min_dist = dist - radius;

	if (min_dist <= 0) {
		SearchLights(out, node->ChildA, hit, threshold);
		SearchLights(out, node->ChildB, hit, threshold);
		return;
	}

	// Geometric term
	//float G = 1.0f / (dist * dist);

	const Vec3f intensity = node->reprecentative->color * hit.p_material->diffuse;
	const Vec3f rep = intensity / (dist * dist);
	const Vec3f worst = intensity / (min_dist * min_dist);

	const Vec3f error = abs(rep - worst);

	//std::cout << area << std::endl;
	if (error.element_sum() < threshold) {
		out.push_back(node->reprecentative);
	}
	else {
		SearchLights(out, node->ChildA, hit, threshold);
		SearchLights(out, node->ChildB, hit, threshold);
	}
}
*/

LightNode* LightTree::generateHierarchy(std::vector<BuildPrimitive>& sortedPrimitives, int first, int last)
{
	LightNode* node = new LightNode();

	if (first == last) {
		node->type = NodeType::Leaf;
		node->ChildA = nullptr;
		node->ChildB = nullptr;
		node->reprecentative = sortedPrimitives[first].light;
		// created node is nullptr
		assert(node != nullptr);
		return node;
	}

	node->type = NodeType::Internal;

	int split = findSplit(sortedPrimitives, first, last);

	node->ChildA = generateHierarchy(sortedPrimitives, first, split);
	node->ChildB = generateHierarchy(sortedPrimitives, split + 1, last);

	node->reprecentative = nullptr;

	// created node is nullptr
	assert(node->ChildA != nullptr);
	assert(node->ChildB != nullptr);
	assert(node != nullptr);

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

	int commonPrefix = static_cast<int>(clz(firstCode.data ^ lastCode.data));
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
			int splitPrefix = static_cast<int>(clz(firstCode.data ^ splitCode.data));
			if (splitPrefix > commonPrefix)
				split = newSplit; // accept proposal
		}
	} while (step > 1);

	return split;
}

void LightTree::CalculateReprecentativeLights(LightNode* node)
{
	assert(node != nullptr); // Node need to be initialized
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
	// node type not recognized
	assert(false);
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

void LightTree::deleteNode(LightNode* node)
{
	if (node == nullptr)
		return;
	if (node->type == NodeType::Internal) {
		deleteNode(node->ChildA);
		deleteNode(node->ChildB);
	}
	delete node;
}
