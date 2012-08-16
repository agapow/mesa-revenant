/**************************************************************************
Epoch.cpp - an action encapsulating an evolutionary epoch

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>

**************************************************************************/


// *** INCLUDES

#include "Epoch.h"
#include "ActionUtils.h"
#include "ExecutionError.h"
#include "StringUtils.h"
#include "MesaGlobals.h"
#include "Reporter.h"
#include "TaxaTraitMatrix.h"
#include "TreeWrangler.h"
#include "ConsoleApp.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <exception>

using std::vector;
using std::stringstream;
using std::string;
using std::random_shuffle;
using std::exception;
using sbl::toString;


// *** CONSTANTS & DEFINES

// *** CLASS DEFINITION **************************************************/

void EpochMacro::execute ()
{
   TreeWrangler		theSavedTrees;
   ContTraitMatrix	theSavedContData;
   DiscTraitMatrix	theSavedDiscData;	
         
	try
	{
		// Preconditions:
		if (begin() == end())
			throw ExecutionError ("no rules in epoch");
      
      if (mRestartIfDead)
      {
         // save all data
         theSavedTrees = *(MesaGlobals::mTreeDataP);
         theSavedContData = *(MesaGlobals::mContDataP);
         theSavedDiscData = *(MesaGlobals::mDiscDataP);	
      }
      
		MesaTree* theTreeP = getActiveTreeP ();
		
		if (isAtEnd () or (theTreeP->countAliveLeaves () == 0))
			return;
		
		// Main:
		sortRules();
		if ((theLocalRules.size() + theGlobalRules.size()) <= 0)
			throw ExecutionError ("no non-conditional rules in epoch");
		
		// until end condition is reached execute rules
		executeEpochLoop ();
	}
   catch (ExecutionError theError)
   {
      if (mRestartIfDead)
      {
         MesaGlobals::mReporterP->print ("Phylogeny dead, restoring and restarting");
         // restore data and run again
         *(MesaGlobals::mTreeDataP) = theSavedTrees; 
         *(MesaGlobals::mContDataP) = theSavedContData;
         *(MesaGlobals::mDiscDataP) = theSavedDiscData;
         this->execute();
      }
      else
         throw;
   }
	catch (...)
	{
		// Report ("A problem has caused the epoch to terminate prematurely.")

		throw;
	}		
/*
	catch (exception &theException)
	{
		string theBuffer ("Epoch terminates: ");
		theBuffer += theException.what();
		MesaGlobals::mReporterP->print (theBuffer.c_str());
	}
*/
}


void EpochMacro::executeEpochLoop ()
{
	// until end condition is reached execute rules
	MesaTree* theTreeP = getActiveTreeP ();
	while ((isAtEnd() == false) and (0 < theTreeP->countAliveLeaves ()))
	{
		// there must be taxa
		// maybe this is ok, as there could be a global or cond that fires
		theTreeP = getActiveTreeP ();
		if (theTreeP->countLeaves() <= 0)
			throw ExecutionError ("no living taxa");

		executeEpochOnce ();
	}
}


bool EpochMacro::hasSpeciationRules ()
{
	for (iterator q = begin (); q != end(); q++)
	{
		if (isSpeciationRule ((EvolRule*) *q))
			return true;
	}
	return false;
}


bool EpochMacro::hasKillRules ()
{
	for (iterator q = begin (); q != end(); q++)
	{
		if (isKillRule ((EvolRule*) *q))
			return true;
	}
	return false;
}



void EpochMacro::sortRules ()
//: gather the rules into the correct group for execution
{
	// Main:
	theLocalRules.clear();
	theGlobalRules.clear();
	theCondRules.clear();
	
	// for every rule
	for (iterator q = begin (); q != end(); q++)
	{
		// if it's a local rule, cast & store
		LocalRule* theLocalRuleP = castAsLocalRule (*q);
		if (theLocalRuleP != NULL)
		{
			theLocalRules.push_back (theLocalRuleP);
			continue;
		}
		
		// if it's a conditional, cast & store
		ConditionalRule* theCondRuleP = castAsConditionalRule (*q);
		if (theCondRuleP != NULL)
		{
			theCondRules.push_back (theCondRuleP);
			continue;
		}
			
		// if it's a gloabl rule, cast & store
		GlobalRule* theGlobalRuleP = castAsGlobalRule (*q);
		if (theGlobalRuleP != NULL)
		{
			theGlobalRules.push_back (theGlobalRuleP);
			continue;
		}
		
		// so what is it then? this shouldn't happen
		assert (false);
	}
	
	// Postcondition:
	assert (size() == (theLocalRules.size() + theGlobalRules.size() + theCondRules.size()));
}


void EpochMacro::executeEpochOnce ()
{
	// Preconditions & preparation:
	MesaTree* theTreeP = getActiveTreeP ();
	if (theTreeP->countAliveLeaves() <= 0)
		throw ExecutionError ("no living taxa");
	
	// DBG_MSG ("tree size: " << theTreeP->countNodes());
	
	// Main:
	// find the first rule to go off, the node it targets & time it takes
	nodeiter_t   theFiringLeaf;
	mesatime_t       theTimeToEvent;
	EvolRule*    theFiringRuleP;
	theFiringRuleP = findFirstRule (theFiringLeaf, theTimeToEvent);
	assert (theFiringRuleP != NULL);
	assert (0.0 <= theTimeToEvent);
	
	// execute it
	nodearr_t theSubjectLeaves;
	theSubjectLeaves.push_back (theFiringLeaf);
	commitAction (theFiringRuleP, theSubjectLeaves, theTimeToEvent);
	
	// do conditionals
	fireConditionals (theFiringRuleP, theSubjectLeaves, theTimeToEvent);
}


EvolRule* EpochMacro::findFirstRule (nodeiter_t& oFiringLeaf, mesatime_t& oTime)
//: find the rule, local or global, that goes off next
{
	// Preconditions & arg preparation
	MesaTree* theTreeP = getActiveTreeP ();
	// should be caught before here
	assert (0 < theTreeP->countAliveLeaves());
	oTime = 1000000; // TODO
	oFiringLeaf = theTreeP->end();
	EvolRule* theFiringRuleP = NULL;

	// Main:	
	// gather the leaves & shuffle them
	// this sucks massively as a way to do it, but there's no other
	// way to assess the taxa in a random order
	typedef vector<nodeiter_t>	nodearr_t;
	nodearr_t theLeaves;
	theTreeP->getLiveLeaves (theLeaves);
	assert (0 < theLeaves.size());
	random_shuffle (theLeaves.begin(), theLeaves.end());
	
	// which local rule combinations fires off?
	// for each local rule and species/leaf combination
	random_shuffle (theLocalRules.begin(), theLocalRules.end());
	for (nodearr_t::iterator q = theLeaves.begin(); q != theLeaves.end(); q++)
	{
		// test every rule to find the "soonest" one
		vector<LocalRule*>::iterator r;
		for (r = theLocalRules.begin(); r != theLocalRules.end(); r++)
		{
			mesatime_t theCurrWait = (*r)->calcNextWait (*q);
			assert (0.0 <= theCurrWait);
			if (theCurrWait < oTime)
			{
				oTime = theCurrWait;
				theFiringRuleP = *r;
				oFiringLeaf = *q;
			}
		}
	}
	
	// assess global rules and choose which happens
	// test every rule to find the "soonest" one
	random_shuffle (theGlobalRules.begin(), theGlobalRules.end());
	vector<GlobalRule*>::iterator t;
	for (t = theGlobalRules.begin(); t != theGlobalRules.end(); t++)
	{
		mesatime_t theCurrWait = (*t)->calcNextWait ();
		if (theCurrWait < oTime)
		{
			oTime = theCurrWait;
			theFiringRuleP = *t;
		}
	}
	
	// Postconditions & return:
	if (oTime <= 0.0)
		DBG_MSG ("Arghhhhhhh");
	assert (0.0 < oTime);
	return theFiringRuleP;
}

 
void EpochMacro::commitAction
(EvolRule* iRuleP, nodearr_t& ioLeaves, mesatime_t iTime)
//: execute the chosen rule
// returns a vector of the leaves it operated on
{
	MesaTree* theTreeP = getActiveTreeP ();
	// in case there are no rules programmed
	assert (iRuleP != NULL);

	// age all the leaves to the point where it happens
	// TO DO: we're not aging twice are we
	theTreeP->ageAllLeaves (iTime);		
	iRuleP->commitAction (ioLeaves, iTime);
}


void EpochMacro::fireConditionals (EvolRule* iRuleP, nodearr_t& ioLeaves, mesatime_t iTime)
//: search the conditionals for any that are fired and do them
{
	/*
	if (dynamic_cast<NullRule*> (iRuleP) != NULL)
	{
		DBG_MSG ("metronome")
	}
	else
	{
		DBG_MSG ("system event");
	}
	*/
		
	random_shuffle (theCondRules.begin(), theCondRules.end());
	vector<ConditionalRule*>::iterator s;
	
	for (s = theCondRules.begin(); s != theCondRules.end(); s++)
	{
		// shuffle leaf array only if necessary (i.e. more than 1 leaf)
		if (1 < ioLeaves.size())
			random_shuffle (ioLeaves.begin(), ioLeaves.end());
		// DBG_MSG ("number of leaves: " << ioLeaves.size());
		// DBG_MSG ("firing rule address: " << iRuleP);
		if ((*s)->isTriggered (iRuleP, ioLeaves)) // TO DO: complete hack
		{
			// DBG_MSG ("is triggered");
			(*s)->commitAction (ioLeaves, iTime);
		}
	}
}

const char* EpochMacro::describe (size_type iIndex)
{
	// Preconditions:
	assert (0 <= iIndex);
	assert (iIndex <= deepSize());
	
	// Main:
	static string theBuffer;
	if (iIndex == 0)
	{
		theBuffer = "epoch: ";
		theBuffer += describeEpoch ();
		return theBuffer.c_str();
	}
	else
	{
		iterator q;
		for (q = begin(); q != end(); q++)
		{
			assert (0 < iIndex);
			
			int theSizeCurrElement = (*q)->deepSize();
			if (iIndex <= theSizeCurrElement)
				// handoff description task to this element
				return (*q)->describe (iIndex - 1);
			else
				// look at later element
				iIndex -= theSizeCurrElement;
		}
	}

	// Postconditions:
	assert (false);   // should never reach here
	return "";        // just to shut compiler up
}


// *** POP LIMIT EPOCH *************************************************/

bool EpochPopLimit::isAtEnd ()
{
	MesaTree* theTreeP = getActiveTreeP ();
	MesaTree:size_type theCount;
	switch (mNodeType)
	{
		case kNodetype_All:
			theCount = theTreeP->countNodes();
			break;
			
		case kNodetype_Tips:
			theCount = theTreeP->countLeaves();
			break;
			
		case kNodetype_Living:
			theCount = theTreeP->countAliveLeaves();
			break;
			
		default:
			assert (false);
			break;
	}
	
	if (theCount < mPopLimit)
		return false;
	else
		return true;
}


const char* EpochPopLimit::describeEpoch ()
{
	static std::string theDescStr;
	
	theDescStr = "evolve while (";
	switch (mNodeType)
	{
		case kNodetype_All:
			theDescStr += "total nodes";
			break;
			
		case kNodetype_Tips:
			theDescStr += "all leaves";
			break;
			
		case kNodetype_Living:
			theDescStr += "extant taxa";
			break;
			
		default:
			assert (false);
			break;
	}

	theDescStr += " < ";
	theDescStr += toString (mPopLimit);
	theDescStr += ")";
	return theDescStr.c_str();
}
	
	
void EpochPopLimit::executeEpochLoop ()
{
	// do normal execution
	EpochMacro::executeEpochLoop();
	
	// if advance to next event
	if ((mAdvance) and (hasSpeciationRules() or hasKillRules ()))
	{		
		// we go up to the point of the next spec/kill rule
		// if there are none, we don't go

		// advance to just before next speciation event
		bool theNextSpeciesNumberEventReached = false;
		while (not theNextSpeciesNumberEventReached)
		{
			MesaTree* theTreeP = getActiveTreeP ();
			if (theTreeP->countAliveLeaves() <= 0)
				throw ExecutionError ("no living taxa");
			// DBG_MSG (theTreeP->countNodes());
				
			// find the first rule to go off?
			nodeiter_t   theFiringLeaf;
			mesatime_t       theTimeToEvent;
			EvolRule*    theFiringRuleP;
			theFiringRuleP = findFirstRule (theFiringLeaf, theTimeToEvent);
			assert (theFiringRuleP != NULL);
			assert (0 <= theTimeToEvent);

			nodearr_t        theTargetLeafArr;
			EndOfEpochRule   theEndRule;

			if (isSpeciationRule (theFiringRuleP) or isKillRule (theFiringRuleP))
			{
				theFiringRuleP = &theEndRule;
				theNextSpeciesNumberEventReached = true;
			}
			else
			{
				theTargetLeafArr.push_back (theFiringLeaf);
			}
			
			commitAction (theFiringRuleP, theTargetLeafArr, theTimeToEvent);
			fireConditionals (theFiringRuleP, theTargetLeafArr, theTimeToEvent);
		}
	}
}


/*
void EpochPopLimit::execute ()
{
	// gather the local rules
	std::vector<LocalRule*>	theLocalRules;
	for (iterator q = begin (); q != end(); q++)
	{
		LocalRule* theCurrRuleP = castAsLocalRule (*q);
		if (theCurrRuleP != NULL)
			theLocalRules.push_back (theCurrRuleP);	
	}
			
	// until end condition is reached execute
	MesaTree* theTreeP = getActiveTreeP ();
	while (theTreeP->countLeaves() < mPopLimit)
	{
		if (theTreeP->countLeaves() == 0)
		{
			// if the tree has died out
			throw ExecutionError ("entire tree has died out");
		}
		
		LocalRule*	theFiringRuleP = NULL;
		nodeiter_t theFiringLeaf;
		time_t theMinWait = 100000; // TO DO fix this
		
		// for each species/leaf
		for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
		{
			if (theTreeP->isLeaf (q))
			{
				// test every rule to find the "soonest" one
				std::vector<LocalRule*>::iterator r;
				for (r = theLocalRules.begin(); r != theLocalRules.end(); r++)
				{
					time_t theCurrWait = (*r)->calcNextWait (q);
					if (theCurrWait < theMinWait)
					{
						theMinWait = theCurrWait;
						theFiringRuleP = *r;
						theFiringLeaf = q;
					}
				}
			}
		}
		
		// age all the leaves to the point where it happens
		theTreeP->ageAllLeaves (theMinWait);
		
		// execute it
		if (theFiringRuleP != NULL) // in case there are no rules programmed
		{
			assert ((getActiveTreeP())->isLeaf (theFiringLeaf));
			theFiringRuleP->commitAction (theFiringLeaf);
		}
	}
	
	// TO DO: untidy, make this more efficient
	if (mAdvance)
	{
		LocalRule*	theFiringRuleP = NULL;
		nodeiter_t theFiringLeaf;
		time_t theMinWait = 100000; // TO DO fix this
		
		// for each species/leaf
		for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
		{
			if (theTreeP->isLeaf (q))
			{
				// test every rule to find the "soonest" one
				std::vector<LocalRule*>::iterator r;
				for (r = theLocalRules.begin(); r != theLocalRules.end(); r++)
				{
					time_t theCurrWait = (*r)->calcNextWait (q);
					if (theCurrWait < theMinWait)
					{
						theMinWait = theCurrWait;
						theFiringRuleP = *r;
						theFiringLeaf = q;
					}
				}
			}
		}
		
		// age all the leaves to the point where it happens
		theTreeP->ageAllLeaves (theMinWait);
	}
}
*/

// *** TIME LIMIT EPOCH ************************************************/
#pragma mark -

bool EpochTimeLimit::isAtEnd ()
{
	MesaTree* theTreeP = getActiveTreeP ();
	if (theTreeP->getTreeAge() < mTimeLimit)
	{
		// DBG_MSG ("tree age " << theTreeP->getTreeAge());
		return false;
	}
	else
	{
		return true;
	}
}


const char* EpochTimeLimit::describeEpoch ()
{
	static std::string theDescStr;
	theDescStr = "evolve until (";
	theDescStr += toString (mTimeLimit);
	theDescStr += " <= time)";
	return theDescStr.c_str();
}



// *** UTILITY FUNCTIONS *************************************************/
#pragma mark -

EpochMacro* castAsEpoch (BasicAction* iActionP)
//: return the parameter cast as a epoch if possible, otherwise nil
{
	EpochMacro* theEpochP = NULL;
	theEpochP = dynamic_cast<EpochMacro*> (iActionP);
	return theEpochP;
}


// *** END ***************************************************************/



