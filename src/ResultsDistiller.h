/**************************************************************************
ResultsDistiller.h - condenses & transposes results file 

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>
**************************************************************************/

#pragma once
#ifndef RESULTSDISTILLER_H
#define RESULTSDISTILLER_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include <fstream>


// *** CONSTANTS & DEFINES


// *** CLASS DECLARATION *************************************************/

class ResultsDistiller
{
public:
	// LIFECYCLE
	ResultsDistiller (int iTrimCols)
		//: ctor that sets ther number of initial columns to trim
		: mTrimCols (iTrimCols)
		{}
						
	// SERVICES
	void	distill (std::ifstream& iInFstream, std::ofstream& iOutFstream);

	// DEPRECIATED & DEBUG
	void	validate	()
		{ /* nothing as yet */ }

	// INTERNALS
private:
	int								mTrimCols;
	
	bool compareStringVec (stringvec_t& iInfo1, stringvec_t& iInfo2);
	bool isData (std::string& iTestString);
};


#endif
// *** END ***************************************************************/



