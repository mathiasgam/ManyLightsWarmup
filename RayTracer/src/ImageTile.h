#pragma once

#include <vector>
//#include "Vec2ui.h"
#include "Vec3f.h"

#include <cassert>

class ImageTile
{
private:
	std::vector<Vec3f> m_data;

public:

	const size_t offsetX;
	const size_t offsetY;
	const size_t sizeX;
	const size_t sizeY;


	ImageTile(size_t offsetX, size_t offsetY, size_t sizeX, size_t sizeY);
	~ImageTile();

	void SetPixel(unsigned int x, unsigned int y, Vec3f value);
};

