#pragma once
#include <cstdint>
#include <queue>
#include <vector>

#include "VoxelGrid.h"


void displayMask(uint8_t mask);
int maskValue(uint8_t mask);

class Octree
{
	// Champs
	Octree* _children;	//Contient NULL (si feuille) ou un tableau de 8 Octree fils
	bool _isEmpty;		//true par default



	// Fonctions recursives servant au fonctions public
	void fillOctree(VoxelGrid& voxGrid, int x, int y, int z, int size); // Fonction recursive servant a fillOctreeWithVoxelGrid
	void convertOctreeBlockToVoxelGrid(VoxelGrid& voxGrid, int x, int y, int z, int size); // Fonction recursive servant a convertOctreeToVoxelGrid
	void encodeWithPointersRec(vector<uint8_t>& masks, vector<int>& pointers, vector<int>& levels, int& index); // Fonction recursive servant a encodeWithPointers
	void loadFromPointerEncodingRec(const std::vector<uint8_t>& masks, const std::vector<int>& pointers, int index); // Fonction recursive servant a loadFromPointerEncoding

public:
	Octree();
	~Octree();
	bool isEmpty();
	bool isLeaf();
	bool isFatherOfLeaves();
	std::vector<Octree*> children();
	void fillOctreeWithVoxelGrid(VoxelGrid& voxGrid);
	void cutEmptyNodes();
	void convertOctreeToVoxelGrid(VoxelGrid& voxGrid);
	uint8_t computeMask();
	void encodeWithPointers(std::vector<uint8_t>& masks, std::vector<int>& pointers, std::vector<int>& levels);
	void loadFromPointerEncoding(const std::vector<uint8_t>& masks, const std::vector<int>& pointers);
	void encodeBreadthFirst(std::vector<uint8_t>& storage);
	void loadFromBreadthFirst(const std::vector<uint8_t>& storage);
};

