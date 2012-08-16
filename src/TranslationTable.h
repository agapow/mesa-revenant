/**************************************************************************
TranslationTable.h - internal structure for phylogenetic trees

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- the reification of the Translation command command found in a Nexus
  TREES block. Can be used to build the translation table (by adding names
  from sucessive trees, then alphabetizing, and getName() for each index.
- indexes always number from 1 upwards
- for speed the map works for mapping names to numbers (for writing out
  trees, which is where this table will be used).
  
To Do:
- it strikes me that a better implementation would be a list<string> with
  operators string [int] and int [string], with the indexes (ints) being
  calculated dynamically, and names inserted in order. A bit slower but
  it solves the problem of items being deleted.
  
**************************************************************************/

#pragma once
#ifndef TRANSLATIONTABLE_H
#define TRANSLATIONTABLE_H


// *** INCLUDES

#include "Error.h"
#include "MesaTypes.h"
#include <string>
#include <map>
#include <list>

using std::vector;
using std::string;
using std::map;


// *** CLASS DECLARATION *************************************************/

class TranslationTable: public map<string, int>
//: stores a mapping from names to consequetive numbers
{
public:
	// PUBLIC TYPE INTERFACE
	typedef   map<string, int>      base_type;
	typedef   base_type::iterator   iter_type;

	// LIFECYCLE
	TranslationTable ()
		: mNextIndex (0)
		//: default ctor
		{}
	
	void addName (const char* iNewName)
	//: add name to the table (if not already there), with the next index in sequence 
	{
		std::string theNewName (iNewName);
		if (find (theNewName) == end())
			(*this)[theNewName] = getNextIndex();
	}

	std::string getName (int theIndex)
	//: return the name with the given index
	{
		// sort names into numerical order into vector
		for (iterator q = begin(); q != end(); q++)
		{
			if (q->second == theIndex)
				return q->first;
		}
		throw sbl::IndexError ("attempt to access translation table at unknown index");
	}

	template <typename OUTITER>
	void getNames (OUTITER iOutputIter)
	//: dump the stored names in order into a waiting output iterator
	{
		// sort names into numerical order into a vector of strings
		vector<string> 	theTmpVec (size());
      
		for (iter_type q = begin(); q != end(); q++)
			theTmpVec [q->second - 1] = q->first;
			
		// output
		for (vector<string>::iterator r = theTmpVec.begin(); r != theTmpVec.end();
			r++, iOutputIter++)
		{
			*iOutputIter = *r;
		}
	}
	
	void alphabetize ()
	//: change mapping of numbers so that the names are alphabetical
	{
		// grab all the names from the table
		stringvec_t 	theSortList;
		for (iterator q = begin(); q != end(); q++)
			theSortList.push_back (q->first);
		// sort them
		std::sort (theSortList.begin(), theSortList.end());
		// clear the map and place them back in numbered order
		clear();
		mNextIndex = 0;
		for (stringvec_t::iterator q = theSortList.begin();
			q != theSortList.end(); q++)
		{
			addName (q->c_str());
		}
	}


// DEBUG & DEPRECIATE
	void dump ()
	{
		DBG_MSG ("Dumping translation table at " << this);		
		DBG_MSG ("The size is " << size());
		iterator q = begin();
		for (q = begin(); q != end(); q++)
		{
			DBG_MSG ("Entry: " << q->first << "-" << q->second);		
		}
		DBG_MSG ("Finished dumping translation table");		
	}

// INTERNALS	
private:
	int mNextIndex;
	int getNextIndex ()
		{ return ++mNextIndex; }
};


#endif
// *** END ***************************************************************/



