/**************************************************************************
CharEvolRule.cpp - evolutionary rules for evolving characters

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

**************************************************************************/


// *** INCLUDES

#include "CharEvolRule.h"
#include "ActionUtils.h"
#include "MesaGlobals.h"
#include <string>

using std::string;


// *** CONSTANTS & DEFINES

typedef SpCharEvolRule::size_type   size_type;


// *** CLASS DECLARATION *************************************************/

bool SpCharEvolRule::isTriggered
// (EvolRule* iFiringRuleP, nodeiter_t& ioFiringLeaf)
(EvolRule* iFiringRuleP, nodearr_t& ioFiringLeaves)
{
	// Preconditions:
	assert (iFiringRuleP != NULL);
	ioFiringLeaves = ioFiringLeaves; // just to shut compiler up
	// Main:
	// ioFiringLeaf = ioFiringLeaf; // to shut compiler up
	return (isSpeciationRule (iFiringRuleP));
}



// *** SYMMETRICAL SPECIATIONAL TRAIT EVOLUTION **************************/

void SymmetricSceRule::commitAction (nodearr_t& iNodes, mesatime_t iTime)
{
	// Preconditions:
	MesaTree* theTreeP = getActiveTreeP();
	nodeiter_t iNode = iNodes[0];
	assert (theTreeP->countChildren (iNode) == 2);

	// Main:
	iTime = 0.0;
	nodeiter_t iNode1 = theTreeP->getChild (iNode, 0);
	nodeiter_t iNode2 = theTreeP->getChild (iNode, 1);

	SchemeArr::iterator p;
	for (p = mSchemes.begin(); p != mSchemes.end(); p++)
	{
		TraitEvolScheme* theSchemeP = *p;
		theSchemeP->evolveChars (iNode1, iTime);
		theSchemeP->evolveChars (iNode2, iTime);
	}

	iNodes[0] = iNode;
}

void SymmetricSceRule::adoptScheme (SchemeArr& iSchemeArr)
{
	mSchemes.adopt (iSchemeArr);
}


const char* SymmetricSceRule::describeRule ()
{
	return "trait change (symmetrical speciational)";
}

const char* SymmetricSceRule::describe (size_type iIndex)
{
	assert (0 <= iIndex);
	assert (iIndex < deepSize());

	size_type theSetSize = mSchemes.size();

	if (iIndex == 0)
	// the rule itself
	{
		return describeRule ();
	}
	else if (iIndex == 1)
	// the first scheme set
	{
		return "main scheme set";
	}
	else if (iIndex <= (theSetSize + 1))
	// inside the first scheme set
	{
		return mSchemes.describe (iIndex - 2);
	}
	else
	{
		// should never get here
		assert (false);
		return ""; // just to shut compiler up
	}
}

void SymmetricSceRule::deleteElement (size_type iIndex)
{
	assert (0 < iIndex); // because you can't delete self
	assert (iIndex <= mSchemes.size());

	if (iIndex == 1)
		mSchemes.removeAll ();
	else
		mSchemes.remove (iIndex - 2);
}

size_type SymmetricSceRule::deepSize ()
{
	return mSchemes.size() + 2; // for rule and scheme
}

size_type SymmetricSceRule::getDepth (size_type iIndex)
{
	assert (0 <= iIndex);
	assert (iIndex < deepSize());

	size_type theSetSize = mSchemes.size();

	if (iIndex == 0)
	// the rule itself
	{
		return 0;
	}
	else if (iIndex == 1)
	// the first scheme set
	{
		return 1;
	}
	else if (iIndex <= (theSetSize + 1))
	// inside the first scheme set
	{
		return 2;
	}
	else
	{
		// should never get here
		assert (false);
		return -13; // just to shut compiler up
	}
}


// *** ASSYMETRICAL SPECIATIONAL TRAIT EVOLUTION *************************/

void AsymmetricSceRule::commitAction (nodearr_t& iNodeArr, mesatime_t iTime)
{
	// Preconditions:
	MesaTree* theTreeP = getActiveTreeP();
	nodeiter_t iNode = iNodeArr[0];
	assert (theTreeP->isNodeBifurcating (iNode));

	// Main:
	iTime = 0.0;
	nodeiter_t iNode1 = theTreeP->getChild (iNode, 0);
	nodeiter_t iNode2 = theTreeP->getChild (iNode, 1);

	if (MesaGlobals::mRng.UniformWhole (2) == 1)
		swap (iNode1, iNode2);

	SchemeArr::iterator p;
	for (p = mSchemes[0].begin(); p != mSchemes[0].end(); p++)
	{
		TraitEvolScheme* theSchemeP = *p;
		theSchemeP->evolveChars (iNode1, iTime);
	}
	for (p = mSchemes[1].begin(); p != mSchemes[1].end(); p++)
	{
		TraitEvolScheme* theSchemeP = *p;
		theSchemeP->evolveChars (iNode2, iTime);
	}

	iNodeArr[0] = iNode;
}

void AsymmetricSceRule::adoptScheme (int iSide, SchemeArr& iSchemeArr)
{
	assert ((iSide == 0) or (iSide == 1));
	mSchemes[iSide].adopt (iSchemeArr);
}


const char* AsymmetricSceRule::describeRule ()
{
	return "trait evolution (asymmetrical speciational)";
}

const char* AsymmetricSceRule::describe (size_type iIndex)
{
	assert (0 <= iIndex);
	assert (iIndex < deepSize());

	size_type theFirstSetSize = mSchemes[0].size();
	size_type theSecondSetSize = mSchemes[1].size();

	if (iIndex == 0)
	// the rule itself
	{
		return describeRule ();
	}
	else if (iIndex == 1)
	// the first scheme set
	{
		return "left scheme set";
	}
	else if (iIndex <= (theFirstSetSize + 1))
	// inside the first scheme set
	{
		return mSchemes[0].describe (iIndex - 2);
	}
	else if (iIndex == (theFirstSetSize + 2))
	// the second scheme set
	{
		return "right scheme set";
	}
	else if (iIndex <= (theFirstSetSize + theSecondSetSize + 2))
	// inside the second scheme set
	{
		return mSchemes[1].describe (iIndex - (theFirstSetSize + 3));
	}
	else
	{
		// should never get here
		assert (false);
		return ""; // just to shut compiler up
	}
}

void AsymmetricSceRule::deleteElement (size_type iIndex)
{
	assert (0 <= iIndex);
	assert (iIndex < deepSize());

	size_type theFirstSetSize = mSchemes[0].size();
	size_type theSecondSetSize = mSchemes[1].size();

	if (iIndex == 0)
	// the rule itself
	{
		assert (false); // cannot delete self
	}
	else if (iIndex == 1)
	// the first scheme set
	{
		mSchemes[0].removeAll ();
	}
	else if (iIndex <= (theFirstSetSize + 1))
	// inside the first scheme set
	{
		mSchemes[0].remove (iIndex - 2);
	}
	else if (iIndex == (theFirstSetSize + 2))
	// the second scheme set
	{
		mSchemes[1].removeAll ();
	}
	else if (iIndex <= (theFirstSetSize + theSecondSetSize + 2))
	// inside the second scheme set
	{
		mSchemes[1].remove (iIndex - (theFirstSetSize + 3));
	}
	else
	{
		// should never get here
		assert (false);
	}
}


size_type AsymmetricSceRule::getDepth (size_type iIndex)
{
	assert (0 <= iIndex);
	assert (iIndex < deepSize());

	size_type theFirstSetSize = mSchemes[0].size();
	size_type theSecondSetSize = mSchemes[1].size();

	if (iIndex == 0)
	// the rule itself
	{
		return 0;
	}
	else if (iIndex == 1)
	// the first scheme set
	{
		return 1;
	}
	else if (iIndex <= (theFirstSetSize + 1))
	// inside the first scheme set
	{
		return 2;
	}
	else if (iIndex == (theFirstSetSize + 2))
	// the second scheme set
	{
		return 1;
	}
	else if (iIndex <= (theFirstSetSize + theSecondSetSize + 2))
	// inside the second scheme set
	{
		return 2;
	}
	else
	{
		// should never get here
		assert (false);
		return -13; // just to shut compiler up
	}
}

size_type AsymmetricSceRule::deepSize ()
{
	size_type theFirstSetSize = mSchemes[0].size();
	size_type theSecondSetSize = mSchemes[1].size();
	return theFirstSetSize + theSecondSetSize + 3;
}



// *** PARENTAL SPECIATIONAL TRAIT EVOLUTION *****************************/

// I/O
const char* TerminalCharEvolRule::describeRule ()
{
	return "trait evolution (parental speciational)";
}

void TerminalCharEvolRule::commitAction (nodearr_t& iNodes, mesatime_t iTime)
{
	// Preconditions:
	assert (0.0 <= iTime);
	nodeiter_t iNode = iNodes[0];
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);

	// Main:
	// get time to Parent
	iTime = theTreeP->getTimeFromNodeToParent (iNode);
	SchemeArr::iterator p;
	for (p = mSchemes.begin(); p != mSchemes.end(); p++)
	{
		TraitEvolScheme* theSchemeP = *p;
		theSchemeP->evolveChars (iNode, iTime);
	}

	iNodes[0] = iNode;
}

bool TerminalCharEvolRule::isTriggered
// (EvolRule* iFiringRuleP, nodeiter_t& ioFiringLeaf)
(EvolRule* iFiringRuleP, nodearr_t& ioFiringLeaves)
// Should be triggered when a species has no chance to change anymore.
// That would be when it speciates, when it dies and at the end of an
// epoch.
{
	ioFiringLeaves = ioFiringLeaves; // just to shut compiler up
	// if this is called on a node that has just been killed & removed,
	// return false
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);
	theTreeP = theTreeP; // just to shut compiler up
	// if (ioFiringLeaf == theTreeP->end())
	//	return false;

	// ioFiringLeaf = ioFiringLeaf;

	// trigger if its the end of epoch
	EndOfEpochRule* theRuleP = dynamic_cast<EndOfEpochRule*> (iFiringRuleP);
	if (theRuleP != NULL)
		return true;

	return isSpeciationOrKillRule (iFiringRuleP);
}



// *** PHYLETIC / TRANSFORMING TRAIT EVOLUTION ***************************/

bool GradualCharEvolRule::isTriggered
// (EvolRule* iFiringRuleP, nodeiter_t& ioFiringLeaf)
(EvolRule* iFiringRuleP, nodearr_t& ioFiringLeaves)
{
	return true;

	// if this is called on a node that has just been killed, return false
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);

	if (ioFiringLeaves.size() == 0)
		return false;
	if (ioFiringLeaves[0] == theTreeP->end())
	{
		// DBG_MSG ("grad char evol is _not_ triggered");
		return false;
	}

	// otherwise say yes for everything else
	iFiringRuleP = iFiringRuleP;
	// DBG_MSG ("grad char evol is triggered");
	return true;
}

const char* GradualCharEvolRule::describeRule ()
{
	return "trait evolution (gradual)";
}

void GradualCharEvolRule::commitAction (nodearr_t& iNodes, mesatime_t iTime)
{
	// DBG_MSG ("grad trait evol called with time " << iTime);
	// Preconditions:
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);
	iNodes = iNodes; // just to shut compiler up

	// Main:
	nodeiter_t theNode;
	for (theNode = theTreeP->begin(); theNode != theTreeP->end(); theNode++)
	{
		if (theTreeP->isNodeAlive (theNode))
		{
			SchemeArr::iterator p;
			for (p = mSchemes.begin(); p != mSchemes.end(); p++)
			{
				TraitEvolScheme* theSchemeP = *p;
				theSchemeP->evolveChars (theNode, iTime);
			}
		}
	}

	// nodeiter_t iNode = iNodes[0]; // what was this about?
}

// *** END ***************************************************************/



