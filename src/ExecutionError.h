/**************************************************************************
ExecutionError.h -  

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.bio.ic.ac.uk/evolve>

**************************************************************************/

#pragma once
#ifndef EXECERROR_H
#define EXECERROR_H


// *** INCLUDES

#include "Error.h"


// *** CONSTANTS AND DEFINES

// *** CLASS TEMPLATE ****************************************************/

class ExecutionError: public sbl::Error
{
public:
	ExecutionError ()
		: Error ("error in executing action")
		{}
	ExecutionError (const char* ikDesc)
		: Error (ikDesc)
		{}
};


#endif
// *** END ***************************************************************/



