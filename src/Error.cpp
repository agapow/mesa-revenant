/**************************************************************************
Error.h - general exception message class

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

About:
- A general class for throwing on exceptions, that contains info to base
  informative & accurate error handling & messaging.
- Note that against normal Sibil documentation guidelines, the function
  what() is used to get the general exception class description. This is
  to maintain orthogonality with the C++ class it is derived on.
- Classes derived from Error should overload what() with a string
  that describes the class. When the class is thrown, it should be
  instantiated as MyError("a description of the specfic problem") and
  the message extracted with GetMsg().
- Methods & members:
  - mWhat holds a description of the general problem,
  - mDesc holds a specific description or qualification of the problem.
  - Ctor () is a simply initialiser, sets the description to null.
  - Ctor (char*) inits the description with a string.
  - what() should be overridden in every subclass and return a meaningful
    msg derived from the specific and general descriptions.
  - GetMsg () constructs a meaningful error statement out of the general
    (what) and specific (mDesc) messages.
- The architecture of this component is that Error determines the
  presentation of the information, its immediate children define
  different classes of errors, their children define different contexts
  in which they might be called. For example ParseError represents all
  logical and format errors encountered during scanning and reading in.
  These errors have the general form "Parse Error (<location>, <message>)"
  which are specified by the subclasses.
  
Changes:
- 99.9.2: created.
- 99.9.15: expanded built-in types, changed name to Exception from
  SblException.
- 99.9.28: ... and thence to Error, as CodeWarrior keeps finding the std::
  "exception.h" rather than my "Exception.h" when it goes looking for the
  file, causing many compilation problems. Also had GetMsg outside the
  definition but because it lead to problems being multiply included in
  files and I didn't want to do a .cpp file just for it, so it's now
  inside the definition. 
- 00.3.1: this used to be a general header for Sibil but now has to be
  specifically included. (This allows Sibil.h to be the single obligate
  header, and trims a bit of fat.) Rehashed the class architecture and
  responsibilities as above. Moved material to a .cpp file as there is
  sufficient now. Rationalized architecture according to above plan.
  
To Do:
- Introduce namespaces
- Inline

**************************************************************************/



// *** INCLUDES

#include "Error.h"

#include <sstream>

using std::string;
using std::stringstream;
using std::exception;
using std::strlen;
using std::ends;


SBL_NAMESPACE_START

// *** CONSTANTS & DEFINES


// *** BASIC ERROR *******************************************************/
/*
// WHAT
// The basic function for constructing error descriptions. Note this is
// virtual in the base class so the derived versions will always be
// called. This implementation is just a sensible default.
// !! Note: the call to GetDesc() requires some explanation. what() is
// and must be a const() function. It recognises GetDesc as a non-const
// fxn and refuses to call it. But GetDesc() cannot be made const ...
// This can be circumvented by casting this to the current class. As the
// cast doesnot include const-ness, the neccesary effect is required. See
// Eckel, "Thinking in C++".
const char* Error::what ()
{ 
	static string theFullDesc;
	theFullDesc = mWhat;
	if (mDesc != "")
	{
		theFullDesc += " (";
		theFullDesc += ((Error*) this)->GetDesc(); 
		theFullDesc += ")";
	}
	return theFullDesc.c_str();
}
*/
	
// *** DEPRECIATED & DEBUG

/*
const char*	Error::GetMsg	()
{
	if (mDesc == "")
	{
		return what ();
	}
	else
	{
		// Had enormous trouble with this fn, it kept losing
		// everything after the mDesc was inserted. Tried a
		// stringstream and a cString buffer to no avail. This
		// solution seems to work. 
		static string	theErrorMsg (mWhat);
		
		theErrorMsg += " (";
		theErrorMsg += mDesc.c_str();
		theErrorMsg += ")";
		return (theErrorMsg.c_str());
	}
}

*/


// *** PARSE ERRORS ******************************************************/

// *** LIFECYCLE

ParseError::ParseError ()
: Error ("Parse Error")
{
	Init();
}

ParseError::ParseError (const char* iDesc)
: Error ("Parse Error", iDesc)
{
	Init();
}

ParseError::ParseError (uint iLineIndex, const char* iDesc)
: Error ("Parse Error", iDesc)
{
	Init();
	SetLocn (iLineIndex);
}


void ParseError::Init ()
{
	mFileLocn = "";
	mLineLocn = kErr_LineUnknown;
}


// *** ACCESS
// Allows extra error information (location of error) to be set

void ParseError::SetLocn (const char* ikFileName)
{
	mFileLocn = ikFileName;
}

void ParseError::SetLocn (int iLineIndex)
{
	mLineLocn = iLineIndex;
}

void ParseError::SetLocn (const char* ikFileName, int iLineIndex)
{
	SetLocn (ikFileName);
	SetLocn(iLineIndex);
}


// *** CONSTRUCT DESCRIPTION

// GET LOCATION
// Returns the appropriate prefix of location information to be
// inserted in a description.
string ParseError::GetLocn	()
{
	static string	theLocnPrefix;
	stringstream 	theLocnStrm;
	
	// enter the location info if it has been supplied
	if (mFileLocn != "")
	{
		theLocnStrm << mFileLocn;
		if (mLineLocn != kErr_LineUnknown)
			theLocnStrm << " ";
	}
	if (mLineLocn != kErr_LineUnknown)
	{
		theLocnStrm << "line ";
		theLocnStrm << mLineLocn;
	}
	
	theLocnPrefix = theLocnStrm.str();
	return theLocnPrefix;
}


// GET DESC
// Prepend extra information (line and file posn) to desc if available.
// Want to return variations on "MyFile.txt line 3: it's wrong".
// Remeber, this is virtual.
string ParseError::GetDesc	()
{
	string	theFullDesc = GetLocn();
	
	if ((theFullDesc != "") and IsDesc())
		theFullDesc += ": ";
		
	if (IsDesc())
		theFullDesc += GetDesc();
	
	return theFullDesc;
}


// *** EXPECTED ERROR

ExpectedError::ExpectedError
(const char* ikExpectedDesc, const char* ikGotDesc, bool iIsInQuotes)
{
	stringstream	theBuffer;
	bool				theExpDescHere = strlen (ikExpectedDesc);
	bool				theGotDescHere = strlen (ikGotDesc);
	
	if (theExpDescHere)
	{
		theBuffer << "expected ";
		if (iIsInQuotes)
			theBuffer << "\"";
		theBuffer << ikExpectedDesc;
		if (iIsInQuotes)
			theBuffer << "\"";
	}

	if (theExpDescHere and theGotDescHere)
	{
		theBuffer << ", ";
	}
	
	if (theGotDescHere)
	{
		theBuffer << "got ";
		if (iIsInQuotes)
			theBuffer << "\"";
		theBuffer << ikGotDesc;
		if (iIsInQuotes)
			theBuffer << "\"";
	}

	theBuffer << std::ends;

	SetDesc (theBuffer.str ());
}


// *** FORMAT ERROR

FormatError::FormatError
(uint iLineIndex, const char* ikExpectedDesc, const char* ikGotDesc)
{
	SetLocn (iLineIndex);
	
	stringstream	theBuffer;
	string			theDesc = ikExpectedDesc;
	
	if (theDesc != "")
	{
		theBuffer << "expected \"" << theDesc
			<< "\", got \"" <<  ikGotDesc << "\")" << std::ends;
	}		
	SetDesc (theBuffer.str ());
}


// *** END OF FILE ERROR

// *** UNCONSUMED ERROR



// *** FILE I/O ERRORS ***************************************************/

// *** BASIC FILE ERROR

FileError::FileError (const char* iDesc, const char* iFileName)
: Error ("File error")
{
	stringstream	theBuffer;
	
	theBuffer << iDesc;
	if (iFileName != "")
		theBuffer << " " << iFileName << std::ends;			
	SetDesc (theBuffer.str ());
}


// *** FILE MISSING ERROR

// *** FILE OPEN ERROR

// *** FILE READ ERROR

// *** FILE WRITE ERROR



// *** MISC ERRORS *******************************************************/

// *** UNIMPLEMENTED ERROR
// During development, throw when a function has not been completed, but
// the calling is in place.


SBL_NAMESPACE_STOP

// *** END ***************************************************************/

