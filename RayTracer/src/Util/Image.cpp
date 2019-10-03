#include "Image.h"
#include "lodepng.h"

#include <vector>


void Image::setPixel(const unsigned int x, const unsigned int y, Vec3f color)
{
	const unsigned int index = y * res[0] * 4 + x * 4;
	for (unsigned int i = 0; i < 3; i++) {
		data[index + i] = (unsigned char)(color[i] * 255);
	}
	data[index + 3] = 255;
}

void Image::setPixel(const unsigned int x, const unsigned int y, Vec4f color)
{
	const unsigned int index = y * res[0] * 4 + x * 4;
	for (unsigned int i = 0; i < 4; i++) {
		data[index + i] = (unsigned char)(color[i] * 255);
	}
}

void Image::save_as(const char *filename)
{
	std::vector<unsigned char> png;

	unsigned error = lodepng::encode(png, data, res[0], res[1]);
	if (!error) lodepng::save_file(png, filename);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

Image::Image(const unsigned int width, const unsigned int height)
{
	res = Vec2ui(width, height);
	data = std::vector<unsigned char>();
	data.resize(width * height * 4);
}

Image::~Image()
{
}


