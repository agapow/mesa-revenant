/**************************************************************************
MesaUtils.h - global utility functions

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at Silwood
  Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- Largely to save defining the callbacks everywhere.
  
Changes:
- 99.10.10: created.
  
**************************************************************************/


#pragma once
#ifndef MESAUTILS_H
#define MESAUTILS_H


// *** INCLUDES

#include <string>


// *** CONSTANTS & DEFINES

// *** UTILITY FUNCTIONS *************************************************/

std::string		nexifyString (const char* iInString);
bool				isContinuousString (const char* iInString);
std::string		intToString (int iInt);
std::string		concatIntToString	(const char* iStr, int iInt);

class IdGenerator
{
public:
	IdGenerator (const char* iBaseCstr)
		: mBaseStr (iBaseCstr), mCurrIndex (0)
		{}
		
	const char* getNextId ();
	
private:
	std::string		mBaseStr;
	std::string		mCurrId;
	int				mCurrIndex;
};


#endif
// *** END ***************************************************************/



