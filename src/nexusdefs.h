#ifndef __NEXUSDEFS_H
#define __NEXUSDEFS_H

// maximum number of states that can be stored; the only limitation is that this
// number be less than the maximum size of an int (not likely to be a problem).
// A good number for this is 76, which is 96 (the number of distinct symbols
// able to be input from a standard keyboard) less 20 (the number of symbols
// symbols disallowed by the Nexus standard for use as state symbols)
//
#define NCL_MAX_STATES         76

#include <stdexcept>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iomanip>
#include <assert.h>
#include <functional>

#include "nxsstring.h"

using std::less;
using std::binary_function;


typedef std::vector<bool> BoolVect;
typedef std::vector<int> IntVect;
typedef std::vector<nxsstring> LabelList;
typedef std::set< int, less<int> > IntSet;
typedef std::map< int, LabelList, less<int> > LabelListBag;
typedef std::map< nxsstring, nxsstring, less<nxsstring> > AssocList;
typedef std::map< nxsstring, IntSet, less<nxsstring> > IntSetMap;
typedef std::vector<LabelList> AllelesVect;
typedef long streampos_t;

struct stri_equal : public binary_function<nxsstring,nxsstring,bool> {
   bool operator()(const nxsstring& x, const nxsstring& y) const;
};
 
typedef LabelList::size_type llistsz_t; // PMA
#endif



