/**************************************************************************
ResultsDistiller.h - condenses & transposes results file 

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>
**************************************************************************/


// *** INCLUDES

#include "ResultsDistiller.h"
#include "StreamScanner.h"
#include "StringUtils.h"
#include <iterator>
#include <list>
#include <cctype>
#include <utility>


// *** CONSTANTS & DEFINES

class OutputCol: public std::pair<stringvec_t, stringvec_t>
{



};
	
	


// *** CLASS DECLARATION *************************************************/

void ResultsDistiller::distill
(std::ifstream& iInFstream, std::ofstream& iOutFstream)
{
	sbl::StreamScanner	theScanner (iInFstream);
	theScanner.SetComments ("", "");
	theScanner.SetLineComment ("#");
	
	// process all the lines into the array
	std::list<OutputCol>		theUniqueLines;
	// int 							theEmptyLines = 0;
	
	while (theScanner)
	{
		// read line
		std::string 			theInLine;
		theScanner.ReadLine (theInLine);
		
		// clean up and split line into components
		sbl::eraseTrailingSpace (theInLine);
		stringvec_t	theBuffer;
		std::back_insert_iterator<stringvec_t> theOutputIter (theBuffer);
		sbl::split (theInLine, theOutputIter, '\t');
		
		/*
		//  if line cleans up to an empty entry
		if (theBuffer.size() <= mTrimCols)
		{
			// theEmptyLines++;
			continue;
		}
		// otherwise process it

		// build the storage structure
		stringvec_t::iterator r;
		for (r = theBuffer.begin() + mTrimCols; r != theBuffer.end() - 1; r++)
		{
			if (isData (*r))
				break;
		}
		*/
		
		OutputCol	theCurrLine;
		if ((unsigned int) mTrimCols < theBuffer.size())
			theCurrLine.first.assign (theBuffer.begin() + mTrimCols, theBuffer.end());
		theUniqueLines.push_back (theCurrLine);
			
		/*
		theCurrLine.first.assign (theBuffer.begin() + mTrimCols, r);
		theCurrLine.second.assign (r, theBuffer.end());
		*/
		
		/*
		// does it already exist in the uniques
		std::list<OutputCol>::iterator q = theUniqueLines.begin();
		for (q = theUniqueLines.begin(); q != theUniqueLines.end(); q++)
		{
			if (compareStringVec (q->first, theCurrLine.first))
				break;
		}
		if (q == theUniqueLines.end())
		{
			theUniqueLines.push_back (theCurrLine);
		}
		else
		{
			q->second.insert (q->second.end(), theCurrLine.second.begin(),
				theCurrLine.second.end());
		}
		*/
	}	
	
	// do output as columns
	// we now have a list of tokenized rows (vectors of strings)
	
	// find how long the longest row (now) / column (to be) is ...
	unsigned long theMaxColSize = 0;
	std::list<OutputCol>::iterator q;
	for (q = theUniqueLines.begin(); q != theUniqueLines.end(); q++)
	{	
		if (theMaxColSize < q->first.size())
			theMaxColSize = q->first.size();
		/*
		long theColSize = q->first.size() + q->second.size();
		if (theMaxColSize < theColSize)
			theMaxColSize = theColSize;
		*/
	}
		
	// for each row (to be)			
	for (unsigned long i = 0; i < theMaxColSize; i++)
	{
		for (q = theUniqueLines.begin(); q != theUniqueLines.end(); q++)
		{
			if (q != theUniqueLines.begin())
				iOutFstream << "\t";
			
			if (i < q->first.size())
			{
				// if printing the prefix/header
				iOutFstream << q->first[i];
			}
			else
			{
				// must be padding at the end
				iOutFstream << "-";
			}
			
			/*
			if (i < q->first.size())
			{
				// if printing the prefix/header
				iOutFstream << q->first[i];
			}
			else if ((i - q->first.size()) < q->second.size())
			{
				// or if printing the data column
				iOutFstream << q->second[i - q->first.size()];
			}
			else
			{
				// must be padding at the end
				iOutFstream << "-";
			}
			*/
		}
		iOutFstream << "\n";
	}		
}


bool ResultsDistiller::compareStringVec (stringvec_t& iInfo1, stringvec_t& iInfo2)
{
	if (iInfo1.size() != iInfo2.size())
		return false;
	else
	{
		for (unsigned long i = 0; i < iInfo1.size(); i++)
		{
			if (iInfo1[i] != iInfo2[i])
				return false;
		}
	}
	return true;
}


bool ResultsDistiller::isData (std::string& iTestString)
//: return true if this is a data string not a prefix
{
	if (iTestString[0] == '#')
		// for those annoying prefixes
		return false;
		
	if (iTestString.substr (0, 3) == "N/A")
		// for null answers
		return true;
		
	if (std::isalpha ((int) iTestString[0]) != 0)
		// if it begins with a letter
		return false;
		
	return true;
}



// *** END ***************************************************************/



