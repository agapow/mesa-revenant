/**************************************************************************
Prune.cpp - trim the active tree by pruning taxa away

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2001, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

**************************************************************************/


// *** INCLUDES

#include "Sbl.h"
#include "Prune.h"
#include "Test.h"
#include "TreeWrangler.h"
#include "MesaGlobals.h"
#include "TreeWrangler.h"
#include "StringUtils.h"


// *** CONSTANTS & DEFINES

// *** BASIC PRUNE ******************************************************/

std::string BasicPrune::describe ()
//: return a description of the analyse
{
	std::string theBuffer ("prune: ");
	theBuffer += initDesc ();
	return theBuffer;
}
	

// *** PRUNE N ***********************************************************/

void PruneN::execute ()
{
	int theAnswer = (int) MesaGlobals::mTreeDataP->countTaxa();
}
	
	
std::string PruneN::initDesc ()
{
	std::string theBuffer ("kill ");
	theBuffer += sbl::toString (mBodyCount);
	theBuffer += "randomly selected taxa";
	return theBuffer;
}


// *** PRUNE FRACTION ****************************************************/

void PruneFraction::execute ()
{
	int theAnswer = (int) MesaGlobals::mTreeDataP->countTaxa();
}


std::string PruneFraction::initDesc ()
{
	std::string theBuffer ("kill ");
	theBuffer += sbl::toString (mKillFraction * 100);
	theBuffer += "% of taxa";
	return theBuffer;
}


// *** PRUNE CHANCE ******************************************************/

void PruneChance::execute ()
{
	int theAnswer = (int) MesaGlobals::mTreeDataP->countTaxa();
}
	

std::string PruneChance::initDesc ()
{
	std::string theBuffer (sbl::toString (mKillChance * 100));
	theBuffer += "% chance of killing any taxa";
	return theBuffer;
}


// *** PRUNE CHARACTER ***************************************************/

PruneCharacter::~PruneCharacter ()
{
	delete mTestP;
}
	
	
void PruneCharacter::execute ()
{
	int theAnswer = (int) MesaGlobals::mTreeDataP->countTaxa();
}
	

std::string PruneCharacter::initDesc ()
{
	std::string theBuffer ("fgdfg");
	return theBuffer;
}


// *** END ***************************************************************/



