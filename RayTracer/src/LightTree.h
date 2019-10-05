#pragma once

#include "Vec3f.h"
#include "PointLight.h"

#include <vector>

class LightTree
{
private:
	enum NodeType {
		cluster,
		leaf
	};
	struct LightNode
	{
		NodeType type;
		LightNode* leftChild;
		LightNode* rightChild;
		PointLight reprecentative;

		LightNode(NodeType type, LightNode* left, LightNode* right, PointLight reprecentative) :
			type(type), leftChild(left), rightChild(right), reprecentative(reprecentative) {
		}
	};

	LightNode* root = nullptr;
	std::vector<PointLight> m_lights;

public:
	LightTree();
	~LightTree() {
		deleteNode(root);
	}

	void build(std::vector<PointLight> lights);
	std::vector<PointLight> cut(Vec3f sample_position, Vec3f sample_normal, float threshold);

private:
	static LightNode* ClusterLightNodes(LightNode* left, LightNode* right);

	static void deleteNode(LightNode* node);

	static uint32_t remap(float t);

};

