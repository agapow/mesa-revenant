/**************************************************************************
StringScanner - a reader/lexer for parsing an STL string

Credits:
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial
  College at Silwood Park, Ascot, Berks, SL5 7PY, UK.
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "BasicScanner.h"
#include <string>

SBL_NAMESPACE_START


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class StringScanner: public BasicScanner
{
public:
// LIFECYCLE
	StringScanner	()
		: mSrcStringP (NULL), mPosn (kScan_SrcBegin)
		{}
	StringScanner	(std::string& iString)
		: mSrcStringP (&iString), mPosn (kScan_SrcBegin)
		{}
	~StringScanner	()
		{}
	
// ACCESS
	void	SetSource (std::string& iString)
		{ mSrcStringP = &iString; }

	// To Do: this should be in the ABC
	operator	bool () const
	{
		// return (mSrcStringP != NULL);

		if ((mSrcStringP == NULL) or ((int) mPosn == kScan_SrcEnd))
			return false;
		else
			return true;
	}
	
// SERVICES
	// low level, obligate overrides
	posn_t	GetPosn 	();						// where	
	posn_t	Goto 		(int iPosn);			// go to given posn
	bool 		GetChar	(char& oCurrChar);	// get next char, return sucess


// DEPRECIATED

	void 		UnreadChar	(char iLastChar);

// INTERNALS
	
public:
	std::string*				mSrcStringP;
	std::string::size_type	mPosn;
};


SBL_NAMESPACE_STOP

// *** END ***************************************************************/





