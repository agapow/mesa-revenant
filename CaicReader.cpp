/**************************************************************************
CaicReader.cpp - reads/loads data

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- This object is not meant to hang around - it is created, does
  its job and then vanishes.

Changes:
- 03.9.23: Created.

**************************************************************************/


// *** INCLUDES

#include "CaicReader.h"
#include "MesaUtils.h"
#include "StreamScanner.h"
#include "Error.h"
#include <sstream>
#include <vector>
#include <map>

using std::ifstream;
using std::stringstream;
using std::string;
using sbl::FormatError;


// *** CONSTANTS & DEFINES

// *** MAIN BODY *********************************************************/

// *** LIFECYCLE *********************************************************/

// ctor in header

CaicReader::CaicReader
(std::ifstream* iPhylStreamP, std::ifstream* iBlenStreamP, const progcallback_t& ikProgressCb)
{
	// Preconditions:
	assert (iPhylStreamP->is_open());
	assert (iBlenStreamP->is_open());
	
	// Main:
	mCallBack = ikProgressCb;
	mPhylStreamP = iPhylStreamP;
	mBlenStreamP = iBlenStreamP;
	// sic the reader onto the input stream
	readData ();
	
	mPhylStreamP->close();
	mBlenStreamP->close();
}



CaicReader::~CaicReader ()
{

}


// *** READING ***********************************************************/

void CaicReader::readData ()
{
	// read branchlength file
	// configure scanner
	sbl::StreamScanner	theBlenScanner (*mBlenStreamP);
	theBlenScanner.SetComments ("", "");
	theBlenScanner.SetLineComment ("#");
	
	// just check this is a tab delimited file
	string 			theInLine;
	theBlenScanner.ReadLine (theInLine);		
	theBlenScanner.Rewind();	// roll back to beginning	
	if (count (theInLine.begin(), theInLine.end(), '\t') < 1)
		throw FormatError ("the branchfile doesn't appear to be tab-delimited");

	// now we can actually read the file
	int theNumRows = 0;	
	// while the eof has not been reached
	while (theBlenScanner)
	{
		string	theCurrLine;
		theBlenScanner.ReadLine (theCurrLine);
		sbl::eraseTrailingSpace (theCurrLine);
		
		// stop at blank lines.
		if (theCurrLine == "")
			break;
	
		// otherwise break into tokens
		stringvec_t									theDataRow;
		std::back_insert_iterator<stringvec_t> 	theSplitIter (theDataRow);
		sbl::split (theCurrLine, theSplitIter, '\t');
		// clean up the tokens
		for (stringvec_t::size_type i = 0; i < theDataRow.size(); i++)
		{
			sbl::eraseFlankingSpace (theDataRow[i]);
		}
		
		// pop the row into the matrix
		mBlenMatrix.push_back (theDataRow);
	}
	
	// clean up & check correctness of data ...
	if (mBlenMatrix.size() < 3)
		throw FormatError ("insufficient branches in data");	
	for (int i = 0; i < mBlenMatrix.size(); i++)
	{
		if (mBlenMatrix[i].size() != 3)
			throw FormatError ("branch missing data");	
	}

	// if people use lower-case in their phylogeny, convert it
	string theTestCode = mBlenMatrix[1][0];
	sbl::toLower (theTestCode);
	bool theCodesInLowerCase = (mBlenMatrix[1][0] == theTestCode);
	if (theCodesInLowerCase)
	{
		// the bastards done wrote it in lower case
		for (int i = 0; i < mBlenMatrix.size(); i++)
		{
			sbl::toUpper (mBlenMatrix[i][0]);
		}
	}
	// read phylogeny file
	// configure scanner
	sbl::StreamScanner	thePhylScanner (*mPhylStreamP);
	thePhylScanner.SetComments ("", "");
	thePhylScanner.SetLineComment ("#");
	
	// while the eof has not been reached
	while (thePhylScanner)
	{
		// get code
		string	theCodeLine;
		thePhylScanner.ReadLine (theCodeLine);
		sbl::eraseTrailingSpace (theCodeLine);
		if (theCodeLine == "")
			break;
		if (theCodesInLowerCase)
			sbl::toUpper (theCodeLine);
			
		// get spp name
		string	theSppLine;
		thePhylScanner.ReadLine (theSppLine);
		sbl::eraseTrailingSpace (theSppLine);
		
		// store
		mPhylCodeMap[theCodeLine] = nexifyString (theSppLine.c_str());
		// DBG_MSG ("Inserting " << theCodeLine << ":" << theSppLine);
	}
	
	
	
}




// *** SERVICES **********************************************************/

void CaicReader::getData (ContTraitMatrix& ioWrangler)
{
	int theNumTaxa = mPhylCodeMap.size();
	ioWrangler.resize (theNumTaxa, 1);
	ioWrangler.resizeCols (0);
	
	// label all the rows with taxa names
	std::map <string, string>::iterator p = mPhylCodeMap.begin();
	int theRowNum = 0;
	for (; p != mPhylCodeMap.end(); p++, theRowNum++)
	{
		string theTmpName = (*p).second;
		theTmpName = nexifyString (theTmpName.c_str());
		ioWrangler.setRowName (theRowNum, theTmpName.c_str());
	}
}


void CaicReader::getData (DiscTraitMatrix& ioWrangler)
{
}


class caictreenode_t
{
public:
	string   mCode;
	double	mTimeToParent;
	double   mTimeToTerminus;
};


void CaicReader::getData (TreeWrangler& ioWrangler)
{
	std::vector <caictreenode_t>  theCaicEntries;
	
	for (int i = 0; i < mBlenMatrix.size(); i++)
	{
		caictreenode_t   theCurrNode;
		
		theCurrNode.mCode = mBlenMatrix[i][0];
		theCurrNode.mTimeToParent = sbl::toDouble (mBlenMatrix[i][1]);
		theCurrNode.mTimeToTerminus = sbl::toDouble (mBlenMatrix[i][2]);
		
		theCaicEntries.push_back (theCurrNode);
	}
	/*
	for (int i = 0; i < theCaicEntries.size(); i++)
	{
		DBG_MSG (i << ": " << theCaicEntries[i].mCode << " - " <<
			theCaicEntries[i].mTimeToParent << " - " <<
			theCaicEntries[i].mTimeToTerminus);
	}
	*/
	/*
	// if people use lower-case in their phylogeny, convert it
	string theTestCode = theCaicEntries[1].mCode;
	sbl::toLower (theTestCode);
	if (theCaicEntries[1].mCode == theTestCode)
	{
		// the bastards done wrote it in lower case
		for (int i = 0; i < theCaicEntries.size(); i++)
		{
			sbl::toUpper (theCaicEntries[i].mCode);
		}
		std::map<string,string>::iterator p = mPhylCodeMap.begin();
		for (; p != mPhylCodeMap.end(); p++)
		{
			sbl::toUpper ((*p).first);
		}
	}
	*/
	MesaTree   theNewTree;
	
	for (int i = 0; i < theCaicEntries.size(); i++)
	{
		sbl::CaicCode  theCode (theCaicEntries[i].mCode.c_str());
		nodeiter_t theNewNode = theNewTree.addNode (theCode);
		if (mPhylCodeMap.find (theCode.str()) != mPhylCodeMap.end())
		{
			string theNewNodeName = mPhylCodeMap[theCode.str()];
			theNewTree.setNodeName (theNewNode, theNewNodeName);
		}
		theNewTree.setEdgeWeight (theNewNode, theCaicEntries[i].mTimeToParent);
	}
	
	
	ioWrangler.addTree (theNewTree);
}	


// *** INTERNALS *********************************************************/


// *** DEPRECATED & TEST FUNCTIONS **************************************/
#pragma mark -

void CaicReader::validate ()
{
//	cout << "*** Testing CaicReader class" << endl;
	
//	cout << "*** Finished testing CaicReader class" << endl;
}

// *** END ***************************************************************/

