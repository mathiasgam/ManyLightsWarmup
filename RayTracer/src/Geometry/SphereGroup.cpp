#include "SphereGroup.h"


SphereGroup::~SphereGroup()
{
}

AABB SphereGroup::get_bbox() const
{
	AABB bbox;
	for (auto& s : spheres) {
		bbox.add_bbox(s.get_bbox());
	}
	return bbox;
}

AABB SphereGroup::get_bbox(unsigned int i) const
{
	return spheres[i].get_bbox();
}
