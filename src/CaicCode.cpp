/*
CaicCode - a representation of a Caic phylogenetic code

Credits:
- By Paul-Michael Agapow, 2000, Dept. Biology, Imperial College at Silwood
  Park, Ascot, Berks, SL5 7PY, UK.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>
- Part of the Sibil code library.

About:
- A simple way of creating, tracking and manipulating phylogenetic codes,
  as used in the CAIC suite of programs. [Purvis, 1995 #850]

Changes:
- (99.12.10) created.
- (00.9.21) created again.
- (01.5.14) changed some names to match new standards, and centralised the
  ordering and progression of codes into the three class functions.

To Do:
- Change to being a derivation of string?

*/


#include "CaicCode.h"

using std::string;

SBL_NAMESPACE_START

typedef  CaicCode::size_type   size_type;


// *** LIFECYCLE ********************************************************/
#pragma mark -

/// Default constructor, initialises code as root.
CaicCode::CaicCode ()
{
	init ("");
}

/// Initialises code as root.
CaicCode::CaicCode (char iCodeChar)
{
	init (&iCodeChar);
}

CaicCode::CaicCode (const char* iCodeCstr)
{
	init (iCodeCstr);
}

CaicCode::CaicCode (CaicCode::size_type iCladeIndex)
{
	char theCharCode = indexToChar (iCladeIndex);
	init (&theCharCode);
}

void CaicCode::init (const char* iCodeCstr)
{
	mCode = iCodeCstr;
}


// *** ACCESSORS ********************************************************/
#pragma mark -

bool CaicCode::operator == (const std::string& ikString)
{
   return (mCode == ikString) ? true : false;
}

bool CaicCode::operator == (const char* ikCstr)
{
   return (mCode == std::string(ikCstr));
}

bool CaicCode::operator != (const std::string& ikString)
{
   return not (mCode == ikString);
}

bool CaicCode::operator != (const char* ikCstr)
{
   return not (mCode == std::string(ikCstr));
}

char& CaicCode::operator []	(int iIndex)
{
   return mCode[iIndex];
}

bool CaicCode::isParentOf (CaicCode& ikChild)
//: is this code the parent of the parameter code?
// Literally, is this code equal to the initial (length-1) chars of the
// parameter?
{
	// if it could be the immediate parent
	if (mCode.length() == (ikChild.size() - 1))
	{
		return isAncestorOf (ikChild);
	}

	return false;
}

bool CaicCode::isChildOf (CaicCode& ikParent)
//: is this code a child of the parameter code?
// Literally, are the initial (length-1) chars of this code equal to the
// whole parameter code?
{
	return ikParent.isParentOf (*this);
}

bool CaicCode::isAncestorOf (CaicCode& ikDescendant)
//: is this code an ancestor of the parameter code?
// Literally, does this code form the initial part of the parameter?
{
	string	theChildCode = ikDescendant.str();

	if (mCode.length() < theChildCode.length())
	{
		for (uint i = 0; i < mCode.length(); i++)
		{
			if (mCode[i] != theChildCode[i])
				return false;
		}

		return true;
	}

	return false;
}

bool CaicCode::isDescendentOf (CaicCode& ikAncestor)
//: is this code a descendent of the parameter code?
// Literally, does the parameter code form the initial part of this code?
{
	return ikAncestor.isAncestorOf(*this);
}

bool CaicCode::isSiblingOf (CaicCode& ikSibling)
//: is the parameter a sister (brother) of this code?
// That is, do they share all but the last letter of their code?
{
	string	theSiblingCode = ikSibling.str();

	// if it could be the immediate parent
	if (mCode.length() == theSiblingCode.length())
	{
		for (uint i = 0; i < (mCode.length() - 1); i++)
		{
			if (mCode[i] != theSiblingCode[i])
				return false;
		}

		return true;
	}

	return false;
}

size_type CaicCode::size ()
{
   return mCode.size();
}

const char* CaicCode::cstr	() const
{
   return mCode.c_str();
}

const std::string& CaicCode::str	() const
{
   return mCode;
}


// *** MUTATORS **********************************************************/
#pragma mark -

/**
'Tick' over to next code in CAIC order.

That is, ABA increments to ABB and then to ABC.
*/
void CaicCode::increment ()
{
	if (mCode == "")
	{
		mCode = indexToChar (0);
	}
	else // incrementing a letter
	{
		string::size_type theFinalIndex = mCode.size() - 1;
		mCode[theFinalIndex] = nextChar (mCode[theFinalIndex]);
	}
}

/**
Go to next phylogenetic level/depth.

That is, become the first child of the current code, by appending
letter. e.g. ABC becomes ABCA.
*/
void CaicCode::extend ()
{
	mCode += indexToChar (0);
}

/**
Go to prev phylogenetic level/depth.

That is, become the parent of of the current code, by deleting last
letter. e.g. ABA becomes AB.
*/
void CaicCode::truncate ()
{
	mCode.erase (mCode.size() - 1, 1);
}

/**
Descend one phylogenetic level/depth.

That is, follow the code down one node and get the code we'd see there. e.g.
ABC would become BC.
*/
char CaicCode::popFront ()
{
	assert (0 < mCode.size());
	char theReturnVal = mCode[0];
	mCode.erase (0, 1);
	return theReturnVal;
}

// *** CLASS UTILITY FXNS ************************************************/
#pragma mark -
// The below three functions encapsulate the essential behaviour of CAIC
// codes (the progression / ordering of characters). They are also static
// so these calculations are available from outside CaicCode objects.

/**
Convert a clade index into the corresponding single letter CAIC code.

The ordering of CAIC codes has been somewhat inconsistent, so
this is what we are settling upon. The code for an empty tree is
undefined. The code for the root is a blank string "". The code for
the 1st node of the root is "A", the 2nd "B" and so on. The code for
the 1st node of the 1st node of the root is "AA", the 2nd node of the
1st node of the root is "AB", and so on. When we run out of uppercase
letters (when there are more than 26 children of a node) we go to
lowercase. When we run out of lowercase, the code breaks down and we
use a dash "-" as this sorts after all letters in ASCII. Clade indexes
(the position of a child relative to it's siblings, e.g. the 3rd of
4 children), start numbering from 0.
*/
char CaicCode::indexToChar (CaicCode::size_type iCladeIndex)
{
	// preconditions:
	assert (0 <= iCladeIndex);

	// Main:
	if (iCladeIndex < 26)
		return 'A' + iCladeIndex;
	else if (iCladeIndex < 52)
		return 'a' + iCladeIndex - 26;
	else
		return '-';
}


/**
Convert a single letter CAIC code into the corresponding clade index.

@see indexToChar()
*/
size_type CaicCode::charToIndex (char iCladeChar)
{
	// Preconditions:
	assert ('A' <= iCladeChar);

	// Main:
	size_type theIndex = -1;
	if (iCladeChar < 'Z')
		theIndex = int (iCladeChar) - int ('A');
	else if (iCladeChar < 'z')
		theIndex = int (iCladeChar) - int ('a') + 26;
	else
	{
		assert (iCladeChar == '-');
		theIndex = 52;
	}

	// Postconditions & return:
	assert (0 <= theIndex);
	assert (theIndex <= 52);
	return theIndex;
}


/**
Increment a single letter CAIC code to the next code.

@see indexToChar()
*/
char CaicCode::nextChar (char iCladeChar)
{
	// Preconditions:
	// note this won't catch all cases of bad codes
	assert ('A' <= iCladeChar);
	assert (iCladeChar <= '-');

	// Main:
	if (('A' <= iCladeChar) and (iCladeChar < 'Z'))
		return iCladeChar + 1;
	else if (iCladeChar == 'Z')
		return 'a';
	else if (('a' <= iCladeChar) and (iCladeChar < 'z'))
		return iCladeChar + 1;
	else
	{
		assert ((iCladeChar == 'Z') or (iCladeChar == '-'));
		return '-';
	}
}


// *** AUXILLIARY & HELPER FXNS ******************************************/

/**
Stream the code to output as a simple string.
*/
std::ostream& operator<< (std::ostream& ioStream, CaicCode& iCode)
{
	ioStream << iCode.str();
	return ioStream;
}


// *** DEPRECATED & TEST FUNCTIONS **************************************/

/*

HARD STUFF!

// IS POSSIBLE NEIGHBOUR TIP OF
// Is the parameter a potential immediate sister tip to this code? For
// example B is a possible neighbouring tip to A, AB and ABB but not AA
// (needs AB), not ABB (needs ABA), not BA or BB (descendants), nor D (C
// intervenes).
bool CaicCode::isPossNeighbourTipOf (CaicCode& ikRelative)
{
	// you cannot be a neighbour of your ancestor or descendent
	if (IsAncestorOf(ikRelative) or IsDescendentOf(ikRelative))
	{
		return false;
	}

	// if siblings, are your codes next to each other?
	if (IsSiblingOf(ikRelative))
	{
		int theLength = Size();
		int theDiff = mCode[theLength] - ikRelative[theLength];
		if (abs(theDiff) == 1)
			return true
		else
			return false;
	}

	// otherwise we are going to have to compare unit by unit
	string theShortCode = mCode;
	string theLongCode = ikRelative.str();
	if (theLongCode.size() < theShortCode.size())
		Swap (theLongCode, theShortCode);

	string	theSiblingCode = ikSibling.str();

	// if it could be the immediate parent
	if (mCode.length() == theSiblingCode.length())
	{
		for (int i = 0; i < mCode.length(); i++)
		{
			if (mCode[i] != theSiblingCode[i])
				return false;
		}

		return true;
	}

	return false;
}
*/


SBL_NAMESPACE_STOP

// *** END

