/**************************************************************************
Bounds.h - a represention of bounded regions

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2001, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

**************************************************************************/

#ifndef BOUNDS_H
#define BOUNDS_H


// *** INCLUDES

#include "Sbl.h"

SBL_NAMESPACE_START


// *** CLASS DECLARATION *************************************************/

class Bounds
{
public:
	enum unbounded_t {};

public:
// LIFECYCLE
	Bounds ();
	Bounds (double iLower, double iUpper);
	Bounds (double iLower, unbounded_t iUpper);
	Bounds (unbounded_t iLower, double iUpper);

// ACCESSORS
	double   getUpper () const;
	double   getLower () const;
	
	bool     hasUpper () const;
	bool     hasLower () const;
	
	bool     isWithin (double iVal);
	bool     isInside (double iVal);
	
// MUTATORS
	void     setUpper (double iVal);
	void     setLower (double iVal);

	void     unsetUpper ();
	void     unsetLower ();
		
// DEPRECIATE & DEBUG
	void     validate ();
	void     dump ();

// INTERNALS
private:
	double   mLower, mUpper;
	bool     mLowerSet, mUpperSet;
};


// *** CONSTANTS & DEFINES

static Bounds::unbounded_t   kUnbounded;



SBL_NAMESPACE_STOP

#endif
// *** END ***************************************************************/



