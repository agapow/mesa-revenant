/**************************************************************************
XMembership.h - a container for quick reference of presence/absence

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef XMEMBERSHIP_H
#define XMEMBERSHIP_H


// *** INCLUDES

#include "Sbl.h"
#include <map>

SBL_NAMESPACE_START


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

template <typename X>
class XMembership
{
private:
// INTERNAL TYPES
	typedef std::map <X, bool>     impl_type;

public:
// PUBLIC TYPE INTERFACE
	typedef typename impl_type::size_type   size_type;

// LIFECYCLE
	// use defaults

// ACCESSORS
	bool        isMember (const X& iValue) const;
	size_type   size () const;
		
// MUTATORS
	void        insert (const X& iValue);
	void        remove (const X& iValue);

// DEPRECIATE & DEBUG
	void        validate () {}
	void        dump (const char* iPrefixCstr = "");

// INTERNALS
private:
	std::map <X, bool>   mData;
};



// *** CLASS DEFINITION **************************************************/

// *** ACCESSORS

template <typename X>
bool
XMembership <X>::isMember (const X& iValue) const
//: is this value on of the membership?
{
	return (mData.find (iValue) != mData.end());
}

template <typename X>
typename XMembership <X>::size_type
XMembership <X>::size () const
{
	return mData.size();
}
	
	
// *** MUTATORS
template <typename X>
void
XMembership <X>::insert (const X& iValue)
{
	mData [iValue] = true;
}

template <typename X>
void
XMembership <X>::remove (const X& iValue)
{
	mData.erase (iValue);
}


// *** DEPRECIATED & DEBUG
template <typename X>
void
XMembership <X>::dump (const char* iPrefixCstr)
{
	DBG_MSG (iPrefixCstr << "Dumping XMembership at " << this << ":");
	DBG_MSG (iPrefixCstr << "Size: " << size() << "; Contents:");
	typename impl_type::iterator q;
	for (q = mData.begin(); q != mData.end(); q++)
		DBG_MSG (iPrefixCstr << "   " << q->first)
	DBG_MSG (iPrefixCstr << "Finished dumping XMembership");
}



SBL_NAMESPACE_STOP

#endif
// *** END ***************************************************************/



