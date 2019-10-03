
#include "Vec3f.h"
#include "Ray.h"
#include "HitInfo.h"


class shader
{
public:
	virtual Vec3f shade(Ray ray, HitInfo hit, bool emit = false) = 0;
};