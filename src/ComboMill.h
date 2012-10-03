/**
@file 

@author   Paul-Michael Agapow, 2003.
          Dept. Biology, University College London, 
          London WC1E 6BT, UNITED KINGDOM.
          <mail://p.agapow@ic.ac.uk>
          <http://www.agapow.net/>
*/

#pragma once
#ifndef MSA_COMBOMILL_H
#define MSA_COMBOMILL_H


// *** INCLUDES

#include "Sbl.h"
#include <vector>
#include <algorithm>


SBL_NAMESPACE_START

using std::vector;
using std::count;
using std::cout;
using std::endl;



// *** MAIN BODY *********************************************************/

/**
Given a container, one by one generate all combinations of the contents.

A reification of a combination generating algorithm that preserves state
between calls. It is necessary to do this because otherwise it is hard
to keep track of where the algorithm has gotten up to. 

And example of how it can be used:
@code
	int theMin = 3, theMax = 5;
	vector<int> theSourceArr;
	for (int i = 0; i < 7; i++)
		theSourceArr.push_back (i);
		
	ComboMill< vector<int> > theMill (theSourceArr.begin(), theSourceArr.end());
	vector<int> theComboArr (theSourceArr);
	vector<int>::iterator theSetIter = theComboArr.begin();

	theMill.firstKJ (theMin,theMax);	
	while (not theSiteComboMill.isLast())
	{
		theMill.getCurrent (theComboArr.begin(), theSetIter);
		// The combo is between theComboArr.begin() & theSetIter.
		// Print it or do something else.
		theMill.nextKJ (theMin,theMax);
	}
@endcode

@todo   It may still be possible to do this as a classless / stateless
		  algorithm.
@todo   Should there be first_K_SubSet, last_K_Subset, next_K_SubSet?
@todo   Make sure this cannot be called or constructed without a
        container argument.
@todo   Should really use output iterators.
*/
template <typename container_t>
class ComboMill
{
private:
	typedef typename container_t::iterator		iterator;
	typedef typename container_t::size_type		size_t;
	
public:
/// @name LIFECYCLE
//@{
	ComboMill (iterator iStartIter, iterator iStopIter)
	{
		init (iStartIter, iStopIter);
	}

	ComboMill (uint iRangeStart, uint iRangeStop)
	{
		initRange (iRangeStart, iRangeStop);
	}
	
	ComboMill (uint iRangeSize)
	{
		initRange (0, iRangeSize - 1);
	}
//@}


/// @name ACCESSORS
//@{
	// is it all ones?
	bool isLast ()
	{
		for (uint i = 0; i < mMembership.size(); i++)
		{
			if (mMembership[i] == true)
				return false;
		}
		return true;
	}	
	
	void getCurrent (iterator iStartOut, iterator& oStopOut)
	{	
		get (iStartOut, oStopOut, true);
	}

	void getComplement (iterator iStartOut, iterator& oStopOut)
	{	
		get (iStartOut, oStopOut, false);
	}

	void get (iterator iStartOut, iterator& oStopOut, bool iState)
	{	
		iterator theCurrIter = mSeqStart;
		oStopOut = iStartOut;

		for (int i = 0; i < mMembership.size(); i++)
		{
			assert (theCurrIter != mSeqStop);
			
			if (mMembership[i] == iState)
			{
				*oStopOut = *theCurrIter;
				// cout << *oStopOut << " " << *theCurrIter << endl;
				oStopOut++;
			}
			
			theCurrIter++;
		}
	}
	
	uint size ()
	{
		int	theResult;
		theResult = count (mMembership.begin(), mMembership.end(), 1);
		return theResult;
	}
//@}
	
	
/// @name MUTATORS
//@{
	/**
	Move to the next unique combination.
	
	This iteration function examines all combinations of all sizes,
	e.g. from [0, -, -, -, -] to [0, 1, 2, 3, 4].
	
	@note   If you try and iterate past the end of the set, an
	        error should occur.
	@todo   Make operator++
	*/
	void next ()
	{
		assert (not isLast());
		
		for (uint i = 0; i < mMembership.size(); i++)
		{
			if (mMembership[i] == false)
			{
				mMembership[i] = true;
				break;
			}
			else
			{
				mMembership[i] = false;
			}
		}
	}
	
	void previous ()
	{
		assert (0 < mMembership.size());
		
		for (int i = mMembership.size() - 1; 0 <= i; i--)
		{
			if (mMembership[i] == true)
			{
				mMembership[i] = false;
				break;
			}
			else
			{
				mMembership[i] = true;
			}
		}
	}
	
	void first ()
	{
		setMembership (false);
		mMembership[0] = true;
	}
	
	void last ()
	{
		setMembership (true);
	}

	/**
	Move to the first combination of size k.
	*/
	void firstK (uint iSubsetSize)
	{
		// Preconditions:
		assert (0 <= iSubsetSize);
		assert (iSubsetSize <= mMembership.size());
		
		// Main:
		firstKJ (iSubsetSize, iSubsetSize);
	}
	
	void nextK (uint iSubsetSize)
	{
		nextKJ (iSubsetSize, iSubsetSize);
	}
	
	void previousK (uint iSubsetSize) 
	{
		previousKJ (iSubsetSize, iSubsetSize);
	}
	
	void lastK (uint iSubsetSize)
	{
		lastKJ (iSubsetSize, iSubsetSize);
	}

	/**
	Move to the first combination of size kj.
	
	Note that effectively this is the same as moving to the first
	combination of size k (which will be the combination including
	the first k elements) as the value of j has no effect. However
	it is kept here in case the implementation changes.
	*/
	void firstKJ (uint iLowerBound, uint iUpperBound)
	{
		// Preconditions:
		assert (iLowerBound <= iUpperBound);
		assert (0 <= iLowerBound);
		assert (iUpperBound <= mMembership.size());
		
		// Main:
		// very inefficient
		// first();
		// if ((size() < iLowerBound) or (iUpperBound < size())) 
		//	nextKJ (iLowerBound, iUpperBound);
		setMembership (false);
		for (uint i = 0; i < iLowerBound; i++)
			mMembership[i] = true;
	}
	
	void nextKJ (uint iLowerBound, uint iUpperBound)
	{
		do
		{
			next ();
			if (isLast())
				break;
		}
		while ((size() < iLowerBound) or (iUpperBound < size()));
	}
	
	void previousKJ (uint iLowerBound, uint iUpperBound)
	{
		do
		{
			previous ();
		}
		while ((size() < iLowerBound) or (iUpperBound < size()));
	}
	
	void lastKJ (uint iLowerBound, size_t iUpperBound)
	{
		assert (iLowerBound <= iUpperBound);
		assert (0 <= iLowerBound);
		assert (iUpperBound <= mMemberShip.size());
		
		last();
		if ((size() < iLowerBound) or (iUpperBound < size())) 
			previousKJ (iLowerBound, iUpperBound);
	}
//@}

	
/// @name DEPRECATED & DEBUG
//@{
   /*
	void dump ()
	{
		cout << "*** Dumping contents of Combomill at " << &this << ":" << endl;
		cout << "* Subject container:" << endl;
		PrintContainer (mSeqStart,mSeqStop);
		cout << "* Membership std::vector:" << endl;
		PrintContainer (mMembership);
	}
    **/
//@}

	// *** INTERNALS
private:		
	std::vector<bool>   mMembership;
	iterator			     mSeqStart;
	iterator			     mSeqStop;
	std::vector<uint>	  mRange;

	void init (iterator iStartIter, iterator iStopIter)
	{
		mSeqStart = iStartIter;
		mSeqStop = iStopIter;
		
		// init bit std::vector
		iterator theCurrIter = mSeqStart;
		while (theCurrIter != mSeqStop)
		{
			mMembership.push_back(false);
			theCurrIter++;
		}
	}
	
	void initRange (uint iRangeStart, uint iRangeStop)
	{
		for (uint i = iRangeStart; i <= iRangeStop; i++)
			mRange.push_back(i);
		init (mRange.begin(), mRange.end());
	}
		
	void setMembership (bool iIsMember)
	{
		for (uint i = 0; i < mMembership.size(); i++)
		{
			mMembership[i] = iIsMember;
		}
	}
	
};


// *** DEPRECIATED FUNCTIONS *********************************************/

using std::cout;
using std::endl;


void TestComboMill ();

template <class ForwardIter>
void PrintContainer (ForwardIter start, ForwardIter stop)
{
	cout << "Container contents: ";

	if (start != stop)
	{
		cout << *start;
		for (start++; start != stop; start++)
			cout << ", " << *start;
	}
	else
	{
		cout << "-";
	}
	cout << endl;
}

template <class Container>
void PrintContainer (Container& theGroup)
{
	PrintContainer (theGroup.begin(), theGroup.end());
}

#define TESTARRSIZE 5

void TestComboMill ()
{
	cout << "Testing ComboMill" << endl;

	// init test array
	std::vector<int>	theTestArr (TESTARRSIZE);
	for (int i = 0; i < TESTARRSIZE; i++)
		theTestArr[i] = 10 - i;
	
	// init result array
	std::vector<int> theResArr(TESTARRSIZE);
	
	// build combo mill
	ComboMill< std::vector<int> > theTestMill (theTestArr.begin(), theTestArr.end());
	cout << "The whole set is:" << endl;
	PrintContainer(theTestArr);
	
	std::vector<int>::iterator theSetIter = theResArr.begin();

	cout << "*** Generating combinations ..." << endl;
	theTestMill.first();	
	for (int i = 0; i < 40; i++)
	{
		//theTestMill.Dump();
		theTestMill.getCurrent (theResArr.begin(), theSetIter);
		PrintContainer (theResArr.begin(), theSetIter);
		theTestMill.next();
	}

	cout << "*** Generating k subsets ..." << endl;
	theTestMill.firstK(2);	
	for (int i = 0; i < 40; i++)
	{
		//theTestMill.Dump();
		theTestMill.getCurrent (theResArr.begin(), theSetIter);
		PrintContainer(theResArr.begin(), theSetIter);
		theTestMill.nextK(2);
	}

	cout << "*** Generating kj subsets ..." << endl;
	theTestMill.firstKJ(2,3);	
	for (int i = 0; i < 40; i++)
	{
		//theTestMill.Dump();
		theTestMill.getCurrent (theResArr.begin(), theSetIter);
		PrintContainer(theResArr.begin(), theSetIter);
		theTestMill.nextKJ(2,3);
	}
	
	
	cout << "Finished testing ComboMill" << endl;
}


SBL_NAMESPACE_STOP
#endif
// *** END ***************************************************************/



