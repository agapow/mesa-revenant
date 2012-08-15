/**************************************************************************
TaxaTraitMatrix.h - for juggling labelled arrays of data

Credits:
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve>


About:
- allows an abstraction of the data away from a strict and limited Nexus
representation. (Required because the NCL makes it hard to modify data.)

Changes:
- 01.1.25: coded.

To Do:
- 

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "MesaTypes.h"
#include "SimpleMatrix.h"
#include "CharStateSet.h"
#include "MesaUtils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using sbl::SimpleMatrix;
using std::string;

// *** CONSTANTS AND DEFINES

// *** CLASS TEMPLATE ****************************************************/

template <typename X>
class TaxaTraitMatrix : public sbl::LabelledSimpleMatrix<X>
{
public:
	// PUBLIC TYPE INTERFACE
	typedef sbl::LabelledSimpleMatrix<X>	       base_type;
	typedef typename base_type::size_type			 size_type;
   typedef typename base_type::labellist_type    labellist_type;
	
	// LIFECYCLE
	TaxaTraitMatrix ()
		: mDataTypeStr (""), mCharPrefix("char_")
		{}
	TaxaTraitMatrix (const char* iBlockIdStr, const char* iCharPrefix)
		: mDataTypeStr (iBlockIdStr), mCharPrefix (iCharPrefix)
		{}
	
	// ACCESSORS
	size_type countTaxa ()
		{ return base_type::countRows(); } ///
	size_type countChars ()
		{ return base_type::countCols(); } ///

	void collectRowNames (stringvec_t& ioNameVec)
	{
		std::back_insert_iterator<stringvec_t> theOutputIter (ioNameVec);
		getRowNames (theOutputIter);
	}

	void setRowNames (stringvec_t& ioNameVec)
	{
		assert (ioNameVec.size() == base_type::countRows ()); ///
		for (long i = 0; i < base_type::countRows (); i++) ///
			setRowName (i, ioNameVec[i].c_str());
	}

	void collectColNames (stringvec_t& ioNameVec)
	{
		std::back_insert_iterator<stringvec_t> theOutputIter (ioNameVec);
		base_type::getColNames (theOutputIter); ///
	}
	
	template <typename OUTITER>
	void getRowNames (OUTITER iOutIter)
	{
		for (typename labellist_type::size_type i = 0; i < base_type::countRows(); i++)
		{
			*iOutIter = getRowName(i);
			iOutIter++;
		}
	}
		
	X& getData (const char* iRowName, size_type iColIndex)
	// TO DO: use binary search
	// NOTE: assertion of indexes takes place in base class
	{
		size_type theRowIndex = base_type::findRowNameIndex (iRowName);
		return at(theRowIndex, iColIndex);
	}
	
	
	// MUTATORS
	void resize (size_type iNewRows, size_type iNewCols, const X& iNewVal = X())
	{
		size_type theOldNumCols = base_type::countCols();
		base_type::resize (iNewRows, iNewCols, iNewVal);
		// if there are new columns, name them
		for (size_type i = theOldNumCols; i < base_type::countCols(); i++)
		{
			setColName (i, (concatIntToString (mCharPrefix.c_str(), i + 1)).c_str());
		}
	}
	
	void cloneRow (const char* iOrigName, const char* iNewName)
	//: duplicate the named row under the new name
	// if table is empty, don't worry about it
	{
		if (countTaxa() == 0)
		{
			return;
		}
		else
		{
			size_type theOrigIndex = base_type::findRowNameIndex (iOrigName);
			base_type::addRows (1);
			size_type theNewRowIndex = base_type::countRows() - 1;
			setRowName (theNewRowIndex, iNewName);
			for (size_type i = 0; i < base_type::countCols(); i++)
				(*this)[theNewRowIndex][i] = (*this)[theOrigIndex][i];
		}
	}
	
	
	void addCols (size_type iColIncr, const X& iNewVal = X())
	//: increase the number of columns by this increment & give default name
	{
		assert (0 < iColIncr);
		resize (base_type::countRows(), base_type::countCols() + iColIncr, iNewVal);
	}

	void deleteTaxa (const char* ikTaxaLabel)
	{
		base_type::deleteRow (ikTaxaLabel);
	}
	

	// I/O
	void summarise (std::ostream& ioOutStream)
	{
		if (countTaxa() == 0)
		{
			assert (countChars() == 0);
			ioOutStream << "There is no " << mDataTypeStr << " data." <<
				std::endl;
		}
		else
		{
			bool	theSingleChar = (1 == countChars());
			bool	theSingleTaxa = (1 == countTaxa());
			
			ioOutStream << "There " << (theSingleChar ? "is " : "are ") <<
				countChars() << " " << mDataTypeStr << " trait" << 
				(theSingleChar ? "" : "s") << " listed for " << countTaxa() <<
				(theSingleTaxa ? " taxon" : " taxa.") << std::endl;
		}
	}
	
	void detailedReport (std::ostream& ioOutStream)
	{
		summarise (ioOutStream);
		if (0 < countTaxa())
		{
			// print out the columns names
			ioOutStream << "   " << "Columns: ";
			for (size_type i = 0; i < base_type::countCols (); i++)
			{
				ioOutStream << " " << getColName (i);
			}
			ioOutStream << std::endl;
			reportMatrix (ioOutStream);
		}
	}

	void reportMatrix (std::ostream& ioOutStream)
	// note the matrix is indented
	{
		// gather string representations of the data		
		sbl::SimpleMatrix<std::string> 	theNumberMatrix;
		std::string::size_type			theMaxDataLen = 0;
		size_type					theNumTaxa = countTaxa ();
		size_type					theNumChars = countChars ();

		theNumberMatrix.resize (theNumTaxa, theNumChars);
		
		for (size_type i = 0; i < theNumTaxa; i++)
		{
			for (size_type j = 0; j < theNumChars; j++)
			{
				std::stringstream theBuffer;
				theBuffer.precision (4);
				theBuffer << at(i,j);
				theNumberMatrix[i][j] = theBuffer.str();
				std::string::size_type theStrSize = theNumberMatrix[i][j].size();
				if (theMaxDataLen < theStrSize)
					theMaxDataLen = theStrSize;
			}
		}
		assert (0 < theMaxDataLen);
		
		// ... and pad them
		/*
		for (ContTraitMatrix::size_type i = 0; i < theNumTaxa; i++)
		{
			for (ContTraitMatrix::size_type j = 0; j < theNumChars; j++)
			{
				theNumberMatrix[i][j].resize (theMaxDataLen + 2);
			}
		}
		*/
		
		// get the max length of a taxa name
		int theMaxNameLen = base_type::getMaxLenRowName ();
		
		// print them out
		for (sbl::SimpleMatrix<string>::size_type i = 0; i < theNumTaxa; i++)
		{
			// print the taxa name in a set width
			ioOutStream << "   " << std::setw (theMaxNameLen + 3) << base_type::getRowName (i);
				
			for (SimpleMatrix<string>::size_type j = 0; j < theNumChars; j++)
			{
				// print every row in a set width
				ioOutStream << std::setw (theMaxDataLen + 2) << theNumberMatrix[i][j];
			}
			// terminate the row
			ioOutStream << std::endl;
		}	
	}


	// DEPRECIATED & DEBUG
	
	// INTERNALS
protected:
	std::string		mDataTypeStr;
	std::string		mCharPrefix;
	
private:
};


class ContTraitMatrix : public TaxaTraitMatrix<conttrait_t>
{
public:
	// PUBLIC TYPE INTERFACE
	typedef TaxaTraitMatrix<conttrait_t>	       base_type;

	// LIFECYCLE
	ContTraitMatrix ()
		: base_type ("continuous", "cchar_")
		{}

	// I/O
	
	// DEPRECIATED & DEBUG
	
	CharStateSet   mStates;
	// INTERNALS
private:
};


class DiscTraitMatrix : public TaxaTraitMatrix<disctrait_t>
{
public:
	// PUBLIC TYPE INTERFACE
	typedef TaxaTraitMatrix<disctrait_t>	       base_type;

	// LIFECYCLE
	DiscTraitMatrix ()
		: base_type ("discrete", "dchar_")
		{}
	
	void gatherCharStates ()
	{
		size_type theNumTaxa = countTaxa ();
		size_type theNumChars = countChars ();
		for (size_type i = 0; i < theNumTaxa; i++)
		{
			for (size_type j = 0; j < theNumChars; j++)
			{
				disctrait_t theChar = at (i, j);
				if ((not mStates.isMember (theChar)) and 
					(theChar != "?") and
					(theChar != "-"))
					mStates.addState (theChar);
			}
		}
		mStates.sort();
	}

	CharStateSet mStates;
	
	// DEPRECIATED & DEBUG
	
	// INTERNALS
private:
};


// *** END ***************************************************************/