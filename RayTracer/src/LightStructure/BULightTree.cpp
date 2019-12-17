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
Vec3f total_radiance = Vec3f(0.0f);
std::priority_queue<NodeCut, std::vector<NodeCut>, NodeCutCompare> queue = std::priority_queue<NodeCut, std::vector<NodeCut>, NodeCutCompare>();

// push the root node to the queue
for (LightNode* n : light_forest) {
	NodeCut root = CreateNodeCut(n, hit);
	queue.push(root);
	total_radiance += root.radiance;
}

while (queue.size() < 1000) {
	NodeCut cut = queue.top();

	//std::cout << "Error: " << cut.error << std::endl;
#if RANDOM_THRESHOLD
	if ((cut.error - total_radiance * threshold * random(0.9f, 1.1)).max_componont() > 0.0f) {
#else
	if ((cut.error - total_radiance * threshold).max_componont() > 0.0f) {
#endif
		queue.pop();
		total_radiance -= cut.radiance;

		NodeCut A = CreateNodeCut(cut.node->ChildA, hit);
		NodeCut B = CreateNodeCut(cut.node->ChildB, hit);
		total_radiance += A.radiance + B.radiance;

		//std::cout << "Prev error: " << cut.errorsum << ", new error: " << A.errorsum + B.errorsum << std::endl;

		queue.push(A);
		queue.push(B);
	}
	else {
		// if the worst error is less than the permitted error, theres no reason to refine more clusters
		//std::cout << "threshold reached: " << cut.errorsum << std::endl;
		break;
	}

}
size_t N = queue.size();
lights.resize(N);
//std::cout << "Num lights: " << N << ", max error: " << queue.top().errorsum << std::endl;
for (int i = 0; i < N; i++) {
	lights[i] = queue.top().node->reprecentative;
	queue.pop();
}

return lights;
}

std::vector<Line> BULightTree::GetTreeEdges() const
{
	std::vector<Line> lines = std::vector<Line>();
	for (auto c : light_forest)
		GetTreeEdgesRecurse(lines, c, Vec3f(1, 0, 0));
	return lines;
}

/*
Vec3f GetErrorBound(const LightNode& node, const HitInfo& hit, const Vec3f& material_term, const Vec3f& radiance) {
	if (node.type == NodeType::Leaf) {
		return Vec3f(0.0f);
	}
	const float min_sqr_dist = minSqrDist(hit.position, node.bbox);
	if (min_sqr_dist <= 0.0f) {
		//std::cout << "min dist fail: " << min_sqr_dist << std::endl;
		return Vec3f(1000.0f);
	}
	const float cos_theta_bound = CosineBound(hit.position, hit.normal, node.bbox);
	const Vec3f worst = (node.reprecentative->color / min_sqr_dist) * material_term;

	return abs(radiance - worst);
}

Vec3f GetRadiance(const LightNode& node, const HitInfo& hit, const Vec3f& material_term) {
	const Vec3f diff = hit.position - node.reprecentative->position;
	const float sqr_dist = diff.sqr_length();
	return (node.reprecentative->color / sqr_dist) * material_term;
}
*/

inline float distance(const PointLight* p1, const PointLight* p2)
{
	return (p1->position - p2->position).length();
}

float BULightTree::distance(const PointLight* p1, const PointLight* p2)
{
	return distance(p1, p2);
}


void BULightTree::BuildTree(std::unordered_set<LightNode*>& clusters)
{
	Graph graph = Graph();
	KdTree<Vec3f, GraphNode*, 3> kdtree = KdTree<Vec3f, GraphNode*, 3>();
	//graph.nodes.resize(clusters.size());
	AABB bbox = AABB();

	graph.nodes.reserve(clusters.size());
	kdtree.Reserve(clusters.size());

	// insert all lights in the cluster set.
	for (LightNode* node : clusters) {
		graph.nodes.push_back(GraphNode(node));
		bbox.add_point(node->reprecentative->position);
	}

	// insert nodes from the graph into the kd tree
	for (GraphNode& node : graph.nodes) {
		kdtree.Insert(node.node->reprecentative->position, &node);
		//std::cout << "Node*: " << &node << std::endl;
	}

	//kdtree.BuildZOrder();
	kdtree.Build();



	std::for_each(std::execution::par, graph.nodes.begin(), graph.nodes.end(), [&bbox, &graph, &kdtree](GraphNode& n1) {
		static std::mutex m = std::mutex();
		LightNode* c1 = n1.node;

		// initial guess of the distance
		float dist = bbox.size().max_componont() / 100.0f;

		Vec3f pos;
		GraphNode* n2;

		// search for a node within the guessed distance
		if (kdtree.Nearest(c1->reprecentative->position, &n1, dist, pos, n2)) {
			std::scoped_lock lock(m);
			graph.edges.push(GraphEdge(&n1, n2, dist));
		}
		else { // if nothing was found with the guess. do the more expensive nearest neighbour search with maximum allowed distance
			dist = std::numeric_limits<float>::max();
			if (kdtree.Nearest(c1->reprecentative->position, &n1, dist, pos, n2)) {
				std::scoped_lock lock(m);
				graph.edges.push(GraphEdge(&n1, n2, dist));
			}
		}
	});

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

PointLight* BULightTree::MergeLights(PointLight* A, PointLight* B)
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
