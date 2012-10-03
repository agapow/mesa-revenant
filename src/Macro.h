/**************************************************************************
MacroAction.h - an action that contains & controls other actions

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef MACROACTION_H
#define MACROACTION_H


// *** INCLUDES

#include "Action.h"
#include <vector>


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

// *** RUN MACRO

class BasicMacro: public BasicAction
//: a container of other actions
{
public:
	// PUBLIC_TYPE INTERFACE
	typedef  BasicAction::size_type        size_type;
	typedef 	BasicAction*						value_type;
	typedef	std::vector<value_type>		   container_type;
	typedef	value_type&							reference;
	typedef	container_type::iterator		iterator;
	
	// LIFECYCLE
	BasicMacro ()
		{}
	virtual ~BasicMacro ();
		
	// ACCESSORS
	iterator begin();
	iterator end();
	reference at (int iIndex);
	bool containsAnalyses ();
		
	// ELEMENT INTERFACE
	size_type size () const;
	size_type deepSize ();
	size_type getDepth (size_type iIndex);
	void deleteElement (int iIndex);
	const char* describe (size_type iIndex);
	virtual const char* describeMacro () = 0;
	
	// MUTATORS
	void adoptAction (value_type iActionP);
	void orphanAction (int iIndex);
	void orphanAll ();
	void deleteAction (int iIndex);
	void deleteAction (iterator iIter);
	void deleteAll ();
	// void deepDeleteAction (int iIndex);

	template <typename ITER>
	void copyContents (ITER iStart, ITER iStop)
	{
		deleteAll ();
		while (iStart != iStop)
		{
			adoptAction (*iStart);
			iStart++;
		}
	}
	
	void copyContents (BasicMacro* iOldMacroP);
	void stealContents (BasicMacro* iOldMacroP);
					
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
protected:
	void assertValidIndex (int iIndex);
	
	void executeMacro ();
	
private:
	container_type			mContents;

};


// *** RUN N MACRO

class RunNMacro: public BasicMacro
//: run the enclosed actions N times
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
	RunNMacro (int iLoops)
		: mLoops (iLoops)
		{}
				
	// I/O
	const char* describeMacro ();
	
	// SERVICES
	void execute ();
		
	// DEPRECIATED & DEBUG
	void	validate	();

	// INTERNALS
protected:
	int	mLoops;
};


// *** RUN ONCE MACRO

class RunOnceMacro: public BasicMacro
//: run the enclosed actions once
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
	RunOnceMacro ()
		{}
				
	// SERVICES
	void execute ();
	
	// I/O
	const char* describeMacro ();
	
	// DEPRECIATED & DEBUG
	void	validate	() {}
};


// *** RUN ONCE MACRO

class TreeMacro: public BasicMacro
//: run the enclosed actions over every tree
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
	TreeMacro ()
		{}
				
	// SERVICES
	void execute ();
	
	// I/O
	const char* describeMacro ();
	
	// DEPRECIATED & DEBUG
	void	validate	() {}
};


class RunAndRestoreMacro: public BasicMacro
//: runs the enclosed commands multiple times, reseting the data between each
// by implication, a run & restore leaves the system the same as when it came.
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
	RunAndRestoreMacro (int iLoops)
		: mLoops (iLoops)
		{}
				
	// SERVICES
	void execute ();
	
	// I/O
	const char* describeMacro ();
	
	// DEPRECIATED & DEBUG
	void	validate	();

	// INTERNALS
private:
	int				mLoops;
};


// *** UTILITY FUNCTIONS *************************************************/

BasicMacro* castAsMacro (BasicAction* iActionP);


#endif
// *** END ***************************************************************/



