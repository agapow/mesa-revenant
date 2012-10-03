/**************************************************************************
StringScanner - a reader/lexer for parsing an STL string

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>


About:
- breaks an STL string up into logical units for parsing.


Changes:
- 01.1.15: Created.

To Do:
- 

**************************************************************************/


// *** INCLUDES

#include "StringScanner.h"
#include "StringUtils.h"

SBL_NAMESPACE_START

#define ASSERT_DO(condition,action)  if (not (condition)) action

// *** CONSTANTS & DEFINES

// *** STREAM SCANNER ****************************************************/

// *** LIFECYCLE *********************************************************/

// see header for implementation



// *** SERVICES **********************************************************/
// These are the low level fxns that must be overridden in every derived
// class. 

// *** MOVEMENT FUNCTIONS
// For changing position within the stream

posn_t StringScanner::GetPosn ()
//: where are we within the string?
// Remember, this is the position of the chracter you are about to read.
// So 0 is at the beginning of the string. -1 by convention is the end.
{
	return (posn_t) mPosn;
}


posn_t StringScanner::Goto (int iPosn)
//: go to a designated position
{
	posn_t theOldPosn = GetPosn ();
	mPosn = (ulong) iPosn;
	return theOldPosn;
}


// *** LOW-LEVEL READING

bool StringScanner::GetChar (char& oCurrChar)
//: grab the next chracter from the source, return sucess
// The lowest, most primitve reading function. Doesn't skip comments or
// do anything other than get the next raw character and keep track of
// position.
{
	// Preconditions & Preparation:
	assert (mSrcStringP);
	std::string::size_type theLen = mSrcStringP->size();
	//assert ((mPosn == kScan_SrcEnd) or ((0 <= mPosn) and (mPosn < theLen)));

	ASSERT_DO ((mPosn == kScan_SrcEnd) or ((0 <= (int) mPosn) and (mPosn < theLen)),
		DBG_MSG (mPosn));
	
	// Main:
	if ((int) mPosn == kScan_SrcEnd)
		return false;

	oCurrChar = (*mSrcStringP)[mPosn];
	mPosn++;
	if (mPosn == theLen)
		mPosn = (ulong) kScan_SrcEnd;
	
	// Postconditions & Return:
	assert (((int) mPosn == kScan_SrcEnd) or ((0 <= mPosn) and (mPosn < theLen)));
	
	return true;
}


// *** DEPRECATED & TEST FUNCTIONS **************************************/

void StringScanner::UnreadChar (char iLastChar)
{
	// Preconditions & Preparation:
	// valid string & position
	assert (mSrcStringP);
	assert (((int) mPosn == kScan_SrcEnd) or ((0 <= mPosn) and
		(mPosn < mSrcStringP->size())));

	// just to shut compiler up
	iLastChar = iLastChar;
	
	// Main:
	switch (mPosn)
	{
		case kScan_SrcBegin:
			// at beginning, stay at beginning
			break;
		case kScan_SrcEnd:
			// at end, goto last character
			mPosn = mSrcStringP->size() - 1;
			break;
		default:
			mPosn -= 1;
	}		
	
	// Preconditions & Preparation:
	assert (((int) mPosn == kScan_SrcEnd) or ((0 <= mPosn) and
		(mPosn < mSrcStringP->size())));
}


SBL_NAMESPACE_STOP

// *** END

