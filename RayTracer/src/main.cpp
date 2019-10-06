#include "Ray.h"
#include "Geometry/Sphere.h"
#include "Geometry/SphereGroup.h"
#include "Structure/ArraySphereStructure.h"
#include "Structure/BVHSphereStructure.h"
#include "Structure/SAHBVHStructure.h"
#include "Structure/LBVHStructure.h"

#include "PointLight.h"

#include "Util/lodepng.h"
#include "Util/Image.h"

#include <random>

#include "Camera/PinHoleCamera.h"

#include "Vec4f.h"
#include "Vec2f.h"

#include "OBJModel.h"

#include <vector>
#include <iostream>

#include <ctime>

#include <cassert>
#include <thread>
#include <atomic>
#include <future>

#include "LightTree.h"
#include "MortonCode.h"

#include "ImageTile.h"

#include "Scene.h"

float random(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

int main() {
	// set the seed for the standard random function
	srand(std::clock());

	// scene added as pointer, for easier access over multiple threads
	Scene* scene = new Scene();

	scene->AddMesh("../models/buddha.obj", Vec3f(1, 0.5, -0.5));
	scene->AddMesh("../models/dragon.obj", Vec3f(0.0f, 0.4f, 0.0f));
	scene->AddMesh("../models/bunny.obj", Vec3f(1.0f, 0.5f, 1.0f));
	scene->AddPlane(Vec3f(0, 0, 0), Vec3f(0, 1, 0));

	const Vec3f light_color = Vec3f(255, 241, 224) / 255;
	std::vector<PointLight> lights = std::vector<PointLight>();
	{
		const float intensity = 70;
		const int num = 1;
		for (int i = 0; i < num; i++) {
			for (int j = 0; j < num; j++) {
				Vec3f position = Vec3f((float)i - ((float)num / 2.0f), 10, (float)j - ((float)num / 2.0f));
				Vec3f light = Vec3f(intensity / (num * num)) * light_color;
				//lights.emplace_back(position, light);
				scene->AddLight(position, light);
			}
		}
	}

	// start timing
	std::clock_t start;
	start = std::clock();

	std::cout << "creating structure\n";

	// build tracing structures. if additional objects is added or moved around, the scene need to be prepared again
	scene->prepare();

	std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC) << " s" << std::endl;


	// create the camera
	PinHoleCamera cam = PinHoleCamera();
	cam.SetPosition(Vec3f(-2.0f, 1.0f, -0.5f));
	cam.LookAt(Vec3f(0.0f, 0.5f, 0.0f));


	// define images to render to
	int width = 512;
	int height = 512;

	Image img(width, height);
	Image img_depth(width, height);


	// start the rendering process
	std::cout << "Rendering image\n";
	start = std::clock();

	float epsilon = 0.001f;

	std::size_t max = static_cast<size_t>(width * height);

	// find the number of cores available
	std::size_t cores = std::thread::hardware_concurrency();
	volatile std::atomic<std::size_t> count(0);
	std::vector<std::future<void>> future_vector;

	// add Async processies until all cores have one
	while (cores--)
		future_vector.emplace_back(
			std::async([=, &cam, &count, &img, &img_depth]()
				{
					while (true)
					{
						// get the next pixel
						std::size_t index = count++;
						if (index >= max)
							break;
						std::size_t i = index % width;
						std::size_t j = index / width;
						float x = (float)j / width;
						float y = (float)i / height;
						float aspect = x / y;

						Ray ray = cam.sample(Vec2f((x - 0.5f) * 2.0f, (y - 0.5f) * 2.0f));
						HitInfo hit = HitInfo();

						Vec3f dir_frag = Vec3f(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);

						if (scene->closest_hit(ray, hit)) {
							Vec3f normal = hit.normal.normalized();
							if (dot(normal, ray.direction) > 0)
								normal = -normal;

							Vec3f result = Vec3f(0.0f);
						
							for (auto light : scene->GetLights(hit.position, hit.normal, 0.01f)) {
								Vec3f dir = light.position - hit.position;
								float dist = dir.length();
								dir = dir.normalized();

								Ray shadow = Ray(hit.position, dir, epsilon, dist - epsilon);
								HitInfo shadow_info = HitInfo();
								if (!scene->any_hit(shadow, shadow_info)) {
									result += (light.color / (dist * dist)) * fmaxf(dot(normal, dir), 0.0f);
								}
							}

							// Add ambient light
							result += light_color * 0.4f;

							result = min(result, Vec3f(1.0f));
							img.setPixel(j, i, result);
						}
						else {
							img.setPixel(j, i, Vec3f(0.0f));
						}

						float f = 1.0f - (1.0f / (hit.trace_depth * 0.01f + 1.0f));
						img_depth.setPixel(j, i, Vec3f(1.0f - f));
					}
				}));

	// Wait for all pixels to be processed
	for (auto& future : future_vector) {
		future.wait();
	}

	std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC) << " s" << std::endl;

	img.save_as("Test.png");
	img_depth.save_as("Trace_depth.png");

	// cleanup memory
	delete scene;
}