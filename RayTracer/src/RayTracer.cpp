#include "RayTracer.h"
#include "Sampling.h"
#include "Vec3f.h"

Vec3f RayTracer::SampleLights(const Vec3f& pos, const Vec3f normal, const Vec3f& material, int& numLights) const {
	Vec3f res = Vec3f(0.0f);
	const float epsilon = p_scene->scene_epsilon;

	for (PointLight* light : p_scene->GetLights(pos, normal, LightThreshold)) {
		Vec3f dir = light->position - pos;
		float dist = dir.length();
		dir = dir.normalized();

		Ray shadow = Ray(pos, dir, epsilon, dist - epsilon);
		if (!Occluded(shadow)) {
			res += material * (light->color / (dist * dist)) * max(dot(normal, dir), 0.0f);
		}

		numLights++;
	}
	return res;
}


TraceResult RayTracer::trace(Ray ray)
{
	TraceResult result;
	result.color = Vec3f(0.0f);
	result.num_rays = 0;
	result.num_occlusion_rays = 0;
	HitInfo& hit = result.hit;

	const float epsilon = p_scene->scene_epsilon;

	const int samples = 50;

	HitInfo visualInfo = HitInfo();
	if (p_scene->trace_visuals(Ray(ray), visualInfo)) {
		result.visualColor = hit.color;
	}
	else {
		result.visualColor = Vec3f(0, 0, 0);
	}

	int light_count = 0;
	if (p_scene->closest_hit(ray, hit)) {
		Vec3f normal = hit.normal.normalized();
		if (dot(normal, ray.direction) > 0)
			normal = -normal;

		//result.color = hit.color * fmaxf(dot(-normal, Vec3f(-1.0f,-1.0f,-1.0f).normalized()), 0.0f);
		//const float sample_factor = 1.0f / samples;
		//for (int i = 0; i < samples; i++) {
		//	Vec3f sample_dir = SampleCosineSphere();
		//	rotate_to_normal(normal, sample_dir);

		//	Ray sample_ray = Ray(hit.position, sample_dir, epsilon, 10000);
		//	result.color += hit.color * sample(sample_ray, 2) * sample_factor;
		//}

		result.color += SampleLights(hit.position, normal, hit.color, light_count);
		result.num_occlusion_rays += light_count;

		//// Add ambient light
		result.color += hit.color * p_scene->GetAmbient(hit.position);

		result.color = min(result.color, Vec3f(1.0f));
		 
	}
	else {
		result.color = p_scene->GetBackground(ray.direction);
		result.num_lights = 0;
		
	}
	result.num_lights = light_count;
	result.num_rays += 1;
	return result;
}

bool RayTracer::Occluded(Ray& ray) const
{
	return p_scene->any_hit(ray);
}

Vec3f RayTracer::sample(Ray ray, int bounces) const
{
	Vec3f res = Vec3f(0.0f);
	HitInfo hit = HitInfo();

	const float epsilon = p_scene->scene_epsilon;
	int light_count = 0;

	if (p_scene->closest_hit(ray, hit)) {
		Vec3f normal = hit.normal.normalized();
		if (dot(normal, ray.direction) > 0)
			normal = -normal;

		if (bounces > 0) {
			Vec3f sample_dir = SampleCosineSphere();
			rotate_to_normal(normal, sample_dir);

			Ray sample_ray = Ray(hit.position, sample_dir, epsilon, 10000);
			res += hit.color * sample(sample_ray, --bounces);
		}

		res += SampleLights(hit.position, normal, hit.color, light_count);
		//res += hit.color * p_scene->GetAmbient(hit.position);
		//res = res / (hit.t * hit.t);
	}

	return res;
}
