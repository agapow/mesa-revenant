/**************************************************************************
CharStateSet.h - a collection of possible character states

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef CHARSTATESET_H
#define CHARSTATESET_H


// *** INCLUDES

#include "MesaTypes.h"
#include <vector>
#include <string>


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class CharStateSet: public std::vector <disctrait_t>
//: holds a group of discrete character states
{
	// TYPE INTERFACE
	typedef std::vector <disctrait_t>    base_type;
	
public:
	// LIFECYCLE
	CharStateSet ()
		{}
	
	// ACCESS
	bool          isMember (const char* iSearchCStr);
	bool          isMember (disctrait_t& iSearchStr);
	iterator      find (const char* iSearchCStr);
	iterator      nextState (const char* iStateCStr);
	iterator      prevState (const char* iStateCStr);
	
	// MUTATORS
	void          sort ();
	void          addState (const char* iStateCstr);
	void          addState (disctrait_t iDelState);
	void          deleteState (long iDelIndex);
	void          deleteState (disctrait_t iDelState);
	void          deleteState (const char* iDelCstr);
	
	// I/O
	std::string   str (const char* iDelim = " ");
	
	// DEBUG & DEPRECIATED
	void          validate ();
	void          dump ();
};



#endif
// *** END ***************************************************************/



