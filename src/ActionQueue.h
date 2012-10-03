/**************************************************************************
ActionQueue.h - a manager for groups of model manipulations 

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.bio.ic.ac.uk/evolve>

**************************************************************************/

#pragma once
#ifndef ACTIONQUEUE_H
#define ACTIONQUEUE_H


// *** INCLUDES

#include "Macro.h"
#include <vector>
#include <ostream>


// *** CONSTANTS AND DEFINES

class BasicAction;


// *** CLASS TEMPLATE ****************************************************/

class ActionQueue
{
public:
	// PUBLIC TYPE INTERFACE
	typedef BasicAction*   value_type;
	typedef RunOnceMacro   container_type;
	typedef int            size_type;
	
	// LIFECYCLE
	ActionQueue ();
	~ActionQueue ();
	
	// NEW INTERFACE
	
	// ACCESSORS
	int           size ();
	const char*   describe (size_type iIndex);
	int           getDepth (size_type iIndex);
	bool          isEmpty ();
	bool          containsAnalyses ();
	
	// MUTATORS
	void          adoptAction (value_type iNewActionP);
	void          deleteElement (size_type iIndex);
	void          clear ();
			
	// I/O
	void          summarise (std::ostream& ioOutStream);
	void          detailedReport (std::ostream* ioOutStreamP);
	
	// SERVICES
	void          runOnce ();
	void          runTrees ();
	void          runN (int iLoops);
	void          runAndRestore (int iLoops);
	
	// DEPRECIATED & DEBUG
	void          validate ();	
	void          dump ();
	
	// INTERNALS
private:
	container_type	mContents;
};



#endif
// *** END ***************************************************************/



