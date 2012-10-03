/**************************************************************************
NclBlocks.h - encapsulating a Nexus DATA block

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef NCLBLOCKS_H
#define NCLBLOCKS_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include "MesaTree.h"
#include "BasicScanner.h"
#include "SimpleMatrix.h"
#include <iostream>
#include <vector>
#include "nexusdefs.h"
#include "nexustoken.h"
#include "nexus.h"
#include "taxablock.h"
#include "treesblock.h"
#include "assumptionsblock.h"
#include "charactersblock.h"


// *** CONSTANTS & DEFINES

class NexusWriter;
class NexusReader;


// *** FXN DECLARATIONS **************************************************/

std::string	makeUnknownCmdStr (nxsstring iNStr);


// *** CLASS DECLARATIONS ************************************************/

//*** TAXA BLOCK
class MyTaxaBlock : public TaxaBlock
{
public:
	MyTaxaBlock (const progcallback_t& ikProgressCb)
		: mProgressCb (ikProgressCb)
		{}
		
	void SkippingCommand (nxsstring s);
	void summarise (ostream& ioOutStream);
	
private:
	progcallback_t	mProgressCb;
};


//*** CHARACTERS BLOCK
class MyCharactersBlock : public CharactersBlock
{
// NCL PORTION
public:
	MyCharactersBlock (TaxaBlock& tb, AssumptionsBlock& ab, const
		progcallback_t& ikProgressCb)
		: CharactersBlock( tb, ab ), mProgressCb (ikProgressCb)
		{}
	
	~MyCharactersBlock ()
	{
		if (taxonPos != NULL)
			delete [] taxonPos;
	}
	
	
	void SkippingCommand (nxsstring s);

protected:	
	void	HandleMatrix (NexusToken& token);
	void	handleContMatrix (NexusToken& token);

private:
	progcallback_t	mProgressCb;
	

// DATA WRANGLER
public:
	typedef sbl::LabelledSimpleMatrix<double>		contmatrix_type;	
	contmatrix_type										mContData;
	stringvec_t												mTaxaNames;

	void 			assertValidIndex (int iRow, int iCol);
	
	friend class NexusWriter;
	friend class NexusReader;
};


//*** CONTINUOUS BLOCK
class MyContBlock : public MyCharactersBlock
{
public:
	MyContBlock 
		(TaxaBlock& tb, AssumptionsBlock& ab, const progcallback_t& ikProgressCb)
		: MyCharactersBlock (tb, ab, ikProgressCb)
	{
		datatype = continuous;
		id = "CONTINUOUS";
	}

	void summarise (ostream& ioOutStream);

protected:
};


//*** DATA BLOCK
class MyDataBlock : public MyCharactersBlock
{
public:
	MyDataBlock
		(TaxaBlock& tb, AssumptionsBlock& ab, const progcallback_t& ikProgressCb)
		: MyCharactersBlock (tb, ab, ikProgressCb)
	{
		id = "DATA";
		newtaxa = true;
	}

	void summarise (ostream& ioOutStream);

protected:
	void Reset();
};


//*** TREES BLOCK
class MyTreesBlock : public TreesBlock
{
public:
// PUBLIC TYPE INTERFACE
	typedef std::vector<MesaTree>				treecontainer_type;
	typedef treecontainer_type::size_type	size_type;
	typedef treecontainer_type::iterator	iterator;
	
// LIFECYCLE	
	MyTreesBlock (TaxaBlock& iTaxa, const progcallback_t& ikProgressCb)
		: TreesBlock(iTaxa), mProgressCb (ikProgressCb)
		{}

// READING
	void Read (NexusToken& token);
	void SkippingCommand (nxsstring s);

// ACCESSORS
//	string getTreeRep (size_type theIndex)
//		{ return mTrees[theIndex].writeNewick (&mTable); }
		
	MesaTree& operator [] (size_type iIndex)
		{ return mTrees[iIndex]; }

	MesaTree& refTree (size_type iIndex)
		{ return (*this)[iIndex]; }

	size_type countTrees ()
		{ return mTrees.size(); }

//	size_type countTaxa ()
//		{ return mTable.size(); }
		
	treecontainer_type	mTrees;	
	//TranslationTable		mTable;

// INTERNALS		
private:
	progcallback_t	mProgressCb;
	void parseTrees ();
	void readNewickTree (sbl::BasicScanner* iSrcScannerP, MesaTree& oTree);
	void readNewickNode (sbl::BasicScanner* iSrcScanner, MesaTree& oTree,
		MesaTree::id_type iParId);
	double readNewickNodeDist (sbl::BasicScanner* iSrcScanner);
};


#endif
// *** END ***************************************************************/



