/**************************************************************************
Prune.h - trim the active tree by pruning taxa away

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
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
		: BasicPrune ("prune_fraction")
		, mKillFraction (iKillFraction)
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
		: BasicPrune ("prune_chance")
		, mKillChance (iKillChance)
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
		: BasicPrune ("prune_character")
		, mTestP (iTestP)
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



