#pragma once

#include "Vec3.h"

class Ray{
public:
	Vec3f _o;
	Vec3f _dir;

	Ray();
	Ray(Vec3f origin, Vec3f direction);
	~Ray();
};