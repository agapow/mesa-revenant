#ifndef __DISCRETEMATRIX_H
#define __DISCRETEMATRIX_H

#include <iostream>
#include <vector>
#include "discretedatum.h"
using std::ostream;

//
// DiscreteMatrix class
//
class DiscreteMatrix
{
	typedef std::vector<DiscreteDatum>	datavec_t;
	typedef std::vector<datavec_t>		datamatrix_t;
	
	datamatrix_t data;

   friend class CharactersBlock;
   friend class AllelesBlock;

private:
   void AddState( DiscreteDatum& d, int value );
	int  IsGap( DiscreteDatum& d );
	int  IsMissing( DiscreteDatum& d );
   int  IsPolymorphic( DiscreteDatum& d );
   DiscreteDatum& GetDiscreteDatum( int i, int j );
	int  GetNumStates( DiscreteDatum& d );
   int  GetState( DiscreteDatum& d, int i = 0 );
	void SetGap( DiscreteDatum& d );
	void SetMissing( DiscreteDatum& d );
   void SetPolymorphic( DiscreteDatum& d, int value );
   void SetState( DiscreteDatum& d, int value );

public:
	DiscreteMatrix( int rows, int cols );
	~DiscreteMatrix();

	int countRows();	// PM
	int countCols(); // PM
	void addCols (int iColIncr); // PM
	
	void AddRows( int nAddRows );
	void AddState( int i, int j, int value );
	void CopyStatesFromFirstTaxon( int i, int j );
   void DebugSaveMatrix( ostream& out, int colwidth = 12 );
   int  DuplicateRow( int row, int count, int startCol = 0, int endCol = -1 );
	void Flush();
	int  GetState( int i, int j, int k = 0 );
	int  GetNumStates( int i, int j );
   int  GetObsNumStates( int j );
	int  IsGap( int i, int j );
	int  IsMissing( int i, int j );
	int  IsPolymorphic( int i, int j );
	void Reset( int rows, int cols );
   void SetGap( int i, int j );
   void SetMissing( int i, int j );
	void SetPolymorphic( int i, int j, int value = 1 );
	void SetState( int i, int j, int value );
};


#endif
