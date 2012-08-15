/**************************************************************************
SimGlobals.h - simplified data acees 

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2001, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

About:
- This is bit of a fudge but all levels of the taskqueue may need access
  to the data. But do you want to be hauling around multiple parameters
  to the queue, epoch and tasks even if they won't use it? So we cheat
  and use a global so that any simulation event can get to the data if
  it needs to.

**************************************************************************/


// *** INCLUDES

#pragma once

/*
#include "Sbl.h"
#include "MesaTypes.h"
#include "DataWrangler.h"
#include "TreeWrangler.h"
#include "Reporter.h"
*/

// *** CONSTANTS & DEFINES


// *** CLASS DECLARATION *************************************************/

//static DiscDataWrangler*	gDiscDataP;
//static ContDataWrangler*	gContDataP;
//static TreeWrangler*			gTreeDataP;
//static Reporter*				gReporterP;

	
// *** END ***************************************************************/



