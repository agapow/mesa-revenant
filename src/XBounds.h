/**************************************************************************
XBounds.h - a representation of bounded regions

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef XBOUNDS_H
#define XBOUNDS_H


// *** INCLUDES

#include "Sbl.h"

SBL_NAMESPACE_START


// *** CONSTANTS & DEFINES

struct boundary_t {};
// TODO: needed?
//static boundary_t kUnbounded;


// *** CLASS DECLARATION *************************************************/

template <typename X>
class XBounds
{
public:
// LIFECYCLE
	XBounds ();
	XBounds (X iLower, X iUpper);
	XBounds (X iLower, boundary_t iUpper);
	XBounds (boundary_t iLower, X iUpper);

// ACCESSORS
	X        getUpper () const;
	X        getLower () const;
	
	bool     hasUpper () const;
	bool     hasLower () const;
	
	bool     isWithin (X iVal);
	bool     isInside (X iVal);
	bool     isOutside (X iVal);
	
// MUTATORS
	void     setUpper (X iVal);
	void     setLower (X iVal);

	void     unsetUpper ();
	void     unsetLower ();
		
// DEPRECIATE & DEBUG
	void     validate ();
	void     dump (const char* iPrefixCstr = "");

// INTERNALS
private:
	X	      mLower, mUpper;
	bool     mLowerSet, mUpperSet;
};



// *** CLASS DEFINITION **************************************************/

template <typename X>
XBounds <X>::XBounds ()
//: default ctor, that sets XBounds as open-ended
: mLower (0.0), mUpper (0.0), mLowerSet (false), mUpperSet (false)
{
}
	
template <typename X>
XBounds <X>::XBounds (X iLower, X iUpper)
//: ctor that sets upper and lower XBounds on XBounds
: mLower (iLower), mUpper (iUpper), mLowerSet (true), mUpperSet (true)
{
	validate ();
}		

template <typename X>
XBounds <X>::XBounds (boundary_t iLower, X iUpper)
//: ctor that sets upper and lower XBounds on XBounds
: mUpper (iUpper), mLowerSet (false), mUpperSet (true)
{
	iLower = iLower; // just to shut compiler up
}		

template <typename X>
XBounds <X>::XBounds (X iLower, boundary_t iUpper)
//: ctor that sets upper and lower XBounds on XBounds
: mLower (iLower), mLowerSet (true), mUpperSet (false)
{
	iUpper = iUpper; // just to shut compiler up
}		


// *** ACCESSORS


template <typename X>
bool
XBounds <X>::hasUpper () const
//: does that XBounds have an upper bound?
{
	return (mUpperSet);
}

template <typename X>
bool
XBounds <X>::hasLower () const
//: does that XBounds have an lower bound?
{
	return (mLowerSet);
}

template <typename X>
X
XBounds <X>::getUpper () const
//: get the upper bound on the XBounds
// Assumes that there is an upper bound.
{
	assert (hasUpper());
	return (mUpper);
}

template <typename X>
X
XBounds <X>::getLower () const
//: get the lower bound on the XBounds
// Assumes that there is an lower bound.
{
	assert (hasLower());
	return (mLower);
}

template <typename X>
bool
XBounds <X>::isWithin (X iVal)
//: is this value within the XBounds (inclusive)?
// This includes the boundary values themselves.
{
	// if there's an upper boundary & value is above it, return false
	if (hasUpper() and (getUpper() < iVal))
		return false;
	// if there's a lower boundary & value is below it, return false
	if (hasLower() and (iVal < getLower()))
		return false;
	// otherwise it must be true
	return true;
}

template <typename X>
bool
XBounds <X>::isInside (X iVal)
//: is this value within the XBounds (exclusive)?
// This does not include the boundary values themselves.
{
	// if there's an upper boundary & value is above it, return false
	if (hasUpper() and (getUpper() <= iVal))
		return false;
	// if there's a lower boundary & value is below it, return false
	if (hasLower() and (iVal <= getLower()))
		return false;
	// otherwise it must be true
	return true;
}

template <typename X>
bool
XBounds <X>::isOutside (X iVal)
//: is this value outside the XBounds (exclusive)?
// This does not include the boundary values themselves.
{
	return isWithin (iVal);
}


// *** MUTATORS


template <typename X>
void
XBounds <X>::setUpper (X iVal)
//: set an upper bound on the XBounds
// The new upper bound must be greater than or equal to the lower bound.
{
	assert ((not hasLower()) or (getLower() <= iVal));
	mUpperSet = true;
	mUpper = iVal;
}

template <typename X>
void
XBounds <X>::setLower (X iVal)
//: set a lower bound on the XBounds
// The new lower bound must be less than or equal to the upper bound.
{
	assert ((not hasUpper()) or (iVal <= getUpper()));
	mLowerSet = true;
	mLower = iVal;
}

template <typename X>
void
XBounds <X>::unsetUpper ()
//: remove upper bound (if any) and give the XBounds an open upper end
{
	mUpperSet = false;
}
	
template <typename X>
void
XBounds <X>::unsetLower ()
//: remove lower bound (if any) and give the XBounds an open lower end
{
	mLowerSet = false;
}
	
	
// *** DEPRECIATE & DEBUG


template <typename X>
void
XBounds <X>::validate ()
//: self-check
{
	if (mLowerSet and mUpperSet)
		assert (mLower <= mUpper);
}

template <typename X>
void
XBounds <X>::dump (const char* iPrefixCstr)
//: dump state of object to cout
{
	DBG_MSG (iPrefixCstr << "Dumping contents of XBounds at " << this << ":");
	if (hasLower())
	{
		DBG_MSG (iPrefixCstr << "\tLower bound: " << getLower());
	}
	else
	{
		DBG_MSG ("\tLower bound: none");
	}
	if (hasUpper())
	{
		DBG_MSG ("\tUpper bound: " << getUpper());
	}
	else
	{
		DBG_MSG ("\tUpper bound: none");
	}
}



SBL_NAMESPACE_STOP

#endif
// *** END ***************************************************************/



