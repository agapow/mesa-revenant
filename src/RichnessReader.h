/**************************************************************************
RichnessReader.h - imports richness data from tab-delimited stream

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef RICHNESSREADER_H
#define RICHNESSREADER_H


// *** INCLUDES

#include "TabReader.h"


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class RichnessReader: public TabReader
{
public:
	// LIFECYCLE
	RichnessReader (std::ifstream& iInStream, progcallback_t& ikProgressCb)
		: TabReader (iInStream, ikProgressCb)
		{}
		
	// SERVICES	
	void	getRichnessData (ContTraitMatrix& ioWrangler);
	
	// DEPRECIATED & DEBUG

	// INTERNALS
private:
	traittype_t	sniffData (std::string& iRawDataStr);
};


#endif
// *** END ***************************************************************/
