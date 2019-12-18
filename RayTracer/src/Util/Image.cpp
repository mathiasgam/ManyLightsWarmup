#include "Image.h"
#include "lodepng.h"

#include <vector>
#include <cassert>


void Image::setPixel(const unsigned int x, const unsigned int y, Vec3f rgb)
{
	//std::cout << "x: " << x << ", y: " << y << ", res: " << res << std::endl;
	assert(x >= 0 && x < res[0]);
	assert(y >= 0 && y < res[1]);
	const unsigned int index = y * res[0] + x;
	data[index] = Vec4f(rgb[0], rgb[1], rgb[2], 1.0f);
}

void Image::setPixel(const unsigned int x, const unsigned int y, Vec4f rgba)
{
	assert(x > res[0]);
	assert(y > res[1]);
	const unsigned int index = y * res[0] + x;
	data[index] = rgba;
}

void Image::save_as(const char *filename)
{
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

Image::Image(const unsigned int width, const unsigned int height)
{
	res = Vec2ui(width, height);
	data = std::vector<Vec4f>();
	data.resize(width * height);
}

Image::~Image()
{
}


