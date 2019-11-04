#pragma once

#include <random>
#include "Vec3f.h"

const float kPi = 3.14159265359f;

inline float random(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

inline Vec3f random(Vec3f from, Vec3f to) {
	float x = random(from[0], to[0]);
	float y = random(from[1], to[1]);
	float z = random(from[2], to[2]);
	return Vec3f(x, y, z);
}

inline void rotate_to_normal(const Vec3f& normal, Vec3f& v)
{
	float sign = copysignf(1.0f, normal[2]);
	const float a = -1.0f / (1.0f + fabsf(normal[2]));
	const float b = normal[0] * normal[1] * a;
	v = Vec3f(1.0f + normal[0] * normal[0] * a, b, -sign * normal[0]) * v[0]
		+ Vec3f(sign * b, sign * (1.0f + normal[1] * normal[1] * a), -normal[1]) * v[1]
		+ normal * v[2];
}

inline Vec3f SampleCosineSphere() {

	const float u1 = random(0, 1);
	const float u2 = random(0, 1);

	const float r = sqrtf(u1);
	const float theta = 2 * kPi * u2;

	const float x = r * cosf(theta);
	const float y = r * sinf(theta);

	return Vec3f(x, y, sqrtf(fmaxf(0.0f, 1.0f - u1)));
}