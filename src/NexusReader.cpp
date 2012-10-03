/**************************************************************************
NexusReader.cpp - reads/loads data

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- This object is not meant to hang around - it is created, does
  its job and then vanishes.
- While we could make a direct call from the app like mReader->read(),
  the reading activity is hidden below so as to shield the rest of the
  model from the read/write process and (incidentally) from the NCL class
  library used to do the reading.

Changes:
- 99.7.13: Created.
- 00.10.10: adapted to use the NCL instead of the non-portable MELib code.

To Do:
- Still evolving in concept.

**************************************************************************/


// *** INCLUDES

#include "NexusReader.h"
#include "NclReader.h"
#include "NclBlocks.h"
#include "MesaUtils.h"
#include <sstream>
#include "distancesblock.h"
#include "nexusdefs.h"
#include "SblTypes.h"

using std::ifstream;
using std::stringstream;
using std::string;


// *** CONSTANTS & DEFINES

// *** MAIN BODY *********************************************************/

// *** LIFECYCLE *********************************************************/

// ctor in header

NexusReader::NexusReader (ifstream& iInStream, const progcallback_t& ikProgressCb)
{
	// Preconditions:
	assert (iInStream.is_open());
	
	// Main:
	// create ncl reader & fill with blocks to look for
	NclReader	theReader (ikProgressCb);

	AssumptionsBlock theAssumptions (*mTaxa);

	mTaxa = new MyTaxaBlock(ikProgressCb);
	theReader.Add (mTaxa);

	mTrees = new MyTreesBlock (*mTaxa, ikProgressCb);
	theReader.Add (mTrees);

	mCharacters = new MyCharactersBlock (*mTaxa, theAssumptions, ikProgressCb);
	theReader.Add (mCharacters);

	mData = new MyDataBlock (*mTaxa, theAssumptions, ikProgressCb);
	theReader.Add (mData);

	mCont = new MyContBlock (*mTaxa, theAssumptions, ikProgressCb);
	theReader.Add (mCont);

	// sic the reader onto the input stream
	theReader.read (iInStream, ikProgressCb);
	
	// Postconditions:
	assert (iInStream.is_open());
	iInStream.close();
}



NexusReader::~NexusReader ()
{
	delete mTaxa;
	delete mTrees;
	delete mCharacters;
	delete mData;
}


// *** SERVICES **********************************************************/

void NexusReader::getData (ContTraitMatrix& ioWrangler)
//: translate any continuous nexus data into the internal format
// Written this way so we can look at multiple blocks later.
{
	// pointer to the block to be translated
	MyCharactersBlock*		theBlockP = NULL;
	
	// if there's a continuous block
	if (isValidBlock (mCont))
	{
		theBlockP = (MyCharactersBlock*) mCont;
		mCont = NULL;
	}
	// otherwise look at character blocks
	else if (isValidBlock (mCharacters) and (mCharacters->GetDataType() ==
		CharactersBlock::continuous))
	{
		theBlockP = mCharacters;
		mCharacters = NULL;
	}
	// otherwise look at data blocks
	else if (isValidBlock (mData) and (mData->GetDataType() ==
		CharactersBlock::continuous))
	{
		theBlockP = (MyCharactersBlock*) mData;
		mData = NULL;
	}
	
	// if you found one, translate into internal format
	if (theBlockP != NULL)
	{
		mergeData (ioWrangler, theBlockP);
		ioWrangler.sortRows();
	}
}


void NexusReader::getData (DiscTraitMatrix& ioWrangler)
//: translate any discrete nexus data into the internal format
// Written this way so we can look at multiple blocks later.
{
	// pointer to the block to be translated
	MyCharactersBlock*		theBlockP = NULL;

	// look at character blocks
	if (isValidBlock (mCharacters) and (mCharacters->GetDataType() ==
		CharactersBlock::standard))
	{
		theBlockP = mCharacters;
		mCharacters = NULL;
	}
	// otherwise look at data blocks
	else if (isValidBlock (mData) and (mData->GetDataType() ==
		CharactersBlock::standard))
	{
		theBlockP = (MyCharactersBlock*) mData;
		mData = NULL;
	}
	
	// if you found one, translate into internal format
	if (theBlockP != NULL)
	{
		mergeData (ioWrangler, theBlockP);
		ioWrangler.sortRows();
	}
}


void NexusReader::mergeData
(ContTraitMatrix& ioWrangler, MyCharactersBlock* iNewBlockP)
//: merge contents of Nexus block into new block
// TO DO: cope with missing and gap data?
{
	// Preconditions:
	assert (iNewBlockP->GetDataType() == CharactersBlock::continuous);
		
	// Main:
	
	mMissingSymbol = iNewBlockP->GetMissingSymbol ();
	mGapSymbol = iNewBlockP->GetGapSymbol ();
	
	// grow matrix to right size
	uint theNumChar = iNewBlockP->GetNChar();
	uint theNumTaxa = iNewBlockP->GetNTax();
	ioWrangler.resize (theNumTaxa, theNumChar);
	
	// label all the rows with taxa names
	for (uint i = 0; i < theNumTaxa; i++)
	{
		nxsstring theTmpName = (iNewBlockP->mTaxaNames[i]).c_str();
		ioWrangler.setRowName (i, nexifyString(theTmpName.c_str()).c_str());
	}
		
	// fill the matrix with the old data
	for (uint i = 0; i < theNumTaxa; i++)
	{
		for (uint j = 0; j < theNumChar; j++)
			ioWrangler[i][j] = iNewBlockP->mContData[i][j];
	}
	
	// fill the label names
	for (uint i = 0; i < theNumChar; i++)
	{
		// can we find a name for the character label?
		string theLabel ("");
		
		// is there that many entries in the list?
		if (i < iNewBlockP->charLabels.size())
			theLabel = (iNewBlockP->charLabels[i]).c_str();
				
		// if the name is null, make up one
		if (theLabel.size() == 0)
		{
			theLabel = "ctrait_";
			theLabel += sbl::toString (i + 1);
		}	
		
		// set the name
		ioWrangler.setColName (i, theLabel.c_str());
	}	
	
	// Postconditions:
	for (uint i = 0; i < ioWrangler.countRows(); i++)
		assert (ioWrangler.getRowName (i) != "");
	for (uint i = 0; i < ioWrangler.countCols(); i++)
		assert (ioWrangler.getColName (i) != "");		
}


void NexusReader::mergeData
(DiscTraitMatrix& ioWrangler, MyCharactersBlock* iNewBlockP)
//: merge contents of Nexus block into new block
// TO DO: note we only cope with single states here
// TO DO: standardise the gap and unknown chars
{
	// Preconditions:
	assert (iNewBlockP->GetDataType() == CharactersBlock::standard);
		
	// Main:
	// grow matrix to right size
	uint theNumChar = iNewBlockP->GetNChar();
	uint theNumTaxa = iNewBlockP->GetNTax();
	ioWrangler.resize (theNumTaxa, theNumChar);
	
	// label all the rows with taxa names
	for (uint i = 0; i < theNumTaxa; i++)
	{
		nxsstring theTmpStr = iNewBlockP->taxa.GetTaxonLabel(i);
		ioWrangler.setRowName (i, nexifyString (theTmpStr.c_str()).c_str());
	}

	// get symbol table
	char* theSymbolCstr = iNewBlockP->GetSymbols();
	ulong theNumSymbols = strlen (theSymbolCstr);
	for (ulong i = 0; i < theNumSymbols; i++)
	{
		string theNewState (1, theSymbolCstr[i]);
		ioWrangler.mStates.addState (theNewState);
	}
	
	mMissingSymbol = iNewBlockP->GetMissingSymbol ();
	mGapSymbol = iNewBlockP->GetGapSymbol ();
	
	// fill the matrix with the old data
	for (uint i = 0; i < theNumTaxa; i++)
	{
		for (uint j = 0; j < theNumChar; j++)
		{
			// ioWrangler[i][j] = iNewBlockP->GetState (i, j);
			
			// TO DO: currently we only look at the first character
			// TO DO: need to translate to missing/gap?
			int k = iNewBlockP->charPos[j];
         if (k < 0)
         	continue;
         stringstream theOutStrm;
			iNewBlockP->ShowStateLabels (theOutStrm, i, k);
			
			ioWrangler[i][j] = translateSymbol (theOutStrm.str());

		}
	}

	
	// fill the label names
	for (uint i = 0; i < theNumChar; i++)
	{
		// can we find a name for the character label?
		string theLabel ("");
		
		// is there that many entries in the list?
		if (i < iNewBlockP->charLabels.size())
			theLabel = (iNewBlockP->charLabels[i]).c_str();
				
		// if the name is null, make up one
		if (theLabel.size() == 0)
		{
			theLabel = "dchar_";
			theLabel += sbl::toString (i + 1);
		}	
		
		// set the name
		ioWrangler.setColName (i, theLabel.c_str());
	}	
	
	// Postconditions:
	for (uint i = 0; i < ioWrangler.countRows(); i++)
		assert (ioWrangler.getRowName (i) != "");
	for (uint i = 0; i < ioWrangler.countCols(); i++)
		assert (ioWrangler.getColName (i) != "");		
}


void NexusReader::getData (TreeWrangler& ioWrangler)
//: translate any nexus tree data into the internal format
// Written this way so we can look at multiple blocks later.
{
	if (isValidBlock (mTrees)) 
	{
		MyTreesBlock::size_type theNumTrees = mTrees->countTrees();
		for (TreeWrangler::size_type i = 0; i < theNumTrees; i++)
			ioWrangler.addTree (mTrees->refTree (i));
	}
}	

const char* NexusReader::translateSymbol (std::string iCurrStr)
//: change local missing & gap symbols to ours
{
	if (iCurrStr == mMissingSymbol)
		return "?";
	else if (iCurrStr == mGapSymbol)
		return "-";
	else
		return iCurrStr.c_str();
}	
        
/*
MyCharactersBlock* NexusReader::adoptDiscreteData ()
//: returns a pointer to a block of discrete data
// Sends back data or characters block, depending on what they are. Sends
// back NULL if no suitable block available. The responsibility for the
// block is transferred with this call.
{
	// the pointer to be returned
	MyCharactersBlock*	theBlockP = NULL;
	
	// if there's a character block
	if ((mCharacters != NULL) and (mCharacters->GetDataType() ==
		CharactersBlock::standard))
	{
		theBlockP = mCharacters;
		mCharacters = NULL;
	}
	// if there's a data block
	else if ((mData != NULL) and (mData->GetDataType() ==
		CharactersBlock::standard))
	{
		theBlockP = (MyCharactersBlock*) mData;
		mData = NULL;
	}
	
	return theBlockP;
}


MyCharactersBlock* NexusReader::adoptContinuousData ()
//: returns a pointer to a block of continuous data
// See notes for adoptDiscreteData().
{
	// the pointer to be returned
	MyCharactersBlock*	theBlockP = NULL;
	
	// if there's a continuous block
	if (mCont != NULL)
	{
		theBlockP = mCont;
		mCharacters = NULL;
	}
	// if there's a character block
	else if ((mCharacters != NULL) and (mCharacters->GetDataType() ==
		CharactersBlock::continuous))
	{
		theBlockP = mCharacters;
		mCharacters = NULL;
	}
	// if there's a data block
	else if ((mData != NULL) and (mData->GetDataType() ==
		CharactersBlock::continuous))
	{
		theBlockP = (MyCharactersBlock*) mData;
		mData = NULL;
	}
	
	return theBlockP;
}

MyTaxaBlock* NexusReader::adoptTaxa ()
//: returns a pointer to a taxablock
// See notes for adoptDiscreteData().
{
	// the pointer to be returned
	MyTaxaBlock*	theBlockP = NULL;

	// if there's a taxa block
	if (mTaxa != NULL)
	{
		theBlockP = mTaxa;
		mTaxa = NULL;
	}
	
	return theBlockP;
}

MyTreesBlock* NexusReader::adoptTrees ()
//: returns a pointer to a treeblock
// See notes for adoptDiscreteData().
{
	// the pointer to be returned
	MyTreesBlock*	theBlockP = NULL;

	// if there's a taxa block
	if (mTrees != NULL)
	{
		theBlockP = mTrees;
		mTrees = NULL;
	}
	
	return theBlockP;
}

*/


// *** INTERNALS *********************************************************/


// *** DEPRECATED & TEST FUNCTIONS **************************************/


void NexusReader::validate ()
{
//	cout << "*** Testing NexusReader class" << endl;
	
//	cout << "*** Finished testing NexusReader class" << endl;
}

// *** END ***************************************************************/

