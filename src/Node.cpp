#include "Node.h"

Node::Node()
{
	_c = Vec3f(0., 0., 0.);
	_r = 0.;
	_leftChild = NULL;
	_rightChild = NULL;
}

Node::Node(const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangle, int profondeur)
{
	_c = barycentre(sommet, triangle);
	_r = maxDist(_c, sommet, triangle);

	if (triangle.size() == 1)
	{
		//feuille = true;
		_leftChild = NULL;
		_rightChild = NULL;
		trianglesContenus = triangle;
	}
	else if (profondeur == 0)
	{
		//feuille = true;
		_leftChild = NULL;
		_rightChild = NULL;
		for (unsigned int i = 0; i < triangle.size(); i++)
			trianglesContenus.push_back(triangle[i]);
	}
	else
	{
		//feuille = false;

		Vec3f dir = maxVarDir(sommet, triangle, _c);

		std::vector<Triangle> fg;
		std::vector<Triangle> fd;

		Vec3f tmp(0., 0., 0.);

		for (unsigned int i = 0; i < triangle.size(); i++)
		{
			tmp = (sommet[triangle[i].v[0]].p + sommet[triangle[i].v[1]].p + sommet[triangle[i].v[2]].p) / 3.;
			if (dot(dir, tmp - _c) > 0.)
				fg.push_back(triangle[i]);
			else
				fd.push_back(triangle[i]);
		}

		if (fg.size() == 0)
			_leftChild = NULL;
		else _leftChild = new Node(sommet, fg, profondeur-1);

		if (fd.size() == 0)
			_rightChild = NULL;
		else _rightChild = new Node(sommet, fd, profondeur-1);
	}
	
}

Node::Node(const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangle)
{
	_c = barycentre(sommet, triangle);
	_r = maxDist(_c, sommet, triangle);

	if (triangle.size() == 1)
	{
		//feuille = true;
		_leftChild = NULL;
		_rightChild = NULL;
		trianglesContenus = triangle;
	}
	else
	{
		//feuille = false;
		Vec3f dir = maxVarDir(sommet, triangle, _c);

		std::vector<Triangle> fg;
		std::vector<Triangle> fd;

		Vec3f tmp(0., 0., 0.);

		for (unsigned int i = 0; i < triangle.size(); i++)
		{
			tmp = (sommet[triangle[i].v[0]].p + sommet[triangle[i].v[1]].p + sommet[triangle[i].v[2]].p) / 3.;
			if (dot(dir, tmp - _c) > 0.)
				fg.push_back(triangle[i]);
			else
				fd.push_back(triangle[i]);
		}

		if (fg.size() == 0)
			_leftChild = NULL;
		else _leftChild = new Node(sommet, fg);

		if (fd.size() == 0)
			_rightChild = NULL;
		else _rightChild = new Node(sommet, fd);
	}

}

int Node::hauteur()
{
	if (_rightChild == NULL && _leftChild == NULL)
		return 0;
	else if (_rightChild == NULL)
		return 1 + _leftChild->hauteur();
	else if (_leftChild == NULL)
		return 1 + _rightChild->hauteur();
	else
		return 1 + std::max(_leftChild->hauteur(), _rightChild->hauteur());
}

int Node::maxCompleteLevel()
{
	if (_rightChild == NULL  || _leftChild == NULL)
		return 0;
	else return 1 + std::min(_leftChild->maxCompleteLevel(), _rightChild->maxCompleteLevel());
}

int Node::maxTailleFeuille()
{
	if (_leftChild == NULL && _rightChild == NULL)
		return trianglesContenus.size();
	else if (_leftChild == NULL)
		return _rightChild->maxTailleFeuille();
	else if (_rightChild == NULL)
		return _rightChild->maxTailleFeuille();
	else return std::max(_rightChild->maxTailleFeuille(), _leftChild->maxTailleFeuille());
}

Vec3f Node::maxVarDir(const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangle, Vec3f bary)
{
	float dir[3] = { 0., 0., 0. };

	for (unsigned int k = 0; k < triangle.size(); k++)
		for (int j = 0; j < 3; j++)
			for (int i = 0; i < 3; i++)
				dir[i] += (sommet[triangle[k].v[j]].p[i] - bary[i])*(sommet[triangle[k].v[j]].p[i] - bary[i]);

	if (dir[0] > dir[1] && dir[0] > dir[2])
		return Vec3f(1., 0., 0.);
	else if (dir[1] > dir[0] && dir[1] > dir[2])
		return Vec3f(0., 1., 0.);
	else return Vec3f(0., 0., 1.);
}

Vec3f Node::barycentre(const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangleList)
{
	Vec3f bary(0., 0., 0.);
	for (unsigned int i = 0; i < triangleList.size(); i++)
	{
		bary += sommet[triangleList[i].v[0]].p + sommet[triangleList[i].v[1]].p + sommet[triangleList[i].v[2]].p;
	}
	bary /= 3.f * triangleList.size();

	return bary;
}

float Node::maxDist(Vec3f centre, const std::vector<Vertex>& sommet, const std::vector<Triangle>& triangleList)
{
	float dist = 0.;
	for (unsigned int j = 0; j < triangleList.size(); j++)
		for (int i = 0; i < 3; i++)
		{
			float d = (centre - sommet[triangleList[j].v[i]].p).length();
			if (d > dist)
				dist = d;
		}
	return dist;
}

Vec3f Node::getCenter()
{
	return _c;
}

float Node::getRadius()
{
	return _r;
}

void Node::drawSphereTree(int level)
{
	if (level == 0 || (_rightChild == NULL && _leftChild == NULL))
		drawSphere(_c, _r);
	else
	{
		if (_rightChild != NULL)
			_rightChild->drawSphereTree(level - 1);

		if (_leftChild != NULL)
			_leftChild->drawSphereTree(level - 1);
	}
}

inline int Node::intersectionCountBeforeTarget(const std::vector<Vertex>& V, const Ray& ray, float distMax)
{
	int count = 0;

	if ((_c - ray._o).length() - _r > distMax)
	{
	}
	else if (_leftChild == NULL && _rightChild == NULL)
	{

		float t;

		for (unsigned int i = 0; i < trianglesContenus.size(); i++)
		{
			if (triangle_intersection(V[trianglesContenus[i].v[0]].p, V[trianglesContenus[i].v[1]].p, V[trianglesContenus[i].v[2]].p, ray._o, ray._dir, &t) && t < distMax)
				count++;
		}
	}
	else if (sphereRayIntersect(_c, _r, ray._o, ray._dir))
	{
		if (_leftChild != NULL)
			count += _leftChild->intersectionCountBeforeTarget(V, ray, distMax);
		if (_rightChild != NULL)
			count += _rightChild->intersectionCountBeforeTarget(V, ray, distMax);
	}
	else
	{
	}

	return count;
}

inline bool Node::intersectionInBox(const std::vector<Vertex>& V, const Ray& ray, float targetDistance, float boxSize)
{

	if ((_c - ray._o).length() - _r > targetDistance + boxSize/2.f){
		return false;
	}
	else if (_leftChild == NULL && _rightChild == NULL)
	{
		float t;

		for (unsigned int i = 0; i < trianglesContenus.size(); i++)
		{
			if (triangle_intersection(V[trianglesContenus[i].v[0]].p, V[trianglesContenus[i].v[1]].p, V[trianglesContenus[i].v[2]].p, ray._o, ray._dir, &t) && std::abs(targetDistance - t) < boxSize/2.f )
				return true;
		}
		return false;
	}
	else if (sphereRayIntersect(_c, _r, ray._o, ray._dir))
	{
		if (_leftChild == NULL)
			return _rightChild->intersectionInBox(V, ray, targetDistance, boxSize);
		if (_rightChild == NULL)
			return _leftChild->intersectionInBox(V, ray, targetDistance, boxSize);
		else
			return _rightChild->intersectionInBox(V, ray, targetDistance, boxSize) || _leftChild->intersectionInBox(V, ray, targetDistance, boxSize);
	}
	else return false;
}

bool Node::estInterieur(const std::vector<Vertex>& V, const Vec3f& point)
{
	Ray ray(Vec3f(point - Vec3f(10.f, 0.f, 0.f)), Vec3f(1.f, 0.f, 0.f));
	return ((intersectionCountBeforeTarget(V, ray, 10.f) % 2) == 1);
}

bool Node::estSurBord(const std::vector<Vertex>& V, const Vec3f& point, float boxSize)
{
	Ray rayX(Vec3f(point - Vec3f(10.f, 0.f, 0.f)), Vec3f(1.f, 0.f, 0.f));
	Ray rayY(Vec3f(point - Vec3f(0.f, 10.f, 0.f)), Vec3f(0.f, 1.f, 0.f));
	Ray rayZ(Vec3f(point - Vec3f(0.f, 0.f, 10.f)), Vec3f(0.f, 0.f, 1.f));
	return intersectionInBox(V, rayX, 10.f, boxSize) || intersectionInBox(V, rayY, 10.f, boxSize) || intersectionInBox(V, rayZ, 10.f, boxSize);
}