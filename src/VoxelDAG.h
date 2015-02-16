#ifndef VOXELDAG_HPP_
#define VOXELDAG_HPP_

#include <vector>
#include <algorithm>
#include <stdint.h>
#include "Octree.h"

static const uint32_t BitCount[] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

// DEBUG
template class std::vector<std::vector<std::vector<int>>>;
template class std::vector<std::vector<int>>;
template class std::vector<int>;

class VoxelDAG {
    std::vector<uint32_t> _dag;

public:
    void buildDAG(Octree& oct) {
        std::vector<uint8_t> masks;
        // masks are stored breadth-first
        oct.encodeBreadthFirst(masks);

        // store level-wise
        // also, don't just store a pointer to the first child, but to all of them
        std::vector<std::vector<std::vector<int>>> octLevels;

        int lvlStart = 0;
        int lvlEnd = 1;
        // each inner vector contains the mask at [0] and the pointers afterwards
        // e.g. octLevels[0] = {{masks[0], pointers[0]}};

        // build octLevels from the top down
        while(lvlStart < masks.size()) {
            // allocate space for current level
            std::vector<std::vector<int>> lvlvec;
            // calculate size of next level
            int nextLvlSize = 0;
            for (int i = lvlStart; i < lvlEnd; ++i) {
                // add mask
                lvlvec.push_back(std::vector<int>(1, masks[i]));
                // add pointers into next level
                if (lvlEnd < masks.size()) {
                    for (int j = 0; j < BitCount[masks[i]]; ++j) {
                        lvlvec.back().push_back(nextLvlSize + j);
                    }
                }
                nextLvlSize += BitCount[masks[i]];
            }
            octLevels.push_back(lvlvec);
            lvlStart = lvlEnd;
            lvlEnd += nextLvlSize;
        }

        // this is just to prove it works
        std::vector<uint8_t> masks_recon;
        for (auto v: octLevels) {
            for (auto vv : v) {
                masks_recon.push_back(vv[0]);
            }
        }
        bool toast = masks_recon == masks;


        int numnodes = 0;
        for (auto v : octLevels) {
            numnodes += v.size();
        }
    }
};



#endif /* VOXELDAG_HPP_ */
