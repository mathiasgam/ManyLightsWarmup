#pragma once

#include <vector>
#include <mutex>

#include "Ray.h"
#include "Util/Image.h"
#include "Scene.h"


class Tracer
{
private:

	std::mutex mutex_queue;
	std::vector<Ray> rayQueue;
	Image result;
	Scene* p_scene = nullptr;

public:
	Tracer();
	~Tracer();

	void setScene(Scene* scene);

	void push(Ray ray);
	void trace();

	Image getResult();

private:

};
