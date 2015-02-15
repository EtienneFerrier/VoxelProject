#include "Octree.h"

// Cree un Octree feuille et vide
Octree::Octree()
{
	_childs = NULL;
	_isEmpty = true;
}

// Efface un Octree et tout son sous arbre
Octree::~Octree()
{
	delete[] _childs;
}

// Renvoie true lorsque la region decrite par l'Octree est vide
bool Octree::isEmpty()
{
	return _isEmpty;
}

// Renvoie true lorsqu'un Octree est une feuille i.e. décrit un voxel
bool Octree::isLeaf()
{
	return _childs == NULL;
}

// Renvoie true lorsqu'un Octree est un niveau juste au dessus des feuilles (i.e. tous ses fils sont des feuilles)
bool Octree::isFatherOfLeaves()
{
	return _childs != NULL &&
		_childs[0].isLeaf() &&
		_childs[1].isLeaf() &&
		_childs[2].isLeaf() &&
		_childs[3].isLeaf() &&
		_childs[4].isLeaf() &&
		_childs[5].isLeaf() &&
		_childs[6].isLeaf() &&
		_childs[7].isLeaf();
}

// Remplit un Octree complet avec une region de l'espace decrite par voxGrid aux coordonnees [x, x + size]*[y, y + size]*[z, z + size]
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

		_isEmpty = _childs[0]._isEmpty &&
				_childs[1]._isEmpty &&
				_childs[2]._isEmpty &&
				_childs[3]._isEmpty &&
				_childs[4]._isEmpty &&
				_childs[5]._isEmpty &&
				_childs[6]._isEmpty &&
				_childs[7]._isEmpty;

	}
}

// Remplit un Octree complet avec tous les voxels d'une grille
void Octree::fillOctreeWithVoxelGrid(VoxelGrid& voxGrid)
{
	fillOctree(voxGrid, 0, 0, 0, voxGrid.getSize());
}

// Enleve et efface les sous arbres completement vides de l'Octree
void Octree::cutEmptyNodes()
{
	if (_isEmpty)
		delete[] _childs;
	else if (_childs != NULL)
	{
		for (int i = 0; i < 8; i++)
			_childs[i].cutEmptyNodes();
	}
}

// Remplit la region de l'espace decrite par voxGrid aux coordonnees [x, x + size]*[y, y + size]*[z, z + size]
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

// Remplit une grille avec tout le contenu de l'Octree
void Octree::convertOctreeToVoxelGrid(VoxelGrid& voxGrid)
{
	convertOctreeBlockToVoxelGrid(voxGrid, 0, 0, 0, voxGrid.getSize());
}

// Affiche les 8 bits de mask en binaire dans la console
void displayMask(uint8_t mask)
{
	for (int i = 0; i < 8; i++)
	{
		cout << ((mask & (128 >> i)) > 0);
	}
	cout << endl;
}

// Renvoie le nombre du bits a 1 dans mask
int maskValue(uint8_t mask)
{
	int res = 0;
	for (int i = 0; i < 8; i++)
	{
		res += ((mask & (128 >> i)) > 0);
	}
	return res;
}

// Encode sur 8 bits l'information "_childs[i] contient de la matiere" pour tout i de 0 a 7
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

// Encode les fils de l'Octree courant et leur sous arbre dans les tableaux masks et pointers a partir de l'index en entree.
void Octree::encodeWithPointersRec(vector<uint8_t>& masks, vector<uint32_t>& pointers, uint32_t& index)
{
	uint32_t firstEmptyChildPt = index; // index du premier fils dont il faudra remplir le pointeur une fois le sous arbre associe encode
	uint8_t mask;

	//Ecriture dans masks
	for (int i = 0; i < 8; i++)
	{
		if (!_childs[i]._isEmpty && !_childs[i].isLeaf())
		{
			mask = _childs[i].computeMask(); // Encodage du masque du fils i
			masks.push_back(mask);
			index++;
			pointers.push_back(0); // Un pointeur non encore decide est mis a zero
		}
	}

	//Ecriture dans pointers
	for (int i = 0; i < 8; i++)
	{
		if (!_childs[i]._isEmpty && !_childs[i].isLeaf())
		{
			if (_childs[i].isFatherOfLeaves()) // Ce test permet de reperer le dernier niveau de l'arbre dans le tableau pointers (la valeur de pointers est alors 0)
				pointers[firstEmptyChildPt] = 0;
			else
				pointers[firstEmptyChildPt] = index; // On retourne écrire le poiteur vers le premier fils du first empty child

			firstEmptyChildPt++;
			_childs[i].encodeWithPointersRec(masks, pointers, index); // On encode les fils du fils i
		}
	}

}

/* Encode l'Octree dans 2 vecteurs dans lequel pour tout index i : 
  - masks[i] encode la presence de matiere dans les fils du noeud i.
  - pointers[i] encode l'index du premier fils non vide du noeud i. 
  - les fils non vides de chaque noeud ont des index successifs.
  - la racine est a l'index 0    */
void Octree::encodeWithPointers(vector<uint8_t>& masks, vector<uint32_t>& pointers)
{
	uint32_t index = 0;
	uint8_t mask = computeMask();
	uint32_t childPt = 1; // Index du 1er fils de la racine

	masks.push_back(mask);
	index++;
	pointers.push_back(childPt);

	encodeWithPointersRec(masks, pointers, index);
}

// Fonction recursive permettant de charger un noeud a partir d'un encodage sur 2 tableaux. Le noeud charge est encode a la position index des 2 tableaux.
void Octree::loadFromPointerEncodingRec(const vector<uint8_t>& masks, const vector<uint32_t>& pointers, int index)
{
	uint8_t mask = masks[index];
	_childs = new Octree[8];

	for (int i = 0; i < 8; i++)
	{
		_childs[i]._isEmpty = ((mask & (128 >> i)) == 0);
	}

	if (pointers[index] > 0)
	{
		int tmp = 0;
		for (int i = 0; i < 8; i++)
		{
			if (!_childs[i]._isEmpty)
			{
				_childs[i].loadFromPointerEncodingRec(masks, pointers, pointers[index] + tmp);
				tmp++;
			}
		}
	}
}

// Charge un Octree a partir d'un encodage sur 2 tableaux
void Octree::loadFromPointerEncoding(const vector<uint8_t>& masks, const vector<uint32_t>& pointers)
{
	uint8_t mask = masks[0];
	if (mask == 0)
		return;
	else
	{
		_childs = new Octree[8];
		_isEmpty = false;
		loadFromPointerEncodingRec(masks, pointers, 0);
	}
	
}

// Encode un Octree en listant de maniere "breath first" les masks des noeuds dans un tableau.
void Octree::encodeBreadthFirst(vector<uint8_t>& storage)
{
	queue < Octree* > file; // file ne signifie pas "Fichier" mais bien "Queue".
	file.push(this);

	while (!file.empty())
	{
		Octree* node = file.front();
		file.pop();

		storage.push_back(node->computeMask());

		for (int i = 0; i < 8; i++)
		{
			if (!node->_childs[i].isEmpty() && !node->_childs[i].isLeaf()) // 2e condition => pas de feuille ajoutée à la file.
			{
				file.push(&(node->_childs[i]));
			}
		}
		
	}
}

// Charge un Octree a partir d'un encodage de maniere "breath first" des masks des noeuds dans un tableau.
void Octree::loadFromBreadthFirst(const vector<uint8_t>& storage)
{
	queue < Octree* > file;
	file.push(this);
	_isEmpty = false;

	int nbNodes = storage.size();
	int ind = 0;

	while (ind < nbNodes)
	{
		Octree* node = file.front();
		file.pop();

		uint8_t mask = storage.at(ind);
		node->_childs = new Octree[8];
		

		for (int i = 0; i < 8; i++)
		{
			if (((mask & (128 >> i)) > 0))
			{
				node->_childs[i]._isEmpty = false;
				file.push(&(node->_childs[i]));
			}
		}

		ind++;
	}
}

