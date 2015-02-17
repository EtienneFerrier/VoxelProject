#include <vector>
#include <algorithm>
#include <stdint.h>
#include "VoxelDAG.h"
#include "Octree.h"
#include "VoxelGrid.h"

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



// GDB won't get all operators without this
template class std::vector<std::vector<std::vector<int>>>;
template class std::vector<std::vector<int>>;
template class std::vector<int>;

using fullDAG = VoxelDAG::fullDAG;

fullDAG VoxelDAG::buildFullSVO(Octree& oct) {
    std::vector<uint8_t> masks;
    // masks are stored breadth-first
    oct.encodeBreadthFirst(masks);

    // store level-wise
    // also, don't just store a pointer to the first child, but to all of them
    fullDAG octLevels;

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
    return octLevels;
}


void VoxelDAG::mergeSVO(fullDAG& dagTree) {
    for (int lvl = dagTree.size()-1; lvl > 0; --lvl) {
        // find all uniques
        std::vector<std::vector<int>> uniques(dagTree[lvl]);
        std::sort(uniques.begin(), uniques.end());
        auto resizeIterator = std::unique(uniques.begin(), uniques.end());
        uniques.resize( std::distance(uniques.begin(), resizeIterator) );

        // merging
        // find an injective mapping from the nodes in the original level to uniques
        std::vector<int> mapping;
        for (auto node : dagTree[lvl]) {
            auto newIt = std::lower_bound(uniques.begin(), uniques.end(), node);
            mapping.push_back(std::distance(uniques.begin(), newIt));
        }

        // update the pointers in the level above to point to the same element in uniques
        for (int prnt = dagTree[lvl-1].size() - 1; prnt >= 0; --prnt) {
            for (int i = dagTree[lvl-1][prnt].size() - 1; i > 0; --i) {
                dagTree[lvl-1][prnt][i] = mapping[dagTree[lvl-1][prnt][i]];
            }
        }

        // actually replace the level with its uniques
        dagTree[lvl] = uniques;
    }
}


void VoxelDAG::buildDAG(Octree& oct) {
    auto dagTree = buildFullSVO(oct);
    mergeSVO(dagTree);
    _dag = dagTree;
}

void VoxelDAG::toVoxelGrid(
    VoxelGrid& voxGrid, int lvl, int node,
    int x, int y, int z,
    bool highlight)
{
    static int highlightNode = -1;
    highlightNode = (highlightNode + 1) % _dag.back().size();
    int halfSize = voxGrid.getSize() >> (lvl + 1);
    int posX[] = {x, x + halfSize, x + halfSize, x, x, x + halfSize, x + halfSize, x};
    int posY[] = {y, y, y + halfSize, y + halfSize, y, y, y + halfSize, y + halfSize};
    int posZ[] = {z, z, z, z, z + halfSize, z + halfSize, z + halfSize, z + halfSize};
    uint8_t children = _dag[lvl][node][0];
    // at the lowest level
    if (_dag.size() == 1 + lvl) {
        for (int i = 0; i < 8; i++) {
            if (children & (128 >> i)) {
                voxGrid.setVoxel(posX[i], posY[i], posZ[i], true);
                if (highlight && node == highlightNode) {
                    voxGrid.setColor(posX[i], posY[i], posZ[i]);
                }
            }
        }
    } else {
        int nodeInd = 1;
        for (int i = 0; i < 8; i++) {
            if (children & (128 >> i)) {
                toVoxelGrid(voxGrid, lvl+1, _dag[lvl][node][nodeInd], posX[i], posY[i], posZ[i], highlightNode);
                nodeInd++;
            }
        }
    }
}

bool VoxelDAG::isEmpty() {
    return _dag.size() == 0;
}

int VoxelDAG::getSize()
{
	int res = 0;
	for (int i = 0; i < _dag.size(); i++)
		for (int j = 0; j < _dag[i].size(); j++)
		{
		res += _dag[i][j].size();
		}

	return res;
}


