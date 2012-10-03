#ifndef __XNEXUS_H
#define __XNEXUS_H

#include "nxsstring.h"
//#include <iosfwd>

typedef int ncl_streampos;

//
// XNexus exception class
//
class XNexus
{
public:
	nxsstring msg;
	//streampos pos;
	int pos;
	long line;
	long col;

	XNexus( nxsstring s, ncl_streampos fp = 0, long fl = 0L, long fc = 0L );
};

#endif
