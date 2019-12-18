#include "Camera.h"

#include <mutex>
#include <algorithm>
#include <execution>

Camera::Camera()
{
}

Camera::~Camera()
{
}

struct pixel_compare {
	bool operator()(Ray const& r1, Ray const& r2)
	{
		return r1.pixel > r2.pixel;
	}
};

std::vector<Ray> Camera::generateRays(Vec2ui image_dim, unsigned int samples_per_pixel) const
{
	std::vector<Ray> rays = std::vector<Ray>();
	unsigned int N = image_dim[0] * image_dim[1] * samples_per_pixel;
	rays.resize(N);

	const float aspect = (float)image_dim[0] / (float)image_dim[1];

	std::atomic<unsigned int> count(0);
	std::for_each(std::execution::par_unseq, rays.begin(), rays.end(), [=, &count, &aspect](Ray& ray) {
		unsigned int index = count++;

		unsigned int y = index / (image_dim[0] * samples_per_pixel);
		unsigned int x = (index / samples_per_pixel) % (image_dim[0]);
		unsigned int sample = index % samples_per_pixel;

		Vec3f center = Vec3f(0.0f);
		Vec3f direction = Vec3f(1.0f).normalized();

		float fx = ((float)x / image_dim[0]);
		float fy = ((float)y / image_dim[1]);

		ray = this->sample(Vec2f((fx - 0.5f) * 2.0f * aspect, (fy - 0.5f) * 2.0f));
		ray.pixel = y * image_dim[0] + x;
		ray.throughput = Vec3f(1.0f / samples_per_pixel);
	});

	std::sort(std::execution::par_unseq, rays.begin(), rays.end(), [](const Ray& a, const Ray & b) -> bool
	{
		return a.pixel < b.pixel;
	});

	return rays;
}

