/**************************************************************************
TreeWrangler.cpp - for juggling arrays of phylogenetic trees

Credits:
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>


About:
- allows an abstraction of the data away from a strict and limited Nexus
representation. (Required because the NCL makes it hard to modify data.)

Changes:
- 01.1.25: coded.

To Do:
- 

**************************************************************************/


// *** INCLUDES

#include "TreeWrangler.h"
#include "StringUtils.h"
#include "MesaUtils.h"
#include "TranslationTable.h"
#include "MesaPrefs.h"
#include "ActionUtils.h"


// *** CONSTANTS AND DEFINES

typedef TreeWrangler::size_type	size_type;
typedef TreeWrangler::iterator	iterator;


// *** SERVICES **********************************************************/

// *** ACCESSORS *********************************************************/

size_type TreeWrangler::countTrees ()
{
	return size ();
}
				

inline TreeWrangler::size_type TreeWrangler::getActiveTreeIndex () const
{
	assert (0 < size());
	assertValidIndex (mDefIndex);
	
	return mDefIndex;
} 	

std::string TreeWrangler::getTreeName (size_type iIndex)
{
	assert (0 < size());
	assertValidIndex (iIndex);
	
	return (at(iIndex)).getTreeName();
}


// *** MUTATORS **********************************************************/
#pragma mark -

void TreeWrangler::seedTree ()
{
	MesaTree	theNewTree;
	theNewTree.insertRoot ();
	theNewTree.setNodeName (theNewTree.getRoot(), nextFakeName());
	addTree (theNewTree);
}


void TreeWrangler::addTree (const MesaTree& iNewTree)
//: add a tree to the collection
// Here we make the few modifications that are needed for a stored tree.
// First, if the tree is unnamed, we name it. Second, all the internal
// nodes are listed as dead.
{
	push_back (iNewTree);
	std::string theNewName = iNewTree.getTreeName ();
	if (theNewName == "")
	{
		size_type theTreeIndex = size() - 1;
		theNewName = concatIntToString("tree_", theTreeIndex + 1);
		setTreeName (theTreeIndex, theNewName.c_str());
		iterator theNewTreeIter = begin() + theTreeIndex;
		theNewTreeIter->makeInternalsDead ();
	}
	if (size() == 1)
	{
		setActiveTreeIndex (0);
	}
}


void TreeWrangler::deleteTree (int iIndex)
{
	// what's happens to the default index? 
	int theDefIndex = getActiveTreeIndex ();
	// if deleting the def tree, set it to the one after if
	// possible else the one before (if it's the last tree)
	if (iIndex == theDefIndex)
	{
		if (iIndex == 0)
			setActiveTreeIndex (0);
		else if (iIndex == (size() - 1)) // if last entry
		{
			setActiveTreeIndex ((iIndex) ? 0 : (theDefIndex - 1));
		}
	}
	
	// now that's done we can erase the tree
	erase (begin() + iIndex);
}


// *** TREE ACCESS
#pragma mark -

MesaTree& TreeWrangler::getActiveTree ()
{
	assert (0 < size());
	size_type theIndex = getActiveTreeIndex();
	assert (0 <= theIndex);
	assert (theIndex < size());
	return (*this)[theIndex];
}


MesaTree* TreeWrangler::getActiveTreeP ()
{
	return & (getActiveTree());
}


iterator TreeWrangler::getActiveTreeIter ()
{
	assert (0 < countTrees());
	return (begin() + getActiveTreeIndex());
}


void TreeWrangler::duplicateActiveTree ()
{
	MesaTree theNewTree = getActiveTree();
	addTree (theNewTree);
	int theIndex = size() - 1;
	setTreeName (theIndex, (concatIntToString ("tree_", theIndex + 1)).c_str());
}


void TreeWrangler::setActiveTreeIndex (size_type iNewIndex)
{
	mDefIndex = iNewIndex;
} 	

void TreeWrangler::setTreeName (size_type iIndex, const char* iNewName)
{
	assert (0 < size());
	assertValidIndex (iIndex);
	
	(at(iIndex)).setTreeName (iNewName);
}


void TreeWrangler::calcTranslationTable (TranslationTable* iTableP)
//: adds all the names in this tree to the translation table provided
{
	// iterate over every tree
	for (iterator theTreeI = begin(); theTreeI != end(); theTreeI++)
	{
		// for every leaf in that tree
		for (nodeiter_t r = theTreeI->begin(); r != theTreeI->end(); r++)
		{
			// grab leaf name and pass to translation table
			if (theTreeI->isLeaf (r))
				iTableP->addName ((theTreeI->getLeafName (r)).c_str());
		}
	}	

	iTableP->alphabetize ();
}
	

// *** I/O ***************************************************************/
#pragma mark -

void TreeWrangler::summarise (std::ostream& ioOutStream)
{
	// Main:
	if (size() == 0)
	{
		ioOutStream << "There are no trees." << std::endl;
	}
	else
	{
		bool	theSingleTree = (1 == size());

		ioOutStream << "There " << (theSingleTree? "is " : "are ") <<
			size() << " tree" << (theSingleTree? "" : "s") << "." << std::endl;
		
		if (not theSingleTree)
			ioOutStream << "The active tree is " << mDefIndex + 1 << std::endl;

		long theNumNodes = (getActiveTreeP())->countNodes();
		long theNumLeaves = (getActiveTreeP())->countLeaves();
		long theNumAlive = (getActiveTreeP())->countAliveLeaves();
		ioOutStream << "The active tree has " << theNumNodes << " nodes (" <<
			theNumLeaves << " leaves, " << theNumAlive << " alive)" << std::endl;
	}
}


void TreeWrangler::detailedReport (std::ostream& ioOutStream)
{
	// Main:
	summarise (ioOutStream);
	
	if (0 < size())
	{
		for (iterator q = begin(); q != end(); q++)
		{
			ioOutStream << "   ";
			if (q == getActiveTreeIter())
				ioOutStream << "* ";
			ioOutStream << q->getTreeName () << " " <<
				q->writeNewick() << std::endl;
		}
	}
}


// *** DEPRECATED & DEBUG ***********************************************/
#pragma mark -

void TreeWrangler::assertValidIndex (size_type iIndex) const
//: just a debug function to catch bad accesses
{
	assert (0 <= iIndex);
	assert (iIndex < size());
}
	
void TreeWrangler::validate ()
//: just a debug function to catch bad accesses
{
	for (long i = 0; i < size(); i++)
		(at (i)).validate ();
		
	if (size () != 0)
	{
		assert (0 <= getActiveTreeIndex ());
		assert (getActiveTreeIndex () < size());
	}
}
	
// *** INTERNALS *********************************************************/
#pragma mark -

inline MesaTree& TreeWrangler::refActiveTree ()
//: returns a reference to the active tree
{	
	return at (getActiveTreeIndex());
}
	
	
const char* TreeWrangler::nextFakeName ()
{
/*
	static int theFirstIndex = 1;
	static std::string theCurrName = concatIntToString ("taxa_", theFirstIndex);
	theFirstIndex++;
	return theCurrName.c_str();
*/
	return getNextFakeName ();
}


// *** END ***************************************************************/



