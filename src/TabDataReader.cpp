/**
@file
@author   Paul-Michael Agapow
          Dept. Biology, University College London, WC1E 6BT, UK.
          <mail://p.agapow@ic.ac.uk> 
          <http://www.agapow.net/code/sibil/>
          
Part of MeSA <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>.
*/


// *** INCLUDES

#include "TabDataReader.h"
#include "StreamScanner.h"
#include "StringUtils.h"
#include "MesaTypes.h"
#include <iterator>

using std::stringstream;
using std::count;
using std::string;
using std::back_insert_iterator;
using sbl::isFloat;
using sbl::isWhole;
using sbl::FormatError;
using sbl::split;
using sbl::beginsWith;


// *** CONSTANTS & DEFINES


// *** DEFINITIONS *******************************************************/

// *** LIFECYCLE *********************************************************/

// *** SERVICES **********************************************************/

/**
Imports a tab-delimited file into the existing data.

This functions produces a matrix of the input, strips and the row and
column names and then splits the matrix into continuous and discrete
components. Assume imported data is rows of species names followed by tab
seperated data. Alll columns must be the same length. Parsing stops at the
first blank line. 
*/
void TabDataReader::read ()
{
	// configure scanner
	sbl::StreamScanner	theScanner (mInStream);
	theScanner.SetComments ("", "");
	theScanner.SetLineComment ("#");

	// just check this is a a tab delimited file
	string theInLine;
	theScanner.ReadLine (theInLine);		
	theScanner.Rewind();	// roll back to beginning	
	if (count (theInLine.begin(), theInLine.end(), '\t') < 1)
		throw FormatError ("this isn't a tab-delimited file");
	
	// now we can actually read the file
	// while the eof has not been reached
	while (theScanner)
	{
		string	theCurrLine;
		theScanner.ReadLine (theCurrLine);
		sbl::eraseTrailingSpace (theCurrLine);
		
		// stop at blank lines.
		if (theCurrLine == "")
			break;
	
		// otherwise break into tokens
		stringvec_t									theDataRow;
		back_insert_iterator<stringvec_t> 	theSplitIter (theDataRow);
		split (theCurrLine, theSplitIter, '\t');
		// clean up the tokens
		for (stringvec_t::size_type i = 0; i < theDataRow.size(); i++)
		{
			sbl::eraseFlankingSpace (theDataRow[i]);
		}
		
		// pop the row into the matrix
		mDataMatrix.push_back (theDataRow);
	}
	
	// clean up & check correctness of data ...
	// 1. matrix contains data
	// 2. species names and at least 1 column of data given
	// 3. all rows the same length
	// 4. no null entries
	if (mDataMatrix.size() == 0)
		throw FormatError ("imported data matrix is empty");	
	if (mDataMatrix[0].size() < 2)
		throw FormatError ("imported data missing species names or data");	
	
	// set the correct type of data
	extractColNames ();
	extractRowNames();
	extractFormat ();
	
	// report progress
	int	theNumTaxa = mDataMatrix.size();
	int	theNumContTraits = mContColNames.size(); 
	int	theNumDiscTraits = mDiscColNames.size(); 
	bool	theSingleRow = (theNumTaxa == 1);
	
	stringstream theBuffer;
	theBuffer << "There " << (theSingleRow ? "is" : "are") << " " <<
		theNumTaxa << " " << (theSingleRow ? "taxon" : "taxa") <<
		" with " << theNumContTraits << " continuous " <<
		((theNumContTraits == 1)? "trait": "traits") << " and " <<
		theNumDiscTraits << " discrete " <<
		((theNumDiscTraits == 1)? "trait": "traits") <<
		" in the import data.";

	string theTmpString = theBuffer.str();
	mProgressCb (kMsg_Progress, theTmpString.c_str());	
}
	

void TabDataReader::getData (DiscTraitMatrix& ioWrangler)
{
	// two choices: there's stuff in the wrangler already or not
	int	theNumOldTraits = ioWrangler.countChars();
	int   theNumNewTraits = mDiscColNames.size();
	int   theTotalNumTraits = theNumOldTraits + theNumNewTraits;
	int	theNumOldTaxa = ioWrangler.countTaxa();	
	int   theNumNewTaxa = 0;    // calc later
	int   theTotalNumTaxa = 0;  // calc later

	// if there's none of the given datatype, get out of here
	if (theNumNewTraits == 0)
		return;

	// see what rows have to be added, assume all columns have to be added
	stringvec_t   theNewRows;
	for (uint i = 0; i < mRowNames.size(); i++)
	{
		if (not ioWrangler.hasRowName (mRowNames[i].c_str()))
			theNewRows.push_back (mRowNames[i]);
	}
	theNumNewTaxa = theNewRows.size();
	theTotalNumTaxa = theNumOldTaxa + theNumNewTaxa;
	
	// expand wrangler to fit new data, name new rows & cols
	ioWrangler.resize (theTotalNumTaxa, theTotalNumTraits);
	for (int i = 0; i < theNumNewTaxa; i++)
	{
		ioWrangler.setRowName (theNumOldTaxa + i, mRowNames[i].c_str());
	}	

	for (int i = 0; i < theNumNewTraits; i++)
	{
		if (mDiscColNames[i] == "")
			ioWrangler.setColName (theNumOldTraits + i,
				mDiscColNames[i].c_str());
	}	
	
	// stuff things into the the wrangler		
	for (int i = 0 ; i < theNumNewTraits; i++)
	{
		for (uint j = 0; j < mDiscDataMatrix.size(); j++)
		{
			ioWrangler.getData (mRowNames[j].c_str(), theNumOldTraits + i)	=
				mDiscDataMatrix[j][i];
		}
	}	
}


void TabDataReader::getData (ContTraitMatrix& ioWrangler)
{
	// two choices: there's stuff in the wrangler already or not
	int	theNumOldTraits = ioWrangler.countChars();
	int   theNumNewTraits = mContColNames.size();
	int   theTotalNumTraits = theNumOldTraits + theNumNewTraits;
	int	theNumOldTaxa = ioWrangler.countTaxa();	
	int   theNumNewTaxa = 0;    // calc later
	int   theTotalNumTaxa = 0;  // calc later

	// if there's none of the given datatype, get out of here
	if (theNumNewTraits == 0)
		return;

	// see what rows have to be added, assume all columns have to be added
	stringvec_t   theNewRows;
	for (uint i = 0; i < mRowNames.size(); i++)
	{
		if (not ioWrangler.hasRowName (mRowNames[i].c_str()))
			theNewRows.push_back (mRowNames[i]);
	}
	theNumNewTaxa = theNewRows.size();
	theTotalNumTaxa = theNumOldTaxa + theNumNewTaxa;
	
	// expand wrangler to fit new data, name new rows & cols
	ioWrangler.resize (theTotalNumTaxa, theTotalNumTraits, 0);
	for (int i = 0; i < theNumNewTaxa; i++)
	{
		// DBG_MSG (theNumOldTaxa << " " << i << " " << ioWrangler.countRows() << " " <<  mRowNames[i].c_str());
		assert ((theNumOldTaxa + i) < ioWrangler.countRows());
		ioWrangler.setRowName (theNumOldTaxa + i, mRowNames[i].c_str());
	}	

	for (int i = 0; i < theNumNewTraits; i++)
	{
		assert ((theNumOldTraits + i) < ioWrangler.countCols());
		if (mContColNames[i] != "")
			ioWrangler.setColName (theNumOldTraits + i,
				mContColNames[i].c_str());
	}	
	
	// stuff things into the the wrangler		
	for (int i = 0 ; i < theNumNewTraits; i++)
	{
		for (uint j = 0; j < mContDataMatrix.size(); j++)
		{
			ioWrangler.getData (mRowNames[j].c_str(), theNumOldTraits + i)	=
				sbl::toDouble (mContDataMatrix[j][i]);
		}
	}	
}


void TabDataReader::orderRows (stringvec_t& ioNames)
//: sort the rows into the same order as these names
{
	// Preconditions:
	assert (mRowNames.size() == mDataMatrix.size());

	// check there is data for all taxa, & order data
	stringvec_t::size_type theNumTaxa = ioNames.size();

	for (stringvec_t::size_type i = 0; i < theNumTaxa; i++)
	{
		// get original taxa name
		string theTaxaName = ioNames[i];

		// look in data matrix
		stringmatrix_t::size_type j;
		for (j = 0; j < mDataMatrix.size(); j++)
		{
			if (theTaxaName == mRowNames[j])
				break;
		}

		if (j == mDataMatrix.size())
		// if the taxa name couldn't be found in the matrix
		{
			string theErrMsg ("couldn't find taxon \'");
			theErrMsg += theTaxaName.c_str();
			theErrMsg += "\' in the imported data matrix";
			throw FormatError (theErrMsg.c_str());
		}
		else
		// if it could found, strip the name off the data and stuff
		// it in a new data matrix, delete the old row.
		{
			//stringvec_t	theDenamedRow (mDataMatrix[j].begin() + 1,
			//	mDataMatrix[j].end());
			//iFilteredData.push_back (theDenamedRow);
			//mDataMatrix.erase (mDataMatrix.begin() + j);
			swap (mRowNames[i], mRowNames[j]);
			swap (mDataMatrix[i], mDataMatrix[j]);
		}		
	}
}


// *** UTILITY FUNCTIONS **************************************************/

/**
Detect the format of the data read in and sort it appropriately.

Note that we assume that column and row names have been stripped off
already and that each column has the same sort of data. Columns that
contain 1 or more floating point and 0 or more integer values are assumed
to be continuous data. Columns that are entirely alphanumeric or integer
are assumed to be discrete. Everything else is an error.
*/
void TabDataReader::extractFormat ()
//: detects the format of the data read in
{
	uint theNumCols = mDataMatrix[0].size();
	uint theNumRows = mDataMatrix.size();

	// check that all rows have the same number of columns
	for (uint i = 0; i < mDataMatrix.size(); i++)
	{
		if (mDataMatrix[i].size() != theNumCols)
			throw FormatError ("imported data matrix has rows of unequal length");
	}
	
	// for each column, establish what sort of data can be found there
	mColumnTypes.clear();
	for (uint i = 0; i < theNumCols; i++)
	{
		traittype_t theOverallType;

		// test column name for clues
		string theColName = mColNames[i];
		sbl::toLower (theColName);
		if (beginsWith (theColName, "site_"))
		{
			theOverallType = kTraittype_Continuous;
		}
		else if (beginsWith (theColName, "rich_"))
		{
			theOverallType = kTraittype_Continuous;
		}
		else if (beginsWith (theColName, "cont_"))
		{
			theOverallType = kTraittype_Continuous;
		}
		else if (beginsWith (theColName, "disc_"))
		{
			theOverallType = kTraittype_Discrete;
		}
		else
		{
			int theCountFloats, theCountInts, theCountAlpha, theCountOther;
			theCountFloats = theCountInts = theCountAlpha = theCountOther = 0;
		
			for (uint j = 0; j < theNumRows; j++)
			{
				if (mDataMatrix[j][i] == "")
					throw FormatError ("imported data matrix contains null entries");
					
				traittype_t theCurrType = extractData (mDataMatrix[j][i]);
				if (theCurrType == kTraittype_Alpha)
					theCountAlpha++;	
				else if (theCurrType == kTraittype_Float)
					theCountFloats++;
				else if (theCurrType == kTraittype_Int)
					theCountInts++;
				else
					theCountOther++;
			}		
			
			// what type of data is in the column?
			/*
			Case 1. if there are alphanumeric strings and no floats, it must be discrete.
			If there are alphanumerics and floats, it must be a mistake.
			*/
			if (0 < theCountAlpha)
			{
				if (0 < theCountFloats)
					throw FormatError ("import column contains continuous and discrete data");
				else theOverallType = kTraittype_Discrete;	
			}
			/*
			Case 2. if there are floats and any number of ints, it must be continuous.
			*/
			else if (0 < theCountFloats)
			{
				assert (theCountAlpha == 0);
				theOverallType = kTraittype_Continuous;	
			}
			/*
			Case 3. no floats, no alphanumeric so there must be ints and this must be
			a discrete file. Otherwise it's a mistake.
			*/
			else
			{
				if (0 < theCountInts)
					theOverallType = kTraittype_Discrete;	
				else
					throw FormatError ("import does not contain meaningful data");
			}	
		}

		mColumnTypes.push_back (theOverallType);
		assert (mColumnTypes.size() == (uint) (i+1));	
	}
	
	/*
	Split up into submatrices by making two copies and stepping
	backwards along them and deleting the collumns that don't belong.
	*/
	mDiscDataMatrix = mDataMatrix;
	mContDataMatrix = mDataMatrix;
	mDiscColNames = mColNames;
	mContColNames = mColNames;
	
	for (int i = mColumnTypes.size() - 1; 0 <= i; i--)
	{
		switch (mColumnTypes[i])
		{
			case kTraittype_Discrete:
				// so delete continuous copies
				mContColNames.erase (mContColNames.begin() + i);
				for (uint j = 0; j < mContDataMatrix.size(); j++)
					mContDataMatrix[j].erase(mContDataMatrix[j].begin() + i);
				break;
				
			case kTraittype_Continuous:
				// so delete discrete copies
				mDiscColNames.erase (mDiscColNames.begin() + i);
				for (uint j = 0; j < mDiscDataMatrix.size(); j++)
					mDiscDataMatrix[j].erase (mDiscDataMatrix[j].begin() + i);			
				break;
				
			default:
				// should never get here
				assert (false); 
		}
	}
	
	mDataMatrix.clear();
	
}


traittype_t TabDataReader::extractData (string& iRawDataStr)
//: what sort of information in in this string?
{
	if (iRawDataStr == "?")
		return kTraittype_Missing;
		
	if (iRawDataStr == "-")
		return kTraittype_Gap;
		
	if (sbl::isFloat (iRawDataStr))
	{
		//if (sbl::toDouble(iRawDataStr) < 0.0)
		//	throw FormatError ("negative species richness not permitted");
		//else
			return kTraittype_Float;
	}
	
	if (sbl::isWhole (iRawDataStr))
	{
		//if (sbl::toLong(iRawDataStr) < 0)
		//	throw FormatError ("negative species richness not permitted");
		//else
			return kTraittype_Int;
	}
		
	return kTraittype_Alpha;
}

void TabDataReader::extractColNames ()
//: see if there are column names, remove from matrix & record them
{
	// extract first row to see if there are column names
	bool theColsHaveNames = true;
	for (stringvec_t::size_type i = 1; i < mDataMatrix[0].size(); i++)
	{
		traittype_t theType = extractData (mDataMatrix[0][i]);
		if (theType != kTraittype_Alpha)
		{
			theColsHaveNames = false;	
			break;
		}
	}
	
	// if the first row from the second element onwards has only
	// alphanumeric strings, then it has column titles. Stitch them into
	// the column names. Eliminate them from the matrix.
	if (theColsHaveNames)
	{
		mColNames.empty();
		for (stringvec_t::size_type i = 1; i < mDataMatrix[0].size(); i++)
			mColNames.push_back (nexifyString (mDataMatrix[0][i].c_str()));	
		
		mDataMatrix.erase (mDataMatrix.begin());
	}
	else
	{
		// give them a bunch of empty names
		mColNames.resize (mDataMatrix[0].size(), string(""));
	}
}

 
void  TabDataReader::extractRowNames ()
//: remove row names from matrix & record them
// The row names must be there. Throw if they aren't
{
	// These _must_ be there.
	mRowNames.empty();
	for (stringmatrix_t::size_type i = 0; i < mDataMatrix.size(); i++)
	{
		traittype_t theType = extractData (mDataMatrix[i][0]);
		if (theType != kTraittype_Alpha)
			throw FormatError ("data file must contain taxa names");
		else
		{
			// store the name and delete it from the matrix
			mRowNames.push_back (nexifyString (mDataMatrix[i][0].c_str()));	
			(mDataMatrix[i]).erase ((mDataMatrix[i]).begin());
		}	
	}
}


 
// *** END ***************************************************************/

