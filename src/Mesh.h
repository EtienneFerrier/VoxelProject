#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Triangle.h"
#include "Vertex.h"
#include "Ray.h"
#include "Quad.h"
#include "Node.h"

#define EPSILON 0.0000001

// Test l'intersection d'un rayon et d'un triangle. out est le parametre d'intersection le long du rayon.
bool triangle_intersection(const Vec3f& V1, const Vec3f& V2, const Vec3f& V3, const Vec3f& O, const Vec3f& D, float* out);

class Node;

/// A Mesh class, storing a list of vertices and a list of triangles indexed over it.
class Mesh {
public:
	Node* BSHtree; // Arbre de spheres englobantes

	std::vector<Vertex> V;
	std::vector<Triangle> T;
	std::vector<Quad> Q;
	std::vector<bool> col;

	/// Loads the mesh from a <file>.off
	void loadOFF(const std::string & filename);

	/// Compute smooth per-vertex normals
	void recomputeNormals();

	/// scale to the unit cube and center at original
	void centerAndScaleToUnit();

	// Teste si un point est a l'interieur du mesh. Attention, le point doit etre de norme <=2 et le mesh doit etre normalise.
	bool estInterieur(const Vec3f& point);

	void computeBSH(int level); // Charge la structure BSH associee au mesh. Si possible, la structure a une profondeur level 
	void computeBSH(); // Comme computeBSH(int level) mais la structure a la profondeur maximale. Attention : prend trop de place memoire.
	bool estInterieurBSH(const Vec3f& point); // Comme estInterieur mais en utilisant la structure BSH
	bool estSurBordBSH(const Vec3f& point, float boxSize); // Comme estInterieurBSH mais determine si le point est sur le bord du mesh.

};

