#pragma once
#include <cstdint>
#include <queue>
#include "VoxelGrid.h"

void displayMask(uint8_t mask);
int maskValue(uint8_t mask);

class Octree
{
	Octree* _childs;	//Contient les 8 Octree fils
	bool _isEmpty;

	void fillOctree(VoxelGrid& voxGrid, int x, int y, int z, int size); // Fonction recursive servant a fillOctreeWithVoxelGrid
	void convertOctreeBlockToVoxelGrid(VoxelGrid& voxGrid, int x, int y, int z, int size); // Fonction recursive servant a convertOctreeToVoxelGrid

public:
	Octree();			// Cree un Octree vide
	~Octree();
	bool isEmpty();
	bool isLeaf();
	bool isFatherOfLeaves();
	void fillOctreeWithVoxelGrid(VoxelGrid& voxGrid); // Remplit un Octree avec le contenu d'une VoxelGrid
	void convertOctreeToVoxelGrid(VoxelGrid& voxGrid); // Remplit une VoxelGrid avec le contenu d'un Octree
	uint8_t computeMask();
	void encodeWithPointersRec(vector<uint8_t>& masks, vector<uint32_t>& pointers, uint32_t& index); // TODO : Debug
	void encodeWithPointers(vector<uint8_t>& masks, vector<uint32_t>& pointers); // TODO : Debug
	void loadFromPointerEncodingRec(const vector<uint8_t>& masks, const vector<uint32_t>& pointers, int index);
	void loadFromPointerEncoding(const vector<uint8_t>& masks, const vector<uint32_t>& pointers);
	void encodeBreadthFirst(vector<uint8_t>& storage);
	void loadFromBreadthFirst(const vector<uint8_t>& storage);
};

