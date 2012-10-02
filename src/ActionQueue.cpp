/**************************************************************************
ActionQueue.h - a manager for groups of model manipulations

Credits:
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
-

Changes:
- 99.7.13: Created.

To Do:
- Still evolving in concept.

**************************************************************************/


// *** INCLUDES

#include "ActionQueue.h"
#include "Action.h"
#include "Reporter.h"
#include "Macro.h"
#include "Sbl.h"
#include <iomanip>


// *** CONSTANTS & DEFINES

typedef ActionQueue::value_type       value_type;
typedef ActionQueue::container_type   container_type;
typedef ActionQueue::size_type        size_type;


// *** MAIN BODY *********************************************************/

// *** LIFECYCLE *********************************************************/

ActionQueue::ActionQueue ()
{
	// nothing as yet
}

ActionQueue::~ActionQueue ()
{
	clear ();
}


// *** ACCESSORS *********************************************************/

int ActionQueue::getDepth (size_type iIndex)
//: get the depth (nestedness) of action i
// Add 1 because your don't want to see the outside container
{
	return (mContents.getDepth (iIndex + 1)) - 1;
}


int ActionQueue::size ()
//: return the number of actions stored recurseively within
// by convention, doesn't include the Macro we use for convenient storage
{
	int theDeepSize = 0;
	container_type::iterator q;
	for (q = mContents.begin(); q != mContents.end(); q++)
		theDeepSize += (*q)->deepSize();

	// Postconditions & return:
	assert (0 <= theDeepSize);
	return theDeepSize;
}


bool ActionQueue::isEmpty ()
//: are there no actions stored in the queue?
{
	return (size() == 0);
}


const char* ActionQueue::describe (size_type iIndex)
{
	/// Preconditions:
	assert (0 <= iIndex);
	assert (iIndex < size());

	// Main:
	return mContents.describe (iIndex + 1);
}


bool ActionQueue::containsAnalyses ()
{
	return mContents.containsAnalyses();
}



// *** MUTATORS **********************************************************/

void ActionQueue::adoptAction (value_type iNewActionP)
{
	mContents.adoptAction (iNewActionP);
}


void ActionQueue::deleteElement (size_type iIndex)
{
	/// Preconditions:
	assert (0 <= iIndex);
	assert (iIndex < size());

	// Main:
	mContents.deleteElement (iIndex + 1);
	/*
	container_type::iterator q;
	for (q = mContents.begin(); q != mContents.end(); q++)
	{
		assert (0 <= iIndex);

		int theSizeCurrElement = (*q)->deepSize();
		if (iIndex == 0)
		{
			// if it's this element
			mContents.deleteAction (q);
			return;
		}
		else if (iIndex <= theSizeCurrElement)
		{
			// handoff description task to this element
			(*q)->deleteElement (iIndex);
			return;
		}
		else
		{
			// look at later element
			iIndex -= theSizeCurrElement;
			return;
		}
	}

	// Postconditions:
	assert (false);   // should never reach here
	*/
}


void ActionQueue::clear ()
{
//	container_type::iterator q;
//	for (q = mContents.begin(); q != mContents.end(); q++)
//		delete (*q);
	mContents.deleteAll ();
}


// *** I/O ***************************************************************/

void ActionQueue::summarise (std::ostream& ioOutStream)
//: output an executive summary of the queue to the stream
{
	ioOutStream << "There ";
	int theSize = size();
	assert (0 <= theSize);

	switch (size ())
	{
		case (0):
			ioOutStream << "are no actions";
			break;

		case (1):
			ioOutStream << "is one action";
			break;

		default:
			ioOutStream << "are " << theSize << " actions";
			break;
	}

	ioOutStream << " programmed in the queue." << std::endl;
}


void ActionQueue::detailedReport (std::ostream* ioOutStreamP)
//: output a full & detailed version of the queue to the stream
{
	if (isEmpty ())
	{
		(*ioOutStreamP) << "There are no actions programmed in the queue" << std::endl;
	}
	else
	{
		long theNumActions = size();
		bool	theMultipleActions = (1 < size());
		(*ioOutStreamP) << "There " << (theMultipleActions? "are" : "is") <<
			" " << size() << " action" << (theMultipleActions? "s" : "") <<
			" programmed in the queue." << std::endl;
		for (long i = 0; i < theNumActions; i++)
		{
			long	theDepth = getDepth (i);
			(*ioOutStreamP) << "  " << std::setw (4) << i + 1;
			for (long j = 0; j < theDepth; j++)
				(*ioOutStreamP) << "   ";
			(*ioOutStreamP) << describe (i) << std::endl;
		}
	}
}


void ActionQueue::dump()
{
	DBG_MSG ("There are " << mContents.size() << "top level actions");
	DBG_MSG ("There are " << size() << "total actions");
	DBG_MSG ("In detail:");
	long theNumActions = size();
	for (long i = 0; i < theNumActions; i++)
	{
		DBG_MSG ("#" << i <<"; Depth: " << getDepth (i));
	}
}



// *** SERVICES **********************************************************/

void ActionQueue::runOnce ()
//: execute the stored chain of actions once in order
{
	mContents.execute ();
}


void ActionQueue::runTrees ()
//: run across every tree
{
	TreeMacro	theMacro;
	theMacro.copyContents (mContents.begin(), mContents.end());
	theMacro.execute ();
	theMacro.orphanAll();
}


void ActionQueue::runN (int iLoops)
//: repeatedely execute the stored chain of actions
{
	RunNMacro	theMacro (iLoops);
	theMacro.copyContents (mContents.begin(), mContents.end());
	theMacro.execute ();
	theMacro.orphanAll();
}


void ActionQueue::runAndRestore (int iLoops)
//: execute the actions repeatedly, restoring the data between each loop
{
	RunAndRestoreMacro	theMacro (iLoops);
	theMacro.copyContents (mContents.begin(), mContents.end());
	theMacro.execute ();
	theMacro.orphanAll();
}


// *** DEPRECATED & TEST FUNCTIONS **************************************/

void ActionQueue::validate ()
//: validates queue by calling all memebers to validate themselves
{
	container_type::iterator q;
	for (q = mContents.begin(); q != mContents.end(); q++)
		(*q)->validate();
}




// *** END ***************************************************************/

