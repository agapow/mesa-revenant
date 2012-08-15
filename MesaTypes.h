/**************************************************************************
MesaTypes.h - project-wide & platform-independent types & constants

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at Silwood
  Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- Largely to save defining the callbacks everywhere.
  
Changes:
- 99.10.10: created.
- 01.01.25: added matrix definitions
- 01.5.23: added constants
  
 **************************************************************************/

#pragma once
#ifndef MESATYPES_H
#define MESATYPES_H


// *** INCLUDES

#include "callback.hpp"
#include <vector>
#include <string>


// *** CONSTANTS & DEFINES

const char kCaicBranchFileSuffix[] = ".BLen";
const char kCaicPhylogenyFileSuffix[] = ".Phyl";
const char kCaicRichnessFileSuffix[] = ".Rich";
const char kCaicDataFileSuffix[] = ".dat";


// message types, esp. for callbacks

enum msg_t {
   kMsg_Progress,
   kMsg_Comment,
   kMsg_Error,
   kMsg_Analysis
};


// Call backs for model reporting to app
typedef CBFunctor2<msg_t, const char*> progcallback_t;
typedef CBFunctor1<int> callback_int_t;

// adts for holding raw data
typedef std::vector<std::string> stringvec_t;
typedef std::vector<stringvec_t> stringmatrix_t;

// the datatypes for characters
typedef double conttrait_t;
typedef std::string disctrait_t;

/**
The type of data in input file entries.
 */
enum traittype_t {
   kTraittype_Both = 0,
   kTraittype_Discrete,
   kTraittype_Continuous,
   kTraittype_All,
   kTraittype_Unknown,
   kTraittype_Float, ///< floating and continuous
   kTraittype_Missing, ///< missing, unavailable or unknown
   kTraittype_Gap, ///< gaps, absent and non-applicable
   kTraittype_Alpha, ///< alphanumeric (character) strings
   kTraittype_Int ///< integer data
};


typedef int colIndex_t; // column index
const colIndex_t kColIndex_None = -1; // when no column is selected
typedef std::vector<colIndex_t> colIndexArr_t;

typedef double mesatime_t; // timeunits
typedef double rate_t; // rate

enum nodetype_t {
   kNodetype_All = 0,
   kNodetype_Tips,
   kNodetype_Living,
   kNodetype_Internal
};

enum fileformat_t {
   kFileFormat_Nexus,
   kFileFormat_Pag,
   kFileFormat_Unknown
};


// BOUNDARY BEHAVIOUR

enum evolbound_t
//: what happens when a trait evolves past a limit
{
   kEvolBound_Replace = 0,
   kEvolBound_Truncate,
   kEvolBound_Ignore
};



#endif
// *** END ***************************************************************/



