/**************************************************************************
MesaGlobals.cpp - application & model-wise variables

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- Variables whose state is referenced throughout the whole program.
- I'm deeply, deeply uneasy about using globals but the transmission of
  state changes throughout the app is cumbersome, requires a lot of
  messaging, tramp functions and  smart updating. Following a reading of
  McConnell (Code Complete), this usage appears to be the best.
  
Changes:
- 99.10.30: created.
  
**************************************************************************/


// *** INCLUDES

#include "MesaGlobals.h"
#include "TaxaTraitMatrix.h"
#include "TreeWrangler.h"
#include "Reporter.h"


// *** MAIN BODY *********************************************************/
// Initialise the static members of the globals class

sbl::RandomService	MesaGlobals::mRng;
MesaPrefs				MesaGlobals::mPrefs;

DiscTraitMatrix*		MesaGlobals::mDiscDataP;
ContTraitMatrix*		MesaGlobals::mContDataP;
TreeWrangler*			MesaGlobals::mTreeDataP;
MesaTree*				MesaGlobals::mActiveTreeP;
Reporter*				MesaGlobals::mReporterP;


// *** END ***************************************************************/
