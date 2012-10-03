/**************************************************************************
TabReader.cpp - imports data from tab-delimited stream

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- takes data from tab-delimited files (rows are named for taxa) and adds
  them to the pre-existing data.
- currently we assume that the table is all continuous or all discrete.
  
Changes:
- 01.1.29: created.

To Do:
- check name doesn't clash with later sbl::TabReader

**************************************************************************/


// *** INCLUDES

#include "TabReader.h"
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


// *** CONSTANTS & DEFINES


// *** MAIN BODY *********************************************************/

// *** LIFECYCLE *********************************************************/

// *** SERVICES **********************************************************/

void TabReader::read ()
//: imports a tab-delimited file into the existing data
// assume imported data is rows of species names followed by tab seperated
// data. Alll columns must be the same length. Parsing stops at the first
// blank line. 
// TO DO: detect things that aren't tab-delimited files
{
	// configure scanner
	sbl::StreamScanner	theScanner (mInStream);
	theScanner.SetComments ("", "");
	theScanner.SetLineComment ("#");


	// just check this is a a tab delimited file
	string 			theInLine;
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
	sniffColNames ();
	sniffRowNames();
	sniffFormat ();
	
	/*
	DBG_MSG ("COLNAMES:");
	for (long i = 0; i < mColNames.size(); i++)
		DBG_MSG (i << ": \"" << mColNames[i] << "\"")
	DBG_MSG ("ROWNAMES:");
	for (long i = 0; i < mRowNames.size(); i++)
		DBG_MSG (i << ": \"" << mRowNames[i] << "\"")
	DBG_MSG ("DATA:");
	for (long i = 0; i < mDataMatrix.size(); i++)
	{
		for (long j = 0; j < mDataMatrix[j].size(); j++)
			DBG_MSG (i << "," << j << ": \"" << mDataMatrix[i][j] << "\"")
	}	
	*/
		
	// Report progress
	int	theNumTaxa = mDataMatrix.size();
	int	theNumChars = mDataMatrix[0].size(); // because of taxa name
	bool	theSingleRow = (theNumTaxa == 1);
	bool	theSingleCol = (theNumChars == 1);
	
	stringstream theBuffer;
	theBuffer << "There " << (theSingleRow ? "is" : "are") << " " <<
		theNumTaxa << " " << (theSingleRow ? "taxon" : "taxa") <<
		" with " << theNumChars << " " << 
		((mDatatype == kTraittype_Discrete) ? "discrete" : "continuous") << 
		" trait" << (theSingleCol ? "" : "s") << " in the import data.";

	string theTmpString = theBuffer.str();
	mProgressCb (kMsg_Progress, theTmpString.c_str());	
}
	

void TabReader::getData (DiscTraitMatrix& ioWrangler)
// NOTE: nasty bit of duplication between here and the other getData().
// Possibly a better way to do this.
{
	// if it's not the correct data type, get out of here
	if (mDatatype != kTraittype_Discrete)
		return;
	
	// two choices: there's stuff in the wrangler already or not
	DiscTraitMatrix::size_type	theNumTaxa = ioWrangler.countTaxa();	
	DiscTraitMatrix::size_type	theNumOldChars = ioWrangler.countChars();
	stringvec_t::size_type		theNumNewChars = mDataMatrix[0].size();

	if (theNumTaxa == 0)
	{
		// if the wrangler is currently empty
		assert (theNumOldChars == 0);
		
		// resize to appropriate size
		theNumTaxa = mDataMatrix.size();
		ioWrangler.resize (theNumTaxa, theNumNewChars);
		
		// name rows
		assert (mRowNames.size() == mDataMatrix.size());
		for (stringvec_t::size_type i = 0; i < theNumTaxa; i++)
		{
			ioWrangler.setRowName (i, mRowNames[i].c_str());
		}	

		// stuff things into the the wrangler
		for (DiscTraitMatrix::size_type i = 0; i < theNumTaxa; i++)
		{
			for (DiscTraitMatrix::size_type j = 0; j < theNumNewChars; j++)
			{
				ioWrangler[i][j] = mDataMatrix[i][j];
			}
		}	
		
		ioWrangler.sortRows ();
	}
	else
	{
		// if there is stuff already in the wrangler
		assert (theNumOldChars != 0);
		
		// sort the new data into a new matrix by the order of taxa already in wrangler		
		// collect row names
		stringvec_t  theRowNames;
	  	back_insert_iterator<stringvec_t> theOutputIter (theRowNames);
		ioWrangler.getRowNames (theOutputIter);
		// filter & order data into new matrix according to order of names
		orderRows (theRowNames);
		
		// expand the wrangler is the appropriate size
		ioWrangler.addCols (theNumNewChars); 
		
		// stuff things into the the wrangler
		for (DiscTraitMatrix::size_type i = 0; i < theNumTaxa; i++)
		{
			for (DiscTraitMatrix::size_type j = 0; j < theNumNewChars; j++)
			{
				ioWrangler[i][theNumOldChars + j] = mDataMatrix[i][j];
			}
		}	
	}
		
	// name the columns if doable
	for (stringvec_t::size_type i = 0; i < mColNames.size(); i++)
		ioWrangler.setColName (theNumOldChars + i, mColNames[i].c_str());			
}


void TabReader::getData (ContTraitMatrix& ioWrangler)
// NOTE: nasty bit of duplication between here and the other getData().
// Possibly a better way to do this.
{
	// if it's not the correct data type, get out of here
	if (mDatatype != kTraittype_Continuous)
		return;
	
	// two choices: there's stuff in the wrangler already or not
	ContTraitMatrix::size_type	theNumTaxa = ioWrangler.countTaxa();	
	ContTraitMatrix::size_type	theNumOldChars = ioWrangler.countChars();
	stringvec_t::size_type			theNumNewChars = mDataMatrix[0].size();

	if (theNumTaxa == 0)
	{
		// if the wrangler is currently empty
		assert (theNumOldChars == 0);
		
		// resize to appropriate size
		theNumTaxa = mDataMatrix.size();
		ioWrangler.resize (theNumTaxa, theNumNewChars);
		
		// name rows
		assert (mRowNames.size() == mDataMatrix.size());
		for (stringvec_t::size_type i = 0; i < theNumTaxa; i++)
		{
			ioWrangler.setRowName (i, mRowNames[i].c_str());
		}	

		// stuff things into the the wrangler
		for (ContTraitMatrix::size_type i = 0; i < theNumTaxa; i++)
		{
			for (ContTraitMatrix::size_type j = 0; j < theNumNewChars; j++)
			{
				ioWrangler[i][j] = sbl::toDouble(mDataMatrix[i][j]);
			}
		}	
		
		ioWrangler.sortRows ();
	}
	else
	{
		// if there is stuff already in the wrangler
		assert (theNumOldChars != 0);
		
		// sort the new data into a new matrix by the order of taxa already in wrangler		
		// collect row names
		stringvec_t  theRowNames;
	  	back_insert_iterator<stringvec_t> theOutputIter (theRowNames);
		ioWrangler.getRowNames (theOutputIter);
		// filter & order data into new matrix according to order of names
		orderRows (theRowNames);
		
		// expand the wrangler is the appropriate size
		ioWrangler.addCols (theNumNewChars); 
		
		// stuff things into the the wrangler
		for (ContTraitMatrix::size_type i = 0; i < theNumTaxa; i++)
		{
			for (ContTraitMatrix::size_type j = 0; j < theNumNewChars; j++)
			{
				ioWrangler[i][theNumOldChars + j] = sbl::toDouble(mDataMatrix[i][j]);
			}
		}	
	}
		
	// name the columns if doable
	for (stringvec_t::size_type i = 0; i < mColNames.size(); i++)
		ioWrangler.setColName (theNumOldChars + i, mColNames[i].c_str());			
}


void TabReader::orderRows (stringvec_t& ioNames)
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

void TabReader::sniffFormat ()
//: detects the format of the data read in
{
	// check the data to see what it's like.	
	// count how many floats, ints and alphanumerics etc you get
	int theCountFloats, theCountInts, theCountAlpha, theCountOther;
	theCountFloats = theCountInts = theCountAlpha = theCountOther = 0;
	
	for (stringmatrix_t::size_type i = 0; i < mDataMatrix.size(); i++)
	{
		if (mDataMatrix[i].size() != mDataMatrix[0].size())
			throw FormatError ("imported data matrix has rows of unequal length");
		for (stringvec_t::size_type j = 0; j < mDataMatrix[i].size(); j++)
		{
			if (mDataMatrix[i][j] == "")
				throw FormatError ("imported data matrix contains null entries");
				
			// attempt to ascertain type of data
			traittype_t theType = sniffData (mDataMatrix[i][j]);
			if (theType == kTraittype_Alpha)
				theCountAlpha++;	
			else if (theType == kTraittype_Float)
				theCountFloats++;
			else if (theType == kTraittype_Int)
				theCountInts++;
			else
				theCountOther++;
		}			
	}
	
	// How we determine the datatype (discrete vs continuous)
	// 1. if there are alphanumeric strings and no floats, it must be discrete.
	// If there are alphanumerics and floats, it must be a mistake.
	if (0 < theCountAlpha)
	{
		if (0 < theCountFloats)
			throw FormatError ("import column contains continuous and discrete data");
		else mDatatype = kTraittype_Discrete;	
	}
	// 2. if there are floats and any number of ints, it must be continuous.
	else if (0 < theCountFloats)
	{
		assert (theCountAlpha == 0);
		mDatatype = kTraittype_Continuous;	
	}
	// 3. no floats, no alphanumeric so there must be ints and this must be
	// a discrete file. Otherwise it's a mistake.
	else
	{
		if (0 < theCountInts)
			mDatatype = kTraittype_Discrete;	
		else
			throw FormatError ("import does not contain meaningful data");
	}	
}


traittype_t TabReader::sniffData (string& iRawDataStr)
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

void  TabReader::sniffColNames ()
//: see if there are column names, remove from matrix & record them
{
	// sniff first row to see if there are column names
	bool theColsHaveNames = true;
	for (stringvec_t::size_type i = 1; i < mDataMatrix[0].size(); i++)
	{
		traittype_t theType = sniffData (mDataMatrix[0][i]);
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
			mColNames.push_back (mDataMatrix[0][i]);	
		
		mDataMatrix.erase (mDataMatrix.begin());
	}
}

 
void  TabReader::sniffRowNames ()
//: remove row names from matrix & record them
// The row names must be there. Throw if they aren't
{
	// These _must_ be there.
	mRowNames.empty();
	for (stringmatrix_t::size_type i = 0; i < mDataMatrix.size(); i++)
	{
		traittype_t theType = sniffData (mDataMatrix[i][0]);
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

