#include "Tracer.h"

#include <algorithm>
#include <execution>
#include <memory>
#include <mutex>

#include "HitInfo.h"
#include "Shader/lambertian.h"
#include "Sampling.h"

Tracer::Tracer() : mutex_queue(), rayQueue(), result(1920, 1080)
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

void Tracer::push(std::list<Ray> rays)
{
	const std::lock_guard<std::mutex> lock(mutex_queue);
	rayQueue.splice(rayQueue.end(), rays);
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
			std::cout << "Swapping buffers" << std::endl;
			const std::lock_guard<std::mutex> lock(mutex_queue);
			trace_rays.resize(rayQueue.size());
			std::copy(rayQueue.begin(), rayQueue.end(), trace_rays.begin());

			/*
			std::atomic<unsigned int> count(0);
			std::for_each(std::execution::seq, rayQueue.begin(), rayQueue.end(), [&trace_rays, &count](Ray& ray) {
				unsigned int i = count++;
				trace_rays[i] = ray;
			});
			*/
			rayQueue.clear();
		}
		unsigned int N = trace_rays.size();

		std::cout << "Num rays: " << N << std::endl;

		// initialize hit infos
		hits.resize(trace_rays.size());
		{
			std::atomic<unsigned int> count(0);
			std::for_each(std::execution::par, hits.begin(), hits.end(), [&hits, &count](HitInfo& hit) {
				const unsigned int i = count++;
				hits[i] = HitInfo();
			});
		}

		// trace all current rays
		std::cout << "Tracing rays" << std::endl;
		trace_structure.closest_hit(trace_rays, hits);

		std::cout << "Shading hits" << std::endl;
		// evaluate hits and generate new rays if needed
		{
			std::atomic<unsigned int> count(0);
			std::for_each(std::execution::par, trace_rays.begin(), trace_rays.end(), [=, &count, &hits, &trace_rays](Ray& r) {
				unsigned int index = count++;
				shade(trace_rays[index], hits[index]);
			});
		}

		//trace_rays.clear();
		//hits.clear();
	}
	std::cout << "Trace done!" << std::endl;
}

void Tracer::shade(Ray & ray, HitInfo & hit)
{
	if (ray.depth == 0) {
		if (ray.hashit) {
			
			const Material* mat = p_scene->GetMaterial(hit.material_index);
			hit.p_material = mat;

			auto lights = p_scene->GetLights(hit, 0.02f);
			unsigned int N = lights.size();

			std::list<Ray> rays = std::list<Ray>();

			for (int i = 0; i < N; i++) {
				auto& light = lights[i];
				Vec3f diff = light->position - hit.position;
				Vec3f dir = diff.normalized();
				float dist = diff.length();


				Vec3f material_term = mat->diffuse * max(0.0f, dot(hit.normal, dir));
				if (material_term.element_sum() == 0.0f)
					continue;

				Vec3f contribution = material_term * (light->color / (dist * dist) * ray.throughput);
				Ray sample = Ray(hit.position, dir, 0.0001f, dist - 0.0001f, ray.depth + 1, ray.pixel, contribution);
				rays.push_back(sample);
			}
			//result.add(ray.pixel, max(Vec3f(0.0f), min(Vec3f(1.0f), hit.position)));
			//result.add(ray.pixel, (float)N / 10);
			push(rays);
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
