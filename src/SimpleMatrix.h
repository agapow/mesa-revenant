/**************************************************************************
SimpleMatrix.h - a minimal representation of a 2D array

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve>


About:
- The simplest possible matrix that can be made: a vector of vectors, 
  relying on inheritance to get most of the functionality.
- This will be expanded as obvious & ncessary functions are discovered.
- Note this does nothing to ensure the matrix is square (i.e. all the 
  rows are the same length).

Changes:
- 01.1.25: coded after informal use.

To Do:
- 

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "Error.h"
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

SBL_NAMESPACE_START


// *** CONSTANTS AND DEFINES

// *** CLASS TEMPLATE ****************************************************/

template <typename X>
class SimpleMatrix : public std::vector< std::vector<X> >
{
public:
	// PUBLIC TYPE INTERFACE
	typedef 		typename std::vector<X>						row_type;
	typedef 		typename std::vector<row_type>			base_type;
	typedef 		typename row_type::size_type				size_type;
	typedef 		typename row_type::reference				reference;
	typedef 		typename row_type::const_reference		const_reference;
	
	// LIFECYCLE

		
	// ACCESSORS
	size_type countRows	() const
		{ return base_type::size(); }

	size_type countCols	() const
	{
		if (countRows () == 0)
			return 0;
		else
			return (base_type::at(0)).size();
	}

	X& at (size_type iRowIndex, size_type iColIndex)
	{
		assertValidIndex (iRowIndex, iColIndex);
		return (base_type::at (iRowIndex)).at (iColIndex);
	}
	
	// MUTATORS
	void fill (const X& ikfillVal)
	{
		for (size_type i = 0; i < countRows(); i++)
			for (size_type j = 0; j < countCols(); j++)
				(*this)[i][j] = ikfillVal;
	}

	void resize (size_type iNewNumRows, size_type iNewNumCols, const X& iNewVal = X())
	// NOTE: we must do it this (long) way and not via resizeRows() & resizeCols()
	// because otherwise we would never be able to resize a [0][0] matrix.
	{
		if ((iNewNumRows == 0) or (iNewNumCols == 0))
		{
			base_type::clear ();
		}
		else
		{
			base_type::resize (iNewNumRows, row_type (iNewNumCols, iNewVal));
			for (size_type i = 0; i < countRows(); i++)
				(*this)[i].resize (iNewNumCols, iNewVal);
		}
	}

	void resizeCols (size_type iNewNumCols, const X& iNewVal = X())
	//: change the number of columns
	{
		if (countRows() != 0)
		{
			for (size_type i = 0; i < countRows(); i++)
				(*this)[i].resize (iNewNumCols, iNewVal);
		}
	}
	
	void resizeRows (size_type iNewNumRows, const X& iNewVal = X())
	//: change the number of rows
	{
		if (countCols() != 0)
			base_type::resize (iNewNumRows, row_type (countCols(), iNewVal));
	}
	
	void addCols (size_type iColIncr, const X& iNewVal = X())
	//: increase the number of columns by this increment
	{
		assert (0 < iColIncr);
		
		resizeCols (countCols() + iColIncr, iNewVal);
	}

	void addRows (size_type iRowIncr, const X& iNewVal = X())
	//: increase the number of rows by this increment
	{
		assert (0 < iRowIncr);
		
		resizeRows (countRows() + iRowIncr, iNewVal);
	}
	
	void appendRow (row_type& iNewRow)
		{ push_back (iNewRow); }

	void swapRows (size_type iIndexA, size_type iIndexB)
	{
		assert (0 <= iIndexA);
		assert (iIndexA < countRows());
		assert (0 <= iIndexB);
		assert (iIndexB < countRows());
		
		if (iIndexA == iIndexB)
			return;
			
		std::swap ((*this)[iIndexA], (*this)[iIndexB]);
	}
	
	void deleteRow (size_type iRowIndex)
	{
		assertValidIndex (iRowIndex, 0);
		if (countRows() == 1)
			resize (0, 0);
		else
			erase (base_type::begin() + iRowIndex);
	}

	void deleteCol (size_type iColIndex)
	{
		if (countCols() == 1)
			resize (0, 0);
		else
		{
			for (size_type i = 0; i < countRows (); i++)
			{
				assertValidIndex (i, iColIndex);
				row_type* theRowP = &(base_type::at(i));
				theRowP->erase (theRowP->begin() + iColIndex);
			}
		}
	}
	
	
	// DEPRECIATED & DEBUG

	// INTERNALS
protected:
	void assertValidIndex (size_type iRowIndex, size_type iColIndex)
	{
		assert (0 <= iRowIndex);
		assert (iRowIndex < countRows());
		assert (0 <= iColIndex);
		assert (iColIndex < countCols());
	}	

private:	
};


template <typename X>
class LabelledSimpleMatrix : public SimpleMatrix<X>
{
public:
	// PUBLIC TYPE INTERFACE
	typedef 	SimpleMatrix<X>				         base_type;
	typedef 	typename base_type::size_type			size_type;
	typedef 	typename base_type::row_type			row_type;
	typedef	typename std::string						label_type;
	typedef	typename std::vector<label_type>		labellist_type;
	
	// LIFECYCLE
	
	// ACCESSORS
	label_type getRowName (size_type i)
	{
		assert (0 <= i);
		assert (i < base_type::countRows());
		assert (i < mRowNames.size());
		return mRowNames[i];
	}

	label_type getColName (size_type i)
	{
		assert (0 <= i);
		assert (i < base_type::countCols());
		assert (i < mColNames.size());
		return mColNames[i];
	}

	template <typename OUTITER>
	void getRowNames (OUTITER iOutIter)
	{
		for (labellist_type::size_type i = 0; i < mRowNames.size(); i++)
		{
			*iOutIter = mRowNames[i];
			iOutIter++;
		}
	}

	template <typename OUTITER>
	void getColNames (OUTITER iOutIter)
	{
		for (labellist_type::size_type i = 0; i < mColNames.size(); i++)
		{
			*iOutIter = mColNames[i];
			iOutIter++;
		}
	}

	bool hasRowName (const char* iSearchStr)
	{
		labellist_type::iterator q = std::find (mRowNames.begin(),
			mRowNames.end(), label_type (iSearchStr));
		return (q != mRowNames.end());
	}
			
	size_type findRowNameIndex (const char* iSearchStr)
	{
		return findNameIndex (mRowNames, iSearchStr);
	}

	size_type findColNameIndex (const char* iSearchStr)
	{
		return findNameIndex (mColNames, iSearchStr);
	}
		
	int getMaxLenRowName ()
	{
		unsigned int theMaxLen = 0;
		for (size_type i = 0; i < base_type::countRows (); i++)
		{
			if (theMaxLen < mRowNames[i].size())
				theMaxLen = mRowNames[i].size();
		}
		return theMaxLen;
	}
	
	// MUTATORS
	void setRowName (size_type i, const char* iNewName)
	{
		assert (0 <= i);
		assert (i < base_type::countRows());
		assert (i < mRowNames.size());
		mRowNames[i] = iNewName;
	}

	void setColName (size_type i, const char* iNewName)
	{
		assert (0 <= i);
		assert (i < base_type::countCols());
		assert (i < mColNames.size());
		mColNames[i] = iNewName;
	}
	
	
	void resize (size_type iNewNumRows, size_type iNewNumCols, const X& iNewVal = X())
	{
		base_type::resize (iNewNumRows, iNewNumCols, iNewVal);
		mRowNames.resize (iNewNumRows, "");
		mColNames.resize (iNewNumCols, "");
	}

	void appendRow (row_type& iNewRow, const char* iNewName = "")
	{
		base_type::appendRow (iNewRow);
		mRowNames.push_back (label_type (iNewName));
	}

	void sortRows ()
	//: sort rows by their title
	{
		// sort the names
		labellist_type theSortedNames = mRowNames;
		std::sort (theSortedNames.begin(), theSortedNames.end());
		
		// go along the sorted names and swap the rows as need be
		for (size_type theNewPos = 0; theNewPos < theSortedNames.size(); theNewPos++)
		{
			// find the old position
			size_type theOldPos = findRowNameIndex (theSortedNames[theNewPos].c_str());
			swapRows (theNewPos, theOldPos);
		}
	}

	void addRows (size_type iRowIncr, const X& iNewVal = X())
	//: increase the number of rows by this increment
	{
		assert (0 < iRowIncr);	
		base_type::addRows (iRowIncr, iNewVal);
		for (size_type i = 0; i < iRowIncr; i++)
			mRowNames.push_back (label_type (""));
	}

	void swapRows (size_type iIndexA, size_type iIndexB)
	{
		std::swap (mRowNames[iIndexA], mRowNames[iIndexB]);
		base_type::swapRows (iIndexA, iIndexB);
	}

	void deleteRow (size_type iRowIndex)
	{
		mRowNames.erase (mRowNames.begin() + iRowIndex);
		base_type::deleteRow (iRowIndex);
	}

	void deleteRow (const char* iRowLabel)
	{
		size_type theIndex = findRowNameIndex (iRowLabel);
		deleteRow (theIndex);
	}

	void deleteCol (size_type iColIndex)
	{
		mColNames.erase (mColNames.begin() + iColIndex);
		base_type::deleteCol (iColIndex);
	}
	
	// DEPRECIATED & DEBUG
	
	// INTERNALS
private:	
	labellist_type		mRowNames;
	labellist_type		mColNames;
		
	size_type findNameIndex (labellist_type& iLabels, const char* iSearchStr)
	{
		labellist_type::iterator q = std::find (iLabels.begin(),
			iLabels.end(), label_type (iSearchStr));
		if (q != iLabels.end())
			return (q - iLabels.begin());
		else
		{
			std::stringstream theBuffer;
			theBuffer << "label \'" << iSearchStr << "\' does not exist in container";
			throw sbl::IndexError ((theBuffer.str()).c_str());
		}
		return -1; // just to keep the compiler happy
	}	
};



SBL_NAMESPACE_STOP

// *** END ***************************************************************/
