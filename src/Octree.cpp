#include "Octree.h"


Octree::Octree()
{
	_childs = NULL;
	_isEmpty = false;
}

Octree::~Octree()
{
	delete[] _childs;
}

bool Octree::isEmpty()
{
	return _isEmpty;
}

void Octree::fillOctree(VoxelGrid& voxGrid, int x, int y, int z, int size)
{
	if (size == 1)
	{
		_isEmpty = !voxGrid.getVoxel(x, y, z);
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



// Affiche les 8 bits de mask
void displayMask(uint8_t mask)
{
	for (int i = 0; i < 8; i++)
	{
		cout << ((mask & (128 >> i)) > 0);
	}
	cout << endl;
}

// Nombre de 1 dans mask
int maskValue(uint8_t mask)
{
	int res = 0;
	for (int i = 0; i < 8; i++)
	{
		res += ((mask & (128 >> i)) > 0);
	}
	return res;
}

//Encode sur 8 bit l'informations "le fils i contient de la matiere"
uint8_t Octree::computeMask()
{
	if (_childs == NULL)
	{
		cout << "No mask for leaves" << endl;
		return 0;
	}
	else
	{
		uint8_t m = 0;
		for (int i = 0; i < 8; i++)
		{
			if (!_childs[i]._isEmpty)
				m |= 128 >> i;
		}
		return m;
	}
}


// TODO : Debug
void Octree::encodeWithPointersRec(ofstream& file, uint32_t& index)
{
	uint32_t emptyChildPt = index + 1;
	uint8_t mask;
	uint32_t tmp;

	for (int i = 0; i < 8; i++)
	{
		if (!_childs[i]._isEmpty)
		{
			mask = _childs[i].computeMask(); // Encodage du masque du fils i
			tmp = 0; // Le pointeur vers les fils du fils i n'est pas encore indefini
			file << mask << tmp; // Ecriture du masque et du pointeur
			index += 5; // 5 bytes ont été écrits
		}
	}

	for (int i = 0; i < 8; i++)
	{
		if (!_childs[i]._isEmpty)
		{
			file.seekp(emptyChildPt); // On retourne écrire dans le poiteurs vers les fils du fils i
			file << index; // On y écrit la position du premier fils du fils i
			emptyChildPt += 5; 
			file.seekp(index); // On retourne a la position du premier fils du fils i
			_childs[i].encodeWithPointersRec(file, index); // On encode les fils du fils i
		}
	}

}

// TODO : Debug
void Octree::encodeWithPointers(string path)
{
	ofstream file(path.c_str(), ios::in | ios::binary);

	if (file) {
		uint32_t index = 0;
		uint8_t mask = computeMask();
		uint32_t childPt = 5; // Index du 1er fils de la racine
		file << mask << childPt; // On ecrit les infos de la racine
		index += 5; // 5 bytes écrits
		encodeWithPointersRec(file, index);

		file.close();
	}
	else
		cout << "Erreur ouverture fichier" << endl;
}

// TODO : Debug
void Octree::encodeBreadthFirst(vector<uint8_t>& storage)
{
	queue < Octree > file;
	file.push(*this);

	while (!file.empty())
	{
		Octree tree = file.front();
		file.pop();

		storage.push_back(tree.computeMask());

		for (int i = 0; i < 8; i++)
		{
			if (!tree._childs[i].isEmpty())
			{
				file.push(tree._childs[i]);
			}
		}
	}
}

