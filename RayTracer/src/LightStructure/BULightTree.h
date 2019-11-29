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

namespace {

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

	struct NodeCut {
		const LightNode* node;
		Vec3f radiance;
		Vec3f error;
		float errorsum;

		NodeCut(const LightNode* node, const Vec3f radiance, const Vec3f error): node(node), radiance(radiance), error(error), errorsum(error.element_sum()) {}

	};
	struct NodeCutCompare {
		bool operator()(NodeCut const& c1, NodeCut const& c2)
		{
			return c1.errorsum < c2.errorsum;
		}
	};

	inline Vec3f GeometricTerm(const Vec3f& x, const Vec3f& y) {
		return 1.0f / (y - x).sqr_length();
	}

	inline Vec3f GeometricLowBound(const Vec3f& x, const AABB& bbox) {
		return 1.0f / minSqrDist(x, bbox);
	}

	inline Vec3f GeometricHighBound(const Vec3f& x, const AABB& bbox) {
		return 1.0f / maxSqrDist(x, bbox);
	}

	inline Vec3f MaterialTerm(const Material* material, const Vec3f& x, const Vec3f& normal, const Vec3f& y) {
		return material->diffuse * max(0.0f, dot(normal, (y - x).normalized()));
	}

	inline Vec3f MaterialBound(const Material* material, const Vec3f& x, const Vec3f& normal, const AABB& bbox) {
		float cosine_bound = max(0.0f, CosineBound(x, normal, bbox));
		//float cosine_bound = dot(normal, (bbox.center() - x).normalized());
		//if (cosine_bound <= 0.0f)
		//	std::cout << "cosine bound is zero: " << cosine_bound << std::endl;
		//if (cosine_bound >= 1.0f)
		//	std::cout << "cosine_bound: " << cosine_bound << std::endl;
		return material->diffuse * cosine_bound;
	}

	inline NodeCut CreateNodeCut(const LightNode* node, const HitInfo& hit) {
		const float cosTheta = max(0.0f, -dot(hit.normal, hit.incomming));
		const Vec3f& lightPos = node->reprecentative->position;
		const Vec3f G = GeometricTerm(hit.position, lightPos);
		const Vec3f M = MaterialTerm(hit.p_material, hit.position, hit.normal, lightPos);
		const Vec3f L = G * M * node->reprecentative->color * cosTheta;

		if (node->type == NodeType::Leaf) {
			return NodeCut(node, L, Vec3f(0.0f));
		}

		const Vec3f G_low_bound = GeometricLowBound(hit.position, node->bbox);
		const Vec3f G_high_bound = GeometricHighBound(hit.position, node->bbox);
		const Vec3f M_bound = MaterialBound(hit.p_material, hit.position, hit.normal, node->bbox);
		if (M_bound.element_sum() == 0.0f)
			return NodeCut(node, L, Vec3f(0.0f));

		const Vec3f L_low_bound = G_low_bound * M_bound * node->reprecentative->color * cosTheta;
		const Vec3f L_high_bound = G_high_bound * M_bound * node->reprecentative->color * cosTheta;

		const Vec3f error = max(abs(L - L_low_bound), abs(L - L_high_bound));

		return NodeCut(node, L, error);
	}
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

	virtual void init(std::vector<PointLight*> lights);
	virtual std::vector<PointLight*> GetLights(const HitInfo& hit, float threshold) const;
	virtual std::vector<Line> GetTreeEdges() const;

private:
	
	void BuildTree(std::unordered_set<LightNode*>& clusters);

	float distance(const PointLight* p1, const PointLight* p2);
	PointLight* MergeLights(PointLight* A, PointLight* B);

	virtual void GetTreeEdgesRecurse(std::vector<Line>& lines, const LightNode* node, Vec3f color) const;

	static void deleteNode(LightNode* node);
};