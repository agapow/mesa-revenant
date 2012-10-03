/**************************************************************************
RichnessReader.cpp - imports richness data from tab-delimited stream

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

**************************************************************************/


// *** INCLUDES

#include "RichnessReader.h"
#include "StringUtils.h"
#include "Error.h"

using sbl::FormatError;


// *** CONSTANTS & DEFINES

// *** MAIN BODY ********************************************************/

traittype_t RichnessReader::sniffData (std::string& iRawDataStr)
//: what sort of information in in this string?
{
	if (sbl::isWhole (iRawDataStr))
		return kTraittype_Float; // this is the big change!
		
	return TabReader::sniffData (iRawDataStr);
}


void RichnessReader::getRichnessData (ContTraitMatrix& ioWrangler)
{
	sniffFormat ();
	if (mDatatype != kTraittype_Continuous)
		throw FormatError ("import file does not contain species richness data");

	ContTraitMatrix::size_type theOldNumCols = ioWrangler.countCols();
	getData (ioWrangler);
	
	if (0 == mColNames.size()) // if no col names
	{
		ContTraitMatrix::size_type i;
		for (i = theOldNumCols; i < ioWrangler.countCols(); i++)
			ioWrangler.setColName (i, (concatIntToString ("rich_", i + 1)).c_str());
	}
}


// *** END ***************************************************************/
