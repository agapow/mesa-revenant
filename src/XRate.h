/**************************************************************************
XRate.h - the finite rate of a process

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2001, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once 
#ifndef XRATE_H
#define XRATE_H


// *** INCLUDES

#include "MesaTree.h"
#include "ActionUtils.h"
#include <string>
#include <map>


// *** CONSTANTS & DEFINES

enum dependentVar_t
{
	kDependentVariable_Age,        // time since origin
	kDependentVariable_ContTrait,  // continuous trait
	kDependentVariable_DiscTrait   // discrete trait
};


// *** CLASS DECLARATION *************************************************/
// encapsulates rates and how they vary

// *** BASIC RATE

class XBasicRate
//! encapsulated rate, maybe dependant on some other parameter
{
public:
// LIFECYCLE
	XBasicRate ();
	virtual ~XBasicRate () {}
	
// ACCESSORS
	virtual rate_t getRate (nodeiter_t iCurrNode);
	virtual rate_t calculateRate (nodeiter_t iCurrNode);
	
	void finishInit ();
	
// MUTATORS
		
// DEPRECATED & DEBUG
	virtual const char* describeRate ();

// INTERNALS
private:
	std::string   mDesc;
	
	virtual void setDesc (std::string& ioDescStr);
};



// *** FLAT RATE

class XFlatRate: public XBasicRate
// a rate that does not vary
{
public:
// LIFECYCLE
	XFlatRate (rate_t iNewRate);

// ACCESSORS
	rate_t calculateRate (nodeiter_t iCurrNode);

// MUTATORS
		
// DEPRECATED & DEBUG

// INTERNALS
private:
	rate_t   mFlatRate;

	virtual void setDesc (std::string& ioDescStr);
};



// *** DEPENDENT RATE
// never directly instantiated

class XBasicDependentRate: public XBasicRate
{
public:
// LIFECYCLE
	XBasicDependentRate (dependentVar_t iDependentVar, int iDepVarQualifier = -1);

// ACCESSORS
	rate_t calculateRate (nodeiter_t iCurrNode);

	virtual rate_t calculateDependentRate (double iInputVal);
	
// MUTATORS
		
// DEPRECATED & DEBUG

// INTERNALS
private:
	dependentVar_t   mDependentVar;
	int              mDepVarQualifier; // what col index etc.

	void setDesc (std::string& ioDescStr);
};



// *** LINEAR DEPENDENT RATE

class XLinearDependentRate: public XBasicDependentRate
{
public:
// LIFECYCLE
	XLinearDependentRate
	(double iSlope, double iIntercept, dependentVar_t iDependentVar, int iDepVarQualifier = -1)
	;

// ACCESSORS
	virtual rate_t calculateDependentRate (double iDependentVar);
	
// MUTATORS
		
// DEPRECATED & DEBUG

// INTERNALS
private:
	double	mSlope;
	double	mIntercept;

	void setDesc (std::string& ioDescStr);
};



// *** TRI-PARAM DEPENDENT RATE

class XTriParamDependentRate: public XBasicDependentRate
{
public:
// LIFECYCLE
	XTriParamDependentRate
	(double iParamA, double iParamB, double iParamC, dependentVar_t iDependentVar, int iDepVarQualifier = -1)
	;

// ACCESSORS
	virtual rate_t calculateDependentRate (double iDependentVar);
	
// MUTATORS
		
// DEPRECATED & DEBUG

// INTERNALS
private:
	double mParamA, mParamB, mParamC;

	void setDesc (std::string& ioDescStr);
};



// *** LOG DEPENDENT RATE

class XLogDependentRate: public XBasicDependentRate
{
public:
// LIFECYCLE
	XLogDependentRate
	(double iLogBase, dependentVar_t iDependentVar, int iDepVarQualifier = -1);

// ACCESSORS
	virtual rate_t calculateDependentRate (double iInputVal);
	
// MUTATORS
		
// DEPRECATED & DEBUG

// INTERNALS
private:
	double	mLogBase;

	void setDesc (std::string& ioDescStr);
};



// *** NORMAL DEPENDENT RATE

class XNormalDependentRate: public XBasicDependentRate
{
public:
// LIFECYCLE
	XNormalDependentRate
	(double iMeanRate, double iBgRate, double iPointOfMaxRate, double iInflexion,
		dependentVar_t iDependentVar, int iDepVarQualifier = -1);

// ACCESSORS
	virtual rate_t calculateDependentRate (double iInputVar);
	
// MUTATORS
		
// DEPRECATED & DEBUG

// INTERNALS
private:
		// user supplied parameters
	double mMeanRate, mBgRate, mPointOfMeanRate, mInflexion;
		// derived parameters
	double mScaling, mStdDev;                       

	void setDesc (std::string& ioDescStr);
};



// *** DISCRETE RATE

class DiscTraitRateArr
{
public:
	void storePair (const std::string& iTraitVal, double iRateVal)
	{
		mRateMap[iTraitVal] = iRateVal;
	}
	
	void storeDefault (double iRateVal)
	{
		mDefaultRate = iRateVal;
	}

	double getRate (const std::string& iTraitVal)
	{
		if (mRateMap.find (iTraitVal) == mRateMap.end())
			return (mRateMap[iTraitVal]);
		else
			return mDefaultRate;	
	}
	
private:
	std::map <std::string, double>   mRateMap;
	double                           mDefaultRate;
};


class XDiscreteRate: public XBasicDependentRate
{
public:
// LIFECYCLE
	XDiscreteRate (const DiscTraitRateArr& iTraitRatePairs, int iDepVarQualifier)
		: XBasicDependentRate (kDependentVariable_DiscTrait, iDepVarQualifier)
	{
		mTraitRateArr = iTraitRatePairs;
		mTraitIndex = iDepVarQualifier;
	}

// ACCESSORS
	rate_t calculateRate (nodeiter_t iCurrNode)
	{
		disctrait_t theVarValue = getDiscData (iCurrNode, (colIndex_t) mTraitIndex);
		return (mTraitRateArr.getRate (theVarValue));
	}
	
// MUTATORS
		
// DEPRECATED & DEBUG

// INTERNALS
private:
	DiscTraitRateArr   mTraitRateArr;
	void setDesc (std::string& ioDescStr)
	{
		char theBuffer[16];
		// sprintf (theBuffer, "%d", mRate);
		ioDescStr = theBuffer;
	}
	int mTraitIndex;
};



#endif
// *** END ***************************************************************/



