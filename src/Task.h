/**************************************************************************
Task.h - a manipulator, query or other command to the model

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

About:
- 

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "SimGlobals.h"


// *** CONSTANTS & DEFINES


// *** CLASS DECLARATION *************************************************/

class BasicTask
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
};


	
// *** END ***************************************************************/



