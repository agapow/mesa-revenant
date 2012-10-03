/**************************************************************************
TreeWrangler.h - for juggling arrays of phylogenetic trees

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.bio.ic.ac.uk/evolve>

**************************************************************************/

#pragma once
#ifndef TREEWRANGLER_H
#define TREEWRANGLER_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTree.h"
#include "MesaPrefs.h"
#include <vector>
#include <iostream>


// *** CONSTANTS AND DEFINES

class TranslationTable;


// *** CLASS TEMPLATE ****************************************************/

class TreeWrangler : public std::vector<MesaTree>
{
public:
	// PUBLIC TYPE INTERFACE
	typedef	std::vector<MesaTree>	base_type;
	typedef	base_type::size_type		size_type;
	typedef	base_type::iterator		iterator;
	
	// LIFECYCLE
	TreeWrangler ()
		: mDefIndex (0), mPrefsCollapseInternalNodes (true)
		{}
	
	// SERVICES
		
	// ACCESSORS
	size_type 		countTrees ();
		
	size_type		getActiveTreeIndex () const; 	

	std::string		getTreeName (size_type iIndex);
	
	iterator			begin () { return base_type::begin(); }
	iterator			end () { return base_type::end(); }
	
	// MUTATORS
	void				seedTree ();
	void				addTree (const MesaTree& iNewTree);	
	void				duplicateActiveTree ();
	void 				deleteTree (int iIndex);
	
	iterator			getActiveTreeIter ();

	void				setActiveTreeIndex (size_type iNewIndex);
	void				setTreeName (size_type iIndex, const char* iNewName);
	
	void				calcTranslationTable (TranslationTable* iTableP);
		
	// I/O
	void 				summarise (std::ostream& ioOutStream);
	void				detailedReport (std::ostream& ioOutStream);
	
	// DEPRECIATED & DEBUG
	void 				assertValidIndex (size_type iIndex) const;
	void				validate ();

	// MesaTree::size_type	countTaxa (); ///< @todo edit out
	// MesaTree::size_type	countAllTaxa ();
	double					calcPhyloDiv ();
	double					calcGeneticDiv ();
	MesaTree&		getActiveTree ();
	MesaTree*		getActiveTreeP ();
	
	// INTERNALS
private:
	size_type				mDefIndex;
	bool						mPrefsCollapseInternalNodes;

	MesaTree& 		refActiveTree ();
	static const char* nextFakeName ();
};


#endif
// *** END ***************************************************************/



