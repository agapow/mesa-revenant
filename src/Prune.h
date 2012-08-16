/**************************************************************************
Prune.h - trim the active tree by pruning taxa away

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2001, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "Action.h"
#include <string>


// *** CONSTANTS & DEFINES

class CharacterTest;

// *** BASIC PRUNE ******************************************************/

class BasicPrune: public BasicAction
//: an action that trims the active tree according to some criteria
{
public:
	// LIFECYCLE
	BasicPrune (const char* iId)
		: /* BasicAction (iId) */ BasicAction ()
		{}
		
	// I/O
	std::string describe ();
	
	virtual std::string initDesc () = 0;
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
};


// *** PRUNE *************************************************************/

// *** PRUNE N

class PruneN: public BasicPrune
//: prune N random taxa from the active tree
{
public:
	// LIFECYCLE
	PruneN (int iBodyCount)
		: BasicPrune ("prune_n"),
		mBodyCount (iBodyCount)
		{}
	
	// SERVICE
	void execute ();
		
	// I/O
	std::string initDesc ();

// INTERNALS
private:
	int mBodyCount;
};


// *** PRUNE FRACTION

class PruneFraction: public BasicPrune
//: prune a randomly selected fraction of taxa from the active tree
{
public:
	// LIFECYCLE
	PruneFraction (double iKillFraction)
		: mKillFraction (iKillFraction),
		BasicPrune ("prune_fraction")
		{}
	
	// SERVICE
	void execute ();
		
	// I/O
	std::string initDesc ();

// INTERNALS
private:
	double mKillFraction;
};


// *** PRUNE CHANCE

class PruneChance: public BasicPrune
//: prune every taxa from the active tree with a given chance
{
public:
	// LIFECYCLE
	PruneChance (double iKillChance)
		: mKillChance (iKillChance),
		BasicPrune ("prune_chance")
		{}
	
	// SERVICE
	void execute ();
		
	// I/O
	std::string initDesc ();

// INTERNALS
private:
	double mKillChance;
};


// *** PRUNE CHARACTER

class PruneCharacter: public BasicPrune
//: prune taxa based on a character state
// GOTCHA: this action is assumed to own the character test it uses.
{
public:
	// LIFECYCLE
	PruneCharacter (CharacterTest* iTestP)
		: mTestP (iTestP),
		BasicPrune ("prune_character")
		{}
	~PruneCharacter ();
		
	// SERVICE
	void execute ();
		
	// I/O
	std::string initDesc ();

// INTERNALS
private:
	CharacterTest* mTestP;
};

// *** END ***************************************************************/



