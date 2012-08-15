/**************************************************************************
Action.h - an abstract interaction with the MesaModel

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
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



