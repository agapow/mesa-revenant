/**************************************************************************
AnalysisAction.h - an action that calculates some result and returns it

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2001, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "Action.h"


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class Analysis: public BasicAction
//: an action that runs some calculation over the data
{
public:
	// PUBLIC_TYPE INTERFACE
	
	// LIFECYCLE
			
	// ACCESSORS
		
	// MUTATORS
		
	// I/O
	std::string describe ()
	//: return a description of the analyse
	{
		std::string theBuffer ("analyse: ");
		theBuffer += describeSelf ();
		return theBuffer;
	}
	
	virtual std::string describeSelf () = 0;
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
};


class CountExtantTaxaAnalysis: public Analysis
//: calculate the number of extant taxa in active tree
{
public:
	// SERVICE
	void execute ()
	{
		int theAnswer = (int) mTreesP->countTaxa();
	}
		
	// I/O
	std::string describeSelf ()
		{ return string ("the number of extant taxa"); }
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
};


class CountAllTaxaAnalysis: public Analysis
//: calculate the total number of taxa, dead or alive, in active tree
{
public:
	// SERVICE
	void execute ()
	{
		int theAnswer = (int) mTreesP->countNodes();
	}
		
	// I/O
	std::string describeSelf ()
		{ return string ("the number of taxa, extant and extinct"); }
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
};

// *** END ***************************************************************/



