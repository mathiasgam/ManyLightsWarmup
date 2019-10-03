#include "PinHoleCamera.h"



PinHoleCamera::PinHoleCamera():
	position(-1,1.5,3), direction(-1,0,0), w(0.2f), h(0.2f), n(0.5f)
{
	direction = Vec3f(0, 0.5f, 0) - position;
	direction = direction.normalized();
	tangent_w = cross(direction, Vec3f(0, 1, 0));
	tangent_h = cross(direction, tangent_w);
}


PinHoleCamera::~PinHoleCamera()
{
}

Ray PinHoleCamera::sample(Vec2f f)
{
	Vec3f center = position;

	Vec3f dir = direction * n + tangent_w * w * f[0] + tangent_h * h * f[1];
	dir = dir.normalized();

	return Ray(center, dir, 0, 1000);
}
