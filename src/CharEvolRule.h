/**************************************************************************
CharEvolRule.h - evolutionary rules for evolving characters

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/


#pragma once
#ifndef CHAREVOLRULE_H
#define CHAREVOLRULE_H


// *** INCLUDES

#include "Action.h"
#include "ActionUtils.h"
#include "EvolRule.h"
#include "CharStateSet.h"
#include "CharEvolScheme.h"


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class SpCharEvolRule: public ConditionalRule
//: a statement of a rule that governs character change at speciation
{
public:
	typedef ConditionalRule::size_type   size_type;
	virtual bool isTriggered (EvolRule* iFiringRuleP, nodearr_t& ioFiringLeaves);
		
private:
};


class SymmetricSceRule:  public SpCharEvolRule
//: a rule governing character evol at speciation, where it is the same for both sides
{
public:
	typedef SpCharEvolRule::size_type   size_type;
	// LIFECYCLE
	// accept defaults
			
	void          commitAction (nodearr_t& iNode, mesatime_t iTime);
	void          adoptScheme (SchemeArr& iSchemeArr);
	
	const char*   describe (size_type iIndex);
	const char*   describeRule ();
	void          deleteElement (size_type iIndex);
	size_type     deepSize ();
	size_type     getDepth (size_type iIndex);

	SchemeArr     mSchemes;
};


class AsymmetricSceRule:  public SpCharEvolRule
//: a rule governing character evol at speciation, where it is different for both sides
{
public:
	// LIFECYCLE
	// accept defaults
					
	void          commitAction (nodearr_t& iNode, mesatime_t iTime);
	void          adoptScheme (int iSide, SchemeArr& iSchemeArr);
	
	const char*   describe (size_type iIndex);
	const char*   describeRule ();
	void          deleteElement (size_type iIndex);
	size_type     deepSize ();
	size_type     getDepth (size_type iIndex);

	SchemeArr     mSchemes[2];

private:
};


class TerminalCharEvolRule: public SymmetricSceRule
//: a rule governing character evol at speciation, where it is the same for both sides
{
public:
	// LIFECYCLE
	// accept defaults
			
	bool          isTriggered (EvolRule* iRuleP, nodearr_t& ioFiringLeaves);
	const char*   describeRule ();
	void          commitAction (nodearr_t& iNode, mesatime_t iTime);
};



class GradualCharEvolRule: public SymmetricSceRule
//: a rule governing character evol at speciation, where it is the same for both sides
{
public:
	bool          isTriggered (EvolRule* iRuleP, nodearr_t& ioFiringLeaves);
	const char*   describeRule ();
	void          commitAction (nodearr_t& iNode, mesatime_t iTime);
};



#endif
// *** END ***************************************************************/



