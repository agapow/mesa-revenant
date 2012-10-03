#ifndef __TREESBLOCK_H
#define __TREESBLOCK_H

#include "taxablock.h"

//
// TreesBlock class
//
class TreesBlock : public NexusBlock
{
   // Adding a new data member? Don't forget to:
   // 1. Describe it in the class header comment at the top of "treesblock.cpp"
   // 2. Initialize it (unless it is self-initializing) in the constructor
   //    and reinitialize it in the Reset function
   // 3. Describe the initial state in the constructor documentation
   // 4. Delete memory allocated to it in both the destructor and Reset function
   // 5. Report it in some way in the Report function
public:
   AssocList translateList;
   LabelList treeName;
   LabelList treeDescription;
   BoolVect rooted;
   TaxaBlock& taxa;
   int ntrees;
   int defaultTree;

protected:
	void Read( NexusToken& token );
	void Reset();

public:
	TreesBlock( TaxaBlock& tb );
   ~TreesBlock();

   int   GetNumDefaultTree();
   int   GetNumTrees();
   nxsstring GetTreeName( int i );
   nxsstring GetTreeDescription( int i );
   nxsstring GetTranslatedTreeDescription( int i );
   int   IsDefaultTree( int i );
   int   IsRootedTree( int i );
	void  Report( ostream& out );
	
	void setDefaultTree (int iDefTreeIndex)
	{
		// Preconditions:
		// new index must be in a permissible range
		assert (0 <= iDefTreeIndex);
		assert (iDefTreeIndex < GetNumTrees());		
		// Main:
		defaultTree = iDefTreeIndex;
	}
};

#endif
