#pragma once
#include <cstdint>
#include <queue>
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
	void encodeWithPointersRec(vector<uint8_t>& masks, vector<uint32_t>& pointers, uint32_t& index); // Fonction recursive servant a encodeWithPointers
	void loadFromPointerEncodingRec(const vector<uint8_t>& masks, const vector<uint32_t>& pointers, int index); // Fonction recursive servant a loadFromPointerEncoding

public:
	Octree();
	~Octree();
	bool isEmpty();
	bool isLeaf();
	bool isFatherOfLeaves();
	void fillOctreeWithVoxelGrid(VoxelGrid& voxGrid);
	void cutEmptyNodes();
	void convertOctreeToVoxelGrid(VoxelGrid& voxGrid);
	uint8_t computeMask();
	void encodeWithPointers(vector<uint8_t>& masks, vector<uint32_t>& pointers);
	void loadFromPointerEncoding(const vector<uint8_t>& masks, const vector<uint32_t>& pointers);
	void encodeBreadthFirst(vector<uint8_t>& storage);
	void loadFromBreadthFirst(const vector<uint8_t>& storage);
};

