/**************************************************************************
Dbg_Matrix.cpp - test harness and functions for the Matrix class

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

About:
- Just a lump o' functions for stressing and testing the Matrix class.
  
Changes:
- 01.1.18: farmed off from Matrix header, to test new ctors

To Do:
- 

**************************************************************************/


// *** INCLUDES

#include "Matrix.h"

#ifdef SBL_DBG_MATRIX

#include <vector>
#include <iterator>

SBL_NAMESPACE_START


// *** DEPRECIATED & TEST FUNCTIONS **************************************/

template <typename X>
void
Matrix<X>::validate ()
{
	// assert (mNumRows == base_type::size());
	for (size_type i = 1; i < countRows(); i++)
		assert (countCols() == (*this)[i].size());
}


template <typename X>
void
Matrix<X>::dump ()
{
	DBG_MSG ("Contents of Matrix:");
	DBG_MSG ("Num of rows: " << countRows() << ", num of cols: "
		<< countCols());
	for (size_type i = 0; i < countRows(); i++)
	{ 
		DBG_PREFIX << "Row " << i <<":  ";
		for (size_type j = 0; j < countCols(); j++)
			DBG_STREAM << (*this)[i][j] << " ";
		DBG_STREAM << std::endl;
	}
}


void debugMatrix ()
{
	DBG_MSG ("*** Testing T Matrix");

	DBG_MSG ("Test ctors ...");
	Matrix<int>	the1stMatrix;
	the1stMatrix.validate();
	the1stMatrix.dump();
	Matrix<int>	the2ndMatrix (5,3);
	the2ndMatrix.validate();
	the2ndMatrix.dump();
	Matrix<int>	the3rdMatrix (6,4,2);
	the3rdMatrix.validate();
	the3rdMatrix.dump();
	Matrix<int>	the4thMatrix (the3rdMatrix);
	the4thMatrix.validate();
	the4thMatrix.dump();	
	std::vector<int>	theTestVec (4, 2);
	std::vector< std::vector<int> >	theTestVecVec (3, theTestVec);
	Matrix<int>	the5thMatrix (theTestVecVec);
	the5thMatrix.validate();
	the5thMatrix.dump();
	
	// assign
	// foreach
	// transform
	
	DBG_MSG ("Test copy semantics ...");
	the4thMatrix = the2ndMatrix;
	the4thMatrix = the1stMatrix;
	the4thMatrix.dump();
	the4thMatrix.validate();
	
	DBG_MSG ("Test resizing ...");
	the4thMatrix.resize (2,6);
	the4thMatrix.resize (6,7,3);
	the4thMatrix.resize (0,0,6);
	the4thMatrix.resize (6,7,4);
	the4thMatrix.dump();
	the4thMatrix.validate();

	DBG_MSG ("Test insertion ...");
	the4thMatrix.fill (5);
	std::vector<int>		theSrcArr;
	for (int i = 0; i < 10; i++)
		theSrcArr.push_back (i);
	DBG_VECTOR(&theSrcArr);	
	the4thMatrix.insertRow (3, theSrcArr.begin(), theSrcArr.end());
	the4thMatrix.insertCol (3, theSrcArr.begin(), theSrcArr.begin() + 3);
	the4thMatrix.dump();
	the4thMatrix.validate();
	
	DBG_MSG ("Test extraction ...");
	std::back_insert_iterator< std::vector<int> > myInsertIter (theSrcArr);
	the4thMatrix.getRow (3, myInsertIter);
	the4thMatrix.getCol (2, myInsertIter);
	DBG_VECTOR(&theSrcArr);
	
	DBG_MSG ("*** Finished testing T Matrix");
}


SBL_NAMESPACE_STOP

#endif

// *** END ***************************************************************/

