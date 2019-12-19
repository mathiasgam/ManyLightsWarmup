#pragma once

/*
Based on 'Efficient BVH Construction via Approximate Agglomerative Clustering' paper
http://graphics.cs.cmu.edu/projects/aac/aac_build.pdf
*/

#include <utility>

#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>

#include "LightStructure.h"
#include "Structure/AABB.h"

#include "Geometry/Line.h"

#include "LightNode.h"

namespace {
	
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

	struct GraphNode {
		LightNode* node;
		bool is_alive;
		GraphNode() : node(nullptr), is_alive(true) {}
		GraphNode(LightNode* n) : node(n), is_alive(true) {}
		bool operator==(const GraphNode v) const {
			return node == v.node;
		}
	};

	struct GraphEdge {
		GraphNode* n1;
		GraphNode* n2;
		float dist;
		GraphEdge(GraphNode* n1, GraphNode* n2, float dist) : n1(n1), n2(n2), dist(dist) {}
	};

	struct CompareDist {
		bool operator()(GraphEdge const& e1, GraphEdge const& e2)
		{
			return e1.dist > e2.dist;
		}
	};

	struct Graph {
		std::vector<GraphNode> nodes;
		std::priority_queue<GraphEdge, std::vector<GraphEdge>, CompareDist> edges;

		Graph() : nodes(), edges() {}
	};

}

class BULightTree : LightStructure
{
private:
	const int N = 1;


	std::vector<PointLight*> ReprecentativeLights;
	//LightNode* root = nullptr;
	std::vector<LightNode*> light_forest;

public:
	BULightTree();
	~BULightTree();

	void init(std::vector<PointLight*> lights);
	std::vector<PointLight*> GetLights(const HitInfo& hit, float threshold) const;
	std::vector<Line> GetTreeEdges() const;

private:
	
	void BuildTree(std::unordered_set<LightNode*>& clusters);

	float distance(const PointLight* p1, const PointLight* p2);
	PointLight* MergeLights(PointLight* A, PointLight* B);

	void GetTreeEdgesRecurse(std::vector<Line>& lines, const LightNode* node, Vec3f color) const;

	static void deleteNode(LightNode* node);
};