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

	class custom_priority_queue : public std::priority_queue<NodePair, std::vector<NodePair>, CompareDist>
	{
	public:
		bool remove(const LightNode* value) {
			bool res = false;
			unsigned int size = 0;
			auto itLeft = this->c.begin();
			auto itRight = this->c.begin()++;
			while (itRight != this->c.end()) {
				if (itRight->n1 == value || itRight->n2 == value) {
					itRight++;
					continue;
				}
				*itLeft = *itRight;
				itLeft++;
				itRight++;
				size++;
			}
			this->c.resize(size);
			//std::make_heap(this->c.begin(), this->c.end(), this->comp);
			return res;
		}
	};

	// min queue
	//std::priority_queue<NodePair, std::vector<NodePair>, CompareDist> Queue = std::priority_queue<NodePair, std::vector<NodePair>, CompareDist>();
	custom_priority_queue Queue = custom_priority_queue();

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

			Queue.remove(left);
			Queue.remove(right);

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
	if (radius <= 0.0001f)
		std::cout << "radius: " << radius << std::endl;
	//assert(radius <= 0.00000f);
	float min_dist = dist - radius;
	float max_dist = dist + radius;

	// Geometric term
	//float G = 1.0f / (dist * dist);

	Vec3f intensity = node->reprecentative->color;
	Vec3f rep = intensity / (dist * dist);
	Vec3f worst = intensity / (max_dist * max_dist);

	Vec3f diff = abs(rep - worst);
	Vec3f error = diff / rep;

	//std::cout << area << std::endl;
	if (error.element_sum() < threshold || diff.max_componont() < 0.001f) {
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
		Vec3f color = A->color / p;
		light = new PointLight(pos, color);
		ReprecentativeLights.push_back(light);
	}
	else {
		Vec3f pos = B->position;
		Vec3f color = B->color / (1.0f - p);
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
