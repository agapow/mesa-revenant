/**************************************************************************
Action.cpp - an abstract interaction with the MesaModel

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- 

**************************************************************************/


// *** INCLUDES

#include "Action.h"
#include <cassert>


// *** CONSTANTS & DEFINES

typedef BasicAction::size_type   size_type;


// *** CLASS DECLARATION *************************************************/

size_type BasicAction::deepSize ()
{
	return 1;
}

	
size_type BasicAction::getDepth (size_type iIndex)
{
	if (iIndex == 0)
		return 0;
	else
	{
		assert (false);
		return -13;
	}
}



// *** END ***************************************************************/



