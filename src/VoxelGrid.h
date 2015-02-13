#pragma once

#include "Mesh.h"
#include <vector>

using namespace std;

// On suppose que la grille est centrée en (0, 0, 0)
// On suppose que la grille est de taille 2 de côté
class VoxelGrid
{
	int _size; // Taille du cote de la grille
	bool* _content; // Tableau de stockage du contenu

public:

	VoxelGrid(int size); 
	~VoxelGrid();
	void fillGrid(Mesh& m); 
	void fillGridBSH(Mesh& m); 
	void fillSparseGridBSH(Mesh& m); 
	void loadOFF(const std::string & filename); 
	void emptyInteriorVoxels(); 
	int nbVoxelPleins(); 
	void setAllGrid(bool value); 
	inline bool getVoxel(int x, int y, int z); 
	inline void setVoxel(int x, int y, int z, bool value); 
	int getSize(); 
	void displayCut(int zAxis);
	inline int vertexIndex(int i, int j, int k);
	void convertToMesh(Mesh& m);
};

