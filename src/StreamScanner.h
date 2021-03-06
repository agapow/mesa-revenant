/**************************************************************************
StreamScanner - a reader/lexer for parsing a stream

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <mail://a.burt@ic.ac.uk>
  <http://www.bio.ic.ac.uk/evolve/>

About:
- breaks an istream up into logical units for parsing.

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "BasicScanner.h"
#include <iostream>


SBL_NAMESPACE_START

// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class StreamScanner: public BasicScanner
{
public:
// LIFECYCLE
	StreamScanner	()
		: mSrcStreamP (NULL)
		{}
	StreamScanner	(std::istream& iStream)
		: mSrcStreamP (&iStream)
		{}
	~StreamScanner	()
		{}
	
// ACCESS
	void	SetSource (std::istream& iSrcStream);

	// To Do: this should be in the ABC
	operator	bool () const
		{ return ((mSrcStreamP != NULL) and (bool (*mSrcStreamP))); }
	
// SERVICES
	// low level, obligate overrides
	posn_t	GetPosn 	();						// where	
	posn_t	Goto 		(posn_t iPosn);			// go to given posn
	bool 		GetChar	(char& oCurrChar);	// get next char, return sucess


// DEPRECIATED

	void 		UnreadChar	(char iLastChar);

// INTERNALS
	
private:
	std::istream*	mSrcStreamP;
};


SBL_NAMESPACE_STOP

// *** END ***************************************************************/





