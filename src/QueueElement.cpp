/**************************************************************************
QueueElement.cpp -  

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve>

**************************************************************************/


// *** INCLUDES

#include "QueueElement.h"


// *** CONSTANTS AND DEFINES

//typedef QueueElement::int   int;


// *** CLASS TEMPLATE ****************************************************/

const char* QueueElement::describe ()
{
	assert (false);
	return "";
}

bool QueueElement::isEmpty ()
{
	return (size() == 0);
}

int QueueElement::size ()
{
	return 0; // just to shut compiler up
}

int QueueElement::deepSize ()
// includes self
{
	return size () + 1;
}

QueueElement* QueueElement::getElement (int iIndex)
{
	if (iIndex == 0)
	{
		return (this);
	}
	else
	{
		assert (false);
		return NULL;      // just to shut compiler up
	}
}

QueueElement* QueueElement::getDeepElement (int iIndex)
{
	return getElement (iIndex); 
}

int QueueElement::getDepth (int iIndex)
{
	if (iIndex == 0)
		return 0;
	else
		assert (false);
}

void QueueElement::deleteElement (int iIndex)
{
	assert (false);
	iIndex = iIndex;   // just to shut compiler up
}

void QueueElement::deleteDeepElement (int iIndex)
{
	assert (false);
	iIndex = iIndex;   // just to shut compiler up
}


void QueueElement::deleteAll ()
{
	while (0 <= size())
		deleteElement (0);
}


// *** END ***************************************************************/



