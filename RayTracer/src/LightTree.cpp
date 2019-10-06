#include "LightTree.h"
#include "MortonCode.h"
#include <vector>
#include <algorithm>
#include <limits>

LightTree::LightTree()
{
	m_lights = std::vector<PointLight>();
}

void LightTree::build(std::vector<PointLight> lights)
{
	// if a tree exists then delete the nodes and rebuild the whole structure
	if (root != nullptr)
		deleteNode(root);

	m_lights.clear();

	std::vector<MortonCode3> codes = std::vector<MortonCode3>();
	
	Vec3f range, offset;
	{
		float _max = std::numeric_limits<float>::min();
		float _min = std::numeric_limits<float>::max();
		for (auto& light : lights) {
			_max = fmaxf(_max, fmaxf(light.position[0], fmaxf(light.position[1], light.position[2])));
			_min = fminf(_min, fminf(light.position[0], fminf(light.position[1], light.position[2])));
		}
		range = _max - _min;
		offset = - _min;
	}

	codes.resize(lights.size());
	for (auto& light : lights) {
		codes.emplace_back((light.position + offset) / range);
		m_lights.emplace_back(light);
	}
	std::sort(codes.begin(), codes.end());


}

std::vector<PointLight> LightTree::cut(Vec3f sample_position, Vec3f sample_normal, float threshold) const
{
	return m_lights;
}

LightTree::LightNode* LightTree::ClusterLightNodes(LightNode* left, LightNode* right)
{
	Vec3f sum = left->reprecentative.color + right->reprecentative.color;
	if (left->reprecentative.color.element_sum() > right->reprecentative.color.element_sum()) {
		return new LightNode(NodeType::cluster, left, right, PointLight(left->reprecentative.position, sum));
	}
	else {
		return new LightNode(NodeType::cluster, left, right, PointLight(right->reprecentative.position, sum));
	}
}

void LightTree::deleteNode(LightNode* node)
{
	if (node == nullptr)
		return;
	if (node->type == NodeType::cluster) {
		deleteNode(node->leftChild);
		deleteNode(node->rightChild);
	}
	delete node;
}
