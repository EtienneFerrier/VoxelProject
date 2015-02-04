#include "Mesh.h"

void Mesh::loadOFF(const std::string & filename) {
	std::ifstream in(filename.c_str());
	if (!in)
		exit(EXIT_FAILURE);
	std::string offString;
	unsigned int sizeV, sizeT, tmp;
	in >> offString >> sizeV >> sizeT >> tmp;
	V.resize(sizeV);
	T.resize(sizeT);
	for (unsigned int i = 0; i < sizeV; i++)
		in >> V[i].p;
	int s;
	for (unsigned int i = 0; i < sizeT; i++) {
		in >> s;
		for (unsigned int j = 0; j < 3; j++)
			in >> T[i].v[j];
	}
	in.close();
	centerAndScaleToUnit();
	recomputeNormals();
}

void Mesh::recomputeNormals() {
	for (unsigned int i = 0; i < V.size(); i++)
		V[i].n = Vec3f(0.0, 0.0, 0.0);

	for (unsigned int i = 0; i < T.size(); i++) {
		Vec3f e01 = V[T[i].v[1]].p - V[T[i].v[0]].p;
		Vec3f e02 = V[T[i].v[2]].p - V[T[i].v[0]].p;
		Vec3f n = cross(e01, e02);
		n.normalize();
		for (unsigned int j = 0; j < 3; j++)
			V[T[i].v[j]].n += n;
	}

	//Ajout pour les quads
	for (unsigned int i = 0; i < Q.size(); i++) {
		Vec3f e01 = V[Q[i].v[1]].p - V[Q[i].v[0]].p;
		Vec3f e02 = V[Q[i].v[2]].p - V[Q[i].v[0]].p;
		Vec3f n = cross(e01, e02);
		n.normalize();
		for (unsigned int j = 0; j < 4; j++)
			V[Q[i].v[j]].n += n;
	}

	for (unsigned int i = 0; i < V.size(); i++)
		V[i].n.normalize();




}

void Mesh::centerAndScaleToUnit() {
	Vec3f c;
	for (unsigned int i = 0; i < V.size(); i++)
		c += V[i].p;
	c /= (float)V.size();
	float maxD = dist(V[0].p, c);
	for (unsigned int i = 0; i < V.size(); i++){
		float m = dist(V[i].p, c);
		if (m > maxD)
			maxD = m;
	}
	for (unsigned int i = 0; i < V.size(); i++)
		V[i].p = (V[i].p - c) / maxD;
}

bool triangle_intersection(const Vec3f& V1, const Vec3f& V2, const Vec3f& V3, const Vec3f& O, const Vec3f& D, float* out)
{
	Vec3f    u, v, n;              // triangle vectors
	Vec3f    dir, w0, w;           // ray vectors
	float     r, a, b;              // params to calc ray-plane intersect

	// get triangle edge vectors and plane normal
	u = V1 - V3;
	v = V2 - V3;
	n = cross(u,v);              // cross product
	if (n == Vec3f(0, 0, 0))             // triangle is degenerate
		return false;                  // do not deal with this case

	dir = D;// R.P1 - R.P0;              // ray direction vector
	w0 = O - V3;
	a = -dot(n, w0);
	b = dot(n, dir);
	if (fabs(b) < EPSILON) {     // ray is  parallel to triangle plane
		if (a == 0)                 // ray lies in triangle plane
			return true;
		else return 0;              // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0)                    // ray goes away from triangle
		return false;                   // => no intersect
	// for a segment, also test if (r > 1.0) => no intersect

	Vec3f I = O + r * dir;            // intersect point of ray and plane

	// is I inside T?
	float    uu, uv, vv, wu, wv, d;
	uu = dot(u, u);
	uv = dot(u, v);
	vv = dot(v, v);
	w = I - V3;
	wu = dot(w, u);
	wv = dot(w, v);
	d = uv * uv - uu * vv;

	*out = r;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / d;
	if (s < 0.0 || s > 1.0)         // I is outside T
		return false;
	t = (uv * wu - uu * wv) / d;
	if (t < 0.0 || (s + t) > 1.0)  // I is outside T
		return false;

	return 1;                       // I is in T
}

bool Mesh::estInterieur(const Vec3f& point)
{
	int interCount = 0;

	float dist = 10.f;
	Vec3f origin = point - Vec3f(dist, 0.f, 0.f);
	Vec3f dir = Vec3f(1.f, 0.f, 0.f);

	float t;
	for (unsigned int i = 0; i < T.size(); i++)
	{
		if (triangle_intersection(V[T[i].v[0]].p, V[T[i].v[1]].p, V[T[i].v[2]].p, origin, dir, &t) && t < dist)
			interCount++;
	}
		
	return ((interCount % 2) == 1);
}

void Mesh::computeBSH(int profondeur)
{
	BSHtree = new Node(V, T, profondeur);
}

void Mesh::computeBSH()
{
	BSHtree = new Node(V, T);
}

bool Mesh::estInterieurBSH(const Vec3f& point){
	return BSHtree->estInterieur(V, point);
}