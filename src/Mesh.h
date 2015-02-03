#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Triangle.h"
#include "Vertex.h"
#include "Ray.h"
#include "Quad.h"

#define EPSILON 0.000001


/// A Mesh class, storing a list of vertices and a list of triangles indexed over it.
class Mesh {
public:
	std::vector<Vertex> V;
	std::vector<Triangle> T;
	std::vector<Quad> Q;

	/// Loads the mesh from a <file>.off
	void loadOFF(const std::string & filename);

	/// Compute smooth per-vertex normals
	void recomputeNormals();

	/// scale to the unit cube and center at original
	void centerAndScaleToUnit();

	// Test l'intersection d'un rayon et d'un triangle. out est le parametre d'intersection le long du rayon.
	bool triangle_intersection(const Vec3f& V1, const Vec3f& V2, const Vec3f& V3, const Vec3f& O, const Vec3f& D, float* out);

	// Teste si un point est a l'interieur du mesh. Attention, le point doit etre de norme <=2 et le mesh doit etre normalise.
	bool estInterieur(const Vec3f& point);

};

