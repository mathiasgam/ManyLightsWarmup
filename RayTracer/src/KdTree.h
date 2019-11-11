#pragma once

#include <vector>

#include "Vec3f.h"

/// Uses the KeyT to map ValT in the KdTree. Example of use could be KeyT = Vec3f and ValT = LightSource*, where the Vec3f is used to map the lights by position
/// The KeyT class is assumed to be an extension of the ArithVec class, but can also consist of others, as long as the used methods are implemented.
template<class KeyT, class ValT, unsigned int K>
class KdTree
{
private:

	class KdNode {
	public:
		KeyT key;
		ValT val;
		short axis;

		KdNode() : axis(-1) {}
		KdNode(const KeyT& _key, const ValT& _val) : key(_key), val(_val), axis(-1) {}
		~KdNode() {}

		float SqrDist(const KeyT& other) const {
			KeyT diff = key - other;
			return dot(diff, diff);
		}

		float Dist(const KeyT& other) const {
			return sqrt(SqrDist(other));
		}

		// unsigned int Depth() const;
		// unsigned int Num() const;
	};

	class KeyAxisCompare {
	private:
		short axis;

	public:
		KeyAxisCompare() : axis(-1) {}
		KeyAxisCompare(short _axis) : axis(_axis) {}
		bool operator() (const KdNode& k0, const KdNode& k1) const {
			return k0.key[axis] < k1.key[axis];
		}

		bool operator() (const KeyT& k0, const KeyT& k1) const {
			return k0[axis] < k1[axis];
		}
	};

	KeyAxisCompare axisCompare[K];

	bool isBuild = false;

	std::vector<KdNode> nodes;
	std::vector<KdNode> keys;

public:
	KdTree();
	~KdTree();

	bool Nearest(const KeyT& key, float dist, KeyT& out_key, ValT& out_val) const;

	void Insert(const KeyT& _key, const ValT& _val);
	//void remove(const T*);
	void Build();

	//const T* Nearest(const T*) const;

	unsigned int Size() const;
	bool IsBuild() const;
	//unsigned int depth() const;

private:

	void BuildRecurse(const int current, const int begin, const int end);
	/// will return the index of the nearest node in the tree within the distance 'dist', if present. 
	int NearestRecurse(const int node, const KeyT& key, float& dist) const;
	int OptimalAxis(const unsigned int begin, const unsigned int end) const;

};



template<class KeyT, class ValT, unsigned int K>
inline KdTree<KeyT, ValT, K>::KdTree()
{
	isBuild = false;
	nodes = std::vector<KdNode>();
	keys = std::vector<KdNode>();
	for (int i = 0; i < K; i++) {
		axisCompare[i] = KeyAxisCompare(i);
	}
}


template<class KeyT, class ValT, unsigned int K>
inline KdTree<KeyT, ValT, K>::~KdTree()
{
}

template<class KeyT, class ValT, unsigned int K>
inline bool KdTree<KeyT, ValT, K>::Nearest(const KeyT& key, float dist, KeyT& out_key, ValT& out_val) const
{
	// check that the tree is in fact build since last insert
	assert(isBuild);

	if (nodes.size() > 1) {
		// float sqr_dist = dist * dist;
		if (int n = NearestRecurse(1, key, dist)) {
			out_key = nodes[n].key;
			out_val = nodes[n].val;
			//dist = sqrt(dist);
			return true;
		}
	}
	return false;
}


template<class KeyT, class ValT, unsigned int K>
inline void KdTree<KeyT, ValT, K>::Insert(const KeyT& _key, const ValT& _val)
{
	if (isBuild) {
		isBuild = false;
	}
	keys.push_back(KdNode(_key, _val));
}

template<class KeyT, class ValT, unsigned int K>
inline void KdTree<KeyT, ValT, K>::Build()
{
	nodes.resize(keys.size());
	BuildRecurse(1, 1, nodes.size());
	isBuild = true;
}


template<class KeyT, class ValT, unsigned int K>
inline unsigned int KdTree<KeyT, ValT, K>::Size() const
{
	return nodes.size();
}


template<class KeyT, class ValT, unsigned int K>
inline bool KdTree<KeyT, ValT, K>::IsBuild() const
{
	return isBuild;
}

template<class KeyT, class ValT, unsigned int K>
inline void KdTree<KeyT, ValT, K>::BuildRecurse(const int current, const int begin, const int end)
{
	const int N = end - begin;

	if (N == 1) {
		nodes[current] = keys[begin];
		nodes[current].axis = -1;
		return;
	}

	// find the best axis to sort split the elements on
	short axis = OptimalAxis(begin, end);
	const int M = 1 << fast_log2(N);
	const int R = N - (M - 1);
	int left_size = (M - 2) / 2;
	int right_size = (M - 2) / 2;
	if (R < M / 2) {
		left_size += R;
	}
	else {
		left_size += M / 2;
		right_size += R - M / 2;
	}

	// find median
	int median = begin + left_size;

	// sort the keys according to the axis
	KdNode* data = keys.data();
	std::nth_element(data + begin, data + median, data + end, axisCompare[axis]);

	// insert node in the trace tree
	nodes[current] = keys[median];
	nodes[current].axis = axis;

	// recursively build left and right subtree
	if (left_size > 0)
		BuildRecurse(current * 2, begin, median);

	if (right_size > 0)
		BuildRecurse(current * 2 + 1, median + 1, end);
}

template<class KeyT, class ValT, unsigned int K>
inline int KdTree<KeyT, ValT, K>::NearestRecurse(const int node, const KeyT& key, float& _dist) const
{
	int return_node = 0;
	float dist = nodes[node].Dist(key);

	if (dist < _dist) {
		_dist = dist;
		return_node = node;
	}

	int axis = nodes[node].axis;
	if (axis != -1) {
		float axis_dist = fabsf(nodes[node].key[axis] - key[axis]);
		bool axis_dir = key[axis] < nodes[node].key[axis];

		if (axis_dir || axis_dist < _dist) {

			int left_child = 2 * node;
			if (left_child < nodes.size())
				if (int nleft = NearestRecurse(left_child, key, _dist))
					return_node = nleft;

		}
		if (axis_dir || axis_dist < _dist) {
			int right_child = 2 * node + 1;
			if (right_child < nodes.size())
				if (int nright = NearestRecurse(right_child, key, _dist))
					return_node = nright;
		}
	}
	return return_node;
}

template<class KeyT, class ValT, unsigned int K>
inline int KdTree<KeyT, ValT, K>::OptimalAxis(const unsigned int _begin, const unsigned int _end) const
{
	KeyT keymin = keys[_begin].key;
	KeyT keymax = keys[_begin].key;
	for (unsigned int i = _begin + 1; i < _end; i++) {
		keymin = min(keymin, keys[i].key);
		keymax = max(keymax, keys[i].key);
	}

	KeyT diff = keymax - keymin;
	return diff.max_axis();
}
