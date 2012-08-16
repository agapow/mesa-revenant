/**
@file     Sbl.h

@author   Paul-Michael Agapow
          Dept. Biology, University College London, WC1E 6BT, UK.
@mail     <mail://p.agapow@ic.ac.uk> 
@web      <http://www.agapow.net/code/sibil/>

@note    Note that as a convention, in the whitespace stripping functions
   use "erase" to mean indicate a removal in place.

@todo   Clean up interface.
@todo   Incorporate some of the string utilty functions.


@brief    The global header for the Sibil library.

This includes all the Sibil peculiar types, debug macros & necessary
headers, placed in one file for convenience. (Especially for users who might
only want to pick and choose from parts of the library.)
*/

#pragma once
#ifndef SBL_H
#define SBL_H



/** A shorthand for opening the Sibil namespace. */
#define SBL_NAMESPACE_START	namespace sbl {
/** A shorthand for closing the Sibil namespace. */
#define SBL_NAMESPACE_STOP		}


#include "SblTypes.h"
#include "SblDebug.h"
			

#endif
// *** END ***************************************************************/



