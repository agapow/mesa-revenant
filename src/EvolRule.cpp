/**************************************************************************
EvolRule.cpp - an evolutionary rule

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

**************************************************************************/


// *** INCLUDES

#include "EvolRule.h"
#include "Sbl.h"
#include "RandomService.h"
#include "MesaGlobals.h"
#include "MesaTypes.h"
#include "ActionUtils.h"
#include "StringUtils.h"
#include "Numerics.h"
#include <cmath>
#include <sstream>

using std::log;
using std::string;
using std::stringstream;
using sbl::toString;


// *** CONSTANTS & DEFINES

double   logE (double iTerm);
double   logN (double iBase, double iTerm);

typedef EvolRule::size_type   size_type;


// *** CLASS DEFINITION **************************************************/

void EvolRule::execute () 
{
	assert (false); // we don't execute evolrules
}

// I/O
const char* EvolRule::describe (size_type iIndex)
{
	// Preconditions:
	assert (iIndex == 0);
	
	// Main:	
	static string theBuffer;
	theBuffer = "evol rule: ";
	theBuffer += describeRule ();
	return theBuffer.c_str();
}


void EvolRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	ioSubjectLeaves = ioSubjectLeaves;
	iTime = iTime;
	assert (false);
}


// *** CLASS DEFINITION **************************************************/


bool ConditionalRule::isTriggered (EvolRule* iFiringRuleP, nodearr_t& ioFiringLeaves)
{
	iFiringRuleP = iFiringRuleP;
	ioFiringLeaves = ioFiringLeaves; // to shut compiler up
	assert (false); // should never get here, only in base class
	return false;
}

const char* ConditionalRule::describeRule ()
{
	assert (false);
	return "";
}


// *** CLASS DEFINITION **************************************************/


mesatime_t GlobalRule::calcNextWait ()
{
	assert (false);
	return -13.0;
}


const char* GlobalRule::describeRule ()
{
	assert (false);
	return "global rule";
}


// *** CLASS DEFINITION **************************************************/


mesatime_t LocalRule::calcNextWait (nodeiter_t iLeafIter)
{ 
	iLeafIter = iLeafIter;
	assert (false);
	return 0.0;
}

void LocalRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{ 
	iTime = iTime;
	ioSubjectLeaves = ioSubjectLeaves;
	assert (false);
}

const char* LocalRule::describeRule ()
{
	assert (false);
	return "local rule";
}


// *** CLASS DEFINITION **************************************************/


void NullRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{ 
	iTime = iTime; // just to shut compiler up
	ioSubjectLeaves = ioSubjectLeaves;
	// DBG_MSG ("the null rule has been called with time=" << iTime);
}

mesatime_t NullRule::calcNextWait ()
// simplest way
{
	return (1.0/mRate);
}

const char* NullRule::describeRule ()
{
	static string theBuffer;
	theBuffer = "null rule (rate ";
	theBuffer += toString (mRate);
	theBuffer += ")";
	return theBuffer.c_str();
}


// *** CLASS DEFINITION **************************************************/


void EndOfEpochRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	iTime = iTime;
	ioSubjectLeaves = ioSubjectLeaves; // to shut compiler up
} 

const char* EndOfEpochRule::describeRule ()
{
	return "end of epoch";
}


// *** CLASS DEFINITION **************************************************/

mesatime_t MarkovSpRule::calcNextWait (nodeiter_t iLeafIter)
{
	iLeafIter = iLeafIter; // to shut compiler up
	mesatime_t theWait = calcWaitFromRate (mRate);
	assert (0.0 <= theWait);
	return theWait;
}

void MarkovSpRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	iTime = iTime;
	nodeiter_t iLeafIter = ioSubjectLeaves[0];
	assert ((getActiveTreeP())->isLeaf (iLeafIter));
	speciate (iLeafIter);
}

const char* MarkovSpRule::describeRule ()
{
	static string theBuffer;
	theBuffer = "markovian speciation (rate ";
	theBuffer += toString (mRate);
	theBuffer += ")";
	return theBuffer.c_str();
}

// *** CLASS DEFINITION **************************************************/

mesatime_t LogisticSpRule::calcNextWait (nodeiter_t iLeafIter)
{
	iLeafIter = iLeafIter; // to shut compiler up
   MesaTree* theTreeP = getActiveTreeP ();
   int extant_taxa_cnt = theTreeP->countAliveLeaves();
   double actual_rate = mRate * (1.0 - (double (extant_taxa_cnt) / double (mCapacity)));
   if (actual_rate < 0.0)
   {
      actual_rate = 0.0;
   }
	mesatime_t theWait = calcWaitFromRate (actual_rate);
	assert (0.0 <= theWait);
	return theWait;
}

const char* LogisticSpRule::describeRule ()
{
	static string theBuffer;
	theBuffer = "logistic speciation (rate ";
	theBuffer += toString (mRate);
	theBuffer = ", carrying capacity ";
	theBuffer += toString (mCapacity);   
	theBuffer += ")";
	return theBuffer.c_str();
}



// *** CLASS DEFINITION **************************************************/


mesatime_t AgeBiasedSpRule::calcNextWait (nodeiter_t iLeafIter)
{
	MesaTree* theTreeP = getActiveTreeP ();
	double theAge = theTreeP->getEdgeWeight (iLeafIter);
	theAge = std::max (theAge, MesaGlobals::mPrefs.mTimeGrain);
	mesatime_t theRate = calcRateFromTriParameter (mRateA, mRateB, mRateC, theAge);
	assert (0.0 <= theRate);
	mesatime_t theWait = calcWaitFromRate (theRate);
	assert (0.0 < theWait);
	return theWait;
}

void AgeBiasedSpRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	iTime = iTime;
	speciate (ioSubjectLeaves[0]);
}

const char* AgeBiasedSpRule::describeRule ()
{
/*
	static string theBuffer;
	theBuffer = "biased speciation with rate (";
	theBuffer += toString (mRateA);
	theBuffer += ", ";
	theBuffer += toString (mRateB);
	theBuffer += ", ";
	theBuffer += toString (mRateC);
	theBuffer += ")";
	return theBuffer.c_str();
*/
	static char theBuffer[64];
	std::sprintf (theBuffer, "age-biased speciation (params %.4g, %.4g, %.4g)", mRateA, mRateB, mRateC);
	return theBuffer;
}


// *** CLASS DEFINITION **************************************************/


mesatime_t CharBiasedSpRule::calcNextWait (nodeiter_t iLeafIter)
{
	MesaTree* theTreeP = getActiveTreeP ();
	conttrait_t	theCharVal = getContData (iLeafIter, mCharCol);
	mesatime_t theRate = calcRateFromTriParameter (mRateA, mRateB, mRateC,
		theCharVal);
	mesatime_t theWait = calcWaitFromRate (theRate);
	assert (0 <= theWait);
	return theWait;
}

void CharBiasedSpRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	iTime = iTime;
	nodeiter_t iLeafIter = ioSubjectLeaves[0];
	speciate (iLeafIter);
}

const char* CharBiasedSpRule::describeRule ()
{
/*
	static string theBuffer;
	theBuffer =  "speciation biased by continuous character ";
	theBuffer += toString (mCharCol + 1);
	theBuffer += " with parameters [";
	theBuffer += toString (mRateA);
	theBuffer += ", ";
	theBuffer += toString (mRateB);
	theBuffer += ", ";
	theBuffer += toString (mRateC);
	theBuffer += "]";
	return theBuffer.c_str();
*/
	
	static char theBuffer[64];
	std::sprintf (theBuffer, "cont. trait-biased speciation (params %.4g, %.4g, %.4g)", mRateA, mRateB, mRateC);
	return theBuffer;
}



// *** CLASS DEFINITION **************************************************/


mesatime_t CharBiasedSpRule_New::calcNextWait (nodeiter_t iLeafIter)
{
	mesatime_t theRate = mRateP->getRate (iLeafIter);
	mesatime_t theWait = calcWaitFromRate (theRate);
	assert (0 <= theWait);
	return theWait;
}

void CharBiasedSpRule_New::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	iTime = iTime;
	nodeiter_t iLeafIter = ioSubjectLeaves[0];
	speciate (iLeafIter);
}

const char* CharBiasedSpRule_New::describeRule ()
{
// TO DO:	
	static char theBuffer[64];
	std::sprintf (theBuffer, "biased speciation (new) (%s)", mRateP->describeRate());
	return theBuffer;
}


// *** CLASS DEFINITION **************************************************/


mesatime_t CharBiasedKillRule::calcNextWait (nodeiter_t iLeafIter)
{
	MesaTree* theTreeP = getActiveTreeP ();
	conttrait_t	theCharVal = getContData (iLeafIter, mCharCol);
	mesatime_t theRate = calcRateFromTriParameter (mRateA, mRateB, mRateC,
		theCharVal);
	mesatime_t theWait = calcWaitFromRate (theRate);
	assert (0 <= theWait);
	return theWait;
}

void CharBiasedKillRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	iTime = iTime;
	MesaTree* theTreeP = getActiveTreeP ();
	theTreeP->killLeaf (ioSubjectLeaves[0]);
}

const char* CharBiasedKillRule::describeRule ()
{
/*
	static string theBuffer;
	theBuffer =  "extinction biased by continuous character ";
	theBuffer += toString (mCharCol + 1);
	theBuffer += " with parameters [";
	theBuffer += toString (mRateA);
	theBuffer += ", ";
	theBuffer += toString (mRateB);
	theBuffer += ", ";
	theBuffer += toString (mRateC);
	theBuffer += "]";
	return theBuffer.c_str();
*/

	static char theBuffer[64];
	std::sprintf (theBuffer, "cont. trait-biased extinction (params %.4g, %.4g, %.4g)", mRateA, mRateB, mRateC);
	return theBuffer;
}


// *** CLASS DEFINITION **************************************************/


mesatime_t BiasedKillRule::calcNextWait (nodeiter_t iLeafIter)
{
	MesaTree* theTreeP = getActiveTreeP ();
	double theAge = theTreeP->getEdgeWeight (iLeafIter);
	mesatime_t theRate = calcRateFromTriParameter (mRateA, mRateB, mRateC, theAge);
	assert (0.0 <= theRate);
	mesatime_t theWait = calcWaitFromRate (theRate);
	assert (0.0 <= theWait);
	return theWait;
}

void BiasedKillRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	iTime = iTime;
	MesaTree* theTreeP = getActiveTreeP ();
	if (not theTreeP->isRoot (ioSubjectLeaves[0]))
		theTreeP->killLeaf (ioSubjectLeaves[0]);
}

const char* BiasedKillRule::describeRule ()
{
/*
	static string theBuffer;
	theBuffer = "biased extinction with freq parameters (";
	theBuffer += toString (mRateA);
	theBuffer += ", ";
	theBuffer += toString (mRateB);
	theBuffer += ", ";
	theBuffer += toString (mRateC); 
	theBuffer += ")";
	return theBuffer.c_str();
*/
	static char theBuffer[64];
	std::sprintf (theBuffer, "age-biased extinction (params %.4g, %.4g, %.4g)", mRateA, mRateB, mRateC);
	return theBuffer;
}


// *** CLASS DEFINITION **************************************************/


mesatime_t LatentSpRule::calcNextWait (nodeiter_t iLeafIter)
{
	iLeafIter = iLeafIter;
	mesatime_t theWait = calcWaitFromRate (mRate);
	assert (0.0 <= theWait);
	mesatime_t theLatency = mLatencyPeriod - iLeafIter->second.getWeight();
	if (theLatency < 0)
		theLatency = 0;
	// TO DO: nasty - fix this.
	return theWait + theLatency;
}

void LatentSpRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	iTime = iTime;
	speciate (ioSubjectLeaves[0]);
}
	
const char* LatentSpRule::describeRule ()
{
/*
	static string theBuffer;
	theBuffer = "speciation with rate ";
	theBuffer += toString (mRate);
	theBuffer += " and a latency period of ";
	theBuffer += toString (mLatencyPeriod);
	return theBuffer.c_str();
*/

	static char theBuffer[64];
	std::sprintf (theBuffer, "speciation with latency (rate %.4g, interval %.4g)",
		mRate, mLatencyPeriod);
	return theBuffer;
}


// *** CLASS DEFINITION **************************************************/


mesatime_t MarkovKillRule::calcNextWait (nodeiter_t iLeafIter)
{
	iLeafIter = iLeafIter;
	mesatime_t theWait = calcWaitFromRate (mRate);
	assert (0.0 <= theWait);
	return theWait;
}

void MarkovKillRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	iTime = iTime;
	MesaTree* theTreeP = getActiveTreeP ();
	if (not theTreeP->isRoot (ioSubjectLeaves[0]))
		theTreeP->killLeaf (ioSubjectLeaves[0]);
}

const char* MarkovKillRule::describeRule ()
{
/*
	static string theBuffer;
	theBuffer = "markov extinction with rate ";
	theBuffer += toString (mRate);
	return theBuffer.c_str();
*/
	static char theBuffer[64];
	std::sprintf (theBuffer, "markovian extinction (rate %.4g)",
		mRate);
	return theBuffer;
}


// *** CLASS DEFINITION **************************************************/


mesatime_t LogisticKillRule::calcNextWait (nodeiter_t iLeafIter)
{
	iLeafIter = iLeafIter; // to shut compiler up
   MesaTree* theTreeP = getActiveTreeP ();
   int extant_taxa_cnt = theTreeP->countAliveLeaves();
   
   double actual_rate = mRate * (double (extant_taxa_cnt) / double (mCapacity));
   if (actual_rate < 0.0)
   {
      actual_rate = 0.0;
   }
   if (mRate < actual_rate)
   {
      actual_rate = mRate;
   }
	mesatime_t theWait = calcWaitFromRate (actual_rate);
	assert (0.0 <= theWait);
	return theWait;
}


const char* LogisticKillRule::describeRule ()
{

	static char theBuffer[64];
	std::sprintf (theBuffer, "logistic extinction (rate %.4g, capacity %d)",
		mRate, mCapacity);
	return theBuffer;

}


// *** CLASS DEFINITION **************************************************/


mesatime_t MassKillRule::calcNextWait ()
{
	return calcWaitFromRate (mRate);
}

void MassKillRule::commitAction (nodearr_t& ioSubjectLeaves, mesatime_t iTime)
{
	// just to shut compiler up
	iTime = iTime;
	// iLeafIter = iLeafIter;
	// get the tree so as to kill its nodes
	MesaTree* theTreeP = getActiveTreeP ();

	// subclass will select nodes to kill
	ioSubjectLeaves = selectTargets ();
	// actually kill the nodes
	for (nodearr_t::iterator q = ioSubjectLeaves.begin (); q != ioSubjectLeaves.end (); q++)
		theTreeP->killLeaf (*q);
}

nodearr_t MassKillRule::selectTargets ()
{
	// should never get here, overridden by subclasses
	assert (false);
	// to shut compiler up
	nodearr_t theTargets;
	return theTargets; 
}

const char* MassKillRule::describeRule ()
{
	// should never get here, overridden by subclasses
	assert (false);
	// to shut compiler up
	return "";
}	

nodearr_t MassKillFixedNumRule::selectTargets ()
{
	// get the live nodes, shuffle them and take the first N
	MesaTree* theTreeP = getActiveTreeP ();
	nodearr_t theTargets;
	theTreeP->getLiveLeaves (theTargets);
	random_shuffle (theTargets.begin(), theTargets.end());
	if ((unsigned) mAbsNum < theTargets.size())
		theTargets.resize (mAbsNum);
		
	return theTargets; 
}

const char* MassKillFixedNumRule::describeRule ()
{
	static char theBuffer[64];
	std::sprintf (theBuffer, "mass extinction (kill %.4i species)", mAbsNum);
	return theBuffer;
}	

nodearr_t MassKillPercentRule::selectTargets ()
{
	// get the live nodes, shuffle them and take the first N percent
	MesaTree* theTreeP = getActiveTreeP ();
	nodearr_t theTargets;
	theTreeP->getLiveLeaves (theTargets);
	nodearr_t::size_type theKillNum = (nodearr_t::size_type) (theTargets.size() * mPercent);
	random_shuffle (theTargets.begin(), theTargets.end());
	theTargets.resize (theKillNum);
		
	return theTargets; 
}


const char* MassKillPercentRule::describeRule ()
{
	static char theBuffer[64];
	std::sprintf (theBuffer, "mass extinction (kill %.4g%% of species)", mPercent);
	return theBuffer;
}	

nodearr_t MassKillProbRule::selectTargets ()
{
	// get the live nodes, shuffle them and take the first N percent
	MesaTree* theTreeP = getActiveTreeP ();
	nodearr_t theTargets;
	theTreeP->getLiveLeaves (theTargets);
	nodearr_t::iterator q;
	for (q = theTargets.begin(); q != theTargets.end(); )
	{
		if (MesaGlobals::mRng.UniformFloat () <= mProb)
			q++;
		else
			theTargets.erase (q);
	}
	
	return theTargets; 
}

const char* MassKillProbRule::describeRule ()
{
	static char theBuffer[64];
	std::sprintf (theBuffer, "mass extinction (%.4g%% of killing species)", mProb);
	return theBuffer;
}


nodearr_t MassKillIf::selectTargets ()
{
	// get the live nodes, shuffle them and take the first N percent
	MesaTree* theTreeP = getActiveTreeP ();
	nodearr_t theTargets;
	theTreeP->getLiveLeaves (theTargets);
	nodearr_t::iterator q;
	for (q = theTargets.begin(); q != theTargets.end(); )
	{
		if (mSppTest.testCharacter (*q))
			q++;
		else
			theTargets.erase (q);
	}
	
	return theTargets; 
}

const char* MassKillIf::describeRule ()
{
	static char theBuffer[64];
	std::sprintf (theBuffer, "mass extinction (kill %s)", mSppTest.describe());
	return theBuffer;
}

	
nodearr_t MassKillTraitBiasedRule::selectTargets ()
{
	MesaTree* theTreeP = getActiveTreeP ();
	nodearr_t theTargets;
	theTreeP->getLiveLeaves (theTargets);
	nodearr_t::iterator q;
	for (q = theTargets.begin(); q != theTargets.end(); )
	{
		conttrait_t theTraitVal = getContData (*q, mTraitIndex);
		double theProb = calcProbFromTriParameter (mTriParamA, mTriParamB,
			mTriParamC, theTraitVal);
			
		if (MesaGlobals::mRng.UniformFloat () <= theProb)
			q++;
		else
			theTargets.erase (q);
	}
	
	return theTargets; 
}


const char* MassKillTraitBiasedRule::describeRule ()
{
	static char theBuffer[64];
	std::sprintf (theBuffer,
		"mass extinction (biased by trait %i, params %.4g, %.4g, %.4g)",
		mTraitIndex, mTriParamA, mTriParamB, mTriParamC);
	return theBuffer;
}

	
// *** UTILITY FUNCTIONS *************************************************/



/*
time_t calcWaitFromRate (time_t iRate)
//: take a frequency and convert it into a time-until-event
// TO DO: what do we return if the frequency is 0?
// NOTE: if frequency is 1.0 return 0.
{
	if (1.0 <= iRate)
		return 0;
		
	double theProb = sRandGen.UniformFloat ();
	assert (theProb < 1.0);
	assert (0.0 < iRate); 
	double theTime = logN (1.0 - iRate, 1.0 - theProb);
	return theTime;
}
*/


mesatime_t calcWaitFromRate (mesatime_t iRate)
//: take an instantaneous rate and convert it to a time-until-event
{
	// Preconditions:
	assert (0.0 <= iRate);
	if (iRate == 0.0)
		return 10000; // TO DO: complete hack to cope with stationary rules
	
	// Main:
	double theProb = MesaGlobals::mRng.UniformFloat ();
	mesatime_t theTime = -log (theProb) / iRate;
	// DBG_MSG ("\t" << theProb << "\t" << theTime << "\t" << iRate);
	assert (0.0 <= theTime);
	theTime = std::max (theTime, MesaGlobals::mPrefs.mTimeGrain);
		
	return theTime;
}


mesatime_t calcRateFromTriParameter
(double iA, double iB, double iC, conttrait_t iCharVal)
//: take a and three parameters and assess as ax^b + c
{
	// Preconditions & preparations:
	double theRate = -1.0; // init as bad to catch problems
	
	// Main:
	if ((iCharVal == 1.0))
	{
		theRate = iA + iC;
	}
	else
	{
		theRate = iA * std::pow (iCharVal, iB) + iC;
	}
	
	if (theRate <= 0.0)
		theRate = 0.0;
	else if (1.0/MesaGlobals::mPrefs.mTimeGrain < theRate)
		theRate = 1.0/MesaGlobals::mPrefs.mTimeGrain;
	
	// Postconditions & return:
	// not a hard and fast rule but it seems unlikely rates will ever get
	// this high
	assert (theRate < 5000);
	
	return theRate;
}

mesatime_t calcProbFromTriParameter
(double iA, double iB, double iC, conttrait_t iCharVal)
//: take a and three parameters and assess as ax^b + c as a probability
{
	double theProb = -1.0; // init as bad to catch problems
	
	if ((iCharVal == 1.0))
	{
		theProb = iA + iC;
	}
	else
	{
		theProb = iA * std::pow (iCharVal, iB) + iC;
	}
	
	if (theProb < 0.0)
		theProb = 0.0;
	else if (1.0 < theProb)
		theProb = 1.0;
				
	return theProb;
}



EvolRule* castAsEvolRule (BasicAction* iActionP)
//: return the parameter cast as an evolutionary rule if possible, otherwise nil
{
	EvolRule* theRuleP = NULL;
	theRuleP = dynamic_cast<EvolRule*> (iActionP);
	return theRuleP;
}


GlobalRule* castAsGlobalRule (BasicAction* iActionP)
//: return the parameter cast as a global rule if possible, otherwise nil
{
	GlobalRule* theRuleP = NULL;
	theRuleP = dynamic_cast<GlobalRule*> (iActionP);
	return theRuleP;
}


LocalRule* castAsLocalRule (BasicAction* iActionP)
//: return the parameter cast as a local rule if possible, otherwise nil
{
	LocalRule* theRuleP = NULL;
	theRuleP = dynamic_cast<LocalRule*> (iActionP);
	return theRuleP;
}


ConditionalRule* castAsConditionalRule (BasicAction* iActionP)
//: return the parameter cast as a conditional rule if possible, otherwise nil
{
	ConditionalRule* theRuleP = NULL;
	theRuleP = dynamic_cast<ConditionalRule*> (iActionP);
	return theRuleP;
}


bool isSpeciationRule (EvolRule* iRuleP)
{
	if (dynamic_cast<MarkovSpRule*> (iRuleP) or
		dynamic_cast<LogisticSpRule*> (iRuleP) or
		dynamic_cast<AgeBiasedSpRule*> (iRuleP) or
		dynamic_cast<CharBiasedSpRule*> (iRuleP) or
		dynamic_cast<CharBiasedSpRule_New*> (iRuleP) or
		dynamic_cast<LatentSpRule*> (iRuleP))
		return true;
	else
		return false;
}

bool isKillRule (EvolRule* iRuleP)
{
	if (dynamic_cast<MarkovKillRule*> (iRuleP) or
		dynamic_cast<BiasedKillRule*> (iRuleP))
		return true;
	else
		return false;
}

bool isSpeciationOrKillRule (EvolRule* iRuleP)
//: is this rule for a speciation or extinction?
{
	if (isSpeciationRule (iRuleP) or isKillRule (iRuleP))
		return true;
	else
		return false;
}


void setRandomSeed (long iSeed)
{
	MesaGlobals::mRng.SetSeed (iSeed);
}


// *** END ***************************************************************/



