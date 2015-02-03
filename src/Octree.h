#pragma once

#include "VoxelGrid.h"

class Octree
{
	Octree* _childs;	//Contient les 8 Octree fils
	bool _isEmpty;
	// Champ a remplacer par un test de _childs == NULL
	bool _isVoxel;		// True lorsque l'Octree represente un unique voxel (pas de fils dans ce cas)

	void fillOctree(VoxelGrid& voxGrid, int x, int y, int z, int size); // Fonction recursive servant a fillOctreeWithVoxelGrid
	void convertOctreeBlockToVoxelGrid(VoxelGrid& voxGrid, int x, int y, int z, int size); // Fonction recursive servant a convertOctreeToVoxelGrid

public:
	Octree();			// Attention, ne cree PAS un Octree representant un voxel
	~Octree();
	void fillOctreeWithVoxelGrid(VoxelGrid& voxGrid); // Remplit un Octree avec le contenu d'une VoxelGrid
	void convertOctreeToVoxelGrid(VoxelGrid& voxGrid); // Remplit une VoxelGrid avec le contenu d'un Octree

};

