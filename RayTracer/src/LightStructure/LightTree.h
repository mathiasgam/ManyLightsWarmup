#pragma once

#include "Vec3f.h"
#include "LightStructure/LightStructure.h"
#include "PointLight.h"
#include "Structure/AABB.h"
#include "MortonCode.h"

#include <vector>

class LightTree : LightStructure
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

	struct BuildPrimitive {
		MortonCode3 code;
		PointLight* light;

		bool operator<(const BuildPrimitive& other) const
		{
			return code < other.code;
		}
	};

	std::vector<PointLight*> ReprecentativeLights;

	LightNode* root = nullptr;

public:
	LightTree() : ReprecentativeLights(0) {}
	~LightTree() {
		deleteNode(root);
		for (PointLight* light : ReprecentativeLights)
			delete light;
	}

	virtual void init(std::vector<PointLight*> lights);
	std::vector<PointLight*> GetLights(Vec3f sample_position, Vec3f sample_normal, float threshold) const;

private:

	void SearchLights(std::vector<PointLight*>& out, LightNode* node, Vec3f pos, Vec3f normal, float threshold) const;

	LightNode* generateHierarchy(std::vector<BuildPrimitive>& sortedPrimitives, int first, int last);
	int findSplit(std::vector<BuildPrimitive>& sortedPrimitives, int first, int last);

	void CalculateReprecentativeLights(LightNode* node);
	PointLight* MergeLights(PointLight* A, PointLight* B);

	static void deleteNode(LightNode* node);

};
