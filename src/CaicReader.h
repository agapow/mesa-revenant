/**************************************************************************
CaicReader.h - loads/reads data from caic file stream

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef CAICREADER_H
#define CAICREADER_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include "TaxaTraitMatrix.h"
#include "TreeWrangler.h"
#include <fstream>
#include <string>
#include <map>


// *** CONSTANTS & DEFINES


// *** CLASS DECLARATION *************************************************/

class CaicReader
{
public:
	// LIFECYCLE
	CaicReader (std::ifstream* iPhylStream, std::ifstream* iBlenStream,
		const progcallback_t& ikProgressCb);
		
	~CaicReader ();
	
	// SERVICES	
	void readData ();
	void	getData (DiscTraitMatrix& ioWrangler);
	void	getData (ContTraitMatrix& ioWrangler);
	void	getData (TreeWrangler& ioWrangler);
		
	// DEPRECIATED & DEBUG
	void	validate	();

	// INTERNALS
private:	
	progcallback_t mCallBack;
	std::ifstream *mPhylStreamP, *mBlenStreamP;
	std::map <std::string, std::string>  mPhylCodeMap;
	stringmatrix_t mBlenMatrix;
	
};


#endif
// *** END ***************************************************************/



