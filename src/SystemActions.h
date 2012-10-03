/**************************************************************************
SystemAction.h - an action that acts on the data system rather than within

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

// *** RUN MACRO

class SystemAction: public BasicAction
//: a container of other actions
{
public:
	// PUBLIC_TYPE INTERFACE
	typedef 	BasicAction*						value_type;
	typedef	std::vector<value_type>			container_type;
	typedef 	container_type::size_type		size_type;
	typedef	value_type&							reference;
	typedef	container_type::iterator		iterator;
	
	// LIFECYCLE
	BasicMacro (const char* iId)
		: BasicAction (iId)
		{}
	~BasicMacro ();
		
	// ACCESSORS
	iterator begin();
	iterator end();
		
	int size ()	const;
	int deepSize ();
	
	reference operator [] (int iIndex);
	reference at (int iIndex);
	value_type deepAt (int iIndex);
	
	int getDepth (size_type i);
	
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
	
	// MUTATORS
	void adoptAction (value_type iActionP);
	void orphanAction (int iIndex);
	void orphanAll ();
	void deleteAction (int iIndex);
	void deleteAll ();
	void deepDeleteAction (int iIndex);
			
	// I/O
	void detailedReport (std::ostream& ioOutStream, int iIndentLevel = 0);
	
	// DEPRECIATED & DEBUG
	void	validate	() {}

	// INTERNALS
protected:
	void assertValidIndex (int iIndex);
	
	void executeMacro ();
	
private:
	container_type			mContents;

	std::string				describeContents ();
};


// *** 
// *** END ***************************************************************/



