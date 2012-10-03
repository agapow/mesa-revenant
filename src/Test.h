/**************************************************************************
Test.h - ask a yes/no question of the data

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "SimGlobals.h"


// *** CONSTANTS & DEFINES

// *** BASIC PRUNE ******************************************************/

class BasicTest
//: an encapsulated yes/no query of the data
{
public:
	// SERVICES
	virtual bool operator () () = 0;
		
	// I/O
	virtual std::string describe () = 0;
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
};


class CharacterTest: public BasicTest
{
public:
	// SERVICES
	// virtual bool operator () () = 0;
		
	// I/O
	// virtual std::string describe () = 0;
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
};

// *** END ***************************************************************/



