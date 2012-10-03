/**************************************************************************
SystemAction.cpp - an action that acts on the data system rather than within

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

**************************************************************************/


// *** INCLUDES

#include "Sbl.h"
#include "SystemAction.h"
#include "TreeWrangler.h"
#include "ActionUtils.h"
#include "StringUtils.h"
#include "MesaUtils.h"
#include "NexusWriter.h"
#include "CaicWriter.h"
#include "TaxaTraitMatrix.h"
#include <sstream>

using std::string;
using std::stringstream;
using sbl::toString;


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

void SystemAction::execute ()
{
	executeSystem ();
}


// *** DUPLICATE TREE SYS ACTION *****************************************/

void DupTreeSysAction::executeSystem ()
{
	MesaGlobals::mTreeDataP->duplicateActiveTree();
}
	

const char* DupTreeSysAction::describeSysAction ()
{
	return "duplicate active tree";
}
	


// *** SAVE SYS ACTION ***************************************************/

void SaveSysAction::executeSystem ()
// TO DO: change so this calls a write file fxn in model
{
	if (mFileType == kSaveFile_Nexus)
	{
		// build file name & establish file stream
		string theFileName = concatIntToString (mBaseFileName.c_str(),
			++mReps);
		theFileName += ".nex";
		std::ofstream theOutFile (theFileName.c_str());
		assert (theOutFile.is_open());
		
		// establish writer & do output
		NexusWriter	theWriter (theOutFile);
		theWriter.writeDiscData (*(MesaGlobals::mDiscDataP));
		theWriter.writeContData (*(MesaGlobals::mContDataP));
		theWriter.writeTrees (*(MesaGlobals::mTreeDataP), true);

		// tidy up & close
		theOutFile.close();
	}
	else if (mFileType == kSaveFile_Caic)
	{
		// build file names
		string theFileName = concatIntToString (mBaseFileName.c_str(),
			++mReps);
		
		// is there data
		bool theDataIsPresent = (MesaGlobals::mContDataP->countTaxa() or
			MesaGlobals::mDiscDataP->countTaxa());
			
		// establish streams
		std::ofstream thePhylFile ((theFileName + kCaicPhylogenyFileSuffix).c_str());
		std::ofstream theBlenFile ((theFileName + kCaicBranchFileSuffix).c_str());
		std::ofstream theDataFile;
		if (theDataIsPresent)
			theDataFile.open ((theFileName + kCaicDataFileSuffix).c_str());
		
		assert (thePhylFile.is_open());
		assert (theBlenFile.is_open());
		
		// establish writer & do output
		CaicWriter	theWriter (&thePhylFile, &theBlenFile, &theDataFile);
		theWriter.writeTrees (*(MesaGlobals::mTreeDataP));

		// tidy up & close
		thePhylFile.close();	
		theBlenFile.close();	

		if (theDataIsPresent)
		{
			assert (theDataFile.is_open());
			theWriter.writeData (*(MesaGlobals::mContDataP), *(MesaGlobals::mDiscDataP));
			theDataFile.close();	
		}

	}
	else
	{
		assert (false); // should never get here
	}
}

const char* SaveSysAction::describeSysAction ()
{
	static string theDescStr;
	theDescStr = "save data as ";
	switch (mFileType)
	{
		case kSaveFile_Nexus:
			theDescStr += "nexus";
			break;
			
		case kSaveFile_Caic:
			theDescStr += "caic";
			break;
			
		default:
			assert (false); // shouldn't get here
	}
		
	theDescStr += " \'";
	theDescStr += mBaseFileName;
	theDescStr += "#\'";
	
	return theDescStr.c_str();
}
	


// *** PRESERVE TAXA ACTION **********************************************/

void PreserveTaxaSysAction::executeSystem ()
{
	MesaGlobals::mPrefs.mPreserveNodes = mSetting;
}


const char* PreserveTaxaSysAction::describeSysAction ()
{
	static string theDescStrm;
	theDescStrm = "set taxa preservation to ";
	string theLabelStr;
	switch (mSetting)
	{
		case kPrefPreserveNodes_None:
			theLabelStr = "none";
			break;
			
		case kPrefPreserveNodes_Root:
			theLabelStr = "the root";
			break;

		case kPrefPreserveNodes_RootChildren:
			theLabelStr = "the root & root's children";
			break;
			
		default:
			DBG_MSG ("Assert failed with case " << mSetting);
			assert (false);
	}			
	theDescStrm += theLabelStr;
	return theDescStrm.c_str();
}



// *** SHUFFLE TRAITS ACTION **********************************************/

void ShuffleTraitsSysAction::executeSystem ()
{
	if (mTraitType == kTraittype_Discrete)
		MesaGlobals::mDiscDataP->shuffleTrait (mTraitIndex);
	else if (mTraitType == kTraittype_Continuous)
		MesaGlobals::mContDataP->shuffleTrait (mTraitIndex);
	else if (mTraitType == kTraittype_All)
	{
		MesaGlobals::mDiscDataP->shuffleAllTraits ();
		MesaGlobals::mContDataP->shuffleAllTraits ();	
	}
	else
		assert (false);
}


const char* ShuffleTraitsSysAction::describeSysAction ()
{
	static string theDescStrm;
	theDescStrm = "shuffle ";
	if (mTraitType == kTraittype_All)
	{
		theDescStrm += "all traits";
	}
	else
	{
		if (mTraitType == kTraittype_Discrete)
			theDescStrm += "discrete trait ";
		else if (mTraitType == kTraittype_Continuous)
			theDescStrm += "continuous trait ";
		theDescStrm += sbl::toString (mTraitIndex);	
	}
	theDescStrm += " across all taxa";
	return theDescStrm.c_str();
}




// *** SET LABELS ACTION *************************************************/

void SetLabelsSysAction::executeSystem ()
{
	MesaGlobals::mPrefs.mCladeLabels = mLabelType;
}


const char* SetLabelsSysAction::describeSysAction ()
{
	static string theDescStrm;
	theDescStrm = "set clade label format to ";
	string theLabelStr;
	switch (mLabelType)
	{
		case kPrefCladeLabels_Phylo:
			theLabelStr = "phylogenetic";
			break;
			
		case kPrefCladeLabels_Caic:
			theLabelStr = "Caic";
			break;

		case kPrefCladeLabels_Series:
			theLabelStr = "numeric series";
			break;
			
		default:
			DBG_MSG ("Assert failed with case " << mLabelType);
			assert (false);
	}			
	theDescStrm += theLabelStr;
	return theDescStrm.c_str();
}



// *** SET TREE LENGTH ACTION ********************************************/

void SetTreeLengthSysAction::executeSystem ()
{
	// Main:
	MesaTree* theTreeP = getActiveTreeP();
	MesaTree::weight_type theOldLen; 
	MesaTree::weight_type theProvisionalLen; 
	for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		switch (mChangeType)
		{
			case 	kTreeLenChange_Set:
				assert (0.0 <= mNewLength);
				theTreeP->setEdgeWeight (q, mNewLength);
				break;
				
			case 	kTreeLenChange_Add:
				theOldLen = theTreeP->getEdgeWeight (q);
				theProvisionalLen = mNewLength + theOldLen;
				theTreeP->setEdgeWeight (q, std::max (0.0, mNewLength + theOldLen));				
				break;
				
			case 	kTreeLenChange_Multiply:
				assert (0.0 <= mNewLength);
				theOldLen = theTreeP->getEdgeWeight (q);
				theTreeP->setEdgeWeight (q, mNewLength * theOldLen);				
				break;
				
			case 	kTreeLenChange_RandomFixed:
				assert (0.0 <= mNewLength);
				theOldLen = theTreeP->getEdgeWeight (q);
				theProvisionalLen = theOldLen + MesaGlobals::mRng.gaussian (0.0, mNewLength);
				theTreeP->setEdgeWeight (q, std::max (0.0, theProvisionalLen));				
				break;
				
			case 	kTreeLenChange_RandomFraction:
				assert (0.0 <= mNewLength);
				theOldLen = theTreeP->getEdgeWeight (q);
				theProvisionalLen = theOldLen + MesaGlobals::mRng.gaussian (0.0, mNewLength * theOldLen);
				theTreeP->setEdgeWeight (q, std::max (0.0, theProvisionalLen));				
				break;
				
			default:
				assert (false); // should never get here
		}
	}
}


const char* SetTreeLengthSysAction::describeSysAction ()
{
	static string theDescStrm;
	switch (mChangeType)
	{
		case kTreeLenChange_Set:
			theDescStrm = "set all branchlengths to ";
			break;
			
		case kTreeLenChange_Add:
			theDescStrm = "increase all branchlengths by ";
			break;
			
		case kTreeLenChange_Multiply:
			theDescStrm = "multiply all branchlengths by ";
			break;

		case kTreeLenChange_RandomFixed:
			theDescStrm = "randomize all branchlengths by ";
			break;

		case kTreeLenChange_RandomFraction:
			theDescStrm = "randomize all branchlengths by fraction ";
			break;
			
		default:
			assert (false); // should never get here
	}
	theDescStrm += toString (mNewLength);
	return theDescStrm.c_str();
}



// *** SET TREE LENGTH ACTION ********************************************/

void ConsolidateTaxaSysAction::executeSystem ()
//: if there are no data, then set all taxa names to the same
{
	// Preconditions:
	// should be more than 1 tree and no data
	
	// Main:
	int theNumTrees = MesaGlobals::mTreeDataP->size();
	int theOldActiveIndex = (int) MesaGlobals::mTreeDataP->getActiveTreeIndex ();
	
	// get names from active tree
	stringvec_t theTaxaNames;
	MesaTree* theTreeP = getActiveTreeP();
	theTreeP->getTaxaNames (theTaxaNames);
	TreeWrangler* theTreeDataP = MesaGlobals::mTreeDataP;
	
	// for every tree ...
	TreeWrangler::iterator theTreeIter = theTreeDataP->begin();
	for (; theTreeIter != theTreeDataP->begin(); theTreeIter++)
	{
		assert (theTaxaNames.size() == theTreeIter->countLeaves());

		// if it's not the active one
		if (theTreeIter != theTreeDataP->getActiveTreeIter())
		{
			nodeiter_t theNodeIter = theTreeIter->begin();
			int i = 0;
			for (; theNodeIter != theTreeIter->end(); theNodeIter++)
			{
				// set the taxa names to those stored
				if (theTreeIter->isLeaf (theNodeIter))
				{
					theTreeIter->setNodeName (theNodeIter, theTaxaNames[i]);
					i++;
				}
			}
		}
	}

	MesaGlobals::mTreeDataP->setActiveTreeIndex (theOldActiveIndex);
}


const char* ConsolidateTaxaSysAction::describeSysAction ()
{
	return "consolidate taxa names";
}
	


// *** DELETE DEAD TAXA ACTION *******************************************/

void DeleteDeadTaxaSysAction::executeSystem ()
//: reduce tree to point where all tips are living
{
	// Preconditions:
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);
	theTreeP->validate();
	
	// Main:

/*
	while (theTreeP->countLeaves () != theTreeP->countAliveLeaves ())
	{
		for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
		{
			i++;
			DBG_MSG ("loop #" << i);
			nodeiter_t theCurrNode = q;
			q++;
			if (theTreeP->isLeaf (theCurrNode) and 
			    (not theTreeP->isNodeAlive (theCurrNode)))
			{
				theTreeP->pruneLeaf (theCurrNode);
				theTreeP->validate();
				break;
			}
		}
	}
*/
	while (theTreeP->countLeaves () != theTreeP->countAliveLeaves ())
	{
		for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end();)
		{
			nodeiter_t theCurrNode = q;
			q++;
			if (theTreeP->isLeaf (theCurrNode) and 
			    (not theTreeP->isNodeAlive (theCurrNode)))
			{
				theTreeP->pruneLeaf (theCurrNode);
				theTreeP->validate();
			}
		}
	}


	// Postconditions:
	assert (theTreeP->countLeaves () == theTreeP->countAliveLeaves ());
}


const char* DeleteDeadTaxaSysAction::describeSysAction ()
{
	return "delete dead leaves";
}
	

void DeleteDeadTraitsSysAction::executeSystem ()
//: get rid of the chracter values for dead taxa
{
	// Preconditions:
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);
	theTreeP->validate();
	
	// Main:
	
	// get row names of disc data
	stringvec_t theDiscTaxaNames;
	MesaGlobals::mDiscDataP->collectRowNames (theDiscTaxaNames);
	// for each name
	stringvec_t::iterator p;
	for (p = theDiscTaxaNames.begin(); p != theDiscTaxaNames.end(); p++)
	{
		// get the node for that name
		nodeiter_t theCurrNode = theTreeP->getIter (p->c_str());
		// if it's not alive
		if (not theTreeP->isNodeAlive (theCurrNode))
			// delete the data
			MesaGlobals::mDiscDataP->deleteRow (p->c_str());
	}

	// get row names of cont data
	stringvec_t theContTaxaNames;
	MesaGlobals::mContDataP->collectRowNames (theContTaxaNames);
	// for each name
	for (p = theContTaxaNames.begin(); p != theContTaxaNames.end(); p++)
	{
		// get the node for that name
		nodeiter_t theCurrNode = theTreeP->getIter (p->c_str());
		// if it's not alive
		if (not theTreeP->isNodeAlive (theCurrNode))
			// delete the data
			MesaGlobals::mContDataP->deleteRow (p->c_str());
	}


	// Postconditions:
}


const char* DeleteDeadTraitsSysAction::describeSysAction ()
{
	return "delete trait values for dead taxa";
}


// *** COLLAPSE SINGLETONS ACTION ****************************************/

void CollapseSingletonsSysAction::executeSystem ()
//: collapse all singleton nodes
{
	// Preconditions:
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);
	
	// Main:
	while (theTreeP->hasTreeSingletons ())
	{
		for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end();)
		{
			nodeiter_t theCurrNode = q;
			q++;
			if (theTreeP->isNodeSingleton (theCurrNode))
				theTreeP->collapseNode (theTreeP->getChild (theCurrNode, 0));
		}
	}

	// Postconditions:
	theTreeP->validate();
	assert (not theTreeP->hasTreeSingletons ());
}


const char* CollapseSingletonsSysAction::describeSysAction ()
{
	return "collapse singletons nodes";
}
	


// *** MAKE NEONTOLOGICAL ACTION *****************************************/

void MakeNeontSysAction::executeSystem ()
//: make neontological
{
	DeleteDeadTaxaSysAction      theDeleteAction;
	CollapseSingletonsSysAction  theCollapseAction;
	
	theDeleteAction.execute();
	theCollapseAction.execute();
}


const char* MakeNeontSysAction::describeSysAction ()
{
	return "reduce tree to neontological core";
}
	


// *** END ***************************************************************/



