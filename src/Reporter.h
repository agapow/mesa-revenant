/**************************************************************************
Reporter.h - adjudicates & channels sim/queue/action output

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef REPORTER_H
#define REPORTER_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class Reporter
{
public:
	// LIFECYCLE
	Reporter (progcallback_t& ikProgressCb)
		: mProgressCb (ikProgressCb), mFileStreamP (NULL)
		{}
	~Reporter ();

	// MUTATORS
	void	pushPrefix (const char* iPrefixStr);
	void	popPrefix ();

	void	setFileStream (std::ofstream* iFileStreamP);

	// I/O
	// note data comes first, result second
	void  printNotApplicable (const char* iDetails, const char* iTitle = NULL);

	void	print	(const char* iCStr, const char* iTitle = NULL);
	void	print	(std::string iStlStr, const char* iTitle = NULL);
	void	print (double iVal, const char* iTitle = NULL);
	void 	print (int iVal, const char* iTitle = NULL);
	void 	print (long iVal, const char* iTitle = NULL);
	void 	print (bool iVal, const char* iTitle = NULL);

	void  print (std::vector<std::string> iValueVec, const char* iRowTitle = NULL);
	void  print (std::vector<double> iValueVec, const char* iRowTitle = NULL);
	void  print (std::vector<int> iValueVec, const char* iRowTitle = NULL);
	void  print (std::vector<bool>& iValueVec, const char* iRowTitle = NULL);

	void alertApplication (const char* iMsg);

	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
	progcallback_t		mProgressCb;
	std::ofstream*		mFileStreamP;

	std::vector <std::string>		mPrefixStack;

	void				rawOutput (std::string& iReportStr, const char* iTitle = NULL);
	std::string		printPrefix ();
};


#endif
// *** END ***************************************************************/



