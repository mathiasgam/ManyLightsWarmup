#include "ImageTile.h"

ImageTile::ImageTile(unsigned int offsetX, unsigned int offsetY, unsigned int sizeX, unsigned int sizeY):
	offsetX(offsetX), offsetY(offsetY), sizeX(sizeX), sizeY(sizeY)
{
	m_data = std::vector<Vec3f>();
	m_data.resize(sizeX * sizeY);
}

ImageTile::~ImageTile()
{
	m_data.clear();
}

void ImageTile::SetPixel(unsigned int x, unsigned int y, Vec3f value)
{
	assert(x > 0 && x < sizeX);
	assert(y > 0 && y < sizeY);
	m_data[sizeX * y + x] = value;
}
