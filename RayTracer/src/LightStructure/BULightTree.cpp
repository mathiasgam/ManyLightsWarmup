#include "BULightTree.h"

#include <limits>
#include <cassert>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>

#include "Sampling.h"


BULightTree::BULightTree() : ReprecentativeLights(0)
{
}

BULightTree::~BULightTree()
{
	deleteNode(root);
	for (PointLight* light : ReprecentativeLights) {
		assert(light != nullptr);
		delete light;
	}
}

void BULightTree::init(std::vector<PointLight*> lights)
{
	std::cout << "Building Light structure\n";
	const size_t num = lights.size();

	// set for holding all the lights during construction.
	//std::set<LightNode*> clusters;
	std::unordered_set<LightNode*> clusters;

	// insert all lights in the cluster set.
	for (PointLight* light : lights) {
		LightNode* node = new LightNode();

		node->type = NodeType::Leaf;
		node->bbox = AABB(light->position);
		node->ChildA = nullptr;
		node->ChildB = nullptr;
		node->reprecentative = light;

		clusters.insert(node);
	}

	struct CompareDist {
		bool operator()(NodePair const& p1, NodePair const& p2)
		{
			return p1.dist > p2.dist;
		}
	};

	std::priority_queue<NodePair, std::vector<NodePair>, CompareDist> Queue = std::priority_queue<NodePair, std::vector<NodePair>, CompareDist>();

	for (LightNode* c1 : clusters) {
		for (LightNode* c2 : clusters) {
			if (c1 != c2) {
				NodePair pair = {};
				pair.n1 = c1;
				pair.n2 = c2;
				pair.dist = distance(c1->reprecentative, c2->reprecentative);
				Queue.push(pair);
			}
		}
	}

	// map for accelerating distance calculation.
	//std::unordered_map<std::pair<LightNode*, LightNode*>, float, hash_pair> umap = std::unordered_map<std::pair<LightNode*, LightNode*>, float, hash_pair>();

	while (clusters.size() > 1) {
		//float best = std::numeric_limits<float>::infinity();

		auto pair = Queue.top();
		Queue.pop();

		if (clusters.find(pair.n1) != clusters.end() && clusters.find(pair.n2) != clusters.end()) {

			LightNode* left = pair.n1;
			LightNode* right = pair.n2;

			assert(left != nullptr);
			assert(right != nullptr);
			assert(right != left);

			LightNode* node = new LightNode();
			node->type = NodeType::Internal;
			node->bbox = AABB(left->bbox, right->bbox);
			node->ChildA = left;
			node->ChildB = right;
			node->reprecentative = MergeLights(left->reprecentative, right->reprecentative);

			clusters.erase(left);
			clusters.erase(right);
			clusters.insert(node);

			for (LightNode* c : clusters) {
				if (c != node) {
					NodePair _pair = {};
					_pair.n1 = node;
					_pair.n2 = c;
					_pair.dist = distance(node->reprecentative, c->reprecentative);
					Queue.push(_pair);
				}
			}

			//std::cout << "cluster size: " << clusters.size() << ", map size: " << Queue.size() << std::endl;
		}

	}

	root = *clusters.begin();
}

std::vector<PointLight*> BULightTree::GetLights(Vec3f position, Vec3f normal, float threshold) const
{
	std::vector<PointLight*> lights = std::vector<PointLight*>();
	SearchLights(lights, root, position, normal, threshold);

	return lights;
}

std::vector<Line> BULightTree::GetTreeEdges() const
{
	std::vector<Line> lines = std::vector<Line>();
	GetTreeEdgesRecurse(lines, root, Vec3f(1, 0, 0));
	return lines;
}

void BULightTree::SearchLights(std::vector<PointLight*>& out, LightNode* node, Vec3f pos, Vec3f normal, float threshold) const
{
	if (node->type == NodeType::Leaf) {
		out.push_back(node->reprecentative);
		return;
	}

	// Test if the bounding box is behind the surface.
	Vec3f t1 = (node->bbox.p_min - pos) / normal;
	Vec3f t2 = (node->bbox.p_max - pos) / normal;
	if (max(t1, t2).max_componont() < 0.0f)
		return;


	float dist = (pos - node->reprecentative->position).length();
	float radius = (node->bbox.p_max - node->bbox.p_min).length() / 2.0f;
	float min_dist = dist - radius;

	// Geometric term
	//float G = 1.0f / (dist * dist);

	float intensity = node->reprecentative->color.max_componont();
	float rep = intensity / (dist * dist);
	float worst = intensity / (min_dist * min_dist);

	assert(dist > min_dist);
	if (pos[0] < 10 && pos[0] > -10 && pos[2] < 10 && pos[2] > -10) {
		//std::cout << "diff: " << dist - min_dist << std::endl;
	}
	else {
		return;
	}

	assert(rep < 0.0001f);
	assert(worst < 0.0001f);
	float error = fabsf(rep - worst) / rep;

	//std::cout << area << std::endl;
	if (error <= threshold) {
		out.push_back(node->reprecentative);
	}
	else {
		SearchLights(out, node->ChildA, pos, normal, threshold);
		SearchLights(out, node->ChildB, pos, normal, threshold);
	}
}

float BULightTree::distance(const PointLight* p1, const PointLight* p2)
{
	return (p1->position - p2->position).length();
}

PointLight* BULightTree::MergeLights(PointLight* A, PointLight* B)
{
	float sumA = A->color.element_sum();
	float sumB = B->color.element_sum();
	float sum = sumA + sumB;
	float p = sumA / sumB;

	PointLight* light = nullptr;
	if (random(0, 1) < p) {
		Vec3f pos = A->position;
		Vec3f color = A->color + B->color;
		light = new PointLight(pos, color);
		ReprecentativeLights.push_back(light);
	}
	else {
		Vec3f pos = B->position;
		Vec3f color = B->color + A->color;
		light = new PointLight(pos, color);
		ReprecentativeLights.push_back(light);
	}
	return light;
}

void BULightTree::GetTreeEdgesRecurse(std::vector<Line>& lines, const LightNode* node, Vec3f color) const
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

void BULightTree::deleteNode(LightNode* node)
{
	if (node == nullptr)
		return;
	if (node->type == NodeType::Internal) {
		deleteNode(node->ChildA);
		deleteNode(node->ChildB);
	}
	delete node;
}
