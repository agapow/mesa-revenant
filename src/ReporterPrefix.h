/**************************************************************************
ReporterPrefix.h - adds and removes prefixes from the reporter strings

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef REPORTERPREFIX_H
#define REPORTERPREFIX_H


// *** INCLUDES

#include "Reporter.h"
	  

// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class ReporterPrefix
{
public:
	// LIFECYCLE
	ReporterPrefix (const char* iPrefixCstr);
	~ReporterPrefix ();	
};


#endif
// *** END ***************************************************************/



