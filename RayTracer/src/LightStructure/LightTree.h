#pragma once

#include "Vec3f.h"
#include "LightStructure/LightStructure.h"
#include "PointLight.h"
#include "Structure/AABB.h"
#include "MortonCode.h"

#include "Geometry/Line.h"

#include <vector>
#include <queue>

#include "LightNode.h"


class LightTree : LightStructure
{
private:
	

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
	std::vector<PointLight*> GetLights(const HitInfo& hit, float threshold) const;
	virtual std::vector<Line> GetTreeEdges() const;

private:

	//void SearchLights(std::vector<PointLight*>& out, LightNode* node, const HitInfo& hit, float threshold) const;

	LightNode* generateHierarchy(std::vector<BuildPrimitive>& sortedPrimitives, int first, int last);
	int findSplit(std::vector<BuildPrimitive>& sortedPrimitives, int first, int last);

	void CalculateReprecentativeLights(LightNode* node);
	PointLight* MergeLights(PointLight* A, PointLight* B);

	virtual void GetTreeEdgesRecurse(std::vector<Line>& lines, const LightNode* node, Vec3f color) const;

	static void deleteNode(LightNode* node);

};

