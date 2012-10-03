/**************************************************************************
CharTest.h - a function for making comparsions between characters

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>

Notes:
- should be called CharTest.h

**************************************************************************/


// *** INCLUDES

#include "CharComparator.h"
#include "ActionUtils.h"
#include "StringUtils.h"
#include <string>

using std::string;


// *** CONSTANTS & DEFINES

// *** CLASS DEFINITION **************************************************/

CharComparator::CharComparator
(colIndex_t iCharIndex, comparator_t iComp, conttrait_t iRhsVal)
: mCharType (kTraittype_Continuous), mContRhsVal (iRhsVal)
{
	init (iCharIndex, iComp);
}


CharComparator::CharComparator
(colIndex_t iCharIndex, comparator_t iComp, disctrait_t iRhsVal)
: mCharType (kTraittype_Discrete), mDiscRhsVal (iRhsVal)
{
	init (iCharIndex, iComp);
}


void CharComparator::init (colIndex_t iColIndex, comparator_t iComp)
{
	mCharIndex = iColIndex;
	mComp = iComp;
}



// *** SERVICES **********************************************************/

bool CharComparator::testCharacter (nodeiter_t iNode)
{
	if (mCharType == kTraittype_Continuous)
		return testCharacterCont (iNode);
	if (mCharType == kTraittype_Discrete)
		return testCharacterDisc (iNode);
	else
	{
		assert (false);   // should never get here
		return false;     // to shut compiler up
	}
}


bool CharComparator::testCharacterCont (nodeiter_t iNode)
{
	conttrait_t theLhsVal = getContData (iNode, mCharIndex);

	switch (mComp)
	{
		case kComparator_LessThan:
			return (theLhsVal < mContRhsVal);
			break;

		case kComparator_LessThanOrEqual:
			return (theLhsVal <= mContRhsVal);
			break;

		case kComparator_Equal:
			return (theLhsVal == mContRhsVal);
			break;

		case kComparator_NotEqual:
			return (theLhsVal != mContRhsVal);
			break;

		case kComparator_GreaterThan:
			return (theLhsVal > mContRhsVal);
			break;

		case kComparator_GreaterThanOrEqual:
			return (theLhsVal >= mContRhsVal);
			break;

		default:
			assert (false);   // should never get here
			return false;     // just to shut compiler up
	}
}


bool CharComparator::testCharacterDisc (nodeiter_t iNode)
{
	disctrait_t theLhsVal = getDiscData (iNode, mCharIndex);

	switch (mComp)
	{
		case kComparator_LessThan:
			return (theLhsVal < mDiscRhsVal);
			break;

		case kComparator_LessThanOrEqual:
			return (theLhsVal <= mDiscRhsVal);
			break;

		case kComparator_Equal:
			return (theLhsVal == mDiscRhsVal);
			break;

		case kComparator_NotEqual:
			return (theLhsVal != mDiscRhsVal);
			break;

		case kComparator_GreaterThan:
			return (theLhsVal > mDiscRhsVal);
			break;

		case kComparator_GreaterThanOrEqual:
			return (theLhsVal >= mDiscRhsVal);
			break;
	}

    assert (false);   // should never get here
    return false;     // just to shut compiler up
}



// *** I/O ***************************************************************/

const char* CharComparator::describe ()
{
	static string theBuffer;

	theBuffer = "where ";

	if (mCharType == kTraittype_Continuous)
		theBuffer += "continuous ";
	else if (mCharType == kTraittype_Discrete)
		theBuffer += "discrete ";
	else
		// should never get here
		assert (false);

	theBuffer += "trait #";
	theBuffer += sbl::toString (mCharIndex + 1);
	theBuffer += " ";

	switch (mComp)
	{
		case kComparator_LessThan:
			theBuffer += "<";
			break;

		case kComparator_LessThanOrEqual:
			theBuffer += "<=";
			break;

		case kComparator_Equal:
			theBuffer += "=";
			break;

		case kComparator_NotEqual:
			theBuffer += "not =";
			break;

		case kComparator_GreaterThan:
			theBuffer += ">";
			break;

		case kComparator_GreaterThanOrEqual:
			theBuffer += ">=";
			break;

		default:
			assert (false); // should never get here
	}

	theBuffer+= " ";
	if (mCharType == kTraittype_Continuous)
		theBuffer+= sbl::toString (mContRhsVal);
	else if (mCharType == kTraittype_Discrete)
		theBuffer+= mDiscRhsVal;
	else
		assert (false); // should never get here

	return theBuffer.c_str();
}



// *** DEBUG & DEPRECATED ***********************************************/

void CharComparator::validate ()
// not much we can actually test here
{
	if (mCharType == kTraittype_Discrete)
		assert (mDiscRhsVal != "");
	assert (0 <= mComp);
	assert (mComp < kComparator_NumItems);
}



// *** END ***************************************************************/



