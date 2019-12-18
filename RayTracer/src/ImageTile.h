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

	const unsigned int offsetX;
	const unsigned int offsetY;
	const unsigned int sizeX;
	const unsigned int sizeY;


	ImageTile(unsigned int offsetX, unsigned int offsetY, unsigned int sizeX, unsigned int sizeY);
	~ImageTile();

	void SetPixel(unsigned int x, unsigned int y, Vec3f value);
};

