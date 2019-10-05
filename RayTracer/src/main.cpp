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

#include <omp.h>

float random(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

int main() {

	OBJModel model = OBJModel("../models/buddha.obj");
	std::cout << "bbox: " << model.meshes[0].get_bbox() << "\n";
	std::cout << "num primitives: " << model.meshes[0].num_primitives() << "\n";

	model.meshes[0].normalize();
	model.meshes[0].translate(Vec3f(1, 0.5, -0.5));
	std::cout << "bbox: " << model.meshes[0].get_bbox() << "\n";

	const Vec3f light_color = Vec3f(255, 241, 224) / 255;
	std::vector<PointLight> lights = std::vector<PointLight>();
	{
		const float intensity = 70;
		const int num = 1;
		for (int i = 0; i < num; i++) {
			for (int j = 0; j < num; j++) {
				Vec3f position = Vec3f((float)i - ((float)num / 2.0f), 10, (float)j - ((float)num / 2.0f));
				Vec3f light = Vec3f(intensity / (num * num)) * light_color;
				lights.emplace_back(position, light);
			}
		}
	}

	LightTree light_tree = LightTree();
	light_tree.build(lights);

	srand(std::clock());

	Plane ground = Plane(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0, 1, 0));

	OBJModel bunny = OBJModel("../models/dragon.obj");
	bunny.meshes[0].normalize();
	bunny.meshes[0].translate(Vec3f(1.0, 0.5, 1.0));

	std::vector<const Geometry*> geometry(0);
	std::vector<const Plane*> planes(0);
	geometry.push_back(&model.meshes[0]);
	geometry.push_back(&bunny.meshes[0]);
	planes.push_back(&ground);

	std::clock_t start;
	start = std::clock();
	std::cout << "creating structure\n";
	//auto struc = ArraySphereStructure(spheres);
	auto struc = LBVHStructure();
	struc.init(geometry, planes);

	std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC) << " s" << std::endl;

	//struc.print();

	PinHoleCamera cam = PinHoleCamera();
	cam.SetPosition(Vec3f(-2.0f, 1.0f, -0.5f));
	cam.LookAt(Vec3f(0.0f, 0.5f, 0.0f));


	int width = 512;
	int height = 512;
	Image img(width, height);
	Image img_depth(width, height);

	std::cout << "Rendering image\n";
	start = std::clock();
	// your test

	float epsilon = 0.001f;

	/*std::vector<ImageTile> tiles = std::vector<ImageTile>();
	{
		const unsigned int TileSize = 64;
		const unsigned int NumTilesX = width / TileSize;
		const unsigned int NumTilesY = height / TileSize;

		for (int i = 0; i < NumTilesX; i++) {
			const int x = i * TileSize;
			for (int j = 0; j < NumTilesY; j++) {
				const int y = j * TileSize;

				tiles.emplace_back(x, y, TileSize, TileSize);
			}
		}
	}*/
	LightTree* tree = &light_tree;

	std::size_t max = width * height;

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

						if (struc.closest_hit(ray, hit)) {
							Vec3f normal = hit.normal.normalized();
							if (dot(normal, ray.direction) > 0)
								normal = -normal;

							Vec3f result = Vec3f(0.0f);

							for (auto light : tree->cut(hit.position, hit.normal, 0.01f)) {
								Vec3f dir = light.position - hit.position;
								float dist = dir.length();
								dir = dir.normalized();

								Ray shadow = Ray(hit.position, dir, epsilon, dist - epsilon);
								HitInfo shadow_info = HitInfo();
								if (!struc.any_hit(shadow, shadow_info)) {
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
	/*
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			float x = (float)j / width;
			float y = (float)i / height;
			float aspect = x / y;

			Ray ray = cam.sample(Vec2f((x - 0.5f) * 2.0f, (y - 0.5f) * 2.0f));
			HitInfo hit = HitInfo();

			Vec3f dir_frag = Vec3f(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);

			if (struc.closest_hit(ray, hit)) {
				Vec3f normal = hit.normal.normalized();
				if (dot(normal, ray.direction) > 0)
					normal = -normal;

				Vec3f result = Vec3f(0.0f);

				for (auto light : light_tree.cut(hit.position, hit.normal, 0.01f)) {
					Vec3f dir = light.position - hit.position;
					float dist = dir.length();
					dir = dir.normalized();

					Ray shadow = Ray(hit.position, dir, epsilon, dist - epsilon);
					HitInfo shadow_info = HitInfo();
					if (!struc.any_hit(shadow, shadow_info)) {
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
	}


	*/


	std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC) << " s" << std::endl;

	img.save_as("Test.png");
	img_depth.save_as("Trace_depth.png");
}