#include "Ray.h"

Ray::Ray()
{
	_o = Vec3f(0.f, 0.f, 0.f);
}

Ray::Ray(Vec3f origin, Vec3f direction)
{
	_o = origin;
	_dir = direction;
}

Ray::~Ray(){}

