#include "PinHoleCamera.h"



void PinHoleCamera::UpdateTangents()
{
	tangent_w = cross(direction, Vec3f(0, 1, 0));
	tangent_h = cross(direction, tangent_w);
}

PinHoleCamera::PinHoleCamera():
	position(0.0f,0.0f,0.0f), direction(1.0f,0.0f,0.0f), w(0.2f), h(0.2f), n(0.5f)
{
	UpdateTangents();
}


PinHoleCamera::~PinHoleCamera()
{
}

void PinHoleCamera::SetPosition(Vec3f _position)
{
	position = _position;
}

void PinHoleCamera::SetDirection(Vec3f _direction)
{
	direction = _direction;
	UpdateTangents();
}

Ray PinHoleCamera::sample(Vec2f f) const
{
	Vec3f center = position;

	Vec3f dir = direction * n + tangent_w * w * f[0] + tangent_h * h * f[1];
	dir = dir.normalized();

	return Ray(center, dir, 0, 10000);
}

void PinHoleCamera::LookAt(Vec3f at)
{
	direction = (at - position).normalized();
	UpdateTangents();
}
