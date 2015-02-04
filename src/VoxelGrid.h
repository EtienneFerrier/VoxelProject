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

	VoxelGrid(int size); // Cree une grille de taille voulue et l'initialise a false
	~VoxelGrid();
	void fillGrid(Mesh& m); // Convertit un mesh en voxelGrid (en prenant l'interieur).
	void fillGridBSH(Mesh& m); // Comme fillGrid mais utilise la structure BSH du mesh.
	void fillSparseGridBSH(Mesh& m); // Comme fillGridBSH mais en ne conservant que les voxels sur le bord du mesh. (plus lent)
	void loadOFF(const std::string & filename); // Convertit un mesh en format OFF en voxelGrid
	int nbVoxelPleins(); // Donne le nombre de voxels pleins de la grille
	void setAllGrid(bool value); // Remplit la grille avec "value" dans chaque case
	inline bool getVoxel(int x, int y, int z); // Retourne le contenu d'une case
	inline void setVoxel(int x, int y, int z, bool value); // Remplit une case avec "value"
	int getSize(); // Retourne la taille de grille
	void displayCut(int zAxis); // Affiche une coupe dans la console
	inline int vertexIndex(int i, int j, int k); // Donne l'index en 1D d'un vertex de coordonnees (i, j, k)
	void convertToMesh(Mesh& m); //  Convertit le voxelGrid en mesh
};

