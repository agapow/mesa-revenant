/**************************************************************************
Bounds.cpp - representing bounded range

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2001, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

About:
- currently limited to doubles

**************************************************************************/

// *** INCLUDES

#include "Bounds.h"
#include <cassert>

SBL_NAMESPACE_START


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

// *** LIFECYCLE

Bounds::Bounds ()
//: default ctor, that sets Bounds as open-ended
: mLower (0.0), mUpper (0.0), mLowerSet (false), mUpperSet (false)
{

}

Bounds::Bounds (double iLower, double iUpper)
//: ctor that sets upper and lower bounds on Bounds
: mLower (iLower), mUpper (iUpper), mLowerSet (true), mUpperSet (true)
{
	assert (iLower <= iUpper);
}

Bounds::Bounds (unbounded_t iLower, double iUpper)
//: ctor that sets upper and lower bounds on Bounds
: mUpper (iUpper), mLowerSet (false), mUpperSet (true)
{
	iLower = iLower; // just to shut compiler up
}

Bounds::Bounds (double iLower, unbounded_t iUpper)
//: ctor that sets upper and lower bounds on Bounds
: mLower (iLower), mLowerSet (true), mUpperSet (false)
{
	iUpper = iUpper; // just to shut compiler up
}


// *** ACCESSORS
#pragma mark -

bool Bounds::hasUpper () const
//: does that Bounds have an upper bound?
{
	return (mUpperSet);
}

bool Bounds::hasLower () const
//: does that Bounds have an lower bound?
{
	return (mLowerSet);
}

double Bounds::getUpper () const
//: get the upper bound on the Bounds
// Assumes that there is an upper bound.
{
	assert (hasUpper());
	return (mUpper);
}

double Bounds::getLower () const
//: get the lower bound on the Bounds
// Assumes that there is an lower bound.
{
	assert (hasLower());
	return (mLower);
}

bool Bounds::isWithin (double iVal)
//: is this value within the Bounds (inclusive)?
// This includes the boundary values themselves.
{
	if (not (hasUpper() and (iVal <= getUpper())))
		return false;
	if (not (hasLower() and (getLower() <= iVal)))
		return false;
	return true;
}

bool Bounds::isInside (double iVal)
//: is this value within the Bounds (exclusive)?
// This does not include the boundary values themselves.
{
	if (not (hasUpper() and (iVal < getUpper())))
		return false;
	if (not (hasLower() and (getLower() < iVal)))
		return false;
	return true;
}


// *** MUTATORS

void Bounds::setUpper (double iVal)
//: set an upper bound on the Bounds
// The new upper bound must be greater than or equal to the lower bound.
{
	assert ((not hasLower()) or (getLower() <= iVal));
	mUpperSet = true;
	mUpper = iVal;
}

void Bounds::setLower (double iVal)
//: set a lower bound on the Bounds
// The new lower bound must be less than or equal to the upper bound.
{
	assert ((not hasUpper()) or (iVal <= getUpper()));
	mLowerSet = true;
	mLower = iVal;
}

void Bounds::unsetUpper ()
//: remove upper bound (if any) and give the Bounds an open upper end
{
	mUpperSet = false;
}

void Bounds::unsetLower ()
//: remove lower bound (if any) and give the Bounds an open lower end
{
	mLowerSet = false;
}


// *** DEPRECIATE & DEBUG
#pragma mark -

void Bounds::validate ()
//: self-check
{
	if (mLowerSet and mUpperSet)
		assert (mLower <= mUpper);
}

void Bounds::dump ()
//: dump state of object to cout
{
	DBG_MSG ("Dumping contents of Bounds at " << this << ":");
	if (hasLower())
	{
		DBG_MSG ("\tLower bound: " << getLower());
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


// *** END ***************************************************************/



