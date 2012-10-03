/**************************************************************************
CaicWriter.h - formats & pours data to output stream

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- 
  
**************************************************************************/

#pragma once
#ifndef CAICWRITER_H
#define CAICWRITER_H


// *** INCLUDES

#include "TaxaTraitMatrix.h"
#include "TreeWrangler.h"
#include <iostream>


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class CaicWriter
{
public:
	// Lifecycle
	CaicWriter (std::ostream* iPhylStream, std::ostream* iBlenStream,
		std::ostream* iDataStream)
		: mPhylStreamP (iPhylStream)
		, mBlenStreamP (iBlenStream)
		, mDataStreamP (iDataStream)
		//: ctor accepts open streams to write to as parameter
		{
		}
		
	~CaicWriter ()
		//: dtor tidies up
		{}
				
	// SERVICES
	void	writeData (ContTraitMatrix& iContWrangler, DiscTraitMatrix& iDiscWrangler);
	void	writeTrees (TreeWrangler& iWrangler);

	// DEPRECIATED & DEBUG
	void	validate	()
		{ /* nothing as yet */ }

	// INTERNALS
private:
	std::ostream*   mPhylStreamP;
	std::ostream*   mBlenStreamP;
	std::ostream*   mDataStreamP;
	
};


#endif
// *** END ***************************************************************/
