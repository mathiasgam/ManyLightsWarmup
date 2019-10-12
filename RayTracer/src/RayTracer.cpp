#include "RayTracer.h"


TraceResult RayTracer::trace(Ray ray)
{
	TraceResult result;
	result.color = Vec3f(0.0f);
	HitInfo& hit = result.hit;

	const float epsilon = p_scene->scene_epsilon;

	int light_count = 0;
	if (p_scene->closest_hit(ray, hit)) {
		Vec3f normal = hit.normal.normalized();
		if (dot(normal, ray.direction) > 0)
			normal = -normal;

		for (PointLight* light : p_scene->GetLights(hit.position, hit.normal, LightThreshold)) {
			Vec3f dir = light->position - hit.position;
			float dist = dir.length();
			dir = dir.normalized();

			light_count++;

			Ray shadow = Ray(hit.position, dir, epsilon, dist - epsilon);
			if (!Occluded(shadow)) {
				result.color += hit.color * (light->color / (dist * dist)) * fmaxf(dot(normal, dir), 0.0f);
			}
		}

		// Add ambient light
		result.color += hit.color * p_scene->GetAmbient(hit.position);

		result.color = min(result.color, Vec3f(1.0f));
		 
	}
	else {
		result.color = p_scene->GetBackground(ray.direction);
		result.num_lights = 0;
		
	}
	result.num_lights = light_count;
	return result;
}

bool RayTracer::Occluded(Ray& ray) const
{
	return p_scene->any_hit(ray);
}
