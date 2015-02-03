#pragma once

/// A Quad class expressed as a quadruplet of indices (over an external vertex list)
class Quad {
public:
	inline Quad() {
		v[0] = v[1] = v[2] = v[3] = 0;
	}
	inline Quad(const Quad & t) {
		v[0] = t.v[0];
		v[1] = t.v[1];
		v[2] = t.v[2];
		v[3] = t.v[3];
	}
	inline Quad(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) {
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
		v[3] = v3;
	}
	inline virtual ~Quad() {}
	inline Quad & operator= (const Quad & t) {
		v[0] = t.v[0];
		v[1] = t.v[1];
		v[2] = t.v[2];
		v[3] = t.v[3];
		return (*this);
	}
	unsigned int v[4];
};

