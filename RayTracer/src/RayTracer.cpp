#include "RayTracer.h"
#include "Sampling.h"
#include "Vec3f.h"

// #define SAMPLE

Vec3f RayTracer::SampleLights(const HitInfo& hit, const Vec3f& material, int& numLights, const float threshold) const {
	Vec3f res = Vec3f(0.0f);
	const float epsilon = p_scene->scene_epsilon;

	for (PointLight* light : p_scene->GetLights(hit, threshold)) {
		Vec3f dir = light->position - hit.position;
		float dist = dir.length();
		dir = dir.normalized();

		Vec3f material_term = material;// *max(0.0f, dot(-hit.normal, hit.incomming));

		if (material_term.element_sum() == 0.0f)
			continue;

		Ray shadow = Ray(hit.position, dir, epsilon, dist - epsilon);
		if (!Occluded(shadow)) {
			res += material_term * (light->color / (dist * dist)) * max(dot(hit.normal, dir), 0.0f);
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
	result.num_lights = 0;
	HitInfo& hit = result.hit;

	const float epsilon = p_scene->scene_epsilon;

	const int samples = 5;

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
		const Material* material = p_scene->GetMaterial(hit.material_index);
		hit.p_material = material;

		result.color = material->diffuse * fmaxf(-dot(normal, Vec3f(-1.0f,-1.0f,-1.0f).normalized()), 0.0f);
		
#ifdef SAMPLE
		const float sample_factor = 1.0f / samples;
		for (int i = 0; i < samples; i++) {
			Vec3f sample_dir = SampleCosineSphere();
			rotate_to_normal(normal, sample_dir);

			Ray sample_ray = Ray(hit.position, sample_dir, epsilon, 10000);
			result.color += material->diffuse * sample(sample_ray, 2) * sample_factor;
		}

#endif // SAMPLE

		result.color += SampleLights(hit, material->diffuse, light_count, LightThreshold);
		result.num_occlusion_rays += light_count;

		//// Add ambient light
		//result.color += hit.color * p_scene->GetAmbient(hit.position);

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

		const Material* material = p_scene->GetMaterial(hit.material_index);

		if (bounces > 0) {
			Vec3f sample_dir = SampleCosineSphere();
			rotate_to_normal(normal, sample_dir);

			Ray sample_ray = Ray(hit.position, sample_dir, epsilon, 10000);
			res += material->diffuse * sample(sample_ray, --bounces);
		}

		res += SampleLights(hit, material->diffuse, light_count, LightThreshold * 10.0f);
		//res += hit.color * p_scene->GetAmbient(hit.position);
		//res = res / (hit.t * hit.t);
	}

	return res;
}
