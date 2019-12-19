#include "Image.h"
#include "lodepng.h"

#include <vector>
#include <cassert>
#include <algorithm>
#include <execution>


void Image::setPixel(const unsigned int x, const unsigned int y, Vec3f rgb)
{
	//std::cout << "x: " << x << ", y: " << y << ", res: " << res << std::endl;
	assert(x >= 0 && x < res[0]);
	assert(y >= 0 && y < res[1]);
	const unsigned int index = y * res[0] + x;
	std::lock_guard<std::mutex> lock(m);
	data[index] = Vec4f(rgb[0], rgb[1], rgb[2], 1.0f);
}

void Image::setPixel(const unsigned int x, const unsigned int y, Vec4f rgba)
{
	assert(x > res[0]);
	assert(y > res[1]);
	const unsigned int index = y * res[0] + x;
	std::lock_guard<std::mutex> lock(m);
	data[index] = rgba;
}

void Image::add(const unsigned int i, const Vec3f rgb)
{
	std::lock_guard<std::mutex> lock(m);
	data[i] += Vec4f(rgb[0], rgb[1], rgb[2], 0.0f);
}

void Image::save_as(const char *filename)
{
	std::lock_guard<std::mutex> lock(m);
	std::vector<unsigned char> tmp = std::vector<unsigned char>();

	int size = res[0] * res[1];
	tmp.resize(size * 4);

	for (int i = 0; i < size; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			tmp[i * 4 + j] = (unsigned char)(min(1.0f, data[i][j]) * 255);
		}
	}

	std::vector<unsigned char> png;

	unsigned error = lodepng::encode(png, tmp, res[0], res[1]);
	if (!error) lodepng::save_file(png, filename);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

Image::Image(const unsigned int width, const unsigned int height) : m(), res(width, height), data()
{
	std::lock_guard<std::mutex> lock(m);
	data.resize(width * height);
	std::for_each(std::execution::par_unseq,data.begin(), data.end(), [](Vec4f& pixel) {
		pixel = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
	});
}

Image::~Image()
{
}


