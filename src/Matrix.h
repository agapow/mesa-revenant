/**************************************************************************
Matrix.h - a two dimensional array class

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve>
- Inspired by Urich Breymann's "Designing Components With the C++ STL"
  and much public domain code on matrices.

About:
- Note that the container is necessarily homogenous: there is no simple
  way to do a container with heterogenous types.
- Note the semantics of inserting rows and columns. The insert is resized
  to the matrix, not the other way around.

Changes:
- 00.7.17: coded, tested, released.
- 00.7.20: introduced isVector() & related methods.
- 01.1.16: adjusted names to new scheme.

To Do:
- adjust private/protected labels on type interface.
- Assign, foreach, transform, row push / pop / erase; resize num rows /
  cols, swap rows. foreach, transform
- Uncertain whether the internal templated fxns (like those using an
  output iterator) can be shifted out of the declaration.
- Symmetric/diagonal and sparse matrices.
- Should there be a basal class for all Matrix representations? And how
  do we inherit from that and vector?
- Mathematical functions.

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include <vector>

SBL_NAMESPACE_START


// *** CONSTANTS AND DEFINES

#define SBL_DBG_MATRIX

#ifdef SBL_DBG_MATRIX
	void debugMatrix ();
#endif


// *** CLASS TEMPLATE ****************************************************/

template <typename X>
class Matrix : public std::vector< std::vector<X> >
{
public:
	// PUBLIC TYPE INTERFACE
	typedef 		std::vector<X>					row_type;
	typedef 		std::vector<typename row_type>			base_type;
	typedef 		row_type::size_type				size_type;
	typedef 		row_type::reference				reference;
	typedef 		row_type::const_reference		const_reference;

	// LIFECYCLE
	Matrix (size_type x = 0, size_type y = 0, const X& ikFill = X())
		: base_type (x, row_type(y))
	//: ctor that inits matrix as size (x,y) with initial value ikFill
	{
		//adjustDims (x, y);
		init (ikFill);

		// Postconditions:
		//assert (0 <= mNumRows);
		//assert (0 <= mNumCols);
	}

	Matrix (base_type& iVecVector)
	//: ctor that accepts a vector of vectors
		: base_type (iVecVector)
	{
		//adjustDims (countRows(), countRows());
	}

/*	Matrix (const Matrix<X>& ikSrcMatrix)
		: base_type (ikSrcMatrix.countRows(), row_type (ikSrcMatrix.countCols()))
	//: copy ctor
	// Don't know why I wrote this - surely the default must be fine?
	{
		//adjustDims (ikSrcMatrix.mNumRows, ikSrcMatrix.mNumCols);

		for (size_type i = 0; i < countRows(); i++)
			for (size_type j = 0; j < countCols(); j++)
				(*this)[i][j] = ikSrcMatrix[i][j];

		// Postconditions:
		//assert (0 <= mNumRows);
		//assert (0 <= mNumCols);
	} */

	/* ~Matrix () {} */


	// OPERATORS
		// Matrix assigns correctly

	// ACCESSORS
	size_type countRows	() const;
	size_type countCols	() const;
	size_type size	() const;

	// need to explicitly do this otherwise the base methods are scoped
	// the methods below.
	row_type& at (size_type iRowIndex)
		{ return base_type::at (iRowIndex); }
	const row_type& at (size_type iRowIndex) const
		{ return base_type::at (iRowIndex); }
	reference at (size_type iRowIndex, size_type iColIndex)
		{ return (at(iRowIndex)).at(iColIndex); }
	const_reference at (size_type iRowIndex, size_type iColIndex) const
		{ return (at(iRowIndex)).at(iColIndex); }

	bool isRowVector () const	{ return (countCols() == 1); }
	bool isColVector () const 	{ return (countRows() == 1); }
	bool isVector () const 		{ return (IsRowVector () or isRowVector ()); }
	bool isSquare () const 		{ return (countCols () == countRows ()); }

	template <class InsertIter>
	InsertIter getRow (size_type iRowIndex, InsertIter iOutputBegin)
	{
		for (size_type i = 0; i < countCols(); i++, iOutputBegin++)
			*iOutputBegin = (*this)[iRowIndex][i];

		return iOutputBegin;
	}

	template <class InsertIter>
	InsertIter getCol (size_type iColIndex, InsertIter iOutputBegin)
	{
		for (size_type i = 0; i < countRows(); i++, iOutputBegin++)
			*iOutputBegin = (*this)[i][iColIndex];

		return iOutputBegin;
	}

	// MUTATORS
	void fill (const X& ikfillVal);
	void resize (size_type iNewNumRows, size_type iNewNumCols, const X& iNewVal = X());

	// insert before this row
	// note the insert semantics, the insert fits the Matrix, not the other way around
	template <class SrcIter>
	void insertRow
	(size_type iSuccRowIndex, size_type iNumCopies, SrcIter iSrcBegin, SrcIter iSrcEnd)
	{
		size_type theNumCols = countCols();
		row_type theNewRow (iSrcBegin, iSrcEnd);
		theNewRow.resize (theNumCols);
		insert (begin() + iSuccRowIndex, iNumCopies, theNewRow);
		//adjustDims (mNumRows + iNumCopies, mNumCols);
	}

	template <class SrcIter>
	void insertRow
	(size_type iSuccRowIndex, SrcIter iSrcBegin, SrcIter iSrcEnd)
	{
		insertRow (iSuccRowIndex, 1, iSrcBegin, iSrcEnd);
	}

	template <class SrcIter>
	void insertCol
	(size_type iSuccColIndex, size_type iNumCopies, SrcIter iSrcBegin, SrcIter iSrcEnd)
	{
		size_type theNumRows = countRows();
		row_type theNewCol (iSrcBegin, iSrcEnd);
		theNewCol.resize (theNumRows);
		for (size_type i = 0; i < theNumRows; i++)
			(*this)[i].insert ((*this)[i].begin() + iSuccColIndex, iNumCopies, theNewCol[i]);
		//adjustDims (mNumRows, mNumCols + iNumCopies);
	}

	template <class SrcIter>
	void insertCol
	(size_type iSuccColIndex, SrcIter iSrcBegin, SrcIter iSrcEnd)
	{
		insertCol (iSuccColIndex, 1, iSrcBegin, iSrcEnd);
	}

	template <class SrcIter>
	void appendRow
	(size_type iNumCopies, SrcIter iSrcBegin, SrcIter iSrcEnd)
	{
		row_type theNewRow (iSrcBegin, iSrcEnd);
		size_type theNumCols = countCols();
		theNewRow.resize (theNumCols);
		insert (end(), iNumCopies, theNewRow);
		// adjustDims (mNumRows + iNumCopies, mNumCols);
	}

	template <class SrcIter>
	void appendRow (SrcIter iSrcBegin, SrcIter iSrcEnd)
	{
		appendRow (1, iSrcBegin, iSrcEnd);
	}

	// DEPRECIATED & DEBUG
#ifdef SBL_DBG_MATRIX
	void validate ();
	void dump ();
#endif

	// INTERNALS
private:
	// size_type	mNumRows, mNumCols;
	void init (const X& ikInitVal)
		{ fill (ikInitVal); }
	// void adjustDims (size_type iNewNumRows, size_type iNewNumCols);
};


// *** FRIEND FUNCTIONS **************************************************/

template<class X>
std::ostream& operator <<
(std::ostream& ioOutStrm, const Matrix<X>& ikOutMatrix )
{
    typedef Matrix<X>::size_type size_type;

    for (size_type i = 0; i < ikOutMatrix.countRows(); i++)
    {
       ioOutStrm << endl << i <<" :  ";
       for (size_type j = 0; j < ikOutMatrix.countCols(); j++)
            ioOutStrm << ikOutMatrix[i][j] <<" ";
    }

    ioOutStrm << endl;
    return ioOutStrm;
}


// *** MEMBER FUNCTION DEFINITIONS ***************************************/




// *** ACCESSORS

template <typename X>
inline
typename Matrix<X>::size_type
Matrix<X>::countRows () const
{
	// DBG_MSG (mNumRows << " " << base_type::size());
	return base_type::size();
}

template <typename X>
inline
typename Matrix<X>::size_type
Matrix<X>::countCols () const
{
	// assert ((mNumCols == 0) or (mNumCols == (*this)[0].size()));
	if (countRows () == 0)
		return 0;
	else
		return (at(0)).size();
}

template <typename X>
inline
typename Matrix<X>::size_type
Matrix<X>::size () const
{
	return (countCols()*countRows());
}



// *** MUTATORS

template <typename X>
void
Matrix<X>::fill (const X& ikFillVal)
{
	for (size_type i = 0; i < countRows(); i++)
		for (size_type j = 0; j < countCols(); j++)
			(*this)[i][j] = ikFillVal;
}


template <typename X>
void
Matrix<X>::resize
(size_type iNewNumRows, size_type iNewNumCols, const X& iNewVal)
{
	if ((iNewNumRows == 0) or (iNewNumCols == 0))
	{
		clear ();
		// mNumRows = mNumCols = 0;
	}
	else
	{
		base_type::resize (iNewNumRows, row_type (iNewNumCols, iNewVal));
		// mNumRows = iNewNumRows;
		for (size_type i = 0; i < countRows(); i++)
			(*this)[i].resize (iNewNumCols, iNewVal);
		// mNumCols = iNewNumCols;
	}
}


// *** INTERNALS

/*
template <typename X>
inline
void
Matrix<X>::adjustDims (size_type iNewNumRows, size_type iNewNumCols)
{
		// Preconditions:
		assert (0 <= iNewNumRows);
		assert (0 <= iNewNumCols);

		if ((iNewNumRows == 0) or (iNewNumCols == 0))
			iNewNumRows = iNewNumCols = 0;

		mNumRows = iNewNumRows;
		mNumCols = iNewNumCols;
}
*/

SBL_NAMESPACE_STOP

// *** END ***************************************************************/
