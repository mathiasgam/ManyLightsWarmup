#pragma once
#include "Camera.h"
class PinHoleCamera :
	public Camera
{
public:

	Vec3f position;
	Vec3f direction;
	Vec3f tangent_w;
	Vec3f tangent_h;
	float w, h, n;

	PinHoleCamera();
	~PinHoleCamera();

	Ray sample(Vec2f f);
};

