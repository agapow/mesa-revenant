/**************************************************************************
Epoch.h - an action encapsulating an evolutionary epoch

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef EPOCH_H
#define EPOCH_H


// *** INCLUDES

#include "Macro.h"
#include "EvolRule.h"


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class EpochMacro: public BasicMacro
//: a macro that runs the contained events as a simulation
{
public:
	// LIFECYCLE
	EpochMacro ()
		{}
	virtual ~EpochMacro ()
		{}
	
	// ACESSORS
	bool hasSpeciationRules ();
	bool hasKillRules ();

	
	// SERVICES
	virtual void	execute ();
	virtual void   executeEpochLoop ();
	virtual void	executeEpochOnce ();

	void			sortRules ();
	EvolRule*	findFirstRule (nodeiter_t& oFiringLeaf, mesatime_t& oTime);
	void			commitAction (EvolRule* iRuleP, nodearr_t& ioLeafI, mesatime_t iTime);
	void			fireConditionals (EvolRule* iRuleP, nodearr_t& ioLeafI, mesatime_t iTime);

	virtual bool isAtEnd () { assert (false); return true; }
	
	// I/O
	const char* describe (size_type iIndex);
	virtual const char* describeEpoch () = 0;
	const char* describeMacro ()  { assert (false); return ""; }

	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
   bool mRestartIfDead;

private:
	std::vector<LocalRule*>			theLocalRules;
	std::vector<GlobalRule*>		theGlobalRules;
	std::vector<ConditionalRule*>	theCondRules;
};


class EpochPopLimit: public EpochMacro
//: run the rules until the pop limit is reached
{
public:
	EpochPopLimit (long iPopLimit, bool iAdvance, nodetype_t iNodeType, bool iRestart)
		: mPopLimit (iPopLimit)
		, mAdvance (iAdvance)
		, mNodeType (iNodeType)
		{ assert (0 < mPopLimit); }

	// SERVICES
	bool	isAtEnd ();
	void	executeEpochLoop ();

	// I/O
	const char* describeEpoch ();
	
	// INTERNALS
private:
	long mPopLimit;
	bool mAdvance;
	nodetype_t mNodeType;
};



class EpochTimeLimit: public EpochMacro
//: run the rules until the time limit is reached
{
public:
	EpochTimeLimit (mesatime_t iTimeLimit, bool iRestart)
		: mTimeLimit (iTimeLimit)
		{ assert (0.0 < iTimeLimit); mRestartIfDead = iRestart; }

	// SERVICES
	bool	isAtEnd ();

	// I/O
	const char* describeEpoch ();
	
	// INTERNALS
private:
	mesatime_t mTimeLimit;
};



// *** UTILITY FUNCTIONS *************************************************/

EpochMacro* castAsEpoch (BasicAction* iActionP);


#endif
// *** END ***************************************************************/



