#pragma once

#include <vector>

#include "Vec3f.h"

/// template for N dimentional Kd trees. Does not assume ownership of the data points.
template<class T, unsigned int K>
class KdTree
{
private:

	class KdNode {
	public:
		T* point = nullptr;
		KdNode* left = nullptr; 
		KdNode* right = nullptr;

		KdNode() {}
		~KdNode() {
			if (left != nullptr)
				delete left;
			if (right != nullptr)
				delete right;
		}

		unsigned int depth() const;
		unsigned int num() const;
	};

	KdNode* root = nullptr;

public:
	KdTree();
	~KdTree();

	// T* Nearest(const T&) const;
	// T* KNearest(const T&, const unsigned int K) const;

	void insert(T*);
	// void remove(const T*);

	unsigned int size() const;
	unsigned int depth() const;

private:

};

template<class T, unsigned int K>
inline KdTree<T, K>::KdTree()
{
}

template<class T, unsigned int K>
inline KdTree<T, K>::~KdTree()
{
	delete root;
}

template<class T, unsigned int K>
inline void KdTree<T, K>::insert(T* point)
{
	KdNode* node = new KdNode();
	node->point = point;

	if (root == nullptr) {
		// tree empty, insert node as root and return
		root = node;
		return;
	}

	// find position
	int depth = 0;
	KdNode* parent = root;
	while (true) {
		unsigned int k = depth % K;
		auto median = parent->point[k];

		if (&point[k] < median) {
			if (parent->left == nullptr) {
				parent->left = node;
				break;
			}
			else {
				parent = parent->left;
				continue;
			}
		}
		else {
			if (parent->right == nullptr) {
				parent->right = node;
				break;
			}
			else {
				parent = parent->right;
				continue;
			}
		}
	}
}

template<class T, unsigned int K>
inline unsigned int KdTree<T, K>::size() const
{
	return root->num();
}

template<class T, unsigned int K>
inline unsigned int KdTree<T, K>::depth() const
{
	return root->depth();
}

template<class T, unsigned int K>
inline unsigned int KdTree<T, K>::KdNode::depth() const
{
	depth = 0;
	if (left != nullptr)
		depth = max(depth, left->depth());
	if (right != nullptr)
		depth = max(depth, right->depth());
	return depth + 1;
}

template<class T, unsigned int K>
inline unsigned int KdTree<T, K>::KdNode::num() const
{
	unsigned int num = 1;
	if (left != nullptr)
		num += left->num();
	if (right != nullptr)
		num += right->num();
	return num;
}
