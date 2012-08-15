/**************************************************************************
Numerics.h - assorted mathematical functions

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef MSA_NUMERICS_H
#define MSA_NUMERICS_H


// *** INCLUDES

// *** CONSTANTS & DEFINES

// *** FUNCTION DECLARATIONS *********************************************/

double  logN (double iBase, double iTerm);
double  logE (double iTerm);
// double  log2 (double iTerm);

double  factorial (long iTerm);

double factorialGosper (long n);
double factorialGamma (long n);
double factorialStirling (long n);
// double gamma (double z);
long swing (long n);
long recFactorial (long n);


#endif
// *** END ***************************************************************/



