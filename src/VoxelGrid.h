#pragma once

#include "Mesh.h"
#include <cstdint>
#include <vector>


// On suppose que la grille est centrée en (0, 0, 0)
// On suppose que la grille est de taille 2 de côté
class VoxelGrid
{
	int _size; // Taille du cote de la grille
	bool* _content; // Tableau de stockage du contenu
	bool* _color;

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
	inline bool getVoxel(int x, int y, int z) {
		if (x < 0 || x >= _size || y < 0 || y >= _size || z < 0 || z >= _size) {
			std::cout << "getVoxel hors champ" << std::endl;
			return false;
		} else {
			return _content[(x*_size + y)*_size + z];
		}
	}
	inline void setVoxel(int x, int y, int z, bool value) {
		if (x < 0 || x >= _size || y < 0 || y >= _size || z < 0 || z >= _size) {
			std::cout << "setVoxel hors champ" << std::endl;
		} else {
			_content[(x*_size + y)*_size + z] = value;
		}
	}
	int getSize(); 
	void displayCut(int zAxis);
	inline int vertexIndex(int i, int j, int k);
	void convertToMesh(Mesh& m);
	void colorSubOctree();
	inline bool getColor(int i, int j, int k)
	{
		return ((_color != NULL) && _color[(i*_size + j)*_size + k]);
	}
	void clearColor();
};

