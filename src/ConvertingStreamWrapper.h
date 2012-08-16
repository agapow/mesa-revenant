/**************************************************************************
ConvertingStreamWrapper.h - changes case of all input

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef CONVERTINGSTREAMWRAPPER_H
#define CONVERTINGSTREAMWRAPPER_H


// *** INCLUDES

#include "Sbl.h"
#include "StringUtils.h"
#include <fstream>
#include <string>
#include "nxsstring.h"


// *** CONSTANTS & DEFINES

// *** HELPER CLASS ******************************************************/

// The format commands that can be squirted at the Wrapper. 
// On reflection, the Wrapper should probably be derived from ofstream,
// so we can get those handy formatting commands. Instead we define
// these place holder classes here and overload the output << operator
// to act appropriately when sending them to the NexusWriter.
// Why this method was chosen:
// If you try and overload either << or write() for MWFormatCmd, it
// doesn't catch the eoln or literal commands, the default does. This
// is because those functions that a reference which does not match a
// reference to derived classes. Bugger.

class literal
{
public:
	literal () : mData ("") {}
	literal (const char* iCStr) : mData (iCStr) {}
	literal (const std::string& iStlStr) : mData (iStlStr) {}
	literal (const nxsstring& iNStr) : mData (iNStr.c_str()) {}
	literal (char iChar) : mData (1, iChar) {}
	
	std::string	mData;
};


// *** CLASS DECLARATION *************************************************/

class ConvertingStreamWrapper
{
public:
	// Lifecycle
	ConvertingStreamWrapper (std::ofstream& iOutStream, sbl::case_t iCase)
		: mOutStream (iOutStream), mLiteral (false), mCase (iCase)
		{}
		
	~ConvertingStreamWrapper ()
		{ mOutStream.close(); }
				
	// Services
	template <typename T>
	void write (T iOutVal)
		{ mOutStream << iOutVal; };

	void write (const char* iOutCStr)
		{ mOutStream << convertCase (iOutCStr); }

	void write (std::string iOutStr)
		{ mOutStream << convertCase (iOutStr.c_str()); }

	void write (nxsstring iOutNxsStr)
		{ mOutStream << convertCase (iOutNxsStr.c_str()); }

	void write (literal iLiteralStr)
		{ mOutStream << iLiteralStr.mData; }

	template <typename T>
	ConvertingStreamWrapper& operator << (T iOutVal)
	{
		write (iOutVal);
		return *this;
	}

	bool setLiteral (bool iState)
	{
		bool theRetVal = mLiteral;
		mLiteral = iState;
		return theRetVal;
	}

	// Depreciated & Debug
	void	validate	()
	{
		assert (mOutStream.is_open());
		assert (convertCase ("test str 1") == convertCase ("TEST STR 1"));
		assert ((sbl::kCase_Upper <= mCase) and (mCase <= sbl::kCase_Mixed));
	}

	// Internals
private:
	std::ofstream&		mOutStream;
	bool					mLiteral;
	sbl::case_t			mCase;
	
	std::string convertCase (const char* iMixedStr)
	//: returns a string converted to the correct case for output to nexus
	{
		std::string theTargetStr (iMixedStr);

		if (mLiteral == false)
		{
			switch (mCase)
			{
				case sbl::kCase_Upper:
					sbl::toUpper (theTargetStr);
					break;
				case sbl::kCase_Lower:
					sbl::toLower (theTargetStr);
					break;
				case sbl::kCase_Mixed:
					break;
				default:
					assert ("shoouldn't get to this case" == false);
			}
		}
		return theTargetStr;
	}	
};


#endif
// *** END ***************************************************************/



