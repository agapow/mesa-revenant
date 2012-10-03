/**************************************************************************
MacroAction.h - an action that contains & controls other actions

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://p.agapow@ic.ac.uk> <http://www.bio.ic.ac.uk/evolve/>

**************************************************************************/


// *** INCLUDES

#pragma once

#include "Sbl.h"
#include "Action.h"


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class BasicMacro: public BasicAction
//: a container of other actions
{
public:
	// PUBLIC_TYPE INTERFACE
	typedef 	BasicAction*								value_type;
	typedef 	std::vector<value_type>::size_type	size_type;
	typedef	value_type&									reference;
	
	// LIFECYCLE
	~BasicMacro ()
		{ deleteAll(); }
		
	// ACCESSORS
	int size ()	const
		{ return int (mContents.size()); } 
		
	reference operator [] (int iIndex)
	{
		return mContents.at((size_type) iIndex);
	}

	reference at (int iIndex)
	{
		assertValidIndex (iIndex);
		return (*this)[iIndex];
	}
		
	// MUTATORS
	void adoptAction (value_type iActionP)
	//: add (and take responsibility for) an action to the macro
		{ mContents.push_back (iActionP); }

	void orphanAction (int iIndex)
	{
		assertValidIndex (iIndex);
		mContents.erase (mContents.begin() + iIndex);
	}
		
	void deleteAction (int iIndex)
	{
		delete at(iIndex);
		mContents.erase (mContents.begin() + iIndex);
	}
	
	void deleteAll ()
	//: delete all the actions and empty the container
	{
		for (size_type i = 0; i < size(); i++)
			delete at (i);
		mContents.clear ();
	}
			
	// I/O
	std::string describe ()
	//: return a description of the macro action and it's contents
	// For macroactions, we return all the descriptions within.
	// TO DO: how do we identify macro contents and end?
	{
		std::string theBuffer ("macro: ");
		theBuffer += describeSelf () + describeContents();
		return theBuffer;
	}
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
protected:
	void assertValidIndex (int iIndex)
	{
		assert (0 <= iIndex);
		assert (iIndex < size());
	}
	
	void executeMacro ()
	//: execute the enclosed block
	// necessary for evolution events
	{
	
	}
	
private:
	std::vector<value_type>	mContents;

	virtual std::string describeSelf () = 0;
	
	std::string describeContents ()
	//: detail the contents of the macroaction
	{
		std::string theBuffer;
		for (int i = 0; i < size(); i++)
			theBuffer += ((*this)[i])->describe();
		return theBuffer;
	}	
};


class RunNMAcro: public BasicMacro
//: run the enclosed actions N times
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
	RunNAction (int iLoops) : mLoops (iLoops) {}
				
	// ACCESSORS
		
	// MUTATORS
		
	// I/O
	std::string describeSelf ()
	{
		std::string theBuffer ("run the enclosed tasks ");
		theBuffer += sbl::toString (mLoops) + " times\n";
		return theBuffer;
	}

	// SERVICES
	void execute ()
	{
		for (int i = 1; i <= mLoops; i++)
			executeMacro();
	}
		
	// DEPRECIATED & DEBUG
	void	validate	()
		{ assert (0 < mLoops); }

	// INTERNALS
private:
	int	mLoops;
};


class RunOnceMacro: public RunNMacro
//: run the enclosed actions one
{
public:
	// PUBLIC_TYPE INTERFACE

	// LIFECYCLE
	RunOnceAction () : RunNAction (1) {}
				
	// ACCESSORS
		
	// MUTATORS
		
	// SERVICES
	void execute ()
	{
		executeMacro();
	}
		
	// I/O
	std::string describeSelf ()
		{ return std::string ("run the enclosed tasks once\n"); }
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
private:
};


// *** END ***************************************************************/



