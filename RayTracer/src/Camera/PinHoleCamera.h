#pragma once
#include "Camera.h"
class PinHoleCamera :
	public Camera
{
private:
	Vec3f position;
	Vec3f direction;
	Vec3f tangent_w;
	Vec3f tangent_h;
	float w, h, n;

	void UpdateTangents();

public:

	PinHoleCamera();
	~PinHoleCamera();

	void SetPosition(Vec3f);
	void SetDirection(Vec3f);

	Ray sample(Vec2f f) const;
	void LookAt(Vec3f position);
};

