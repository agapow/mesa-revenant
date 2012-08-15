/**************************************************************************
TabReader.h - imports data from tab-delimited stream

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- takes data from tab-delimited files (rows are named for taxa) and adds
  them to the pre-existing data.
  
**************************************************************************/

#pragma once
#ifndef TABREADER_H
#define TABREADER_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include "TaxaTraitMatrix.h"
#include <fstream>
#include <string>


// *** CONSTANTS & DEFINES


// *** CLASS DECLARATION *************************************************/

class TabReader
{
public:
	// LIFECYCLE
	TabReader (std::ifstream& iInStream, progcallback_t& ikProgressCb)
		: mInStream (iInStream),
		mProgressCb (ikProgressCb),
		mDatatype (kTraittype_Unknown)
		//: ctor that reads data from stream reporting problems
		{}
		
	~TabReader ()
		{ mInStream.close(); }
	
	// SERVICES	
	void	read ();
	void	getData (DiscTraitMatrix& ioWrangler);
	void	getData (ContTraitMatrix& ioWrangler);

	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
protected:
	virtual traittype_t	sniffData (std::string& iRawDataStr);
	traittype_t			mDatatype;
	void					sniffFormat ();
	stringvec_t			mColNames;

private:
	progcallback_t			mProgressCb;
	std::ifstream&			mInStream;
	stringmatrix_t 		mDataMatrix;
	stringvec_t				mRowNames;
	
	void	orderRows	(stringvec_t& iRowNames);
	void	sniffColNames ();
	void	sniffRowNames ();
};


#endif
// *** END ***************************************************************/
