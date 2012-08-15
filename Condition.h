/**************************************************************************
Condition.h - a boolean query of the system state

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2001, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

About:
- 

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "SimGlobals.h"


// *** CONSTANTS & DEFINES

enum kOperator
{
	kOp_Equal,
	kOp_LessThan,
	kOp_GreaterThan,
	kOp_EqualLessThan,
	kOp_EqualGreaterThan
};


// *** CLASS DECLARATION *************************************************/

class BasicCondition
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
				
	// ACCESSORS
		
	// MUTATORS
		
	// I/O
	virtual std::string describe () = 0; //: returns a plain english description
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
	kOperator	mCompareOp;
};


	
// *** END ***************************************************************/



