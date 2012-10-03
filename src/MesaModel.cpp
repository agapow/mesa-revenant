/**************************************************************************
MesaModel.cpp - the Mesa domain model and data structures

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- Ideally this should be completely divorcable from any interface or
  application loop so as to be portable to other architectures.
- This is what the model should include:
  - the tree and related data
  - the task queue
  - the transformation functions
  - the history
  - the scripting interface
  - the settings/prefs

Changes:
- 99.7.13: Created.
- 00.10.10: adapted to use the NCL instead of the non-portable MELib code.

To Do:
- Still evolving in concept.

**************************************************************************/


// *** INCLUDES


#include "MesaModel.h"
#include "StringUtils.h"
#include "TabDataReader.h"
#include "Error.h"
#include "StreamScanner.h"
#include "RichnessReader.h"
#include "NexusReader.h"
#include "CaicReader.h"
#include "NexusWriter.h"
#include "CaicWriter.h"
#include "SblTypes.h"
#include <algorithm>

using std::string;
using std::ifstream;
using std::ostream;
using std::endl;
using std::sort;
using std::unique;
using sbl::hasExtension;
using sbl::ReplaceExtension;
using sbl::toUpper;
using sbl::kStr_NoLimit;




// *** CONSTANTS & DEFINES

// *** MAIN BODY *********************************************************/


// *** LIFECYCLE *********************************************************/

// *** ACCESSORS *********************************************************/

int MesaModel::countTrees ()
{
	return int (mTreeData.size());
}


int MesaModel::countDiscTraits ()
{
	return int (mDiscData.countChars());
}


int MesaModel::countContTraits ()
{
	return int (mContData.countChars());
}

/** 
Return the number of taxa (rows) in the data matrices.

@todo   Is it guaranteed that both matrices have the same number of rows?
*/
int MesaModel::countTaxa ()
{
	return mContData.countTaxa();
}


int MesaModel::countSiteTraits ()
{
	colIndex_t theNumContTraits = countContTraits();
	int theNumSites = 0;
	for (colIndex_t i = 0; i < theNumContTraits; i++)
	{
		if (isSiteTrait (i))
			theNumSites++;
	}
	return theNumSites;
}

bool MesaModel::isSiteTrait (colIndex_t iIndex)
{
	// Preconditions:
	assert (0 <= iIndex);
	assert (iIndex < countContTraits());
	
	// Main: 
	std::string theTraitName = getTraitName (kTraittype_Continuous, iIndex);
	return (theTraitName.compare (0, 4, "site") == 0);
}


std::string MesaModel::getSiteName (int iIndex)
{
	// Preconditions:
	assert (isSiteTrait (iIndex));
	
	// Main: 
	std::string theTraitName = getTraitName (kTraittype_Continuous, iIndex);
	return (string (theTraitName.begin() + 5, theTraitName.end()));
}
	
	
std::string MesaModel::getTraitName (traittype_t iDataType, int iIndex)
{
	if (iDataType == kTraittype_Discrete)
	{
		 return mDiscData.getColName (iIndex);
	}
	else if (iDataType == kTraittype_Continuous)
	{
		 return mContData.getColName (iIndex);
	}
	else
	{
		assert (false);       // should never get here
		return string ("");   // just to keep compiler quiet
	}
}
	

// *** MUTATORS **********************************************************/


void MesaModel::seedTree ()
//: create a new tree, with just a root
{
	mTreeData.seedTree ();
}


void MesaModel::selectTree (int iTreeIndex)
{
	mTreeData.setActiveTreeIndex ((TreeWrangler::size_type) iTreeIndex);
//	MesaGlobals::mActiveTreeP = mTreeData.getActiveTreeP();
}


void MesaModel::deleteTree (int iTreeIndex)
{
	mTreeData.deleteTree (iTreeIndex);
//	MesaGlobals::mActiveTreeP = mTreeData.getActiveTreeP();
}


void MesaModel::duplicateActiveTree ()
{
	mTreeData.duplicateActiveTree();
}


void MesaModel::deleteChar (traittype_t iDataType, int iIndex)
{
	if (iDataType == kTraittype_Discrete)
	{
		 mDiscData.deleteCol (iIndex);
	}
	else if (iDataType == kTraittype_Continuous)
	{
		 mContData.deleteCol (iIndex);
	}
	else
	{
		assert (false);
	}
}
	
	
void MesaModel::newContChar (double iNewVal)
//: add a new column of continuous characters with this value
{
	if (countContTraits() != 0)
	{
		mContData.addCols (1, iNewVal);
	}
	else 
	{
		// have to get names from trees
		stringvec_t theTaxaNames;
		for (ulong i = 0; i < mTreeData.size(); i++)
		{
			mTreeData[i].getTaxaNames (theTaxaNames);
		}
		unique (theTaxaNames.begin(), theTaxaNames.end());
		mContData.resize (theTaxaNames.size(), 1, iNewVal);
		mContData.setRowNames (theTaxaNames);
	}
}


void MesaModel::newDiscChar (const char* iNewVal)
//: add a new column of discrete characters with this value
{
	if (countDiscTraits() != 0)
	{
		mDiscData.addCols (1, iNewVal);
	}
	else 
	{
		// have to get names from trees
		stringvec_t theTaxaNames;
		for (ulong i = 0; i < mTreeData.size(); i++)
		{
			mTreeData[i].getTaxaNames (theTaxaNames);
		}
		unique (theTaxaNames.begin(), theTaxaNames.end());
		mDiscData.resize (theTaxaNames.size(), 1, iNewVal);
		mDiscData.setRowNames (theTaxaNames);
	}
}

/**
Store a pristine copy of all the mutatable data.

@todo   Convert to using pointers so as to use less memory.
*/
void MesaModel::backupData ()
{
	mBackupTreeData = mTreeData;
	mBackupContData = mContData;
	mBackupDiscData = mDiscData;
}


void MesaModel::restoreData ()
{
	 mTreeData = mBackupTreeData;
	 mContData = mBackupContData;
	 mDiscData = mBackupDiscData;
}


// *** I/O ***************************************************************/


fileformat_t MesaModel::sniffFormat (std::ifstream& iInStream)
{
	// grab the first line of the file
	sbl::StreamScanner theScanner (iInStream);
	string theInLine;
	theScanner.ReadLine (theInLine);	
	sbl::eraseTrailingSpace (theInLine);						
	sbl::toUpper (theInLine);
	theScanner.Rewind();
		
	if (theInLine == "#NEXUS")
		return kFileFormat_Nexus;
	else if (theInLine == "#PAG")
		return kFileFormat_Pag;
	else
		return kFileFormat_Unknown;
}


void MesaModel::readModel (std::ifstream& iInStream,  string& ikFileName)
//: given an open stream, initialise the model with its contents
{
	// Preconditions:
	assert (iInStream.is_open());
	
	// Main:
	// first try to work out file format from contents
	switch (sniffFormat (iInStream))
	{
		case kFileFormat_Nexus:
		{
			readNexus (iInStream);
		}
		break;
		
		// otherwise work it out from extension
		default:
		{
			// convert filename to upper
			string theFileName (ikFileName);
			sbl::toUpper (theFileName);
			
			// see what file name ends with
			if (hasExtension (theFileName, "PAG"))
			{
				throw sbl::FormatError ("can't read PAG files");
			}
			else if (hasExtension (theFileName, "BLEN") or 
				hasExtension (theFileName, "PHYL"))
			{
				// throw sbl::FormatError ("can't read CAIC files");
				readCaic (iInStream, ikFileName);
			}
			else if (hasExtension (theFileName, "NEX") or
				hasExtension (theFileName, "NXS") or
				hasExtension (theFileName, "NEXUS") or
				hasExtension (theFileName, "PAUP"))
			{
				readNexus (iInStream);
			}
			else
			{
				throw sbl::FormatError ("unrecognised file format");
			}
		}
		break;
	} 
}


void MesaModel::readCaic (std::ifstream& iInStream,  const string& ikFileName)
{
	ifstream theOtherFStream;
	ifstream *thePhylStreamP, *theBlenStreamP;
	
	// convert filename to upper just in case
	string theFileName (ikFileName);
	sbl::toUpper (theFileName);
	
	// first find the other file
	if (hasExtension (theFileName, "BLEN"))
	{
		// find "phyl" file
		string thePhylName (ikFileName);
		ReplaceExtension (thePhylName, "Phyl", kStr_NoLimit);
		theOtherFStream.open (thePhylName.c_str());
		if (not theOtherFStream)
		{
			// just in event of case fuckups
			ReplaceExtension (thePhylName, "phyl", kStr_NoLimit);
			theOtherFStream.open (thePhylName.c_str());
		}
		if (not theOtherFStream)
		{
			// just in event of case fuckups
			ReplaceExtension (thePhylName, "PHYL", kStr_NoLimit);
			theOtherFStream.open (thePhylName.c_str());
		}		
		if (not theOtherFStream)
			throw sbl::MissingFileError ("couldn't open phylogeny file");
		else
		{
			theBlenStreamP = &iInStream;
			thePhylStreamP = &theOtherFStream;
		}
	}
	else
	{
		// find "blen" file
		string theBlenName (ikFileName);
		ReplaceExtension (theBlenName, "BLen", kStr_NoLimit);
		theOtherFStream.open (theBlenName.c_str());
		if (not theOtherFStream)
		{
			// just in event of case fuckups
			ReplaceExtension (theBlenName, "blen", kStr_NoLimit);
			theOtherFStream.open (theBlenName.c_str());
		}
		if (not theOtherFStream)
		{
			// just in event of case fuckups
			ReplaceExtension (theBlenName, "Blen", kStr_NoLimit);
			theOtherFStream.open (theBlenName.c_str());
		}

		if (not theOtherFStream)
			throw sbl::MissingFileError ("couldn't open branchlength file");
		else
		{
			thePhylStreamP = &iInStream;
			theBlenStreamP = &theOtherFStream;
		}
	}
	
	// should now have two streams for reading data from
	CaicReader	theReader (thePhylStreamP, theBlenStreamP, mProgressCb);

	//theReader.getData (mDiscData);
	//theReader.getData (mContData);
	theReader.getData (mTreeData);
}


void MesaModel::readNexus (std::ifstream& iInStream)
{
	// create the reader & let it parse the stream contents
	NexusReader	theReader (iInStream, mProgressCb);

	theReader.getData (mDiscData);
	theReader.getData (mContData);
	theReader.getData (mTreeData);
}



void MesaModel::writeCaic
(std::ostream* iPhylStrm, std::ostream* iBlenStrm, std::ostream* iDataStrm)
{
	CaicWriter	theWriter (iPhylStrm, iBlenStrm, iDataStrm);
	// write the tree data
	theWriter.writeTrees (mTreeData);
	// if there is some data, write it
	if (mContData.countTaxa() or mDiscData.countTaxa())
	{
		theWriter.writeData (mContData, mDiscData);
	}
}

void MesaModel::writeNexus (std::ofstream& iOutStream)
//: given an open stream, spew the contents of the model to it
// CHANGE (01.4.5): need a taxa block. Bunner.
{
	// Preconditions:
	assert (iOutStream.is_open());
	
	// Main:
	// establish writer & do output
	NexusWriter	theWriter (iOutStream);
	
	if (MesaGlobals::mPrefs.mWriteTaxaBlock)
	{
		stringvec_t theRowNames;
		mDiscData.collectRowNames (theRowNames);
		sort (theRowNames.begin(), theRowNames.end());
		theWriter.writeTaxa (theRowNames);
	}
	
	theWriter.writeDiscData (mDiscData);
	theWriter.writeContData (mContData);
	theWriter.writeTrees (mTreeData, MesaGlobals::mPrefs.mWriteTransCmd);
	
	// TO DO: sim writes itself out
	
	// Postconditions:
	// NOTE: stream closes on writer destruction
	assert (iOutStream.is_open());	
}


void MesaModel::importTab (ifstream& iInStream)
//: imports a tab-delimited file into the existing data
{
	// Preconditions:
	assert (iInStream.is_open());	
	
	// Main:
	// create the reader & let it parse the stream contents
	TabDataReader	theReader (iInStream, mProgressCb);
	theReader.read();
	
	theReader.getData (mDiscData);
	theReader.getData (mContData);
	
	// Postconditions:
	// NOTE: stream closes on reader destruction
	assert (iInStream.is_open());	
}


void MesaModel::importRich (ifstream& iInStream)
//: imports a tab-delimited spp richness data
{
	// Preconditions:
	assert (iInStream.is_open());	
	
	// Main:
	// create the reader & let it parse the stream contents
	RichnessReader	theReader (iInStream, mProgressCb);
	theReader.read();
	
	theReader.getRichnessData (mContData);
	
	// Postconditions:
	// NOTE: stream closes on reader destruction
	assert (iInStream.is_open());	
}


void MesaModel::exportData
(ostream* iOutStream, traittype_t theDataType, nodetype_t theTipType)
{
	MesaTree* theActiveTree = mTreeData.getActiveTreeP(); 
	(*iOutStream) << "# exported trait data" << endl;
	
	if ((theDataType == kTraittype_Both) or (theDataType == kTraittype_Discrete))
	{
		// export discrete data
		(*iOutStream) << "# exported discrete data" << endl;
		DiscTraitMatrix::size_type theNumTraits = mDiscData.countChars();
		DiscTraitMatrix::size_type theNumTaxa = mDiscData.countTaxa();
		DiscTraitMatrix::size_type i, j;

		for (i = 0; i < theNumTaxa; i++)
		{
			string theTaxaName = mDiscData.getRowName (i);
			nodeiter_t theNode = theActiveTree->getIter (theTaxaName.c_str());
			
			if ((theTipType == kNodetype_Tips) and (not theActiveTree->isLeaf (theNode)))
				continue;
				
			if ((theTipType == kNodetype_Living) and (not theActiveTree->isNodeAlive (theNode)))
				continue;
				
			(*iOutStream) << theTaxaName;
			for (j = 0; j < theNumTraits; j++)
			{
				(*iOutStream) << "\t" << mDiscData.at (i, j);
			}
			(*iOutStream) << endl;
		}
	}
	if ((theDataType == kTraittype_Both) or (theDataType == kTraittype_Continuous))
	{
		// export continuous data
		(*iOutStream) << "# exported continuous data" << endl;
		ContTraitMatrix::size_type theNumTraits = mContData.countChars();
		ContTraitMatrix::size_type theNumTaxa = mContData.countTaxa();
		ContTraitMatrix::size_type i, j;

		for (i = 0; i < theNumTaxa; i++)
		{
			string theTaxaName = mContData.getRowName (i);
			nodeiter_t theNode = theActiveTree->getIter (theTaxaName.c_str());
			
			if ((theTipType == kNodetype_Tips) and (not theActiveTree->isLeaf (theNode)))
				continue;
				
			if ((theTipType == kNodetype_Living) and (not theActiveTree->isNodeAlive (theNode)))
				continue;
				
			(*iOutStream) << theTaxaName;
			for (j = 0; j < theNumTraits; j++)
			{
				(*iOutStream) << "\t" << mContData.at (i, j);
			}
			(*iOutStream) << endl;
		}
	}

}			



// *** REPORTING *********************************************************/


void MesaModel::summarise (ostream& ioOutStream)
{
	mDiscData.summarise (ioOutStream);
	mContData.summarise (ioOutStream);
	mTreeData.summarise (ioOutStream);
	mActionQueue.summarise (ioOutStream);
}


void MesaModel::detailedReport (ostream& ioOutStream)
{
	mDiscData.detailedReport (ioOutStream);
	mContData.detailedReport (ioOutStream);
	mTreeData.detailedReport (ioOutStream);
	mActionQueue.detailedReport (&ioOutStream);
}



// *** DEPRECATED & TEST FUNCTIONS **************************************/


void MesaModel::validate ()
{
	mTreeData.validate();
	mContData.validate();
	mDiscData.validate();
}



// *** END ***************************************************************/

