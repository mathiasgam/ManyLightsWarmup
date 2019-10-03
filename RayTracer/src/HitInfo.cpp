#include "HitInfo.h"


HitInfo::HitInfo() :
	t(MAX_DIST), position(0, 0, 0), normal(1, 0, 0), trace_depth(0)
{
}


HitInfo::~HitInfo()
{
}
