#pragma once

#include "Structure/AABB.h"
#include "PointLight.h"
#include "Material.h"
#include "HitInfo.h"

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

struct NodeCut {
	const LightNode* node;
	Vec3f radiance;
	Vec3f error;
	float errorsum;

	NodeCut(const LightNode* node, const Vec3f radiance, const Vec3f error) : node(node), radiance(radiance), error(error), errorsum(error.element_sum()) {}

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

