/**************************************************************************
MesaTree.h - internal structure for phylogenetic trees

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef MESATREE_H
#define MESATREE_H


// *** INCLUDES

#include "SimpleTree.h"
#include "MesaTypes.h"
#include "XMembership.h"
#include "CaicCode.h"
#include <string>
#include <vector>
#include <cmath>
#include <iterator>
#include <utility>


// *** CONSTANTS & DEFINES

typedef std::vector<long>	nodeidvec_t;

class TranslationTable;

class MesaTreeNode
//: the data contained within a node of a tree
// done this way so it can act as an agent or be expanded later.
{                                                     
public:
	std::string		mName;

};


// *** CLASS DECLARATION *************************************************/

/**
A phylogenetic tree.

By convention accessors of tree properties are called "aaaTreeBbb" and
those of nodes in the tree "aaaNodeBbb" or "aaaLeafBbb" (where the 
node must be a tip/leaf.
*/
class MesaTree: public sbl::SimpleTree <MesaTreeNode>
{
private:
	// INTERNAL TYPES
	typedef sbl::SimpleTree <MesaTreeNode>		base_type;
	typedef sbl::XMembership<id_type>         membership_type;
	
public:
	// PUBLIC TYPE INTERFACE
	typedef base_type::weight_type				weight_type;
	typedef base_type::iterator					iterator;
	typedef base_type::id_type					   id_type;
	
	// LIFECYCLE
	/* nothing thus far */	
				
	// ACCESSORS
	std::string		getTreeName () const;
	void				setTreeName (const char* iNameStr);
	
	size_type      countAliveLeaves ();

	bool				isTreeRooted () { return true; }
	bool           isTreeBifurcating ();
	bool           hasTreeSingletons ();
	bool           isTreeAlive ();
	bool           isTreePaleo ();
	bool           isTreeNeo ();
	
	double         calcPhyloDiversity ();
	double         calcGeneticDiversity();
	
	std::string 	getLeafName (iterator& iTargetIter);
	std::string 	getNodeName (iterator& iTargetIter);
	void 				setNodeName (iterator iTargetIter, std::string iName);
	
	std::string 	getNodeLabelPhylo (iterator& iTargetIter);
	std::string 	getNodeLabelCaic (iterator& iTargetIter);
	std::string 	getNodeLabelSeries (iterator& iTargetIter);
	
	weight_type    getPhyloAge ();
	weight_type    getTreeAge ();
	weight_type    getRootAge ();
		
	weight_type		getNodeAge (iterator& iNodeIter);
	weight_type    getTimeSinceNodeTerminus (iterator& ikNode);
	weight_type    getTimeSinceNodeOrigin (iterator& ikNode);
	weight_type		getTimeFromNodeToParent (iterator& iNodeIter);
	weight_type    getTimeFromNodeToAncestor (iterator iNode, iterator iAncestor);
	weight_type    getTimeFromNodeToRoot (iterator iNode);
	
	iterator       getLivingDescendant (iterator & iNodeIter);
	
	size_type 		getDistance (iterator iChildIter, iterator iParIter);
	
	bool				isNodeAlive (iterator& iNode);
	bool           isNodeBifurcating (iterator& iNode);
	bool           isNodeSingleton (iterator& iNode);
	
	iterator		   getIter (const char* iNameStr);
	
	iterator       getNodeByCaicCode (const char* iCaicStr);
	iterator			getNodeByCaicCode (iterator iStartNode, const char* iCaicStr);

	// MUTATORS	
	void		ageAllLeaves (double iAgeIncr);
	
	void		speciate (iterator iSplitIter, iterator& oChildIter1, iterator& oChildIter2);
	void		killLeaf (iterator& iLeafIter);

	void     makeDead (iterator iDeadNode);
	void     makeInternalsDead ();
	
	void		moveSubtree (iterator& ioNewChild, iterator& ioNewParent);
	void		collapseNode (iterator ioNodeIter);
	void		collapseBranch (iterator ioNodeIt);

	iterator     addNode (sbl::CaicCode theNewCode);
	iterator     addNodeHelper (sbl::CaicCode iNewCode, iterator iParentIt);


	// I/O
	std::string		writeNewick (TranslationTable* iTranslatorP = NULL);
	
	bool				mPrefsCollapseInternalNodes;	

	// DEPRECIATED & DEBUG
	void dump ();

	iterator getIter (id_type iId)
	{
		return base_type::getIter (iId);
	}

	void 			getLiveLeaves (std::vector<iterator>& ioIters);
	void 			getLeaves (std::vector<iterator>& ioIters);
	void			collectLeaveIds (id_type iTargetId, nodeidvec_t& iResultVec);
	std::string getNodeLabel (MesaTree::id_type iTargetId);
	void 			setNodeName (id_type theNewId, std::string iName);
	std::string	getNodeNamebyId (id_type iTargetId);
	id_type		getLeafIdbyName (std::string& iName);
	void			getTaxaNames (stringvec_t& ioLabelVec);
	void			getNodeVec (nodeidvec_t& iResultVec);
	// void ageLeaf (iterator iLeafIter, double iAgeIncr);
	// weight_type getAge (const iterator& iNodeIter);
	// void getNodeChildrenVec (id_type iTargetId, nodeidvec_t& iResultVec);
	//	void getNodeChildrenVec (iterator iSubtreeIter, nodeidvec_t& iResultVec);
	// weight_type getTimeToRoot (iterator iNodeIter);


	// INTERNALS
private:
	std::string       mName;
	membership_type   mDeadList; // store the id's of dead nodes

	size_type	getChildIndex (iterator& iChildIter);

	const char* getFirstTipName (id_type iTargetId);

	void			writeNewickNode (id_type iNodeId, std::stringstream& ioRep,
						TranslationTable* iTranslatorP, bool iDoDistances);
};


typedef MesaTree::iterator         nodeiter_t;
typedef std::vector <nodeiter_t>   nodearr_t;



#endif
// *** END ***************************************************************/



