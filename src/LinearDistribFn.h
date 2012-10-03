/**************************************************************************
LinearDistribFn.h - calculates a result from a simple linear distribution

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

About:
- constructed with 3 parameters (a,b, c), this embodies a functor that
  when called with x, provides the answer to ax^b + c

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include <ccmath>


// *** CONSTANTS & DEFINES


// *** CLASS DECLARATION *************************************************/

class LinearDistribFn
{
public:
	// LIFECYCLE
	LinearDistribFn (double a, double b, double c)
		: mA(a), mB(b), mC(c)
		{}
				
	// SERVICES
	operator () (double x)
	{
		return (mA * std::pow (x, mB)) + mC;
	}
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
	double mA, mB, mC;
	
	
};


// *** END ***************************************************************/



