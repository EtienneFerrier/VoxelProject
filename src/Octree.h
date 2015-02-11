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
	Octree();			// Attention, ne cree PAS un Octree representant un voxel
	~Octree();
	bool isEmpty();
	void fillOctreeWithVoxelGrid(VoxelGrid& voxGrid); // Remplit un Octree avec le contenu d'une VoxelGrid
	void convertOctreeToVoxelGrid(VoxelGrid& voxGrid); // Remplit une VoxelGrid avec le contenu d'un Octree
	uint8_t computeMask();
	void encodeWithPointersRec(ofstream& file, uint32_t& index); // TODO : Debug
	void encodeWithPointers(string path); // TODO : Debug
	void encodeBreadthFirst(vector<uint8_t>& storage); // TODO : Debug
};

