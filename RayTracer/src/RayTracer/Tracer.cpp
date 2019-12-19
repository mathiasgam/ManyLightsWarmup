#include "Tracer.h"

#include <algorithm>
#include <execution>
#include <memory>
#include <mutex>

#include "HitInfo.h"
#include "Shader/lambertian.h"
#include "Sampling.h"

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
	auto& trace_structure = p_scene->GetTraceStructure();


	std::vector<Ray> trace_rays = std::vector<Ray>();
	std::vector<HitInfo> hits = std::vector<HitInfo>();



	while (!rayQueue.empty()) {
		{// swap the rays in the queue to thge active trace buffer
			const std::lock_guard<std::mutex> lock(mutex_queue);
			trace_rays.resize(rayQueue.size());
			std::atomic<unsigned int> count(0);
			std::for_each(std::execution::par_unseq, rayQueue.begin(), rayQueue.end(), [&trace_rays, &count](Ray& ray) {
				unsigned int i = count++;
				trace_rays[i] = Ray(ray);
			});
			rayQueue.clear();
		}
		unsigned int N = trace_rays.size();

		std::cout << "Num rays: " << N << std::endl;

		// initialize hit infos
		hits.resize(trace_rays.size());
		std::for_each(std::execution::par_unseq, hits.begin(), hits.end(), [](HitInfo& hit) {
			hit = HitInfo();
		});

		// TODO implement rest of function
		//assert(false);

		// trace all current rays
		std::cout << "Tracing rays" << std::endl;
		trace_structure.closest_hit(trace_rays, hits);

		std::cout << "Shading hits" << std::endl;
		// evaluate hits and generate new rays if needed
		std::atomic<unsigned int> count(0);
		std::for_each(std::execution::par_unseq, trace_rays.begin(), trace_rays.end(), [=,&count, &hits, &trace_rays](Ray& r) {
			unsigned int index = count++;
			shade(trace_rays[index], hits[index]);
		});
		
		trace_rays.clear();
		hits.clear();
	}
	std::cout << "Trace done!" << std::endl;
}

void Tracer::shade(Ray & ray, HitInfo & hit)
{
	if (ray.depth == 0) {
		for (int i = 0; i < 10; i++) {
			Vec3f dir = SampleCosineSphere();
			rotate_to_normal(hit.normal, dir);

			Ray sample = Ray(hit.position, dir, 0.001f, 1000, ray.depth + 1, ray.pixel, ray.throughput * 0.1f);
			push(sample);
		}
	}
	else {
		if (!ray.hashit) {
			result.add(ray.pixel, ray.throughput);
		}
	}
}

Image& Tracer::getResult()
{
	return result;
}
