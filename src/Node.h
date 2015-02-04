#pragma once

#include <vector>
#include <algorithm>
#include "Mesh.h"
#include "Vec3.h"
#include "Triangle.h"
#include "Vertex.h"
#include "Ray.h"

void drawSphere(Vec3f center, float radius);

// Teste intersection entre Sphere(c, r) et Rayon(o, d)
// ATETENTION : d doit etre de norme 1 
static inline bool sphereRayIntersect(const Vec3f& c, float r, const Vec3f& o, const Vec3f& d)
{
	float delta = dot(d, o - c)*dot(d, o - c) - dot(o - c, o - c) + r*r;
	if (delta > 0 && sqrt(delta) - dot(d, o - c) > 0)
		return true;
	else
		return false;
}

class Node {
public:

	Node();
	Node(const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangle, int profondeur);
	Node(const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangle);
	int hauteur();
	int maxCompleteLevel();
	int maxTailleFeuille();
	Vec3f maxVarDir(const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangle, Vec3f bary);
	Vec3f barycentre(const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangleList);
	float maxDist(Vec3f centre, const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangleList);
	inline bool isLeaf(){
		return feuille;
		//return (_leftChild == NULL);
	}
	inline virtual Node::~Node(){
		if (!isLeaf()){
			delete _leftChild;
			delete _rightChild;
		}
	}
	Vec3f getCenter();
	float getRadius();
	void drawSphereTree(int level);
	void drawContenu();
	// On suppose que la direction du rayon est normée de norme 1
	inline int intersectionCountBeforeTarget(const std::vector<Vertex>& V, const Ray& ray, float distMax);
	bool estInterieur(const std::vector<Vertex>& V, const Vec3f& point);

private:
	Vec3f _c;
	float _r;
	Node *_leftChild;
	Node *_rightChild;
	bool feuille;
	std::vector<Triangle> trianglesContenus;

};