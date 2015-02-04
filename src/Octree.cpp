#include "Octree.h"


Octree::Octree()
{
	_childs = NULL;
	_isEmpty = false;
	_isVoxel = false;
}

Octree::~Octree()
{
	delete[] _childs;
}

void Octree::fillOctree(VoxelGrid& voxGrid, int x, int y, int z, int size)
{
	if (size == 1)
	{
		_isEmpty = !voxGrid.getVoxel(x, y, z);
		_isVoxel = true;
	}
	else
	{
		_childs = new Octree[8];

		int halfSize = size / 2;

		_childs[0].fillOctree(voxGrid, x, y, z, halfSize);
		_childs[1].fillOctree(voxGrid, x + halfSize, y, z, halfSize);
		_childs[2].fillOctree(voxGrid, x + halfSize, y + halfSize, z, halfSize);
		_childs[3].fillOctree(voxGrid, x, y + halfSize, z, halfSize);
		_childs[4].fillOctree(voxGrid, x, y, z + halfSize, halfSize);
		_childs[5].fillOctree(voxGrid, x + halfSize, y, z + halfSize, halfSize);
		_childs[6].fillOctree(voxGrid, x + halfSize, y + halfSize, z + halfSize, halfSize);
		_childs[7].fillOctree(voxGrid, x, y + halfSize, z + halfSize, halfSize);

		if (_childs[0]._isEmpty &&
			_childs[1]._isEmpty &&
			_childs[2]._isEmpty &&
			_childs[3]._isEmpty &&
			_childs[4]._isEmpty &&
			_childs[5]._isEmpty &&
			_childs[6]._isEmpty &&
			_childs[7]._isEmpty)
			_isEmpty = true;
	}
}

void Octree::fillOctreeWithVoxelGrid(VoxelGrid& voxGrid)
{
	fillOctree(voxGrid, 0, 0, 0, voxGrid.getSize());
}

void Octree::convertOctreeBlockToVoxelGrid(VoxelGrid& voxGrid, int x, int y, int z, int size)
{
	if (size == 1)
		voxGrid.setVoxel(x, y, z, !_isEmpty);
	else if (_isEmpty)
	{
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
				{
					voxGrid.setVoxel(x + i, y + j, z + k, false);
				}
	}
	else
	{
		int halfSize = size / 2;

		_childs[0].convertOctreeBlockToVoxelGrid(voxGrid, x, y, z, halfSize);
		_childs[1].convertOctreeBlockToVoxelGrid(voxGrid, x + halfSize, y, z, halfSize);
		_childs[2].convertOctreeBlockToVoxelGrid(voxGrid, x + halfSize, y + halfSize, z, halfSize);
		_childs[3].convertOctreeBlockToVoxelGrid(voxGrid, x, y + halfSize, z, halfSize);
		_childs[4].convertOctreeBlockToVoxelGrid(voxGrid, x, y, z + halfSize, halfSize);
		_childs[5].convertOctreeBlockToVoxelGrid(voxGrid, x + halfSize, y, z + halfSize, halfSize);
		_childs[6].convertOctreeBlockToVoxelGrid(voxGrid, x + halfSize, y + halfSize, z + halfSize, halfSize);
		_childs[7].convertOctreeBlockToVoxelGrid(voxGrid, x, y + halfSize, z + halfSize, halfSize);
	}
}

void Octree::convertOctreeToVoxelGrid(VoxelGrid& voxGrid)
{
	convertOctreeBlockToVoxelGrid(voxGrid, 0, 0, 0, voxGrid.getSize());
}
