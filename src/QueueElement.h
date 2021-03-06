/**************************************************************************
QueueElement.h -  

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve>

**************************************************************************/

#ifndef QUEUEELEMENT_H
#define QUEUEELEMENT_H


// *** INCLUDES

#include "Error.h"


// *** CONSTANTS AND DEFINES

// *** CLASS TEMPLATE ****************************************************/

class QueueElement
{
public:
	virtual ~QueueElement() {};
	
	virtual const char*     describe ();
	bool                    isEmpty ();
	virtual int             size ();
	virtual int             deepSize ();
	virtual QueueElement*   getElement (int iIndex);
	virtual QueueElement*   getDeepElement (int iIndex);
	virtual int             getDepth (int iIndex);
	virtual void            deleteElement (int iIndex);
	virtual void            deleteDeepElement (int iIndex);
	virtual void            deleteAll ();
};


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



