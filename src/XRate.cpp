/**************************************************************************
XRate.cpp - the finite rate of a process

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/


// *** INCLUDES

#include "XRate.h" 
#include "MesaGlobals.h"
#include "MesaTypes.h"
#include "ActionUtils.h"
#include <cassert>
#include <string>
#include <cstdlib>
#include <cmath>

using std::string;
using std::sprintf;


// *** CONSTANTS & DEFINES

const double kInflexionTrans = 1.177410023;
// NOTE: used in normal distribution. Equal to sqrt (- ln (1/4)).
// TO DO: improve
const double kRootTwoPi = 2.506628275;
// NOTE: used in normal distribution. sqrt (2 * pi)



// *** CLASS DECLARATION *************************************************/
// encapsulates rates and how they vary

// *** BASIC RATE ********************************************************/

XBasicRate::XBasicRate ()
	// ctor, just initialises description
{
	XBasicRate* theRate = this;
	// initDesc();
	// setDesc (mDesc);
}

	
// *** ACCESSORS
rate_t XBasicRate::getRate (nodeiter_t iCurrNode)
// return the rate for the given node
// normalise to sane values if negative or too big
{
	double theRate =  calculateRate (iCurrNode);
	if (theRate <= 0.0)
		theRate = 0.0;
	else if (1.0/MesaGlobals::mPrefs.mTimeGrain < theRate)
		theRate = 1.0/MesaGlobals::mPrefs.mTimeGrain;
	return theRate;
}

rate_t XBasicRate::calculateRate (nodeiter_t iCurrNode)
// actually calculate the rate
// override in derived classes
{
	// should never be called
	assert (false);
	// just to shut compiler up
	iCurrNode = iCurrNode;
	return -9.0;
}
	
void XBasicRate::finishInit ()
{
	setDesc (mDesc);
}
	
	
// *** MUTATORS
		
// *** DEPRECATED & DEBUG
const char* XBasicRate::describeRate ()
{
	// should never be called 
	// assert (false);
	// just to shut compiler up 
	return "";
}

// *** INTERNALS
void XBasicRate::setDesc (std::string& ioDescStr)
{
	// should never be called 
	// assert (false);
	// just to shut compiler
	ioDescStr = ioDescStr;
	DBG_MSG ("called setDesc in BasicRate");
}



// *** FLAT RATE *********************************************************/

// *** LIFECYCLE

XFlatRate::XFlatRate (rate_t iNewRate)
{
	assert (0.0 < mFlatRate);
	mFlatRate = iNewRate;
}

// *** ACCESSORS
rate_t XFlatRate::calculateRate (nodeiter_t iCurrNode)
{
	// just to shut compiler up
	iCurrNode = iCurrNode; 
	return mFlatRate;
}

// *** MUTATORS
	
// *** DEPRECATED & DEBUG

// *** INTERNALS

void XFlatRate::setDesc (std::string& ioDescStr)
{
	char theBuffer[16];
	sprintf (theBuffer, "%g", mFlatRate);
	// DBG_MSG ("called setDesc in XFlatRate");
	ioDescStr = theBuffer;
}



// *** DEPENDENT RATE ****************************************************/
// never directly instantiated

// *** LIFECYCLE
XBasicDependentRate::XBasicDependentRate
	(dependentVar_t iDependentVar, int iDepVarQualifier)
{
	mDependentVar = iDependentVar;
	mDepVarQualifier = iDepVarQualifier;
}

// *** ACCESSORS
rate_t XBasicDependentRate::calculateRate (nodeiter_t iCurrNode)
{
	double theVarValue;
	if (mDependentVar == kDependentVariable_Age)
	{
		MesaTree* theActiveTree = getActiveTreeP ();
		theVarValue = theActiveTree->getTimeSinceNodeOrigin (iCurrNode);
		assert (0.0 < theVarValue);
	}
	else if (mDependentVar == kDependentVariable_ContTrait)
	{
		MesaTree* theActiveTree = getActiveTreeP ();
		theVarValue = getContData (iCurrNode, (colIndex_t) mDepVarQualifier);
	}
	else
	{
		assert (false);
	}
	
	return calculateDependentRate (theVarValue);
}

rate_t XBasicDependentRate::calculateDependentRate (double iInputVal)
{
	// override in derived classes
	// should never be called 
	assert (false);
	// just to shut compiler up
	iInputVal = iInputVal;
	return -9.0;
}

// *** MUTATORS
	
// *** DEPRECATED & DEBUG

// *** INTERNALS
void XBasicDependentRate::setDesc (std::string& ioDescStr)
{
	char theBuffer[16];
	// sprintf (theBuffer, "%d", mRate);
	ioDescStr = theBuffer;
}



// *** LINEAR DEPENDENT RATE *********************************************/


// *** LIFECYCLE
XLinearDependentRate::XLinearDependentRate
	(double iSlope, double iIntercept, dependentVar_t iDependentVar, int iDepVarQualifier)
	: XBasicDependentRate (iDependentVar, iDepVarQualifier)
{
	mSlope = iSlope;
	mIntercept = iIntercept;
}

// *** ACCESSORS
rate_t XLinearDependentRate::calculateDependentRate (double iDependentVar)
{
	return ((mSlope * iDependentVar) + mIntercept);
}

// *** MUTATORS
	
// *** DEPRECATED & DEBUG

// *** INTERNALS

void XLinearDependentRate::setDesc (std::string& ioDescStr)
{
	char theBuffer[16];
	// sprintf (theBuffer, "%d", mRate);
	ioDescStr = theBuffer;
}



// *** TRI-PARAM DEPENDENT RATE ******************************************/

// *** LIFECYCLE
XTriParamDependentRate::XTriParamDependentRate
	(double iParamA, double iParamB, double iParamC, dependentVar_t iDependentVar, int iDepVarQualifier)
	: XBasicDependentRate (iDependentVar, iDepVarQualifier)
	, mParamA (iParamA), mParamB (iParamB), mParamC (iParamC)
{
}

// *** ACCESSORS
rate_t XTriParamDependentRate::calculateDependentRate (double iDependentVar)
{
	// Preconditions & preparations:
	double theRate = -1.0; // init as bad to catch problems
	
	// Main:
	if ((iDependentVar == 1.0))
	{
		theRate = mParamA + mParamC;
	}
	else
	{
		theRate = mParamA * std::pow (iDependentVar, mParamB) + mParamC;
	}
	
	// Postconditions & return:
	return theRate;
}

// *** MUTATORS
	
// *** DEPRECATED & DEBUG

// *** INTERNALS

void XTriParamDependentRate::setDesc (std::string& ioDescStr)
{
	char theBuffer[16];
	// sprintf (theBuffer, "%d", mRate);
	ioDescStr = theBuffer;
}



// *** LOG DEPENDENT RATE ************************************************/


// *** LIFECYCLE

XLogDependentRate::XLogDependentRate
	(double iLogBase, dependentVar_t iDependentVar, int iDepVarQualifier)
	: XBasicDependentRate (iDependentVar, iDepVarQualifier)
	, mLogBase (iLogBase)
{
	assert (1.0 < mLogBase);
}

// *** ACCESSORS
rate_t XLogDependentRate::calculateDependentRate (double iInputVal)
{
	// because the log (base anything) of numbers less than 1.0 is undefined
	if (iInputVal < 1.0)
		iInputVal = 1.0;
	return (std::log (iInputVal) / std::log (mLogBase));
}

// *** MUTATORS
	
// *** DEPRECATED & DEBUG

// *** INTERNALS

void XLogDependentRate::setDesc (std::string& ioDescStr)
{
	char theBuffer[16];
	// sprintf (theBuffer, "%d", mRate);
	ioDescStr = theBuffer;
}



// *** NORMAL DEPENDENT RATE *********************************************/

// *** LIFECYCLE
XNormalDependentRate::XNormalDependentRate
	(double iMeanRate, double iBgRate, double iPointOfMaxRate, double iInflexion,
		dependentVar_t iDependentVar, int iDepVarQualifier)
	: XBasicDependentRate (iDependentVar, iDepVarQualifier)
	, mMeanRate (iMeanRate)    // what is the maximum rate (peak) produced?
	, mBgRate (iBgRate)        // what is the minimum rate (baseline) produced?
	, mPointOfMeanRate (iPointOfMaxRate)  // where (what input value) gives the max rate?
	, mInflexion (iInflexion)  // how far away from the max does the rate halve?
{
	// it can be shown that the std deviation equals inflexion / 1.17741
	mStdDev = mInflexion / kInflexionTrans;
	// for scaling the result
	mScaling = mMeanRate - mBgRate;
	
	// Postconditions:
	assert (0.0 < mStdDev);
}


// *** ACCESSORS
rate_t XNormalDependentRate::calculateDependentRate (double iInputVar)
{
	/*
	PDF_normal =>
		1 / (stddev * sqrt (2 * pi)) * exp (-1/2 * ((x - mean) / stddev)^2)
		
	R_max (the maximum rate) = 1 / (stddev * sqrt (2 * pi))
	
	T_max (the point of / input value that produces R_max) = mean
	
	It can be shown that where I is the inflexion (the distance from T_max
	at which R = 1/2 R_max):
	
	stddev = I / sqrt (-ln (1/4)) = I / 1.17741 = I / kInflexionTrans
	
	As the area under the curve is unity, we must multiply by a scaling
	factor to get the correct shape of curve.
	*/

	// Preconditions & preparations:
	double theRate = -1.0; // init as bad to catch problems
	
	// Main:
	theRate = std::exp (- std::pow ((iInputVar - mPointOfMeanRate) /
		mStdDev, 2.0) / 2.0);
	theRate /= (mStdDev * kRootTwoPi);
	theRate *= mScaling;
	theRate += mBgRate;
	
	// Postconditions & return:
	assert (mBgRate <= theRate);
	assert (theRate <= mMeanRate);
	
	return theRate;
}

// *** MUTATORS
	
// *** DEPRECATED & DEBUG

// *** INTERNALS

void XNormalDependentRate::setDesc (std::string& ioDescStr)
{
	char theBuffer[16];
	// sprintf (theBuffer, "%d", mRate);
	ioDescStr = theBuffer;
}



// *** END ***************************************************************/



