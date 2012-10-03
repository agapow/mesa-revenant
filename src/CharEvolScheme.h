/**************************************************************************
TraitEvolScheme.h - evolutionary trajectory for evolving characters

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef CHAREVOLSCHEME_H
#define CHAREVOLSCHEME_H


// *** INCLUDES

#include "MesaTree.h"
#include "MesaTypes.h"
#include "CharStateSet.h"
#include "XBounds.h"
#include <vector>
#include <string>
#include <cassert>


// *** CONSTANTS & DEFINES

typedef sbl::XBounds<conttrait_t>   contcharrange_t;

// *** CLASS DECLARATIONS ************************************************/

// *** BASIC CLASS *******************************************************/

class TraitEvolScheme
//: details the changing of a character
{
public:
	// LIFECYCLE
	TraitEvolScheme ()
		{}
		
	TraitEvolScheme (int iColIndex)
		: mColIndex (iColIndex)
		{}

	virtual ~TraitEvolScheme() {}
		
	virtual void evolveChars (nodeiter_t& ioLeafIter, mesatime_t iTime)
		{
			iTime = iTime;
			ioLeafIter = ioLeafIter; // to shut compiler up
			assert (false);
		}	
	// int& referColIndex ();
	virtual const char* describe ();
	
	int mColIndex;
};


// *** NULL CLASS ********************************************************/

class NullCeScheme: public TraitEvolScheme
//: nothing changes
{
public:
	void evolveChars (nodeiter_t& ioLeafIter, mesatime_t iTime);
	const char* describe ();
};


// *** DISCRETE CLASSES **************************************************/

class DiscCeScheme: public TraitEvolScheme
{
public:	
	DiscCeScheme (int iColIndex, CharStateSet iStateSet)
		: TraitEvolScheme (iColIndex)
		, mCharStates (iStateSet)
	{
		assert (1 < mCharStates.size());
	}

	disctrait_t& referState (nodeiter_t& ioLeaf);

	CharStateSet   mCharStates;
};


class MarkovianCeScheme: public DiscCeScheme
//: an equal chance of going to any other possible state
{
public:
	MarkovianCeScheme (int iColIndex, CharStateSet iStateSet, float iProb)
		: DiscCeScheme (iColIndex, iStateSet)
		, mProb (iProb)
	{
		assert (0.0 < mProb);
		assert (mProb <= 1.0);
	}

	void evolveChars (nodeiter_t& ioLeafIter, mesatime_t iTime);
	const char* describe ();
			
	float mProb;
};


class RankedMarkovianCeScheme: public DiscCeScheme
//: an equal chance of going to any other possible state
{
public:
	RankedMarkovianCeScheme
		(int iColIndex, CharStateSet iStateSet, float iProbFall, float iProbRise)
		: DiscCeScheme (iColIndex, iStateSet)
		, mProbRise (iProbRise)
		, mProbFall (iProbFall)
	{
		assert (0.0 < iProbRise);
		assert (iProbRise < 1.0);
		assert (0.0 < iProbFall);
		assert (iProbFall < 1.0);
		assert ((iProbRise + iProbFall) <= 1.0);
	}

	void evolveChars (nodeiter_t& ioLeafIter, mesatime_t iTime);
	const char* describe ();
		
	float mProbRise, mProbFall;
};


/*
class BinaryCeScheme: public DiscCeScheme
//: a rule for flip-flopping between two chars
{
public:
	BinaryCeScheme
		(int iColIndex, CharStateSet iStateSet, float iProbForward, float iProbBack)
		: DiscCeScheme (iColIndex, iStateSet)
		, mProbForward (iProbForward)
		, mProbBack (iProbBack)
	{
		assert (0.0 <= iProbForward);
		assert (iProbForward <= 1.0);
		assert (0.0 <= iProbBack);
		assert (iProbBack <= 1.0);
	}

	void evolveChars (nodeiter_t& ioLeafIter, time_t iTime);
	const char* describe ();
		
	float mProbForward, mProbBack;
};
*/

// *** CONTINUOUS CLASSES ************************************************/

class ContCeScheme: public TraitEvolScheme
{
public:
	ContCeScheme (int iColIndex)
		: TraitEvolScheme (iColIndex)
		{}
		
	conttrait_t& referState (nodeiter_t& ioLeaf);
};


class ContBrownianScheme: public ContCeScheme
//: an equal chance of going to any other possible state
{
public:
	ContBrownianScheme
		(int iColIndex, double iMean, double iStdDev, bool iIsPunct, contcharrange_t& iRange,
		evolbound_t iBoundsBehaviour)
		: ContCeScheme (iColIndex)
		, mMean (iMean)
		, mStdDev (iStdDev)
		, mIsPunct (iIsPunct)
		, mRange (iRange)
		, mBoundsBehaviour (iBoundsBehaviour)
		{ validate (); }
					
	void evolveChars (nodeiter_t& ioLeafIter, mesatime_t iTime);
	virtual conttrait_t proposeNewState (nodeiter_t& ioLeafIter, mesatime_t iTime);
	const char* describe ();
	void validate ();
	
	double            mMean;
	double            mStdDev;
	bool              mIsPunct;
	contcharrange_t   mRange;
	evolbound_t       mBoundsBehaviour;
};



class ContLogNormalScheme: public ContBrownianScheme
//: log-normal variant of brownian scheme, after Heard
{
public:
	ContLogNormalScheme
		(int iColIndex, double iMean, double iStdDev, bool iIsPunct, contcharrange_t& iRange,
		evolbound_t iBoundsBehaviour)
		: ContBrownianScheme (iColIndex, iMean, iStdDev, iIsPunct, iRange, iBoundsBehaviour)
		{ validate (); }
					
	virtual conttrait_t    proposeNewState (nodeiter_t& ioLeafIter, mesatime_t iTime);
	virtual const char*   describe ();
};

					
					
// *** SCHEME STORAGE
// so the schemes within are disposed of automgaically

class SchemeArr: public std::vector <TraitEvolScheme*>
{
	typedef std::vector <TraitEvolScheme*>   base_type;

public:
	~SchemeArr ();
	
	void          adopt (TraitEvolScheme* iSchemeP);
	void          adopt (SchemeArr& iScheme);
	void          remove (size_type iIndex);
	void          removeAll ();
	void          evolveChars (nodeiter_t& ioLeaf, mesatime_t iTime);
	const char*   describe (int iIndex);
};



#endif
// *** END ***************************************************************/



