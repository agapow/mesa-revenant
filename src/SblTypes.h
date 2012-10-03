/**************************************************************************
SblTypes.h - library header

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK) Silwood
  Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- All the Sibil peculiar types. Mostly they are defined for brevity.
  
Changes:
- 00.7.25: moved out from "Sbl.h"
  
To Do:
- other types will no doubt be needed.

**************************************************************************/


#pragma once
#ifndef SBLTYPES_H
#define SBLTYPES_H


SBL_NAMESPACE_START

// *** UNSIGNED NUMBERS
typedef unsigned char	uchar;
typedef unsigned int		uint;
typedef unsigned long	ulong;

// *** FIXED SIZE NUMBERS
// TO DO: unsure how to get size out of system, hence assert.
// assert (sizeof (ulong) == 4);
typedef ulong				word32;
typedef unsigned char	word8;
typedef word8 				byte;


SBL_NAMESPACE_STOP

#endif
// *** END ***************************************************************/



