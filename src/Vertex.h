#pragma once

#include "Vec3.h"

/// A simple vertex class storing position and normal
class Vertex {
public:
	inline Vertex() {}
	inline Vertex(const Vec3f & p, const Vec3f & n) : p(p), n(n) {}
	inline virtual ~Vertex() {}
	Vec3f p;
	Vec3f n;
};