#pragma once

#include <list>
#include <vector>
#include <mutex>

#include "Ray.h"
#include "Util/Image.h"
#include "Scene.h"


class Tracer
{
private:

	std::mutex mutex_queue;
	std::list<Ray> rayQueue;
	Image result;
	Scene* p_scene = nullptr;

public:
	Tracer();
	~Tracer();

	void setScene(Scene* scene);

	void push(Ray ray);
	void push(std::list<Ray> rays);
	void trace();
	void shade(Ray& ray, HitInfo& hit);

	Image& getResult();

private:

};
