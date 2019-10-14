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
			return p1.dist < p2.dist;
		}
	};

	std::priority_queue<NodePair, std::vector<NodePair>, CompareDist> Queue = std::priority_queue<NodePair, std::vector<NodePair>, CompareDist>();

	for (LightNode* c1 : clusters) {
		for (LightNode* c2 : clusters) {
			NodePair pair = {};
			pair.n1 = c1;
			pair.n2 = c2;
			pair.dist = distance(c1->reprecentative, c2->reprecentative);
			Queue.push(pair);
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
				NodePair _pair = {};
				_pair.n1 = node;
				_pair.n2 = c;
				_pair.dist = distance(node->reprecentative, c->reprecentative);
				Queue.push(_pair);
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

void BULightTree::SearchLights(std::vector<PointLight*>& out, LightNode* node, Vec3f pos, Vec3f normal, float threshold) const
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

float BULightTree::distance(const PointLight* p1, const PointLight* p2)
{
	return (p1->position - p2->position).length();
}

PointLight* BULightTree::MergeLights(PointLight* A, PointLight* B)
{
	float sumA = A->color.element_sum();
	float sumB = B->color.element_sum();
	float sum = sumA + sumB;
	Vec3f pos = random(0, 1) < sumA / sum ? A->position : B->position;
	Vec3f color = A->color + B->color;
	PointLight* light = new PointLight(pos, color);
	ReprecentativeLights.push_back(light);
	return light;
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
