#include "BULightTree.h"
#include "config.h"

#include <limits>
#include <cassert>


#include "Sampling.h"
#include "KdTree.h"


BULightTree::BULightTree() : ReprecentativeLights(0), light_forest(0)
{
}

BULightTree::~BULightTree()
{
	for (auto c : light_forest) {
		deleteNode(c);
	}
	for (PointLight* light : ReprecentativeLights) {
		assert(light != nullptr);
		delete light;
	}
}

void BULightTree::init(std::vector<PointLight*> lights)
{
	/*
	- Define all lights as clusters.
	- Build KdTree over all clusters.
	- Find N nearest neighbors for each cluster.
	- Create edges between all the nearest neighbors.
	- Insert edges into priority queue.
	- Begin collapsing edges and insert new clusters in the tree.
		- Ignore edges to clusters which has been collapsed.
	*/

	std::cout << "Building Light structure\n";
	const size_t num = lights.size();

	// set for holding all the lights during construction.
	//std::set<LightNode*> clusters;
	std::unordered_set<LightNode*> clusters;

	// insert all lights in the cluster set.
	for (int i = 0; i < num; i++) {
		LightNode* node = new LightNode();

		node->type = NodeType::Leaf;
		node->bbox = AABB(lights[i]->position);
		node->ChildA = nullptr;
		node->ChildB = nullptr;
		node->reprecentative = lights[i];

		clusters.insert(node);
	}

	while (clusters.size() > 2) {
		BuildTree(clusters);
	}
	/*
	if (clusters.size() == 2) {
		// extract the two nodes
		LightNode* A = *clusters.begin();
		LightNode* B = *(clusters.begin()++);

		// create parent node
		LightNode* parent = new LightNode();

		// clear the two clusters from the set
		clusters.clear();

		parent->reprecentative = MergeLights(A->reprecentative, B->reprecentative);
		parent->ChildA = A;
		parent->ChildB = B;
		parent->bbox = AABB(A->bbox, B->bbox);
		parent->type = NodeType::Internal;

		clusters.insert(parent);
	}

	*/
	for (auto c : clusters) {
		light_forest.push_back(c);
	}
	std::cout << "clusters: " << clusters.size() << std::endl;
	//root = *clusters.begin();
}

std::vector<PointLight*> BULightTree::GetLights(const HitInfo& hit, float threshold) const
{
	std::vector<PointLight*> lights = std::vector<PointLight*>();
	for (auto c : light_forest)
		SearchLights(lights, c, hit, threshold);


	return lights;
}

std::vector<Line> BULightTree::GetTreeEdges() const
{
	std::vector<Line> lines = std::vector<Line>();
	for (auto c : light_forest)
		GetTreeEdgesRecurse(lines, c, Vec3f(1, 0, 0));
	return lines;
}

void BULightTree::SearchLights(std::vector<PointLight*>& out, LightNode* node, const HitInfo& hit, float threshold) const
{
	if (node->type == NodeType::Leaf) {
		out.push_back(node->reprecentative);
		return;
	}
	/*
	// Test if the bounding box is behind the surface.
	Vec3f t1 = (node->bbox.p_min - hit.position) / hit.normal;
	Vec3f t2 = (node->bbox.p_max - hit.position) / hit.normal;
	if (max(t1, t2).max_componont() < 0.0f)
		return;
	*/
	if (node->bbox.contained(hit.position)) {
		SearchLights(out, node->ChildA, hit, threshold);
		SearchLights(out, node->ChildB, hit, threshold);
		return;
	}

	const float cos_theta_bound = CosineBound(hit.position, hit.normal, node->bbox);
	if (cos_theta_bound <= 0.0f) {
		return;
	}

	if (cos_theta_bound > 1.0001f) {
		std::cout << "COS_THETA_BOUND: " << cos_theta_bound << ", FAILED!!!" << std::endl;
	}


	Vec3f dir = hit.position - node->reprecentative->position;
	const float dist = dir.length();
	dir /= dist;
	const float radius = (node->bbox.p_max - node->bbox.p_min).length() / 2.0f;

	const float min_dist = dist - radius;

	if (min_dist <= 0) {
		SearchLights(out, node->ChildA, hit, threshold);
		SearchLights(out, node->ChildB, hit, threshold);
		return;
	}

	// Geometric term
	//float G = 1.0f / (dist * dist);


	const Vec3f intensity = node->reprecentative->color * hit.p_material->diffuse * cos_theta_bound;
	const Vec3f rep = intensity / (dist * dist);
	const Vec3f worst = intensity / (min_dist * min_dist);

	const Vec3f error = abs(rep - worst);

	// add a random value to the threshold to remove artefacts
#if RANDOM_THRESHOLD
	if (error.element_sum() < threshold * random(1.0f, 1.5f)) {
#else
	if (error.element_sum() < threshold) {
#endif // RANDOM_THRESHOLD
		out.push_back(node->reprecentative);
	}
	else {
		SearchLights(out, node->ChildA, hit, threshold);
		SearchLights(out, node->ChildB, hit, threshold);
	}
	}


void BULightTree::BuildTree(std::unordered_set<LightNode*>& clusters)
{
	Graph graph = Graph();
	KdTree<Vec3f, GraphNode*, 3> kdtree = KdTree<Vec3f, GraphNode*, 3>();
	//graph.nodes.resize(clusters.size());

	// insert all lights in the cluster set.
	for (LightNode* node : clusters) {
		graph.nodes.push_back(GraphNode(node));
	}

	// insert nodes from the graph into the kd tree
	for (GraphNode& node : graph.nodes) {
		kdtree.Insert(node.node->reprecentative->position, &node);
		//std::cout << "Node*: " << &node << std::endl;
	}

	kdtree.Build();

	{
		// find the best neighbors to each node
		for (GraphNode& n1 : graph.nodes) {
			LightNode* c1 = n1.node;
			float dist = std::numeric_limits<float>::max();
			std::vector<KDTreeRecord<Vec3f, GraphNode*>> res = std::vector<KDTreeRecord<Vec3f, GraphNode*>>();
			kdtree.NNearest(c1->reprecentative->position, &n1, dist, res, N);
			//std::cout << "closest dist: " << res[0].dist << std::endl;
			for (int i = 0; i < N; i++) {
				graph.edges.push(GraphEdge(&n1, res[i].val, distance(n1.node->reprecentative, res[i].val->node->reprecentative)));
			}
		}
	}

	std::cout << "Graph: nodes = " << graph.nodes.size() << ", edges = " << graph.edges.size() << std::endl;

	// map for accelerating distance calculation.
	//std::unordered_map<std::pair<LightNode*, LightNode*>, float, hash_pair> umap = std::unordered_map<std::pair<LightNode*, LightNode*>, float, hash_pair>();

	while (graph.edges.size() > 1) {
		//float best = std::numeric_limits<float>::infinity();

		auto edge = graph.edges.top();
		graph.edges.pop();

		if (edge.n1->is_alive && edge.n2->is_alive) {

			GraphNode* left = edge.n1;
			GraphNode* right = edge.n2;

			assert(left != nullptr);
			assert(right != nullptr);
			assert(right != left);

			clusters.erase(left->node);
			clusters.erase(right->node);

			LightNode* node = new LightNode();
			node->type = NodeType::Internal;
			node->bbox = AABB(left->node->bbox, right->node->bbox);
			node->ChildA = left->node;
			node->ChildB = right->node;

			PointLight* leftLight = left->node->reprecentative;
			PointLight* rightLight = right->node->reprecentative;
			float sumA = leftLight->color.element_sum();
			float sumB = rightLight->color.element_sum();
			float sum = sumA + sumB;
			float p = sumA / sumB;

			PointLight* light = nullptr;
			if (random(0, 1) < p) {
				Vec3f pos = leftLight->position;
				Vec3f color = leftLight->color + rightLight->color;
				light = new PointLight(pos, color);
				ReprecentativeLights.push_back(light);
				left->node = node;
				right->is_alive = false;
				//AdjacencyMatrix.insert(std::make_pair(node, AdjacencyMatrix[left]));
			}
			else {
				Vec3f pos = rightLight->position;
				Vec3f color = rightLight->color + leftLight->color;
				light = new PointLight(pos, color);
				ReprecentativeLights.push_back(light);
				right->node = node;
				left->is_alive = false;
				//AdjacencyMatrix.insert(std::make_pair(node, AdjacencyMatrix[right]));
			}

			node->reprecentative = light;

			clusters.insert(node);

			//SparseAdjacencyMatrix.remove(left);
			//SparseAdjacencyMatrix.remove(right);


			/*for (auto& neighbor : AdjacencyMatrix[node]) {
				if (neighbor.val != node) {
					NodePair _pair = {};
					_pair.n1 = node;
					_pair.n2 = neighbor.val;
					_pair.dist = neighbor.dist;
					edge_queue.push(_pair);
				}
			}*/


			//std::cout << "nodes: " << graph.nodes.size() << ", edges: " << graph.edges.size() << std::endl;
		}

	}

	std::cout << "clusters: " << clusters.size() << std::endl;
}

float BULightTree::distance(const PointLight * p1, const PointLight * p2)
{
	return (p1->position - p2->position).length();
}

PointLight* BULightTree::MergeLights(PointLight * A, PointLight * B)
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

void BULightTree::GetTreeEdgesRecurse(std::vector<Line> & lines, const LightNode * node, Vec3f color) const
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

void BULightTree::deleteNode(LightNode * node)
{
	if (node == nullptr)
		return;
	if (node->type == NodeType::Internal) {
		deleteNode(node->ChildA);
		deleteNode(node->ChildB);
	}
	delete node;
}
