/**************************************************************************
TraitEvolScheme.cpp - evolutionary trajectory for evolving characters

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>

**************************************************************************/


// *** INCLUDES

#include "MesaTypes.h"
#include "MesaGlobals.h"
#include "CharEvolScheme.h"
#include "ActionUtils.h"
#include "StringUtils.h"
#include "EvolRule.h"


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATIONS ************************************************/

// *** BASIC CLASS *******************************************************/

const char* TraitEvolScheme::describe ()
{
	return "char evolution scheme";
}


// *** NULL CLASS ********************************************************/

void NullCeScheme::evolveChars (nodeiter_t& ioLeafIter, mesatime_t iTime)
{
	iTime = iTime;
	ioLeafIter = ioLeafIter; // just to shut compiler up
}

const char* NullCeScheme::describe ()
{
	return "null trait change scheme";
}


// *** DISCRETE CLASSES **************************************************/
		
disctrait_t& DiscCeScheme::referState (nodeiter_t& ioLeaf)
{
	return referDiscState (ioLeaf, mColIndex);
}

/*			
bool hasState (disctrait_t& iSearchState)
{
	return (find (mCharStates.begin(), mCharStates.end(),
		iSearchState) != mCharStates.end());
}

long findState (disctrait_t& iSearchState)
{
	CharStateSet::iterator p = find (mCharStates.begin(), mCharStates.end(),
		iSearchState);
	assert (p != mCharStates.end());
	return (p - mCharStates.begin());
}

disctrait_t nextState (disctrait_t& iSearchState)
{ 
	long theOldIndex = findState (iSearchState);
	if (theOldIndex != (countStates() - 1))
		theOldIndex++;
	return getState (theOldIndex);
}
	
disctrait_t prevState (disctrait_t& iSearchState)
{ 
	long theOldIndex = findState (iSearchState);
	if (theOldIndex != 0)
		theOldIndex--;
	return getState (theOldIndex);
}

long countStates ()
	{ return long (mCharStates.size()); }
	
disctrait_t getState (long iIndex)
	{ return mCharStates [iIndex]; }
*/


// *** MARKOVIAN DISC

void MarkovianCeScheme::evolveChars (nodeiter_t& ioLeafIter, mesatime_t iTime)
{
	/* 
	Arse. We didn't make use of time here, and that introduced problems when
	doing gradual eveolution of discrete traits. What I'll do now is compare the wait to the
	time.
	*/
	/*
	original
	
	iTime = iTime;
	// disctrait_t theOldState = referState (ioLeafIter);
	disctrait_t theOldState = getDiscData (ioLeafIter, mColIndex);
	if (mCharStates.isMember (theOldState) and (MesaGlobals::mRng.UniformFloat () <= mProb))
	{
		disctrait_t theNewState = "";
		long theNumStates = mCharStates.size();
		while (theNewState == theOldState)
		{
			long theChoice = MesaGlobals::mRng.UniformWhole (theNumStates);
			theNewState = mCharStates[theChoice];
		}
	
		// referState (ioLeafIter) = theNewState;
		setDiscData (ioLeafIter, mColIndex, theNewState);
	}
	*/

	
	iTime = iTime;
	// disctrait_t theOldState = referState (ioLeafIter);
	disctrait_t theOldState = getDiscData (ioLeafIter, mColIndex);
	double theTimeToEvent = calcWaitFromRate (mProb);
	if (mCharStates.isMember (theOldState) and (theTimeToEvent <= iTime))
	{
		disctrait_t theNewState = theOldState;
		long theNumStates = mCharStates.size();
		while (theNewState == theOldState)
		{
			long theChoice = MesaGlobals::mRng.UniformWhole (theNumStates);
			theNewState = mCharStates[theChoice];
		}
	
		// referState (ioLeafIter) = theNewState;
		setDiscData (ioLeafIter, mColIndex, theNewState);
	}	
}

const char* MarkovianCeScheme::describe ()
{
	static std::string theMsg;
	theMsg = "discrete markovian change scheme (p=";
	theMsg.append (sbl::toString (mProb));
	theMsg.append (")");
	return theMsg.c_str();
}


// *** RANKED MARKOVIAN

void RankedMarkovianCeScheme::evolveChars (nodeiter_t& ioLeafIter, mesatime_t iTime)
{
	iTime = iTime;
	disctrait_t theOldState = referState (ioLeafIter);
	if (mCharStates.isMember (theOldState))
	{
		float theProb = MesaGlobals::mRng.UniformFloat ();
		if (theProb <= mProbRise)
		{
			referState (ioLeafIter) = *(mCharStates.nextState (theOldState.c_str()));
		}
		else if (theProb <= (mProbRise + mProbFall))
		{
			referState (ioLeafIter) = *(mCharStates.prevState (theOldState.c_str()));
		}
		else
		{
			// nothing happens
		}
	}
}

const char* RankedMarkovianCeScheme::describe ()
{
	static std::string theMsg;
	theMsg = "discrete ranked markovian change scheme (p=";
	theMsg.append (sbl::toString (mProbFall));
	theMsg.append (", ");
	theMsg.append (sbl::toString (mProbRise));
	theMsg.append (")");
	return theMsg.c_str();
}
			

// *** BINARY
/*
void BinaryCeScheme::evolveChars (nodeiter_t& ioLeafIter, time_t iTime)
{
	disctrait_t theOldState = referState (ioLeafIter);
	if (mCharStates.isMember (theOldState))
	{
		if ((theOldState == mCharStates[0]) and
			(MesaGlobals::mRng.UniformFloat () <= mProbForward))
		{
			// if first state & prob
			referState (ioLeafIter) = mCharStates[1];
		}
		if ((theOldState == mCharStates[1]) and
			(MesaGlobals::mRng.UniformFloat () <= mProbBack))
		{
			// if first state & prob
			referState (ioLeafIter) = mCharStates[0];
		}
	}
}

const char* BinaryCeScheme::describe ()
{
	static std::string theMsg;
	theMsg = "discrete binary change scheme (p=";
	theMsg.append (sbl::toString (mProbForward));
	theMsg.append (", ");
	theMsg.append (sbl::toString (mProbBack));
	theMsg.append (")");
	return theMsg.c_str();
}
			
*/

// *** CONTINUOUS CLASSES ************************************************/
		
conttrait_t& ContCeScheme::referState (nodeiter_t& ioLeaf)
{
	return referContState (ioLeaf, mColIndex);
}


// *** BROWNIAN

void ContBrownianScheme::evolveChars (nodeiter_t& ioLeafIter, mesatime_t iTime)
{
	// Preconditions:
	assert (0.0 <= iTime);
	
	// Main:
	conttrait_t theNewState;
	
	switch (mBoundsBehaviour)
	{
		case kEvolBound_Ignore:
			theNewState = proposeNewState (ioLeafIter, iTime);
			break;
			
		case kEvolBound_Truncate:
			theNewState = proposeNewState (ioLeafIter, iTime);
			if (mRange.hasUpper() and (mRange.getUpper() < theNewState))
				theNewState = mRange.getUpper();
			if (mRange.hasLower() and (theNewState < mRange.getLower()))
				theNewState = mRange.getLower();
			break;
			
		case kEvolBound_Replace:
			do
			{
				theNewState = proposeNewState (ioLeafIter, iTime);
			}
			while (not mRange.isWithin (theNewState));
			break;
			
		default:
			assert (false);
	}
		
		setContData (ioLeafIter, mColIndex, theNewState);
		referState (ioLeafIter) = theNewState;
}
	
conttrait_t ContBrownianScheme::proposeNewState (nodeiter_t& ioLeafIter, mesatime_t iTime)
{
	// get old state
	conttrait_t theOldState = getContData (ioLeafIter, mColIndex);
	if (mIsPunct)
		iTime = 1.0;	
	// generate change
	double theChange = MesaGlobals::mRng.gaussian (mMean * iTime, mStdDev * std::sqrt (iTime));
	// generate & return putative new state
	return (theOldState + theChange);
}

const char* ContBrownianScheme::describe ()
{
	static std::string theMsg;
	theMsg = "cont brownian change scheme (mean ";
	theMsg.append (sbl::toString (mMean));
	theMsg.append (", std dev ");
	theMsg.append (sbl::toString (mStdDev));
	theMsg.append (" per time)");
	return theMsg.c_str();
}
		
void ContBrownianScheme::validate ()
{
	mRange.validate();
}

					
					
// *** LOG NORMAL
#pragma mark -
	
conttrait_t ContLogNormalScheme::proposeNewState (nodeiter_t& ioLeafIter, mesatime_t iTime)
{
	// get old state
	conttrait_t theOldState = getContData (ioLeafIter, mColIndex);
	if (mIsPunct)
		iTime = 1.0;	
	// generate change
	double theChange = MesaGlobals::mRng.gaussian (mMean * iTime, mStdDev * std::sqrt (iTime));
	conttrait_t theNewState = std::exp (std::log (theOldState) + theChange);
	// generate & return putative new state
	return (theNewState);
}

const char* ContLogNormalScheme::describe ()
{
	static std::string theMsg;
	theMsg = "cont log normal change scheme (mean ";
	theMsg.append (sbl::toString (mMean));
	theMsg.append (", std dev");
	theMsg.append (sbl::toString (mStdDev));
	theMsg.append (" per time)");
	return theMsg.c_str();
}

					
					
// *** SCHEME STORAGE
// so the schemes within are disposed of automgaically
#pragma mark -

SchemeArr::~SchemeArr ()
{
	for (iterator p = begin(); p != end(); p++)
		delete *p;
}

void SchemeArr::adopt (TraitEvolScheme* iSchemeP)
//: insert and become responsible for a scheme
{
	push_back (iSchemeP);
}

void SchemeArr::adopt (SchemeArr& iScheme)
//: insert and become responsible for another array's schemes
// The other array therefore orphan's the others.
{
	for (iterator p = iScheme.begin(); p != iScheme.end(); p++)
		push_back (*p);
	iScheme.resize(0);
}

void SchemeArr::evolveChars (nodeiter_t& ioLeaf, mesatime_t iTime)
{
	for (iterator p = begin(); p != end(); p++)
	{
		(*p)->evolveChars (ioLeaf, iTime);
	}
}

void SchemeArr::remove (size_type iIndex)
{
	assert (0 <= iIndex);
	assert (iIndex < size());
	delete at (iIndex);
	erase (begin() + iIndex);
}

void SchemeArr::removeAll ()
{
	while (0 < size())
		remove (0);
}

const char* SchemeArr::describe (int iIndex)
{
	return (at(iIndex))->describe();
}

// *** END ***************************************************************/



