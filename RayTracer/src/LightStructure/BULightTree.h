#pragma once

/*
Based on 'Efficient BVH Construction via Approximate Agglomerative Clustering' paper
http://graphics.cs.cmu.edu/projects/aac/aac_build.pdf
*/

#include <utility>

#include "LightStructure.h"
#include "Structure/AABB.h"

#include "Geometry/Line.h"

class BULightTree : LightStructure
{
private:
	enum NodeType {
		Internal,
		Leaf
	};
	struct LightNode
	{
		int type;
		AABB bbox;
		LightNode* ChildA;
		LightNode* ChildB;
		PointLight* reprecentative;
	};

	struct hash_pair {
		template <class T1, class T2>
		size_t operator()(const std::pair<T1, T2>& p) const
		{
			auto hash1 = std::hash<T1>{}(p.first);
			auto hash2 = std::hash<T2>{}(p.second);
			return hash1 ^ hash2;
		}
	};

	struct NodePair
	{
		LightNode* n1;
		LightNode* n2;
		float dist;
	};

	


	std::vector<PointLight*> ReprecentativeLights;
	LightNode* root = nullptr;

public:
	BULightTree();
	~BULightTree();

	virtual void init(std::vector<PointLight*> lights);
	virtual std::vector<PointLight*> GetLights(Vec3f position, Vec3f normal, float threshold) const;
	virtual std::vector<Line> BULightTree::GetTreeEdges() const;

private:
	void SearchLights(std::vector<PointLight*>& out, LightNode* node, Vec3f pos, Vec3f normal, float threshold) const;

	float distance(const PointLight* p1, const PointLight* p2);
	PointLight* MergeLights(PointLight* A, PointLight* B);

	virtual void BULightTree::GetTreeEdgesRecurse(std::vector<Line>& lines, const LightNode* node, Vec3f color) const;

	static void deleteNode(LightNode* node);
};