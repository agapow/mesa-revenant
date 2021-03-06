/**************************************************************************
ReporterPrefix.cpp - adds and removes prefixes from the reporter strings

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- a classic stack object which pushes the prefix into the reporter on
  creation and pops/removes it on destruction.
  
**************************************************************************/


// *** INCLUDES

#include "ReporterPrefix.h"
#include "ActionUtils.h"
	  

// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

ReporterPrefix::ReporterPrefix (const char* iPrefixCstr)
{
	pushReportPrefix (iPrefixCstr);
}


ReporterPrefix::~ReporterPrefix ()
{
	popReportPrefix ();
};


// *** END ***************************************************************/



