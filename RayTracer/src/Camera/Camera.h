#pragma once

#include "Ray.h"
#include "Vec2f.h"

class Camera
{
public:

	Camera();
	~Camera();

	virtual Ray sample(Vec2f f) = 0;

protected:
	
};

