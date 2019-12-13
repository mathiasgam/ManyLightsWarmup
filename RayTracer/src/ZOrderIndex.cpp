#include "ZOrderIndex.h"
#include <algorithm>

ZOrderIndex::ZOrderIndex(const Vec3f & vec)
{
	max_axis = vec.max_axis();
	val = vec;
	//std::sort(val.data, val.data + 3, std::greater<float>());
}

ZOrderIndex::~ZOrderIndex()
{
}
