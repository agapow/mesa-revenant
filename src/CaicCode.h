/**
@file
@author   Paul-Michael Agapow
          Dept. Biology, University College London, WC1E 6BT, UK.
          <mail://p.agapow@ic.ac.uk> 
          <http://www.agapow.net/code/sibil/>
*/

#pragma once
#ifndef SBL_CAICCODE_H
#define SBL_CAICCODE_H


// *** INCLUDES

#include "Sbl.h"
#include <string>
#include <iostream>

SBL_NAMESPACE_START


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

/**
A representation of CAIC format phylogenetic codes.
*/
class CaicCode
{
public:
	typedef std::string::size_type	size_type;

/// @name LIFECYCLE
//@{
	CaicCode (); 
	CaicCode (char iCodeChar);
	CaicCode (const char* iCodeCstr);
	CaicCode (size_type iSppIndex);
//@}

/// @name ACCESSORS
//@{
	bool operator == (const std::string& ikString);
	bool operator == (const char* ikCstr);
	bool operator != (const std::string& ikString);
	bool operator != (const char* ikCstr);

	char& operator []	(int iIndex);

	bool	isParentOf		(CaicCode& ikChild);
	bool	isChildOf		(CaicCode& ikParent);
	bool	isAncestorOf	(CaicCode& ikDescendant);
	bool	isDescendentOf	(CaicCode& ikAncestor);
	bool	isSiblingOf		(CaicCode& ikSibling);

	size_type				size ();

	const char*				cstr () const;
	const std::string& 	str () const;
//@}

/// @name MUTATORS
//@{
	CaicCode&	operator =	(const std::string& ikString)
		{ mCode = ikString; return *this; }
	CaicCode&	operator =	(const char* ikCstr)
		{ mCode = ikCstr; return *this; }

	void increment		();
	void extend			();
	void truncate		();
	char popFront     ();

//@}
	
/// @name SERVICES
//@{
	static char				indexToChar (size_type iCladeIndex);
	static size_type		charToIndex (char iCladeChar);
	static char				nextChar (char iCladeChar);
//@}

/// @name DEBUG & DEPRECATED
//@{
	const char*				CString		() const	{ return cstr (); }
	const std::string& 	StlString	() const { return str (); }
//@}


// INTERNALS
private:
	std::string		mCode;	// actual character representation
	
	void init		(const char* iCodeCstr);
};


// *** AUXILLIARY & HELPER FXNS ******************************************/

std::ostream& operator << (std::ostream& ioStream, CaicCode& iCode);


SBL_NAMESPACE_STOP
#endif
// *** END ***************************************************************/



