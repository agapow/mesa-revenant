/**************************************************************************
ActionUtils.cpp - utility functions for the simulation / action classes

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

**************************************************************************/


// *** INCLUDES

#include "Sbl.h"
#include "ActionUtils.h"
#include "Action.h"
#include "MesaGlobals.h"
#include "TaxaTraitMatrix.h"
#include "TreeWrangler.h"
#include "Reporter.h"

using std::string;


// *** CONSTANTS & DEFINES

// *** FUNCTIONS *********************************************************/

/*
BasicMacro* castAsMacro (BasicAction* iActionP)
//: return the parameter cast as a macro is possible, otherwise nil
// Necessary if we can't get the dynamic cast to work as desired. May
// need to place ids in actions and test them.
{
	BasicMacro* theMacroP = NULL;
	theMacroP = dynamic_cast<BasicMacro*> (iActionP);
	return theMacroP;
}
*/

void setContData (nodeiter_t iNodeIter, int iColIndex, conttrait_t& iNewVal)
{
	std::string theNodeName = MesaGlobals::mTreeDataP->getActiveTreeP()->getNodeName (iNodeIter);
	MesaGlobals::mContDataP->getData (theNodeName.c_str(), iColIndex) = iNewVal;
}

void setDiscData (nodeiter_t iNodeIter, int iColIndex, disctrait_t& iNewVal)
{
	std::string theNodeName = MesaGlobals::mTreeDataP->getActiveTreeP()->getNodeName (iNodeIter);
	MesaGlobals::mDiscDataP->getData (theNodeName.c_str(), iColIndex) = iNewVal;
}

conttrait_t getContData (const char* iTaxaName, int iColIndex)
//: return the continuous data of this taxa at this column
{
	return MesaGlobals::mContDataP->getData (iTaxaName, iColIndex);
}

conttrait_t getContData (nodeiter_t iLeafIter, int iColIndex)
//: return the continuous data of this leaf at this column
{
	std::string theNodeName = MesaGlobals::mTreeDataP->getActiveTreeP()->getNodeName (iLeafIter);
	return getContData (theNodeName.c_str(), iColIndex);
}


disctrait_t getDiscData (const char* iTaxaName, int iColIndex)
//: return the discrete data of this taxa at this column
{
	return MesaGlobals::mDiscDataP->getData (iTaxaName, iColIndex);
}

disctrait_t getDiscData (nodeiter_t iLeafIter, int iColIndex)
//: return the continuous data of this leaf at this column
{
	std::string theNodeName = (MesaGlobals::mTreeDataP->getActiveTreeP())->getLeafName (iLeafIter);
	return getDiscData (theNodeName.c_str(), iColIndex);
}


disctrait_t& referDiscState (const char* iTaxaName, int iColIndex)
{
	return MesaGlobals::mDiscDataP->getData (iTaxaName, iColIndex);
}

disctrait_t& referDiscState (nodeiter_t& iNode, int iColIndex)
{
	std::string theNodeName;
	theNodeName = MesaGlobals::mTreeDataP->getActiveTreeP()->getNodeName (iNode);
	return referDiscState (theNodeName.c_str(), iColIndex);
}

conttrait_t& referContState (const char* iTaxaName, int iColIndex)
{
	return MesaGlobals::mContDataP->getData (iTaxaName, iColIndex);
}

conttrait_t& referContState (nodeiter_t& iNode, int iColIndex)
{
	std::string theNodeName;
	theNodeName = MesaGlobals::mTreeDataP->getActiveTreeP()->getNodeName (iNode);
	return referContState (theNodeName.c_str(), iColIndex);
}


int getRichnessData (int iLeafId, int iRichCol)
//: return the spp richness of this leaf, using this data column
{
	MesaTree* theTreeP = (MesaGlobals::mTreeDataP->getActiveTreeP());
	nodeiter_t q = theTreeP->getIter (iLeafId);
	assert (theTreeP->isLeaf (q));
	
	if (iRichCol == kColIndex_None)
	{
		return 1;
	}
	else
	{
		std::string theNodeName = theTreeP->getNodeName (q);
		conttrait_t theRichness = getContData (theNodeName.c_str(), iRichCol);
		return int (theRichness);
	}
}


int getRichnessData (nodeiter_t iLeafIter, int iRichCol)
//: return the spp richness of this leaf, using this data column
{
	MesaTree* theTreeP = (MesaGlobals::mTreeDataP->getActiveTreeP());
	assert (theTreeP->isLeaf (iLeafIter));
	
	if (iRichCol == kColIndex_None)
	{
		return 1;
	}
	else
	{
		std::string theNodeName = theTreeP->getLeafName (iLeafIter);
		conttrait_t theRichness = getContData (theNodeName.c_str(), iRichCol);
		return int (theRichness);
	}
}


int countLeaves (nodeiter_t iNodeIter, int iRichCol)
//: return the number of leaves on this subtree
{
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP ();
	assert (theTreeP != NULL);
	
	int theNumLeaves = 0;
	int theNumChildren = theTreeP->countChildren (iNodeIter);
		
	if (theNumChildren == 0)
	{
		if (iRichCol == kColIndex_None)
			theNumLeaves = 1;
		else
			theNumLeaves = getRichnessData (iNodeIter, iRichCol);
	}
	else
	{
		// add up children
		for (int i = 0; i < theNumChildren; i++)
		{
			nodeiter_t q = theTreeP->getChild (iNodeIter, i);
			theNumLeaves += countLeaves (q, iRichCol);
		}
	}
	
	// Postconditions & return:
	assert (0 < theNumLeaves);
	return theNumLeaves;
}


void pushReportPrefix (const char* iPrefCstr)
{
	MesaGlobals::mReporterP->pushPrefix (iPrefCstr);
}

void popReportPrefix ()
{
	MesaGlobals::mReporterP->popPrefix ();
}


MesaTree* getActiveTreeP ()
{
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();
	assert (theTreeP != NULL);
	return theTreeP;
}


const char* getNextFakeName ()
{
	static int theFirstIndex = 1;
	static std::string theCurrName;
	theCurrName = concatIntToString ("taxa_", theFirstIndex);
	theFirstIndex++;
	return theCurrName.c_str();
}


void speciate (nodeiter_t iLeafIter)
{
	try
	{
		MesaTree* theTreeP = getActiveTreeP ();
		assert (theTreeP->isLeaf (iLeafIter));

		// get name of old taxa and generate new ones
		// static IdGenerator theTaxaNames ("tx");
		std::string theParName = theTreeP->getLeafName (iLeafIter);
		std::string theChildName1 = getNextFakeName ();
		std::string theChildName2 = getNextFakeName ();
		
		// split the parent node in the tree and name the children
		nodeiter_t theNewNode1, theNewNode2;
		theTreeP->speciate (iLeafIter, theNewNode1, theNewNode2);
		theTreeP->setNodeName (theNewNode1, theChildName1.c_str());
		theTreeP->setNodeName (theNewNode2, theChildName2.c_str());
		
		// clone the data in the wranglers
		MesaGlobals::mContDataP->cloneRow (theParName.c_str(), theChildName1.c_str());
		MesaGlobals::mContDataP->cloneRow (theParName.c_str(), theChildName2.c_str());
		MesaGlobals::mDiscDataP->cloneRow (theParName.c_str(), theChildName1.c_str());
		MesaGlobals::mDiscDataP->cloneRow (theParName.c_str(), theChildName2.c_str());
	}
	catch (...)
	{
		DBG_MSG ("in speciation");
		throw;
	}
}

std::string getNodeLabel (nodeiter_t iNodeIter)
//: return the labels of the given node, in the format set in prefs
{
	MesaTree* theTreeP = getActiveTreeP ();
	
	switch (MesaGlobals::mPrefs.mCladeLabels)
	{
		case kPrefCladeLabels_Phylo:
			return theTreeP->getNodeLabelPhylo (iNodeIter);
			break;
			
		case kPrefCladeLabels_Caic:
			return theTreeP->getNodeLabelCaic (iNodeIter);
			break;
			
		case kPrefCladeLabels_Series:
			return theTreeP->getNodeLabelSeries (iNodeIter);
			break;
	}
	
	// Postconditions:
	assert (false); // shouldn't get here!
	return (string ()); // just to shut compiler up	
}


// *** END ***************************************************************/



