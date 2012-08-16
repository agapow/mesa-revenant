/**************************************************************************
NexusReader.h - loads/reads data from stream

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef NEXUSREADER_H
#define NEXUSREADER_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include "TaxaTraitMatrix.h"
#include "TreeWrangler.h"
#include <fstream>
#include <string>


// *** CONSTANTS & DEFINES
class MyTaxaBlock;
class AssumptionsBlock;
class MyTreesBlock;
class MyAllelesBlock;
class MyCharactersBlock;
class MyDataBlock;
class MyContBlock;
class DistancesBlock;


// *** CLASS DECLARATION *************************************************/

class NexusReader
{
public:
	// LIFECYCLE
	NexusReader (std::ifstream& iInStream, const progcallback_t&
		ikProgressCb);
		
	~NexusReader ();
	
	// SERVICES	
	void	getData (DiscTraitMatrix& ioWrangler);
	void	getData (ContTraitMatrix& ioWrangler);
	void	getData (TreeWrangler& ioWrangler);
	
	void	mergeData (DiscTraitMatrix& ioWrangler, MyCharactersBlock* iNewBlockP);
	void	mergeData (ContTraitMatrix& ioWrangler, MyCharactersBlock* iNewBlockP);
	
	/*
	MyCharactersBlock*	adoptDiscreteData ();
	MyCharactersBlock*	adoptContinuousData ();
	MyTaxaBlock* 			adoptTaxa ();
	MyTreesBlock* 			adoptTrees ();
	*/
	
	// DEPRECIATED & DEBUG
	void	validate	();

	// INTERNALS
private:
	MyTaxaBlock*			mTaxa;
	MyTreesBlock* 			mTrees;
	MyCharactersBlock* 	mCharacters;
	MyDataBlock* 			mData;
	MyContBlock* 			mCont;
	
	std::string mMissingSymbol;
	std::string mGapSymbol;

	const char* translateSymbol (std::string iCurrStr);
	
	template <class NEXUSBLOCKPTR>
	bool isValidBlock (NEXUSBLOCKPTR iTestBlockP)
	//: tests block for to see if it exists, and if it is empty
	{
		if (iTestBlockP == NULL)
			return false;
		if (iTestBlockP->IsEmpty())
			return false;
		return true;
	}
};


#endif
// *** END ***************************************************************/



