/**************************************************************************
MesaUtils.cpp - global utility functions

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

**************************************************************************/


// *** INCLUDES

#include "MesaUtils.h"
#include "StringUtils.h"
#include <algorithm>
#include <cctype>

using std::string;
using std::replace;
using std::isspace;


// *** CONSTANTS & DEFINES

// *** UTILITY FUNCTIONS **************************************************/

std::string nexifyString (const char* iInString)
//: return a string in nexus format (i.e. all spaces are underscores)
{
	string theBuffer (iInString);
	std::replace (theBuffer.begin(), theBuffer.end(), ' ', '_');
	return theBuffer;
}


bool isContinuousString (const char* iInString)
//: does this string consist of a continuous string of non-whitespace?
{
	string theBuffer (iInString);
	for (string::iterator q = theBuffer.begin(); q != theBuffer.end(); q++)
	{
		if (isspace (int (*q)))
			return false;
	}
	return true;
}


std::string intToString (int iInt)
{
	return sbl::toString (iInt);
}


std::string	concatIntToString	(const char* iStr, int iInt)
{
	std::string	theBuffer (iStr);
	return theBuffer + sbl::toString (iInt);
}


// *** ID & NAME GENERATION **********************************************/

const char* IdGenerator::getNextId ()
{
	mCurrId = concatIntToString (mBaseStr.c_str(), ++mCurrIndex);
	return mCurrId.c_str();
}


// *** END ***************************************************************/



