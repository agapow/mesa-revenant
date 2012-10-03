/**************************************************************************
CharComparator.h - a function for making comparsions between characters

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

Notes:
- should be called CharTest.h

**************************************************************************/

#pragma once
#ifndef CHARCOMPARATOR_H
#define CHARCOMPARATOR_H


// *** INCLUDES

#include "MesaTypes.h"
#include "MesaTree.h"


// *** CONSTANTS & DEFINES

enum comparator_t
{
	kComparator_LessThan = 0,
	kComparator_LessThanOrEqual,
	kComparator_Equal,
	kComparator_NotEqual,
	kComparator_GreaterThan,
	kComparator_GreaterThanOrEqual,
	
	kComparator_NumItems
};	
	

// *** CLASS DECLARATION *************************************************/

class CharComparator
//: a functor for comparing character states
{
public:
	// LIFECYCLE
	CharComparator ()
		{}
	CharComparator
		(colIndex_t iCharIndex, comparator_t iComp, conttrait_t iRhsVal);
	CharComparator
		(colIndex_t iCharIndex, comparator_t iComp, disctrait_t iRhsVal);
	
	// SERVICES
	bool testCharacter (nodeiter_t iNode);

	// MUTATORS
	
	// I/O
	const char*   describe ();
	
	// DEBUG & DEPRECIATED
	void validate ();
	
	// void          dump ();

	// INTERNALS
private:
	traittype_t    mCharType;    // character type
	colIndex_t     mCharIndex;   // character index
	comparator_t   mComp;        // comparision operator
	disctrait_t     mDiscRhsVal;  // right-hand-side value if discrete
	conttrait_t     mContRhsVal;  // right-hand-side value if continuous
	
	void init (colIndex_t iColIndex, comparator_t iComp);

	bool testCharacterCont (nodeiter_t iNode);
	bool testCharacterDisc (nodeiter_t iNode);		
};



#endif
// *** END ***************************************************************/



