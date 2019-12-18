#include "Tracer.h"

#include <algorithm>
#include <memory>
#include <mutex>

#include "HitInfo.h"
#include "Shader/lambertian.h"

Tracer::Tracer() : mutex_queue(), rayQueue(), result(1920,1080)
{
}

Tracer::~Tracer()
{
}

void Tracer::setScene(Scene * scene)
{
	p_scene = scene;
}

void Tracer::push(Ray ray)
{
	const std::lock_guard<std::mutex> lock(mutex_queue);
	rayQueue.push_back(ray);
}

void Tracer::trace()
{
	if (!p_scene->IsReady()) {
		p_scene->prepare();
	}


	std::vector<Ray> trace_rays = std::vector<Ray>();
	// swap the rays in the queue to thge active trace buffer
	{
		const std::lock_guard<std::mutex> lock(mutex_queue);
		trace_rays.swap(rayQueue);
	}
	unsigned int N = trace_rays.size();

	std::cout << "Num rays: " << N << std::endl;

	std::vector<HitInfo> hits = std::vector<HitInfo>();
	hits.resize(trace_rays.size());

	// TODO implement rest of function
	assert(false);
	auto& trace_structure = p_scene->GetTraceStructure();

	trace_structure.closest_hit(trace_rays, hits);

	for (int i = 0; i < N; i++) {
		Vec3f res = hits[i].p_material->diffuse;
		std::cout << res << std::endl;
	}

	std::cout << "Trace done!" << std::endl;
}

Image Tracer::getResult()
{
	return result;
}
