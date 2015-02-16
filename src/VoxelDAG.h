#ifndef VOXELDAG_HPP_
#define VOXELDAG_HPP_

#include <vector>
#include <stdint.h>
#include "Octree.h"

class VoxelDAG {
    std::vector<uint32_t> _dag;

public:
    void buildDAG(Octree& oct) {
        vector<uint8_t> masks;
        vector<int> pointers;
        vector<int> levels;
        oct.encodeWithPointers(masks, pointers, levels);
    }
};



#endif /* VOXELDAG_HPP_ */
