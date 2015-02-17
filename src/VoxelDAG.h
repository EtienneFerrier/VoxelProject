#ifndef VOXELDAG_HPP_
#define VOXELDAG_HPP_

#include <vector>
#include <algorithm>
#include <stdint.h>

class Octree;
class VoxelGrid;

class VoxelDAG {
public:
    typedef std::vector<std::vector<std::vector<int>>> fullDAG;

private:
    fullDAG _dag;

public:
    fullDAG buildFullSVO(Octree& oct);
    void mergeSVO(fullDAG& dagTree);
    void buildDAG(Octree& oct);

    void toVoxelGrid(
        VoxelGrid& voxGrid, int lvl = 0, int node = 0,
        int x = 0, int y = 0, int z = 0,
        bool highlight = false);

	int getSize();

    bool isEmpty();
};


#endif /* VOXELDAG_HPP_ */
