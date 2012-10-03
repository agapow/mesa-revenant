/**************************************************************************
Numerics.cpp - assorted maths functions

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

**************************************************************************/


// *** INCLUDES


#include "Numerics.h"
#include <cmath>
#include <cassert>

using std::log;


// *** CONSTANTS & DEFINES


const double pi = 3.141592654;


// *** FUNCTION DEFINITIONS **********************************************/


double logN (double iBase, double iTerm)
//: return the log (of this base of e) of the term
{
	assert (0.0 < iBase);
	assert (0.0 < iTerm);
	return (log (iTerm) / log (iBase));
}


double logE (double iTerm)
//: return the natural log (i.e. log base of e) of the argument
{
	assert (0.0 < iTerm);
	const double kInvLogOfE = 2.302585093;
	return (log (iTerm) * kInvLogOfE);
	// **** this should have term on bottom line as well
	// @todo what does this mean?
}

// calculate consts as statics?

double log2 (double iTerm)
//: return the log 2 of the argument
{
	assert (0.0 < iTerm);
	const double kInvLogOfTwo = 3.321928095;
	return (log (iTerm) * kInvLogOfTwo); 
}



double factorial (long iTerm)
/// @todo cache results
{
	assert (0 < iTerm);
	if (iTerm == 1)
		return 1; 
	else
		return (double (iTerm) * factorial (iTerm - 1));
}

/*
template< unsigned n >
struct factorial
{
    static const unsigned value = n * factorial<n-1>::value;
};

template<>
struct factorial<0>
{
    static const unsigned value = 1;
}; 
*/

/**
Calculate a factorial by Gosper's riff on Stirling's Approximation.

It's neccessary to use an approximation for factorials because the sums get
stupid real fast - 18! is 6.4e15.
*/
double factorialGosper (long n)
{
	return std::sqrt (((2 * n) + (1/3)) * pi) * 
      std::pow (double (n), double (n)) * std::exp (-n);
}

/**
Return the gamma function for argument.

@param  n  Only works for 1 <= z <= 142, but does allow non-integral values.
@todo      Expand to other types via template?
@todo      Better way of getting pi constant?
@credits   Borrowed from <http://mindprod.com/jgloss/factorial.html>.
*/
double gamma (double z)
{
	return std::exp (-z) * std::pow (z, z-0.5) * std::sqrt (2*pi) *
		(1 + 1/(12 * z) + 1/(288 * z * z) - 139 / (51840 * z * z * z) -
		571 / (2488320 * z * z * z * z));
}


/**
Calculate a factorial via the gamma function.

"Enjoys" the limitations of the gamma function itself. 
*/
double factorialGamma (long n)
{
	return gamma (n + 1);
}

/**
Calculate a factorial by Stirling's Approximation.
*/
double factorialStirling (long n)
{
	return std::sqrt (2 * n * pi) * std::pow (double (n), double (n)) * std::exp (-n);
}





long F;
long M;

long swing (long n) 
{
	long s = M - 1 + ((n - M + 1) % 4); 
	bool theOddN = (M & 1) != 1;
	
	for (; M <= s; M++) 
	{
		theOddN = ! theOddN;
		if (theOddN)
			F *= M;
		else
			F = (F << 2) / M;
	}
	
	if (theOddN)
	{
		for (; M <= n; M += 4)
		{
			long m = ((M + 1) * (M + 3)) << 1;
			long d = (M * (M + 2)) >> 3;
			F = (F * m) / d;
		}
	}
	else
	{
		for (; M <= n; M += 4)
		{
			long m = (M * (M + 2)) << 1;
			long d = ((M + 1) * (M + 3)) >> 3;
			F = (F * m) / d;
		}
	}

	return F;
}

long recFactorial (long n) 
{
	// Preconditions:
	assert (0 <= n); // can only take factorial of +ve integers
	
	// Main:
	if (n < 2)
		return 1; 
	else
	{
		M = F = 1;
		return long (std::pow (double (recFactorial (n/2)), 2) * swing (n));
	}
}




// *** END ***************************************************************/



