/**************************************************************************
EvolRule.h - an evolutionary rule

Credits:
- Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef EVOLRULE_H
#define EVOLRULE_H


// *** INCLUDES

#include "Action.h"
#include "MesaTree.h"
#include "XRate.h"
#include "CharComparator.h"


// *** CONSTANTS & DEFINES

class XBasicRate;


// *** CLASS DECLARATION *************************************************/

class EvolRule: public BasicAction
//: a statement of an evolutionary rule
{
public:
	// LIFECYCLE
	EvolRule ()
		{}
				
	virtual ~EvolRule ()
		{}
		
	// SERVICES
	//time_t calcWaitFromRate (time_t iRate);
	
	// SERVICES
	void execute ();
	virtual void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	
	// I/O
	virtual const char* describe (size_type iIndex);
	virtual const char* describeRule () = 0;
	void deleteElement (size_type iIndex) { assert (false); iIndex = iIndex; }
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
};


class ConditionalRule: public EvolRule
//: a rule that may be triggered by another
{
public:
	// LIFECYCLE
	ConditionalRule ()
		{}
				
	virtual ~ConditionalRule ()
		{}
		
	// ACCESSORS
	// virtual bool isTriggered (EvolRule* iFiringRuleP, nodeiter_t& ioFiringLeaf);
	virtual bool isTriggered (EvolRule* iFiringRuleP, nodearr_t& ioFiringLeaves);
		
	// I/O
	const char* describeRule ();
	
	// DEPRECIATED & DEBUG

	// INTERNALS
private:
};


class GlobalRule: public EvolRule
//: a rule that acts across the whole of a tree
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
	GlobalRule ()
		{}
				
	virtual ~GlobalRule ()
		{}
		
	// ACCESSORS
	virtual mesatime_t calcNextWait ();
	
	// MUTATORS
		
	// SERVICES

	// I/O
	const char* describeRule ();
	
	// DEPRECIATED & DEBUG

	// INTERNALS
private:
};


class LocalRule: public EvolRule
//: a rule that acts across the whole of a tree
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
	LocalRule ()
		{}
				
	virtual ~LocalRule ()
		{}
		
	// SERVICES
	virtual mesatime_t calcNextWait (nodeiter_t iLeafIter);
	virtual void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	
	// I/O
	virtual const char* describeRule ();
	
	// DEPRECIATED & DEBUG

	// INTERNALS
private:
};


class NullRule: public GlobalRule
//: an evolutionary rule that does nothing, just ticks over
// The Null rule must provide a commit action, because when it is chosen
// it will be run. 
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
	NullRule (mesatime_t iRate)
		: mRate (iRate)
		{}
		
	// SERVICES
	 mesatime_t calcNextWait ();
	
	// SERVICES
	virtual void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);

	// I/O
	virtual const char* describeRule ();
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
	double mRate;
};


class MassKillRule: public GlobalRule
{
public:
	// LIFECYCLE
	MassKillRule (mesatime_t iRate)
		: mRate (iRate)
		{}
		
	// SERVICES
	mesatime_t calcNextWait ();
	void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	virtual nodearr_t selectTargets ();
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	double   mRate;
};


class MassKillFixedNumRule: public MassKillRule
//: a mass extinction that kills a fixed number of species
{
public:
	// LIFECYCLE
	MassKillFixedNumRule (mesatime_t iRate, int iAbsNum)
		: MassKillRule (iRate), mAbsNum (iAbsNum)
		{}
		
	// SERVICES
	nodearr_t selectTargets ();
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	int   mAbsNum;
	
	void validate ()
	{
		assert (0 <= mAbsNum);
	}
};


class MassKillPercentRule: public MassKillRule
//: a mass extinction that kills a percentage of species
{
public:
	// LIFECYCLE
	MassKillPercentRule (mesatime_t iRate, double iPercent)
		: MassKillRule (iRate), mPercent (iPercent)
		{}
		
	// SERVICES
	nodearr_t selectTargets ();
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	double   mPercent;
	
	void validate ()
	{
		assert (0.0 <= mPercent);
	}
};


class MassKillProbRule: public MassKillRule
//: a mass extinction that has x probability of killing any species
{
public:
	// LIFECYCLE
	MassKillProbRule (mesatime_t iRate, double iProb)
		: MassKillRule (iRate), mProb (iProb)
		{}
		
	// SERVICES
	nodearr_t selectTargets ();
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	int   mProb;
	
	void validate ()
	{
		assert (0.0 <= mProb);
	}
};

class MassKillTraitBiasedRule: public MassKillRule
//: a mass extinction that has x probability of killing any species
{
public:
	// LIFECYCLE
	MassKillTraitBiasedRule (mesatime_t iRate, colIndex_t iTraitCol, double iParamA,
				double iParamB, double iParamC)
		: MassKillRule (iRate)
		, mTraitIndex (iTraitCol)
		, mTriParamA (iParamA)
		, mTriParamB (iParamB)
		, mTriParamC (iParamC)
		{}
 
		
	// SERVICES
	nodearr_t selectTargets ();
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	colIndex_t   mTraitIndex;
	double       mTriParamA;
	double       mTriParamB;
	double       mTriParamC;
		
	void validate ()
	{
		assert (0 <= mTraitIndex);
	}
};


class MassKillIf: public MassKillRule
//: a mass extinction that takes species that meet a set condition
{
public:
	// LIFECYCLE
	MassKillIf (mesatime_t iRate, CharComparator& iSppTest)
		: MassKillRule (iRate), mSppTest (iSppTest)
		{}
		
	// SERVICES
	nodearr_t selectTargets ();
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	CharComparator   mSppTest;
	
	void validate ()
	{
		mSppTest.validate ();
	}
};


class EndOfEpochRule: public EvolRule
//: a rule that is transmitted into the system as a signal
{
public:
	// LIFECYCLE
	EndOfEpochRule ()
		{}
				
	// SERVICES
	void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);

	// I/O
	const char* describeRule ();
};




class MarkovSpRule: public LocalRule
//: a rule that allows every spp to speciate with equal likelyhood
{
public:
	// LIFECYCLE
	MarkovSpRule (mesatime_t iRate)
		: mRate (iRate)
		{}

	// SERVICES
	mesatime_t calcNextWait (nodeiter_t iLeafIter);
	void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
	double mRate;
};

class LogisticSpRule: public MarkovSpRule
//: a rule that allows every spp to speciate with equal likelyhood, limited
// by a carrying capacity
{
public:
	// LIFECYCLE
	LogisticSpRule (mesatime_t iRate, int capacity)
		: MarkovSpRule (iRate)
	{
      mCapacity = capacity;
   }

	// SERVICES
	mesatime_t calcNextWait (nodeiter_t iLeafIter);
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	int mCapacity;
};


class AgeBiasedSpRule: public LocalRule
//: a rule that gives a shifting chance of spp depending on sp age
{
public:
	// LIFECYCLE
	AgeBiasedSpRule (double iRateA, double iRateB, double iRateC)
		: mRateA (iRateA), mRateB (iRateB), mRateC (iRateC)
		{}

	// SERVICES
	mesatime_t calcNextWait (nodeiter_t iLeafIter);
	void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	double mRateA, mRateB, mRateC;
};


class CharBiasedSpRule: public LocalRule
//: a rule that gives a shifting chance of spp depending on character value
{
public:
	// LIFECYCLE
	CharBiasedSpRule (colIndex_t iCharCol, mesatime_t iRateA, mesatime_t iRateB, mesatime_t iRateC)
		: mRateA (iRateA)
		, mRateB (iRateB)
		, mRateC (iRateC)
		, mCharCol (iCharCol)
		{}

	// SERVICES
	mesatime_t	calcNextWait (nodeiter_t iLeafIter);
	void		commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	double	mRateA, mRateB, mRateC;
	int		mCharCol;
};

class CharBiasedSpRule_New: public LocalRule
//: a rule that gives a shifting chance of spp depending on character value
{
public:
	// LIFECYCLE
	CharBiasedSpRule_New (XBasicRate* iRateP)
		: mRateP (iRateP)
		{}

	~CharBiasedSpRule_New ()
		// dtor
		{ if (mRateP != NULL) delete mRateP; }
		
	// SERVICES
	mesatime_t	calcNextWait (nodeiter_t iLeafIter);
	void		commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	XBasicRate*	mRateP;
};


class CharBiasedKillRule: public LocalRule
//: a rule that gives a shifting chance of death depending on character value
{
public:
	// LIFECYCLE
	CharBiasedKillRule (colIndex_t iCharCol, mesatime_t iRateA, mesatime_t iRateB, mesatime_t iRateC)
		: mRateA (iRateA)
		, mRateB (iRateB)
		, mRateC (iRateC)
		, mCharCol (iCharCol)
		{}

	// SERVICES
	mesatime_t	calcNextWait (nodeiter_t iLeafIter);
	void		commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	double	mRateA, mRateB, mRateC;
	int		mCharCol;
};

class BiasedKillRule: public LocalRule
//: a rule that gives a shifting chance of extinction depending on sp age
{
public:
	// LIFECYCLE
	BiasedKillRule (mesatime_t iRateA, mesatime_t iRateB, mesatime_t iRateC)
		: mRateA (iRateA), mRateB (iRateB), mRateC (iRateC)
		{}

	// SERVICES
	mesatime_t calcNextWait (nodeiter_t iLeafIter);
	void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	
	// I/O
	const char* describeRule ();
	
	// INTERNALS
private:
	double mRateA, mRateB, mRateC;
};


class LatentSpRule: public LocalRule
//: a rule that forbids speciation for a given period
{
public:
	// LIFECYCLE
	LatentSpRule (mesatime_t iRate, mesatime_t iLatentPeriod)
		: mRate (iRate), mLatencyPeriod (iLatentPeriod)
		{}

	// SERVICES
	mesatime_t calcNextWait (nodeiter_t iLeafIter);
	void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
		
	// I/O
	const char* describeRule ();
	
	// INTERNALS	
private:
	mesatime_t 	mRate;
	mesatime_t	mLatencyPeriod;
};


class MarkovKillRule: public LocalRule
//: a rule that allows every spp go extinct with equal likelihood
{
public:
	// LIFECYCLE
	MarkovKillRule (mesatime_t iRate)
		: mRate (iRate)
		{}

	// SERVICES
	mesatime_t calcNextWait (nodeiter_t iLeafIter);
	void commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime);
	
	// I/O
	const char* describeRule ();

	// INTERNALS	
	mesatime_t mRate;
};

class LogisticKillRule: public MarkovKillRule
//: a rule that allows every spp go extinct with equal likelihood, with an
// incresing global rate that maximise at a capacity
{
public:
	// LIFECYCLE
	LogisticKillRule (mesatime_t iRate, int capacity)
		: MarkovKillRule (iRate)
	{
      mCapacity = capacity;
   }

	// SERVICES
	mesatime_t calcNextWait (nodeiter_t iLeafIter);
	
	// I/O
	const char* describeRule ();

	// INTERNALS	
private:
	int mCapacity;
};



// *** UTILITY FUNCTIONS *************************************************/

EvolRule*          castAsEvolRule (BasicAction* iActionP);
GlobalRule*        castAsGlobalRule (BasicAction* iActionP);
LocalRule*         castAsLocalRule (BasicAction* iActionP);
ConditionalRule*   castAsConditionalRule (BasicAction* iActionP);

bool     isSpeciationRule (EvolRule* iRuleP);
bool     isKillRule (EvolRule* iRuleP);
bool     isSpeciationOrKillRule (EvolRule* iRuleP);

void     setRandomSeed (long iSeed);

mesatime_t   calcWaitFromRate (mesatime_t iRate);
mesatime_t   calcRateFromTriParameter (double iA, double iB, double iC, conttrait_t iCharVal);
mesatime_t   calcProbFromTriParameter (double iA, double iB, double iC, conttrait_t iCharVal);



#endif
// *** END ***************************************************************/



