/**************************************************************************
ExecutionError.h -  

Credits:
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
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



