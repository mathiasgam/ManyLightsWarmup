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

	struct CompareDist {
		bool operator()(NodePair const& p1, NodePair const& p2)
		{
			return p1.dist > p2.dist;
		}
	};


	std::vector<PointLight*> ReprecentativeLights;
	LightNode* root = nullptr;

public:
	BULightTree();
	~BULightTree();

	virtual void init(std::vector<PointLight*> lights);
	virtual std::vector<PointLight*> GetLights(const HitInfo& hit, float threshold) const;
	virtual std::vector<Line> GetTreeEdges() const;

private:
	void SearchLights(std::vector<PointLight*>& out, LightNode* node, const HitInfo& hit, float threshold) const;

	float distance(const PointLight* p1, const PointLight* p2);
	PointLight* MergeLights(PointLight* A, PointLight* B);

	virtual void GetTreeEdgesRecurse(std::vector<Line>& lines, const LightNode* node, Vec3f color) const;

	static void deleteNode(LightNode* node);
};