/**************************************************************************
MesaGlobals.h - application & model-wise variables

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at Silwood
  Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef MESAGLOBALS_H
#define MESAGLOBALS_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaPrefs.h"
#include "RandomService.h"

class DiscTraitMatrix;
class ContTraitMatrix;
class TreeWrangler;
class MesaTree;
class Reporter;


// *** VARIABLES

static MesaPrefs gPrefs;

class MesaGlobals
{
public:
	static MesaPrefs				mPrefs;
	
	static sbl::RandomService	mRng;
	
	static DiscTraitMatrix* 	mDiscDataP;
	static ContTraitMatrix* 	mContDataP;
	static TreeWrangler* 		mTreeDataP;
	static MesaTree*				mActiveTreeP;
	static Reporter*				mReporterP;
};


#endif
// *** END ***************************************************************/



