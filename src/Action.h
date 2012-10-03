/**************************************************************************
Action.h - an abstract interaction with the MesaModel

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- 

**************************************************************************/

#pragma once
#ifndef ACTION_H
#define ACTION_H


// *** INCLUDES

// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class BasicAction
{
public:
	// TYPE INTERFACE
	typedef int   size_type;
	
	// LIFECYCLE
	BasicAction ()
		{}
	virtual ~BasicAction ()
		{}
			
	// SERVICES
	virtual void          execute () = 0;
	virtual const char*   describe (size_type iIndex) = 0;
	virtual size_type     deepSize ();
	virtual void          deleteElement (size_type iIndex) = 0;
	virtual size_type     getDepth (size_type iIndex);

	virtual void          validate () {}
};



#endif
// *** END ***************************************************************/



