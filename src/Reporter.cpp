/**************************************************************************
Reporter.cpp - adjudicates & channels sim/queue/action output

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>

About:

**************************************************************************/


// *** INCLUDES

#include "Reporter.h"
#include "MesaGlobals.h"
#include <iostream>
#include <fstream>

#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <cstdio>
#include "MesaTypes.h"
#include "StringUtils.h"	

using std::vector;
using std::string;
using sbl::eraseTrailingSpace;
using std::ostringstream;
using std::ios;


// *** CONSTANTS & DEFINES

// *** CLASS DEFINITION **************************************************/

// *** LIFECYCLE *********************************************************/

Reporter::~Reporter ()
{
	if (mFileStreamP)
		mFileStreamP->close();
}


// *** MUTATORS **********************************************************/

void Reporter::setFileStream (std::ofstream* iFileStreamP)
{
	mFileStreamP = iFileStreamP;
}


void Reporter::pushPrefix (const char* iPrefixStr)
{
	assert (iPrefixStr != "");
	mPrefixStack.push_back (string (iPrefixStr));
}


void Reporter::popPrefix ()
{
	mPrefixStack.pop_back ();
}



// *** I/O ***************************************************************/


void  Reporter::printNotApplicable (const char* iDetails, const char* iTitle)
{
	string theMsg;
	
	theMsg = "N/A (";
	theMsg += iDetails;
	theMsg += ")";
	
	print (theMsg.c_str(), iTitle);
}


void Reporter::print	(const char* iCStr, const char* iTitle)
//: format and print a C-style string value for output
{
	print (string (iCStr), iTitle);
}


void Reporter::print	(std::string iStlStr, const char* iTitle)
//: format and print an STL-style string value for output
{
		// split the string into chunks
		stringvec_t  theOutputLines;
		std::back_insert_iterator<stringvec_t> theOutputIter (theOutputLines);
		sbl::split (iStlStr, theOutputIter, '\n');
		
		for (unsigned long i = 0; i < theOutputLines.size(); i++)
		{
			sbl::eraseFlankingSpace (theOutputLines[i]);
			rawOutput (theOutputLines[i], iTitle);
		}
}


void Reporter::print	(int iVal, const char* iTitle)
//: format and print an integer value for output
{
	print (sbl::toString (iVal), iTitle);
}


void Reporter::print	(long iVal, const char* iTitle)
//: format and print an long value for output
{
	print (sbl::toString (iVal), iTitle);
}


void Reporter::print	(double iVal, const char* iTitle)
//: format and print a float value for output
// NOTE: the toString conversion commented out below and alternatives
// using streams directly don't give me the number of decimal points I
// require - "1.0" is always reduced to "1", which is inappropriate for
// float answers.
{
	char	theBuffer[16];
	std::sprintf (theBuffer, "%6f", iVal);
	
	// print (sbl::toString (iVal)); // see note
	print (theBuffer, iTitle);
}


void Reporter::print	(bool iVal, const char* iTitle)
//: format and print a boolean value for output
// TO DO: allow for a variety of formats like 't/f', 'yes/no', '0/1' etc.
{
	if (iVal)
		print ("true", iTitle);
	else
		print ("false", iTitle);
}


void Reporter::print (vector<string> iValueVec, const char* iRowTitle)
{
	std::stringstream theBufferStrm;
	
	for (vector<string>::size_type i = 0; i < iValueVec.size(); i++)
	{
		theBufferStrm << ((i == 0)? "" : "\t") << iValueVec[i];
	}
		
	string theBuffer = theBufferStrm.str();
	rawOutput (theBuffer, iRowTitle);
}


void Reporter::print (vector<double> iValueVec, const char* iRowTitle)
{
	vector<string>	theStringVec (iValueVec.size());
	
	for (vector<double>::size_type i = 0; i < iValueVec.size(); i++)
	{
      ostringstream buf;
      buf.setf (ios::fixed);
      buf.setf (ios::showpoint);
      buf.precision (2);
      buf << iValueVec[i];

		// char	theBuffer[8];
		//int tmp = std::sprintf (theBuffer, "%.6f", iValueVec[i]);
		// theStringVec[i] = theBuffer;
		theStringVec[i] = buf.str();
	}

	print (theStringVec, iRowTitle);
}

void Reporter::print (vector<int> iValueVec, const char* iRowTitle)
{
	vector<string>	theStringVec (iValueVec.size());
	
	for (vector<int>::size_type i = 0; i < iValueVec.size(); i++)
	{
		char	theBuffer[16];
		std::sprintf (theBuffer, "%i", iValueVec[i]);
		theStringVec[i] = theBuffer;
	}

	print (theStringVec, iRowTitle);
}

void Reporter::print (vector<bool>& iValueVec, const char* iRowTitle)
{
	vector<string>	theStringVec (iValueVec.size());
	
	for (vector<double>::size_type i = 0; i < iValueVec.size(); i++)
	{
		theStringVec[i] = (iValueVec[i] ?  't' : 'f');
	}

	print (theStringVec, iRowTitle);
}


void Reporter::alertApplication (const char* iMsg)
{
	mProgressCb (kMsg_Analysis, iMsg);
}



// *** INTERNALS *********************************************************/


void Reporter::rawOutput (std::string& iReportStr, const char* iTitle)
//: format & send a single line of output to appropriate places
// This is the central point for reporting and should never be called
// directly, only via the print functions above. We assume by this point
// the string is not line-terminated. 
{
	if (MesaGlobals::mPrefs.mAnalysisOut != kPrefAnalysisOut_AllFile)
	{
		std::stringstream theBuffer;
		theBuffer << printPrefix() << " ";
		if ((iTitle != NULL) and (iTitle != ""))
		{
			theBuffer << iTitle << ":\t";
		}
		
		theBuffer << iReportStr;
		string theBufferStr = theBuffer.str();
		
		mProgressCb (kMsg_Analysis, theBufferStr.c_str());
	}
	
	if (mFileStreamP)
	{
		(*mFileStreamP) << printPrefix();
		if ((iTitle != NULL) and (iTitle != ""))
			(*mFileStreamP) << iTitle << "\t";
		(*mFileStreamP) << iReportStr << std::endl;
	}
}


std::string Reporter::printPrefix ()
{
	std::stringstream theBuffer;
	for (unsigned long i = 0; i < mPrefixStack.size(); i++)
		theBuffer << mPrefixStack[i] << "\t";
	return theBuffer.str();
}



// *** END ***************************************************************/



