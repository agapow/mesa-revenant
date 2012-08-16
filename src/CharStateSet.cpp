/**************************************************************************
CharStateSet.cpp - a collection of possible character states

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>

**************************************************************************/


// *** INCLUDES

#include "CharStateSet.h"
#include "Sbl.h"
#include <algorithm>
#include <cassert>
#include <string>


// *** CONSTANTS & DEFINES

typedef CharStateSet::iterator   iterator;


// *** CLASS DECLARATION *************************************************/

// *** LIFECYCLE
// accept defaults


// *** ACCESS

bool CharStateSet::isMember (const char* iSearchCStr)
//: is this state a member of the set?
{
	return (find (iSearchCStr) != end());
}

bool CharStateSet::isMember (std::string& iSearchStr)
//: is this state a member of the set?
{
	return (isMember (iSearchStr.c_str()));
}

iterator CharStateSet::find (const char* iSearchCStr)
//: return an iterator to this state
// Returns end() if not found.
{
	std::string theSearchStr (iSearchCStr);
	return (std::find (begin(), end(), theSearchStr));
}

iterator CharStateSet::nextState (const char* iStateCStr)
//: return an iterator to the state after this one
// Fails is this is the last state or doesn't exist.
{
	iterator theStateIter = find (iStateCStr);
	assert (theStateIter != end());
	return ++theStateIter;
}

iterator CharStateSet::prevState (const char* iStateCStr)
//: return an iterator to the state prior to this one
// Fails is this is the first state or doesn't exist.
{
	iterator theStateIter = find (iStateCStr);
	assert (theStateIter != end());
	assert (theStateIter != begin());
	return --theStateIter;
}


// *** MUTATORS

void CharStateSet::sort ()
//: sorts the states into a natural order
{
	std::sort (begin(), end());
}

void CharStateSet::addState (const char* iStateCstr)
{
	push_back (std::string (iStateCstr));
}

void CharStateSet::addState (disctrait_t iDelState)
{
	push_back (iDelState);
}

void CharStateSet::deleteState (long iIndex)
{
	assert (0 <= iIndex);
	assert (iIndex < size());
	erase (begin() + iIndex);
}

void CharStateSet::deleteState (disctrait_t iDelState)
{
	erase (find (iDelState.c_str()));
}

void CharStateSet::deleteState (const char* iDelCstr)
{
	erase (find (iDelCstr));
}
	
	
// *** I/O

std::string CharStateSet::str (const char* iDelim)
{		
	std::string theBuffer;
	for (iterator q = begin(); q != end(); q++)
	{
		if (q != begin())
			theBuffer.append (iDelim);
		theBuffer.append (*q);
	}
	return theBuffer;
}

	
// *** DEBUG & DEPRECATED

void CharStateSet::validate ()
{
	assert (2 <= size());
	for (iterator q = begin(); q != end(); q++)
		assert (*q != "");
}

void CharStateSet::dump ()
{
	DBG_MSG ("Dumping contents of CharStateSet at" << this);
	DBG_MSG ("Contains" << size() << " states ");
	for (iterator q = begin(); q != end(); q++)
		DBG_MSG (q - begin() << ": " << *q);
	DBG_MSG ("Finished dumping CharStateSet");
}
	
	
// *** END ***************************************************************/



