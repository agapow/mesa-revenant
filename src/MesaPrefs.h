/**************************************************************************
MesaPrefs.h - data structure for Mesa prefs and settings

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- we allow direct access to these vars to minimise

**************************************************************************/

#pragma once
#ifndef MESAPREFS_H
#define MESAPREFS_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include "StringUtils.h"
#include <fstream>


// *** CONSTANTS & DEFINES

enum pref_verbosity_t
{
	kPrefVerbosity_Quiet,
	kPrefVerbosity_Normal,
	kPrefVerbosity_Loud
};

enum pref_logging_t
{
	kPrefLogging_Disabled,
	kPrefLogging_Enabled
};

enum pref_analysisout_t
{
	kPrefAnalysisOut_AllScreen,   // all to screen
	kPrefAnalysisOut_AllFile,     // all to file
	kPrefAnalysisOut_Normal       // auto to file, quick to screen
};


// CLADE LABELLING
enum pref_cladelabels_t
{
	kPrefCladeLabels_Phylo = 0,
	kPrefCladeLabels_Caic,
	kPrefCladeLabels_Series
};

static const char* kPrefCladeLabels_Cstrs [] =
{
	"label phylogenetically",
	"label CAIC-style",
	"label as hierachy"
};


// TREATMENT OF DEAD NODES

enum pref_preservenodes_t
{
	kPrefPreserveNodes_None = 0,      // kill
	kPrefPreserveNodes_Root,          // kill & delete
	kPrefPreserveNodes_RootChildren   // kill, delete, collapse
};

static const char* kPrefPreserveNodes_Cstrs [] =
{
	"allow killing of all nodes",
	"preserve the root",
	"preserve the root & its children"
};


// ALLOW TOTAL EXTINCTION

enum pref_totalextinction_t
//: what happens when the last species in a phylogeny is killed?
/*
There are two ways:
1. allow it to happen, the epoch terminates
2. "bounce" from the lower limit - the last species can never go extinct
*/
{
	kPrefTotalExtinction_Allowed = 0,
	kPrefTotalExtinction_Bounce
};

static const char* kPrefTotalExtinction_Cstrs [] =
{
	"allow last species to die",
	"last species never dies"
};




// *** CLASS DECLARATION *************************************************/

class MesaPrefs
{
public:
	// Lifecycle
	MesaPrefs ()
		: mVerbosity (kPrefVerbosity_Normal)
		, mAnalysisOut (kPrefAnalysisOut_AllScreen)
		, mLogging (kPrefLogging_Disabled)
		, mCase (sbl::kCase_Upper)
		, mCladeLabels (kPrefCladeLabels_Phylo)
		, mPadNumericOutput (true)
		, mPreserveNodes (kPrefPreserveNodes_None)
		, mWriteTaxaBlock (false)
		, mWriteTransCmd (true)
		, mTimeGrain (0.0001)
		{}
	// ~MesaPrefs		();

	// The values
	pref_verbosity_t       mVerbosity;
	pref_analysisout_t     mAnalysisOut;
	pref_logging_t         mLogging;
	sbl::case_t            mCase;
	pref_cladelabels_t     mCladeLabels;
	bool                   mPadNumericOutput;
	pref_preservenodes_t   mPreserveNodes;
	bool                   mWriteTaxaBlock;
	bool                   mWriteTransCmd;
	double                 mTimeGrain;

	// Depreciated & Debug
	void	validate	()
	{
		assert ((kPrefVerbosity_Quiet <= mVerbosity) and
			(mVerbosity <= kPrefVerbosity_Loud));
		/* assert ((kPrefLogging_Disabled <= mLogging) and
			(mLogging <= kPrefVerbosity_Loud)); */
		assert ((sbl::kCase_Upper <= mCase) and (mCase <= sbl::kCase_Mixed));
	}
};



#endif
// *** END ***************************************************************/



