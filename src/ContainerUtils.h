/*******************************************************************************
ContainerUtils.h - assorted container utility functions 

From SIBIL, the Silwood Biocomputing Library.
Paul-Michael Agapow, 2004, Dept. Biology, University College London,
Darwin Building, Gower St, London WC1E 6BT, UNITED KINGDOM.
<mail://sibil@agapow.net>
<http://www.agapow.net/software/sibil/>

About:
- Some templated functions for use with most containers.
- StringsUtils has some specialisations of these.

Changes:
- 00.12.8: created from portions of StringsUtils.
- 04.5.7: ported to new system.

To Do:
- 

*******************************************************************************/

#pragma once
#ifndef SBL_CONTAINERUTILS_H
#define SBL_CONTAINERUTILS_H


// *** INCLUDES

#include "Sbl.h"
#include <algorithm>
#include <cstdlib>

SBL_NAMESPACE_START


// *** CONSTANTS, DEFINES & PROTOTYPES



// *** SHUFFLE ****************************************************************/


/**
Randomly shuffle the elements of the container.

This is intended as a drop in replacement for the STL random_shuffle algorithm.
STL implementations often use rand() or their own random noise function that is
often not easily or obviously seedable. (See XCode and CWPro8.) Thus separate
runs of a program will give the same results. To ensure some consistency, this
function presented which can be seeded appropriately by srand().

@todo random seeding function
@todo concept checking on iterators
*/
template <typename RANDITER>
inline void
shuffle (RANDITER iFirst, RANDITER iLast)
{
   if (iFirst != iLast)
	{
		for (RANDITER p = iFirst + 1; p != iLast; p++)
			iter_swap (p, iFirst + (std::rand() % ((p - iFirst) + 1)));
	}
}


// *** MEMBERSHIP FUNCTIONS ***************************************************/


/**
Can any of terms in the first container be found in the second?

This only tells you whether any of the search terms are in the target, not where
they are or how many times they occur. Use the raw STL algorithms for that.

@todo should this be const?
*/
template <typename TERMITER, typename TARGETITER>
bool
isMemberOf
(TERMITER iStartTerms,  TERMITER iStopTerms, TARGETITER iStartTarget,
	TARGETITER iStopTarget)
{
// Generalized to non-string containers, the design of this fxn deserves
// some explanation. If either the iterator or the search term are passed
// in via references (which you would do if you wanted to pass out the
// position of the found term or just save time copying) then you can't
// make  calls like this: "isMemberOf ('c', myStr.begin(), myStr.end());"
// as neither the first iterator nor the searchterm can be modified.

	// for each value in the search terms container
	for (; iStartTerms != iStopTerms; iStartTerms++)
	{
		// see if you can find it in the target container
		if (std::find (iStartTarget, iStopTarget, *iStartTerms) != iStopTarget)
			return true;
	}
	return false;
}


/**
Can the search term be found in the target container?

See notes on isMemberOf (iter1, iter1, iter2, iter2).
*/
template <typename X, typename TARGETITER>
bool
isMemberOf
(X iSearchTerm, TARGETITER iStartTarget, TARGETITER iStopTarget)
{
	if (std::find (iStartTarget, iStopTarget, iSearchTerm) == iStopTarget)
		return false;
	else
		return true;
}


/**
Can all of terms in the first container be found in the second?

This only tells you whether all of the search terms are in the target, not where
they are or how many times they occur.
*/
template <typename TERMITER, typename TARGETITER>
bool
isSubsetOf
(TERMITER iStartTerms,  TERMITER iStopTerms, TARGETITER iStartTarget,
	TARGETITER iStopTarget)
{
	// for each value in the search terms container
	for (; iStartTerms != iStopTerms; iStartTerms++)
	{
		// see if you can find it in the target container
		if (std::find (iStartTarget, iStopTarget, *iStartTerms) == iStopTarget)
			return false;
	}
	return true;
}


// *** DEPRECIATED, DEBUG, DEVELOPMENT ****************************************/


#if 0
/// A test suite for the container utils module.
void testContainerUtils ()
{
	DBG_MSG ("*** Testing container utils ...");

	DBG_MSG ("");
	DBG_MSG ("* Testing membership functions:");
	
	int	theArr1[] = {1, 2, 3};
	int	theArr2[] = {3};
	int	theArr3[] = {4, 6, 2};
	DBG_MSG ("Array 1 is {1, 2, 3}");
	DBG_MSG ("Array 2 is {3}");
	DBG_MSG ("Array 3 is {4, 6, 2}");
	
	bool theResult[6];
	theResult[0] = isMemberOf (theArr1, theArr1+3, theArr2, theArr2+1);
	theResult[1] = isMemberOf (theArr1, theArr1+3, theArr3, theArr3+3);
	theResult[2] = isMemberOf (theArr2, theArr2+1, theArr1, theArr1+3);
	theResult[3] = isMemberOf (theArr2, theArr2+1, theArr3, theArr3+3);
	theResult[4] = isMemberOf (theArr3, theArr3+3, theArr1, theArr1+3);
	theResult[5] = isMemberOf (theArr3, theArr3+3, theArr2, theArr2+1);
	
	DBG_MSG ("The seq membership results are:");
	DBG_MSG ("  is array 1 in array 2 (expect t): " << theResult[0]);
	DBG_MSG ("  is array 1 in array 3 (expect t): " << theResult[1]);
	DBG_MSG ("  is array 2 in array 1 (expect t): " << theResult[2]);
	DBG_MSG ("  is array 2 in array 3 (expect f): " << theResult[3]);
	DBG_MSG ("  is array 3 in array 1 (expect t): " << theResult[4]);
	DBG_MSG ("  is array 3 in array 2 (expect f): " << theResult[5]);

	DBG_MSG ("The single membership results are:");
	DBG_MSG ("  is 2 in array 1 (expect t): " << isMemberOf (2,
		theArr1, theArr1+3));
	DBG_MSG ("  is 2 in array 2 (expect f): " << isMemberOf (2,
		theArr2, theArr2+1));
	DBG_MSG ("  is 2 in array 3 (expect t): " << isMemberOf (2,
		theArr3, theArr3+3));
		
	DBG_MSG ("Finished testing container utils.");

}

#endif

SBL_NAMESPACE_STOP

#endif
// *** END ********************************************************************/
