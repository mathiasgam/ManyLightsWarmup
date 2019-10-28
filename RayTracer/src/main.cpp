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

#include "MortonCode.h"

#include "ImageTile.h"

#include "Scene.h"
#include "RayTracer.h"

#include "Sampling.h"

void addLightCluster(Scene* scene, Vec3f center, Vec3f dim, Vec3f color, unsigned int N) {
	for (int i = 0; i < N; i++) {
		Vec3f pos = random(center - dim, center + dim);
		scene->AddLight(pos, color / N);
	}
}

Vec3f color(unsigned int r, unsigned int g, unsigned int b) {
	return Vec3f(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f);
}

void prepareScene(Scene* scene) {

	// add some models to the scene
	//scene->AddMesh("../models/san-miguel_tri.obj", color(196, 160, 106), Vec3f(0, 0, 0));
	scene->AddMesh("../models/dragon.obj", color(222, 10, 2), Vec3f(0.0f, 0.3f, 0.0f));
	scene->AddMesh("../models/bunny.obj", color(122, 75, 39), Vec3f(1.0f, -0.3f, 1.0f));
	scene->AddPlane(Vec3f(0, 0, 0), color(92, 85, 74), Vec3f(0, 1, 0));

	//scene->AddLight(Vec3f(20, 2, 2), Vec3f(0, 0, 50));
	const Vec3f light_color = color(255, 241, 224);

	scene->SetAmbient(light_color * 0.2f);

	const float intensity = 50.0f;
	//scene->AddLight(Vec3f(16.0f, 5.0f, 6.0f), light_color * 100);
	//scene->AddLight(Vec3f(15, 10, 0), light_color * 100);
	//scene->AddLight(Vec3f(24, 10, 0), light_color * 100);

	const int num_lights = 100;

	addLightCluster(scene, Vec3f(0.0f, 1.0f, 0.0f), Vec3f(1,1,1), light_color * intensity * 0.1f, num_lights * 0.2);
	addLightCluster(scene, Vec3f(10, 5, 0), Vec3f(1,1,1), light_color * intensity, num_lights * 0.2);
	addLightCluster(scene, Vec3f(-10, 5, -3), Vec3f(1,1,1), light_color * intensity, num_lights * 0.6);
}

int main() {
	// set the seed for the standard random function with constant value for comparable results across runs
	srand(42);

	int width = 720;
	int height = 480;
	float aspect = (float)width / (float)height;
	Vec2f pixel_dim = Vec2f(1.0f / width, 1.0f / height);
	std::cout << "Width: " << width << ", Height: " << height << ", Aspect: " << aspect << std::endl;

	// create the camera
	PinHoleCamera cam = PinHoleCamera();
	//cam.SetPosition(Vec3f(-2.5f, 1.0f, 0.0f));
	cam.SetPosition(Vec3f(-20.0f, 10.0f, 10.0f));
	cam.LookAt(Vec3f(0.0f, 2.5f, 0.0f));

	// scene added as pointer, for easier access over multiple threads
	Scene* scene = new Scene();
	prepareScene(scene);

	RayTracer* tracer = new RayTracer(scene);
	tracer->SetLightThreshold(0.02f);

	// start timing
	std::clock_t start;
	start = std::clock();

	// build tracing structures. if additional objects is added or moved around, the scene need to be prepared again
	scene->prepare();

	std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC) << " s" << std::endl;


	// define images to render to
	Image img(width, height);
	Image img_lights(width, height);
	Image img_depth(width, height);
	Image img_visual(width, height);


	// start the rendering process
	std::cout << "Rendering image\n";
	start = std::clock();

	float epsilon = 0.01f;

	std::size_t max = static_cast<size_t>(width * height);

	struct Report {
		unsigned int num_rays;
		unsigned int num_occlusion_rays;
	};

	// find the number of cores available
	std::size_t cores = std::thread::hardware_concurrency();
	volatile std::atomic<std::size_t> count(0);
	std::vector<std::future<Report>> future_vector;

	// add Async processies until all cores have one
	while (cores--)
		future_vector.emplace_back(
			std::async([=, &cam, &count, &img, &img_depth, &img_lights, &img_visual]()
				{
					Report report = {};
					report.num_rays = 0;
					report.num_occlusion_rays = 0;

					while (true)
					{
						// get the next pixel
						std::size_t index = count++;
						if (index >= max)
							break;

						if ((index + 1) % (max / 10) == 0)
							std::cout << ".";

						std::size_t i = index % width;
						std::size_t j = index / width;

						Vec3f color = Vec3f(0.0f);
						float f = 0.0f;
						float fd = 0.0f;

						float x = ((float)i / width);
						float y = ((float)j / height);

						Ray ray = cam.sample(Vec2f((x - 0.5f) * 2.0f * aspect, (y - 0.5f) * 2.0f));

						TraceResult res = tracer->trace(ray);

						report.num_rays += res.num_rays;
						report.num_occlusion_rays += res.num_occlusion_rays;

						color += res.color;
						//color = (res.hit.normal + 1.0f) / 2.0f;
						fd += 1.0f - (1.0f / (res.hit.trace_depth * 0.01f + 1.0f));
						f += static_cast<float>(res.num_lights) / scene->GetNumLights();

						img.setPixel(i, j, color);
						img_visual.setPixel(i, j, res.visualColor);
						img_lights.setPixel(i, j, f);
						img_depth.setPixel(i, j, fd);
					}
					return report;
				}));

	Report report_final = {};
	report_final.num_rays = 0;
	report_final.num_occlusion_rays = 0;

	// Wait for all pixels to be processed
	for (auto& future : future_vector) {
		future.wait();
		Report report_tmp = future.get();
		report_final.num_rays += report_tmp.num_rays;
		report_final.num_occlusion_rays += report_tmp.num_occlusion_rays;
	}


	std::cout << std::endl;

	const double seconds = (std::clock() - start) / (double)(CLOCKS_PER_SEC);
	const double mil_rays = (report_final.num_rays + report_final.num_occlusion_rays) / 10e6;
	std::cout << "\nReport: \n";
	std::cout << "- Time: " << seconds << " sec" << std::endl;

	std::cout << "- M Rays: " << mil_rays << "\n- M Rays / sec: " << mil_rays / seconds << std::endl;

	img.save_as("Test.png");
	img_depth.save_as("Trace_depth.png");
	img_lights.save_as("Trace_lights.png");
	img_visual.save_as("Visual.png");

	// cleanup memory
	delete tracer;
	delete scene;
}