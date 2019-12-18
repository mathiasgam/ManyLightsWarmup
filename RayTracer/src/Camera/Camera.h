#pragma once

#include <vector>

#include "RayTracer/Ray.h"
#include "Vec2f.h"
#include "Vec2ui.h"

class Camera
{
public:

	Camera();
	~Camera();

	virtual Ray sample(Vec2f f) const = 0;
	virtual std::vector<Ray> generateRays(Vec2ui image_dim, unsigned int samples_per_pixel) const;

protected:
	
};


