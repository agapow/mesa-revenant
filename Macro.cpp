/**************************************************************************
MacroAction.h - an action that contains & controls other actions

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>

**************************************************************************/


// *** INCLUDES

#include "ActionUtils.h"
#include "Macro.h"
#include "MesaUtils.h"
#include "TaxaTraitMatrix.h"
#include "TreeWrangler.h"
#include "MesaGlobals.h"
#include "Analysis.h"
#include "Reporter.h"
#include "ReporterPrefix.h"
#include "StringUtils.h"
#include <string>

using std::string;
using sbl::toString;


// *** CONSTANTS & DEFINES

typedef BasicMacro::value_type       value_type;
typedef BasicMacro::size_type        size_type;
typedef BasicMacro::reference        reference;
typedef BasicMacro::container_type   container_type;
typedef BasicMacro::iterator         iterator;


// *** BASIC MACRO *******************************************************/

BasicMacro::~BasicMacro ()
{
	deleteAll();
}
	
	
// *** ACCESSORS

bool BasicMacro::containsAnalyses ()
{
	iterator q;
	for (q = mContents.begin(); q != mContents.end(); q++)
	{
		BasicAction* theActionP = *q;
		BasicMacro*  theMacroP = castAsMacro (theActionP);
		if (theMacroP != NULL)
		{
			if (theMacroP->containsAnalyses())
				return true;
		}
		else
		{
			BasicAnalysis*  theAnalysisP = castAsAnalysis (theActionP);
			if (theAnalysisP != NULL)
				return true;
		}
	}
	return false;
}


BasicMacro::iterator BasicMacro::begin()
{
	return mContents.begin ();
}

BasicMacro::iterator BasicMacro::end()
{
	return mContents.end ();
}

reference BasicMacro::at (int iIndex)
{
	assertValidIndex (iIndex);
	return mContents[iIndex];
}


size_type BasicMacro::getDepth (size_type iIndex)
{
	// Preconditions:
	assert (0 <= iIndex);
	assert (iIndex <= deepSize());
	
	// Main:
	if (iIndex == 0)
	{
		return 0;
	}
	else
	{
		size_type theDepth = 0;
		
		iterator q;
		for (q = mContents.begin(); q != mContents.end(); q++)
		{
			assert (0 <= iIndex);
			
			size_type theSizeCurrElement = (*q)->deepSize();
			if (iIndex <= theSizeCurrElement)
				// handoff description task to this element
				return (*q)->getDepth (iIndex - 1) + 1;
			else
				// look at later element
				iIndex -= theSizeCurrElement;
		}
	}
	
	// Postonditions:		
	assert (false);  // should never get here
	return -13;      // just to shut compiler up
}

void BasicMacro::copyContents (BasicMacro* iOldMacroP)
//: for converting one type of macro into another
{
	int theNewSize = iOldMacroP->size();
	mContents.resize (theNewSize);
	for (int i = 0; i < theNewSize; i++)
	{
		at(i) = iOldMacroP->at(i);
	}
}


void BasicMacro::stealContents (BasicMacro* iOldMacroP)
//: for converting one type of macro into another
{
	copyContents (iOldMacroP);
	iOldMacroP->orphanAll ();
}


// *** MUTATORS

void BasicMacro::adoptAction (value_type iActionP)
//: add (and take responsibility for) an action to the macro
{
	mContents.push_back (iActionP);
}


void BasicMacro::orphanAction (int iIndex)
{
	assertValidIndex (iIndex);
	mContents.erase (mContents.begin() + iIndex);
}


void BasicMacro::orphanAll ()
{
	mContents.resize (0);
}
	
	
void BasicMacro::deleteAll ()
{
	while (0 < mContents.size())
		deleteAction (0);
}

void BasicMacro::deleteAction (int iIndex)
//: delete the nth action in this macro
// NOTE: this is a shallow deletion, it does not recurse into contained
// actions.
{
	assert (0 <= iIndex);
	assert (iIndex < mContents.size());
	deleteAction (mContents.begin() + iIndex);
}

void BasicMacro::deleteAction (iterator iIter)
//: delete the nth action in this macro
// NOTE: this is a shallow deletion, it does not recurse into contained
// actions.
{
	BasicAction* thePtr = *iIter;
	thePtr->validate();
	delete thePtr;
	mContents.erase (iIter);
}

const char* BasicMacro::describe (size_type iIndex)
{
	// Preconditions:
	assert (0 <= iIndex);
	assert (iIndex <= deepSize());
	
	// Main:
	if (iIndex == 0)
	{
		static string theBuffer;
		theBuffer = "macro: ";
		theBuffer += describeMacro ();
		return theBuffer.c_str();
	}
	else
	{
		for (iterator q = begin(); q != end(); q++)
		{
			assert (0 <= iIndex);
			
			int theSizeCurrElement = (*q)->deepSize();
			if (iIndex <= theSizeCurrElement)
			{
				// handoff description task to this element
				return (*q)->describe (iIndex - 1);
			}
			else
			{
				// look at later element
				iIndex -= theSizeCurrElement;
			}
		}
	}

	// Postconditions:
	assert (false);   // should never reach here
	return "";        // just to shut compiler up
}

int BasicMacro::size () const
{
	return int (mContents.size());
}

int BasicMacro::deepSize ()
//: how many events are in this macro, recursively?
// Counts the container itself, and those within, so it is always at least
// 1.
{
	size_type theDeepSize = 1;
	iterator q;
	for (q = mContents.begin(); q != mContents.end(); q++)
		theDeepSize += (*q)->deepSize();

	// Postconditions & return:
	assert (0 <= theDeepSize);
	return theDeepSize;
} 



void BasicMacro::deleteElement (int iIndex)
// NOTE: this is a shallow deletion, it does not recurse into contained
// actions.
{
	// Preconditions:
	assert (0 <= iIndex);
	assert (iIndex <= deepSize());
	
	// Main:
	if (iIndex == 0)
	{
		assert (false); // can't delete self
		return; 
	}
	else
	{
		size_type theDepth = 0;
		
		iterator q;
		for (q = mContents.begin(); q != mContents.end(); q++)
		{
			assert (0 < iIndex);
			
			if (iIndex == 1)
			{
				delete (*q);
				mContents.erase (q);
				return;
			}
			else
			{
				size_type theSizeCurrElement = (*q)->deepSize();
				if (iIndex <= theSizeCurrElement)
				{
					// handoff deletion task to this element
					(*q)->deleteElement (iIndex - 1);
					return;
				}
				else
				{
					// look at later element
					iIndex -= theSizeCurrElement;
				}
			}
		}
	}
	
	// Postonditions:		
	assert (false);  // should never get here
}

		
// *** I/O

/*
void BasicMacro::detailedReport
(std::ostream& ioOutStream, int iIndentLevel)
//: output a full & detailed version of the queue to the stream
{
	BasicAction::detailedReport (ioOutStream, iIndentLevel);
	for (int i = 0; i < size(); i++)
	{
		(at(i))->detailedReport (ioOutStream, iIndentLevel + 1);
	}
}
*/

// *** DEPRECATED & DEBUG

// *** INTERNALS

void BasicMacro::assertValidIndex (int iIndex)
{
	assert (0 <= iIndex);
	assert (iIndex < size());
}


void BasicMacro::executeMacro ()
//: execute the enclosed block 
// NOTE: necessary for evolution events
{
	for (int i = 0; i < mContents.size(); i++)
		(mContents.at(i))->execute();
		
	// MesaGlobals::mTreeDataP->validate();
}



// *** RUN N MACRO *******************************************************/
#pragma mark -

void RunNMacro::execute ()
{
	for (int i = 1; i <= mLoops; i++)
	{
		std::string thePrefixStr ("run ");
		thePrefixStr += toString (i);
		thePrefixStr += " of ";
		thePrefixStr += toString (mLoops);		
		ReporterPrefix	thePrefix (thePrefixStr.c_str());
		
		executeMacro();
	}
}

const char* RunNMacro::describeMacro ()
{
	static std::string theDesc;
	theDesc = "run the enclosed actions ";
	theDesc += toString (mLoops);
	theDesc += " times";
	return theDesc.c_str();
}

void	RunNMacro::validate	()
{
	assert (0 < mLoops);
}


// *** RUN TREE MACRO *****************************************************/
#pragma mark -

void TreeMacro::execute ()
// NOTE: restores the same active tree as was at the start.
// NOTE: is robust to actions duplicating trees within loop, as it only
// loops over those trees that existed at the beggining. Also notice that
// no way to delete trees is allowed.
{
	int theNumTrees = MesaGlobals::mTreeDataP->size();
	int theOldActiveIndex = (int) MesaGlobals::mTreeDataP->getActiveTreeIndex ();
	
	for (int i = 0; i < theNumTrees; i++)
	{
		MesaGlobals::mTreeDataP->setActiveTreeIndex (i);
		 
		std::string thePrefixStr ("run over tree ");
		thePrefixStr += sbl::toString (i + 1);
		thePrefixStr += " of ";
		thePrefixStr += sbl::toString (theNumTrees);		
		
		ReporterPrefix	thePrefix (thePrefixStr.c_str());
		
		executeMacro();
	}

	MesaGlobals::mTreeDataP->setActiveTreeIndex (theOldActiveIndex);
}

const char* TreeMacro::describeMacro ()
{
	return "run over every tree";
}



// *** RUN ONCE MACRO ****************************************************/
#pragma mark -

void RunOnceMacro::execute ()
{
	ReporterPrefix	thePrefix ("run once");	
	executeMacro();
}

const char* RunOnceMacro::describeMacro ()
{
	return "run the enclosed actions once";
}



// *** RUN & RESTORE MACRO ***************************************************/
#pragma mark -

void RunAndRestoreMacro::execute ()
{	
	TreeWrangler		theSavedTrees = *(MesaGlobals::mTreeDataP);
	ContTraitMatrix	theSavedContData = *(MesaGlobals::mContDataP);
	DiscTraitMatrix	theSavedDiscData = *(MesaGlobals::mDiscDataP);	
		
	for (int i = 1; i <= mLoops; i++)
	{
		std::string thePrefixStr ("run & restore ");
		thePrefixStr += sbl::toString (i);
		thePrefixStr += " of ";
		thePrefixStr += sbl::toString (mLoops);		
		ReporterPrefix	thePrefix (thePrefixStr.c_str());
		
		executeMacro();
		
		*(MesaGlobals::mTreeDataP) = theSavedTrees; 
		*(MesaGlobals::mContDataP) = theSavedContData;
		*(MesaGlobals::mDiscDataP) = theSavedDiscData;
		// MesaGlobals::mActiveTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();
	}
}

const char* RunAndRestoreMacro::describeMacro ()
{
	static std::string theDesc;
	theDesc = "run * restore ";
	theDesc += sbl::toString (mLoops);
	theDesc += " times";
	return theDesc.c_str();
}

void	RunAndRestoreMacro::validate	()
{
	assert (0 < mLoops);
}



// *** UTILITY FUNCTIONS *************************************************/
#pragma mark -

BasicMacro* castAsMacro (BasicAction* iActionP)
//: return the parameter cast as a macro is possible, otherwise nil
// Necessary if we can't get the dynamic cast to work as desired. May
// need to place ids in actions and test them.
{
	BasicMacro* theMacroP = NULL;
	theMacroP = dynamic_cast<BasicMacro*> (iActionP);
	return theMacroP;
}


// *** DEPRECATED & DEBUG ***********************************************/


/*


value_type BasicMacro::deepAt (int iIndex)
//: fetch the indexed item stored (recursively) within this container
// By convention index 0 is this container.
// NOTE: this function unfortunately returns a value_type not reference.
// This is because it may have to return a pointer to itself (this).
{
//	assert (0 < iIndex);
	assert (iIndex < deepSize());

	if (iIndex == 0)
	{
		return (this);
	}
	else
	{
		iIndex--;
		for (int i = 0; i < size (); i++)
		{
			assert (0 <= iIndex);
			
			BasicMacro* theElementP = NULL;
			int theElementSize = 0;
			theElementP = castAsMacro (at(i));
			if (theElementP)
			{
				theElementSize = theElementP->deepSize();
			}
			else 
			{
				theElementSize = 1;
			}
			
			if (iIndex < theElementSize)
			{
				if (theElementP)
				{
					assert (iIndex < theElementP->deepSize());
					return theElementP->deepAt(iIndex);
				}
				else
				{
					assert (iIndex == 0);
					return at(i);
				}
			}
			else
				iIndex -= theElementSize;
		}
		
		assert (false);
		return NULL; // just to shut compiler up
	}
}

void BasicMacro::deepDeleteAction (int iIndex)
{
	// Preconditions:
	// because you can't delete youself & must be an element you own
	assert (0 < iIndex);
	assert (iIndex < deepSize());

	// Main:
	for (int i = 0; i < size (); i++)
	{
		// it's the current action, delete it
		if (iIndex == 1)
		{
			deleteAction (i);
			return;
		}

		// how big is the current element?
		BasicMacro* theMacroP = NULL;
		theMacroP = castAsMacro (at(i));
		
		if (theMacroP)
		{
			// if it's a macro dive into it
			int theElementSize = theMacroP->deepSize();
			if (iIndex <= theElementSize)
			{
				theMacroP->deepDeleteAction (iIndex - 1);
				return;
			}
			else
			{
				iIndex -= theElementSize;
			}
		}
		else 
		{
			// if it's a standard action, just tick along
			iIndex--;
		}
	}
}

*/


// *** END ***************************************************************/



