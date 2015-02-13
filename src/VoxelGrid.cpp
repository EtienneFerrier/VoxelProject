#include "VoxelGrid.h"

// Cree une grille de taille voulue et l'initialise a vide
VoxelGrid::VoxelGrid(int size)
{
	_size = size;
	_content = new bool[size*size*size];
	setAllGrid(false);
}

// Efface tout le contenu de la grille
VoxelGrid::~VoxelGrid()
{
	delete[] _content;
}

// Convertit un mesh en voxelGrid (en prenant l'interieur).
void VoxelGrid::fillGrid(Mesh& m)
{
	float step = 2.f / ((float)_size);

	for (int i = 0; i < _size; i++)
		for (int j = 0; j < _size; j++)
			for (int k = 0; k < _size; k++)
			{
				if (m.estInterieur(Vec3f(-1.f + step / 2.f + i*step, -1.f + step / 2.f + j*step, -1.f + step / 2.f + k*step)))
					setVoxel(i, j, k, true);
			}
}

// Comme fillGrid mais utilise la structure BSH du mesh.
void VoxelGrid::fillGridBSH(Mesh& m)
{
	float step = 2.f / ((float)_size);

	for (int i = 0; i < _size; i++)
		for (int j = 0; j < _size; j++)
			for (int k = 0; k < _size; k++)
			{
		if (m.estInterieurBSH(Vec3f(-1.f + step / 2.f + i*step, -1.f + step / 2.f + j*step, -1.f + step / 2.f + k*step)))
			setVoxel(i, j, k, true);
			}


}

// Comme fillGridBSH mais en ne conservant que les voxels sur le bord du mesh. (plus lent)
void VoxelGrid::fillSparseGridBSH(Mesh& m)
{
	float step = 2.f / ((float)_size);

	for (int i = 0; i < _size; i++)
		for (int j = 0; j < _size; j++)
			for (int k = 0; k < _size; k++)
			{
				if (m.estSurBordBSH(Vec3f(-1.f + step / 2.f + i*step, -1.f + step / 2.f + j*step, -1.f + step / 2.f + k*step), step))
					setVoxel(i, j, k, true);
			}


}

// Convertit un mesh en format OFF en voxelGrid
void VoxelGrid::loadOFF(const std::string & filename)
{
	Mesh m;
	m.loadOFF(filename);
	fillGrid(m);
}

// Efface les voxels qui sont entoures de voxels dans toutes les directions (voxels invisibles)
void VoxelGrid::emptyInteriorVoxels()
{
	for (int i = 1; i < _size-1; i++)
		for (int j = 1; j < _size-1; j++)
			for (int k = 1; k < _size-1; k++){
		if (getVoxel(i, j, k)
			&& getVoxel(i + 1, j, k)
			&& getVoxel(i - 1, j, k)
			&& getVoxel(i, j - 1, k)
			&& getVoxel(i, j + 1, k)
			&& getVoxel(i, j, k - 1)
			&& getVoxel(i, j, k + 1))
				setVoxel(i, j, k, false);
			}
}

// Donne le nombre de voxels pleins de la grille
int VoxelGrid::nbVoxelPleins()
{
	int nb = 0;
	for (int x = 0; x < _size; x++)
		for (int y = 0; y < _size; y++)
			for (int z = 0; z < _size; z++){
				if (_content[(x*_size + y)*_size + z])
					nb++;
			}
	return nb;
}

// Remplit la grille avec "value" dans chaque case
void VoxelGrid::setAllGrid(bool value)
{
	for (int x = 0; x < _size; x++)
		for (int y = 0; y < _size; y++)
			for (int z = 0; z < _size; z++)
				_content[(x*_size + y)*_size + z] = value;
}

// Retourne le contenu d'une case
inline bool VoxelGrid::getVoxel(int x, int y, int z)
{
	if (x < 0 || x >= _size || y < 0 || y >= _size || z < 0 || z >= _size)
	{
		cout << "getVoxel hors champ" << endl;
		return false;
	}
	else return _content[(x*_size + y)*_size + z];
}

// Remplit une case avec "value"
inline void VoxelGrid::setVoxel(int x, int y, int z, bool value)
{
	if (x < 0 || x >= _size || y < 0 || y >= _size || z < 0 || z >= _size)
	{
		cout << "setVoxel hors champ" << endl;
		return;
	}
	else _content[(x*_size + y)*_size + z] = value;
}

// Retourne la taille de grille
int VoxelGrid::getSize()
{
	return _size;
}

// Affiche une coupe dans la console
void VoxelGrid::displayCut(int zAxis)
{
	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _size; j++)
			cout << getVoxel(i, j, zAxis);
		cout << endl;
	}
}

// Donne l'index en 1D d'un vertex de coordonnees (i, j, k)
inline int VoxelGrid::vertexIndex(int i, int j, int k)
{
	return ((_size + 1)*i + j)*(_size + 1) + k;
}

//  Convertit le voxelGrid en mesh
void VoxelGrid::convertToMesh(Mesh& m)
{
	m.V = vector<Vertex>((_size + 1)*(_size + 1)*(_size + 1));
	m.T = vector<Triangle>();
	m.Q = vector<Quad>();

	float step = 2.f / ((float)_size);

	for (int i = 0; i <= _size; i++)
		for (int j = 0; j <= _size; j++)
			for (int k = 0; k <= _size; k++)
				m.V[vertexIndex(i, j, k)].p = Vec3f(-1.f + i*step, -1.f + j*step, -1.f + k*step);

	for (int i = 0; i < _size; i++)
		for (int j = 0; j < _size; j++)
			for (int k = 0; k < _size; k++){
				if (getVoxel(i, j, k))
				{
					int i1 = vertexIndex(i, j, k);
					int i2 = vertexIndex(i + 1, j, k);
					int i3 = vertexIndex(i + 1, j + 1, k);
					int i4 = vertexIndex(i, j + 1, k);
					int i5 = vertexIndex(i, j, k + 1);
					int i6 = vertexIndex(i + 1, j, k + 1);
					int i7 = vertexIndex(i + 1, j + 1, k + 1);
					int i8 = vertexIndex(i, j + 1, k + 1);

					/*m.Q.push_back(Quad(i1, i4, i3, i2));	//z-
					m.Q.push_back(Quad(i5, i6, i7, i8));	//z+
					m.Q.push_back(Quad(i1, i5, i8, i4));	//x-
					m.Q.push_back(Quad(i1, i2, i6, i5));	//y-
					m.Q.push_back(Quad(i2, i3, i7, i6));	//x+
					m.Q.push_back(Quad(i3, i4, i8, i7));	//y+
					*/	

					if (i == 0 || !getVoxel(i - 1, j, k))
						m.Q.push_back(Quad(i1, i5, i8, i4));
					if (i == _size - 1 || !getVoxel(i+1, j, k))
						m.Q.push_back(Quad(i2, i3, i7, i6));
					if (j == 0 || !getVoxel(i, j-1, k))
						m.Q.push_back(Quad(i1, i2, i6, i5));
					if (j == _size - 1 || !getVoxel(i, j+1, k))
						m.Q.push_back(Quad(i3, i4, i8, i7));
					if (k == 0 || !getVoxel(i, j, k-1))
						m.Q.push_back(Quad(i1, i4, i3, i2));
					if (k == _size - 1 || !getVoxel(i, j, k+1))
						m.Q.push_back(Quad(i5, i6, i7, i8));
				}
			}

	m.recomputeNormals();
}
