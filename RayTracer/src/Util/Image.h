#pragma once
#include "Vec4f.h"
#include "Vec3f.h"
#include "Vec2ui.h"
#include "vector"

#include <mutex>

class Image
{
protected:
	std::mutex m;
	Vec2ui res;
	std::vector<Vec4f> data;

public:
	Image(const unsigned int width, const unsigned int height);

	virtual ~Image();

	Vec2ui getResolution() {
		return res;
	}

	void setPixel(const unsigned int x, const unsigned int y, Vec3f color);
	void setPixel(const unsigned int x, const unsigned int y, Vec4f color);

	void add(const unsigned int i, const Vec3f rgb);

	void save_as(const char*);

};

