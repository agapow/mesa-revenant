/**
@file
@author   Paul-Michael Agapow
          Dept. Biology, University College London, WC1E 6BT, UK.
          <mail://p.agapow@ic.ac.uk> 
          <http://www.agapow.net/code/sibil/>
          
Part of MeSA <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>.
*/

#pragma once
#ifndef TABDATAREADER_H
#define TABDATAREADER_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include "TaxaTraitMatrix.h"
#include <fstream>
#include <string>
#include <vector>


// *** CONSTANTS & DEFINES



// *** CLASS DECLARATION *************************************************/

/**
Read data from a tab-delimited stream.

This parses a tab-delimited series of rows and columns. The first column
is assumed to be row (species) names. Column names are optional. Use in
the following fashion:
@code
	TabDataReader theReader (ioStream, ioCallback);
	theReader.read();
	theReader.getData (ioDiscreteMatrix);
	theReader.getData (ioContinuousMatrix);
@endcode

@todo   Check name doesn't clash with later sbl::TabDataReader.
*/
class TabDataReader
{
public:
/// @name LIFECYCLE
//@{
	TabDataReader (std::ifstream& iInStream, progcallback_t& ikProgressCb)
		: mInStream (iInStream)
		, mProgressCb (ikProgressCb)
		{}
		
	~TabDataReader ()
		{ mInStream.close(); }
//@}
	
/// @name SERVICES
//@{
	void	read ();
	void	getData (DiscTraitMatrix& ioWrangler);
	void	getData (ContTraitMatrix& ioWrangler);
//@}

/// @name DEPRECATED & DEBUG
	void	validate	() {}
//@}

/// @name INTERNALS
private:
	std::ifstream&			mInStream;   // the stream that is being read
	progcallback_t			mProgressCb; // for sending progress reports
	stringmatrix_t 		mDataMatrix, mContDataMatrix, mDiscDataMatrix;
	stringvec_t				mRowNames;
	stringvec_t			   mColNames;
	stringvec_t			   mContColNames;
	stringvec_t			   mDiscColNames;
	int                  mNumDiscTraits, mNumContTraits;
	std::vector<traittype_t>   mColumnTypes;
	
	void	orderRows	(stringvec_t& iRowNames);
	traittype_t	extractData (std::string& iRawDataStr);
	void extractFormat ();
	void extractColNames ();
	void extractRowNames ();
//@}
};


#endif
// *** END ***************************************************************/
