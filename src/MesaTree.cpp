/**************************************************************************
MesaTree.cpp - internal structure for phylogenetic trees

Credits:
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- a reification of tree structure with info held in nodes.

Changes:
- 01.1.30: made name private and added accessors and mutators.

**************************************************************************/


// *** INCLUDES


#include "MesaTree.h"
#include "Sbl.h"
#include "MesaGlobals.h"
#include "TranslationTable.h"
#include "CaicCode.h"
#include <sstream>
#include <iterator>
#include <iomanip>
#include <vector>

using std::back_insert_iterator;
using sbl::kTree_IdUnknown;
using sbl::kTree_IdNone;
using sbl::kTree_DefaultWt;
using sbl::CaicCode;
using std::stringstream;
using std::setfill;
using std::setw;
using std::vector;


// *** CONSTANTS & DEFINES


typedef MesaTree::size_type		size_type;
typedef MesaTree::weight_type		weight_type;
typedef MesaTree::iterator			iterator;
typedef MesaTree::id_type        id_type;


// *** MAIN BODY *********************************************************/

// *** ACCESSORS *********************************************************/

bool MesaTree::isNodeAlive (MesaTree::iterator& iNode)
{
	// if not a tip, it must be dead
	if (not isLeaf (iNode))
		return false;
	else
	{
		id_type theId = getId (iNode);
		return (not mDeadList.isMember (theId));
	}
}


bool MesaTree::isNodeBifurcating (iterator& iNode)
{
	// Preconditions:
	assert (iNode != end());
	
	// Main:
	return  (countChildren(iNode) == 2);
}


bool MesaTree::isNodeSingleton (iterator& iNode)
{
	// Preconditions:
	assert (iNode != end());
	
	// Main:
	return  (countChildren(iNode) == 1);
}


weight_type MesaTree::getTimeSinceNodeTerminus (iterator& ikNode)
//: how much time has passed since this node stopped existing?
// Should be 0 for extant nodes.
{
/*
TO DO:  due to float arithmetic not being commutative (due to rounding off
and imprecise representation in binary) the extant tips can return very 
small and negative values. Bugger. Hence a hack to "flatten" out very small
values.

NOTE: initially I assumed that if no time had passed since the node
terminated, it must be alive. This is untrue - a epoch could end with the
last death, or just after a speciation has occurred. Hence this assertion
has been dyked out.
*/

	// Main:
	if (isNodeAlive (ikNode))
		return 0.0;
		
	weight_type theRootAge = getRootAge ();
	weight_type theTime = theRootAge - getTimeFromNodeToRoot (ikNode);
	// NOTE: utter hack
	if (theTime < 0.0)
	{
		DBG_VAL (theRootAge);
		DBG_VAL (theTime);
		DBG_MSG (isRoot (ikNode));
	}
	
	if (std::abs (theTime) < .0001)
		theTime = 0.0;
	
	if (theTime < 0.0)
	
	// Postconditions & return:
	assert (0.0 <= theTime); // the age must be positive
	// if (0.0 == theTime)
	//	assert (isNodeAlive (ikNode));
		
	return theTime;
}


weight_type MesaTree::getTimeSinceNodeOrigin (iterator& ikNode)
//: how much time has passed since this node sprang into existence?
{
	weight_type theTime = getTimeSinceNodeTerminus (ikNode);
	theTime += getTimeFromNodeToParent (ikNode);
	return theTime;
}


weight_type MesaTree::getNodeAge (iterator& ikNode)
//: calculate the age of a node, i.e. the distance from it to the tips
// CHANGE: this used to calculate age by measuring time from the
// node to any tip. This assumed the tree was ultrametric (and therefore
// wasn't paleontological). Now we calculate age by getting the age of
// the tree and then subtracting the weight to root for this node and the
// root weight. Hence this is a very expensive operation.
{
	// Main:
	
/*
	// Main:
	weight_type theAge = 0.0;
	iterator theBranchIter = iNodeIter;
	while (not isLeaf (theBranchIter))
	{
		theBranchIter = getChild (theBranchIter, 0);
		theAge += getEdgeWeight (theBranchIter);
	}
	
	assert (0.0 <= theAge);
	return theAge;
*/

	weight_type theRootAge = getRootAge();
	weight_type theNodeAge = theRootAge - getTimeFromNodeToRoot (ikNode);
	
	// Postconditions:
	// the age must be positive
	if (theNodeAge < 0.0)
	{
		DBG_MSG (theRootAge << " " << theNodeAge);
	}
	
	if (not isRoot (ikNode))
		assert (0.0 <= theNodeAge);
	// if can be 0.0 if this is a leaf or there are no distances in tree
	/*
	if (0.0 == theNodeAge)
		assert (isLeaf (ikNode));
	*/
	return theNodeAge;
}


weight_type MesaTree::getTimeFromNodeToParent (iterator& ikNode)
//: how long is it since the node split from its parent?
// This is simply the length of the branch between the node and its
// parent and is in a function by itself purely for clarity of purpose.
{
	// Preconditions:
	// assert (not isRoot (ikNode));
	
	// Main:
	weight_type theTime = getEdgeWeight (ikNode);
	
	// Postconditions & return:
	assert (0.0 <= theTime);
	return theTime;
}


weight_type MesaTree::getTimeFromNodeToAncestor (iterator iNode, iterator iAncestor)
//: what is the sum of weights from this node to the ancestor
// Assumes that the ancestor is correctly identified by asserting that
// the node will never march off the top of the tree.
{
	// Preconditions:
	assert (iNode != end());
	assert (iAncestor != end());
	
	// Main:
	if (iNode == iAncestor)
	{
		return 0.0;
	}
	else
	{
		weight_type theDist = 0.0;
		
		while (iNode != iAncestor)
		{
			assert (not isRoot (iNode));
			theDist += getTimeFromNodeToParent (iNode);
			iNode = getParent (iNode);
			assert (iNode != end());
		}
		
		assert (0.0 <= theDist);
		return theDist;
	}
}


weight_type MesaTree::getTimeFromNodeToRoot (iterator iNode)
//: what is the sum of weights from this node to the root
{
	if (isRoot (iNode))
		return 0.0;
	else
		return getTimeFromNodeToAncestor (iNode, getRoot());

}

/*
weight_type  MesaTree::getTimeToRoot (iterator iNodeIter)
//: what is the sum of weights from here to the root?
// better to say ancestor or something
{
	iterator	theRootIter = getRoot();
	if (iNodeIter == theRootIter)
	{
		return 0.0;
	}
	else
	{
		weight_type theDist = 0.0;
		while (iNodeIter != theRootIter)
		{
			theDist += getEdgeWeight (iNodeIter);
			iNodeIter = getParent (iNodeIter);
			assert (iNodeIter != end());
		}
		return theDist;
	}
}
*/


iterator MesaTree::getLivingDescendant (iterator & iNode)
//: return a descendent of this node that is still alive / extant
// If there are none, return failure (i.e. end())
{
	// if this is alive, return this
	if (isNodeAlive (iNode))
		return iNode;
	
	// if it is a tip and has no descendants, return end()
	if (isLeaf (iNode))
		return end();
		
	// otherwise ask each child in turn for a living descendant, and if
	// one is found, return that
	size_type theNumChildren = countChildren (iNode);
	for (size_type i = 0; i < theNumChildren; i++)
	{
		iterator theChild = getChild (iNode, i);
		iterator theAnswer = getLivingDescendant (theChild);
		if (theAnswer != end())
			return theAnswer;
	}

	// otherwise all descendants must be dead, return failure
	return end();
}




// *** TREE ACCESSORS ******************************************************/


/**
What is the age of the tree?

We define this as being the age of the root of the tree, being the longest
distance from a tip to the root. This is slow to calculate but is more
robust than other methods, as we have no way of knowing which tips are
extant (living) or dead.
*/
weight_type MesaTree::getPhyloAge ()
{
	// Preconditions
	assert (not isEmpty());
	weight_type theAge = -1000.0; // trap with crap value
	
	// Main:
	// look for maximum distance from tip to root
	for (iterator q = begin(); q != end(); q++)
	{
		double theNewAge = getTimeFromNodeToRoot (q);
		theAge = std::max (theAge, theNewAge);
	}
	
	// Postconditions & return:
	assert (0.0 <= theAge);
	return theAge;	
}


weight_type MesaTree::getRootAge ()
//: what is the age of the root of the tree?
// Calculate the age as being the sum of distances from tip to root. If a
// living node can be found,
// measure the distance from there. (This assumes that the living portion
// of the tree is ultrametric.) If there are no living nodes, measure
// the distance from every tip and take the greatest.
{
	// Preconditions
	assert (not isEmpty());
	// trap with crap value
	weight_type theAge = -1000.0; 
	
	// Main:
	// find a living tip
	iterator q;
	for (q = begin(); q != end(); q++)
	{
		if (isNodeAlive (q))
			break;
	}
	
	if (isNodeAlive (q))
	{
		// if tree alive, measure from living tip to root
		theAge = getTimeFromNodeToRoot (q);
	}
	else
	{
		// otherwise look for maximum distance from tip to root
		for (q = begin(); q != end(); q++)
		{
			double theNewAge = getTimeFromNodeToRoot (q);
			theAge = std::max (theAge, theNewAge);
		}
	}
	
	// Postconditions & return:
	assert (0.0 <= theAge);
	return theAge;	
}

weight_type MesaTree::getTreeAge ()
//: what is the age of the tree?
// This is the age of the root, including the distance on the root.
{
	// Main:
	weight_type theAge = getRootAge();
	
	// get weight on root
	iterator	theRootIter = getRoot();
	theAge += getEdgeWeight (theRootIter);

	// Postconditions & return:
	assert (0.0 <= theAge);
	return theAge;	
}


bool MesaTree::isTreeAlive ()
//: is the tree still growing (i.e. does it have have living taxa)?
{
	for (iterator q = begin(); q != end(); q++)
	{
		if (isLeaf (q) and isNodeAlive (q))
			return true;
	}
	
	return false;
}


bool MesaTree::isTreeBifurcating ()
//: is this tree perfectly bifurcating?
// Test this by asking if every node is either a leaf or has 2 children.
{
	for (iterator q = begin(); q != end(); q++)
	{
		if (isInternal (q) and (not isNodeBifurcating (q)))
			return false;
	}
	
	return true;
}


bool MesaTree::hasTreeSingletons ()
//: does the tree have singleton (ghost) nodes?
{
	for (iterator q = begin(); q != end(); q++)
	{
		if (isInternal (q) and (isNodeSingleton (q)))
			return true;
	}
	
	return false;
}

bool MesaTree::isTreePaleo ()
//: is this a paleontological tree (i.e. does it have dead tips)?
{
	for (iterator q = begin(); q != end(); q++)
	{
		if (isLeaf (q) and (not isNodeAlive (q)))
			return true;
	}
	
	return false;
}

bool MesaTree::isTreeNeo ()
//: is this a neontological tree (i.e. are all tips alive)?
{
	return (not isTreePaleo ());

/*	
	for (iterator q = begin(); q != end(); q++)
	{
		if (isLeaf (q) and (not isNodeAlive (q)))
			return false;
	}
	
	return true;
*/
}

size_type MesaTree::countAliveLeaves ()
{
	size_type theTotal = 0;
	for (iterator q = begin(); q != end(); q++)
	{
		if (isNodeAlive (q))
			theTotal++;
	}

	return theTotal;	
}


/**
Calculate the phylogenetic diversity (sum of branchlengths).

As several functions have to calculate this, we include it as part of
the tree.

@returns  The phylogenetic diversity. Note that this can be 0.0 if there
        are no distances in the tree.
        
@note   By convention, this does not include any distance on the root.
@note   Also by convention, this does not consider "dead/alive" status. If
        you only want to consider living nodes, prune the dead ones first.
*/
double MesaTree::calcPhyloDiversity ()
{
	double		 theAnswer = 0.0;
	nodeiter_t	 q;
	nodeiter_t   theRoot = getRoot ();
	
	for (q = begin(); q != end(); q++)
	{
		weight_type theDistance = getTimeFromNodeToParent (q);
		assert (0.0 <= theDistance);
		if (q != theRoot)
			theAnswer += theDistance;
	}
	
	return theAnswer;
}


/**
Calculate the genetic diversity (product of branchlength complements).

As several functions have to calculate this, we include it as part of
the tree. GD is calculated as (1 - (1 - b_1)(1 - b_2)(1 - b_3) ...), where
b_n is the length of the nth branch. This calculation requires that
distances in the tree are allelic, that is probabilities that taxa will
share alleles, and thus between 0.0 and 1.0. 

@returns  The genetic diversity. Note that this can be 1.0 if there are no
        distances in the tree or 0.0 if there are non-allellic distances
        in the tree.
        
@note   By convention, this does not include any distance on the root.
@note   Also by convention, this does not consider "dead/alive" status. If
        you only want to consider living nodes, prune the dead ones first.
@note   Note that if the distances between nodes are large, then GD will
        tend towards 1.0. For example, if all branchlengths are 0.7, in a
        tree of 20 tips, the GD is effectively 1.0.
        
@todo   Better error reporting of non-allelic distances?
*/
double MesaTree::calcGeneticDiversity ()
{
	double		theAnswer = 1.0;
	bool			theDistancesAreAllelic = true;
	nodeiter_t	q;
	nodeiter_t 	theRootIter = getRoot ();
	
	for (q = begin(); q != end(); q++)
	{
		weight_type theDistance = getTimeFromNodeToParent (q);
		if (1.0 <= theDistance)
		{
			theDistancesAreAllelic = false;
			break;
		}
		assert (0.0 <= theDistance);
		if ((q != theRootIter) and (0.0 < theDistance))
			theAnswer *= (1.0 - theDistance);
	}

	if (theDistancesAreAllelic == false)
		// non-allelic distances
		return 0.0;
	else if (theAnswer == 1.0)
		// no distances at bloody all
		return 1.0;
	else
	{
		theAnswer = 1.0 - theAnswer;
		assert (theAnswer <= 1.0);
		assert (0.0 <= theAnswer);
		return theAnswer;
	}
}


// *** NODE COLLECTION ******************************************************/



/*
void MesaTree::getNodeChildrenVec
(MesaTree::id_type iTargetId, nodeidvec_t& iResultVec)
//: collect the IDs of all the children of this node
{
	std::back_insert_iterator<nodeidvec_t> theOutputIter (iResultVec);
	getNodeChildren (getIter (iTargetId), theOutputIter);
}


void MesaTree::getNodeChildrenVec
(MesaTree::iterator iSubtreeIter, nodeidvec_t& iResultVec)
//: collect the IDs of all the children of this node
{
	getNodeChildrenVec (iSubtreeIter->first, iResultVec);
}
*/

void MesaTree::getNodeVec (nodeidvec_t& iResultVec)
//: return the IDs of the nodes in the tree
{
	std::back_insert_iterator<nodeidvec_t> theOutputIter (iResultVec);
	for (iterator q = begin(); q != end(); q++, theOutputIter++)
	{
		*theOutputIter = q->first;
	}
}


void MesaTree::getLiveLeaves (vector<iterator>& ioIters)
{
	std::back_insert_iterator< vector<iterator> > theOutputIter (ioIters);
	for (iterator q = begin(); q != end(); q++)
	{
		if (isNodeAlive (q))
		{
			*theOutputIter = q;
			theOutputIter++;
		}
	}
}

void MesaTree::getLeaves (vector<iterator>& ioIters)
{
	std::back_insert_iterator< vector<iterator> > theOutputIter (ioIters);
	for (iterator q = begin(); q != end(); q++)
	{
		if (isLeaf (q))
		{
			*theOutputIter = q;
			theOutputIter++;
		}
	}
}



size_type MesaTree::getChildIndex (iterator& iChildIter)
//: what number child is this (e.g. 2nd of 4)?
// NOTE: as always, numbering is from 0. Meaningless for root.
{
	// Preconditions:
	assert (not isRoot (iChildIter));

	// Main:
	iterator theParIter = getParent (iChildIter);
	size_type theNumChildren = countChildren (theParIter);
	for (size_type i = 0; i < theNumChildren; i++)
	{
		if (getChild (theParIter, i) == iChildIter)
			return i;
	}
	
	assert (false); // if you get this far, there's a problem
	return -1; // to shut compiler up
}



void MesaTree::getTaxaNames (stringvec_t& ioLabelVec)
{
	for (iterator q = begin(); q != end(); q++)
	{
		if (isLeaf (q))
			ioLabelVec.push_back (getLeafName (q));
	}
}


// *** NODE NAMING & LABELLING **********************************************/


void MesaTree::setNodeName (iterator iTargetIter, std::string iName)
{
	iTargetIter->second.mData.mName = iName;
}

std::string MesaTree::getLeafName (iterator& iTargetIter)
//: return the name of the leaf/tip pointed to by this iterator
{
	// Preconditions:
	assert (isLeaf (iTargetIter));
	
	// Main:
	return getNodeName (iTargetIter);			
}

std::string MesaTree::getNodeName (iterator& iTargetIter)
//: return the name of the leaf/tip pointed to by this iterator
{
	// Main:
	return iTargetIter->second.mData.mName;			
}

std::string MesaTree::getNodeLabelPhylo (iterator& iTargetIter)
//: return the label of the node pointed to by this iterator
// TO DO: throw if bad
// NOTE: is this expensive?
{
	if (isRoot (iTargetIter))
		return (std::string("root"));

	if (isLeaf (iTargetIter))
	{
		return getLeafName (iTargetIter); 
	}
	else
	{
		assert (2 <= iTargetIter->second.countChildren ());
		MesaTree::id_type theFirstId = iTargetIter->second.getChildId(0);
		MesaTree::id_type theSecondId = iTargetIter->second.getChildId(1);
		
		std::string theName = getFirstTipName (theFirstId);
		theName += "/";
		theName += getFirstTipName (theSecondId);
		
		return theName;
	}			
}

std::string MesaTree::getNodeLabelCaic (iterator& iNodeIter)
//: return the CAIC label of the node pointed to by this iterator
// NOTE: expensive op
{
	if (isRoot (iNodeIter))
	{
		return (std::string(""));
	}
	else
	{
		size_type theChildIndex = getChildIndex (iNodeIter);
		char theChildCode = CaicCode::indexToChar (theChildIndex);
		iterator theParIter = getParent (iNodeIter);
		return getNodeLabelCaic (theParIter) + theChildCode;
	}
}

std::string MesaTree::getNodeLabelSeries (iterator& iNodeIter)
//: return the "series" label of the node pointed to by this iterator
// NOTE: expensive op
// NOTE: varies width of the label as need be
{
	if (isRoot (iNodeIter))
	{
		return (std::string("root"));
	}
	else
	{
		size_type theChildIndex = getChildIndex (iNodeIter);
		stringstream theBuffer; 
		theBuffer << "." << setfill ('0') << setw (3) << theChildIndex;
		iterator theParIter = getParent (iNodeIter);
		return getNodeLabelSeries (theParIter) + theBuffer.str ();
	}
}


iterator MesaTree::getNodeByCaicCode (const char* iCaicStr)
{
	return getNodeByCaicCode (getRoot(), iCaicStr);
}


iterator MesaTree::getNodeByCaicCode
(iterator iStartNode, const char* iCaicStr)
{
	if (iStartNode == end())
		return end();
		
	if (std::string("") == iCaicStr)
		return iStartNode;
		
	size_type theChildIndex = iCaicStr[0] - 'A';
	assert (0 <= theChildIndex);
	if (countChildren (iStartNode) < theChildIndex)
		return end();
	else
		return getNodeByCaicCode (getChild (iStartNode, theChildIndex), iCaicStr+1);
}


// *** TREE NAMING *******************************************************/


std::string MesaTree::getTreeName () const
{
	return mName;
}

void MesaTree::setTreeName (const char* iNameStr)
{
	mName = iNameStr;
}

	
// *** LEAF AGING *******************************************************/


/*
void MesaTree::ageLeaf (iterator iLeafIter, double iAgeIncr)
// CHANGE (01.10.16): eliminated and moved all functionality to
// ageAllLeaves() because a leaf should never be aged by itself. This
// function was removed so as to avoid any accidental use.
{
	assert (isLeaf (iLeafIter));
	double theOldAge = getEdgeWeight (iLeafIter);
	setEdgeWeight (iLeafIter, theOldAge + iAgeIncr);
}
*/

void MesaTree::ageAllLeaves (double iAgeIncr)
// assume you mean living leaves
// CHANGE (01.10.16): eliminated ageLeaf() and moved all functionality to here
{
	for (iterator q = begin (); q != end(); q++)
	{
		if (isNodeAlive (q))
		{
			// ageLeaf (q, iAgeIncr);
				
			assert (isLeaf (q));
			double theOldAge = getEdgeWeight (q);
			setEdgeWeight (q, theOldAge + iAgeIncr);
		}
	}
}

void MesaTree::makeDead (iterator iDeadNode)
//: add node to dead list
{
	mDeadList.insert (iDeadNode->first);
}

void MesaTree::makeInternalsDead ()
//: make all internal nodes dead
{
	for (iterator q = begin(); q != end(); q++)
	{
		if (isInternal (q))
			makeDead (q);
	}
}


/**
Adds a node (or nodes) to the tree according a CAIC code.

This lets us build a tree from CAIC codes. Given a code, this function
constructs (if need be) the path of nodes that leads to that code.

@param    iNewCode  The code of the node to be created.
@return   An iterator to the node that has just been constructed.
*/
nodeiter_t MesaTree::addNode (CaicCode iNewCode)
{
	if (isEmpty())
		insertRoot();
	
	// DBG_MSG (countNodes());

	return addNodeHelper (iNewCode, getRoot());
}


nodeiter_t MesaTree::addNodeHelper (CaicCode iNewCode, nodeiter_t iParentIt)
{
	// DBG_MSG (iNewCode.str() << ":" << countNodes());
	
	if (iNewCode == "")
		return iParentIt;
	else
	{
		// work out how many children there are supposed to be here
		char theNewTaxaChar = iNewCode.popFront();
		uint  theNewTaxaIndex = CaicCode::charToIndex (theNewTaxaChar);
		uint  theNumImpliedNodes = theNewTaxaIndex + 1;
		// create the extra ones if need be
		while (countChildren (iParentIt) < theNumImpliedNodes)
		{
			insertChild (iParentIt);
		} 
		// descend into the last one
		iterator theNewTaxaIt = getChild (iParentIt, theNewTaxaIndex);
		return addNodeHelper (iNewCode, theNewTaxaIt);
	}
}



// *** MISC *************************************************************/


size_type  MesaTree::getDistance (iterator iChildIter, iterator iParIter)
//: how many time do you have to step up to find the parent?
// better to say ancestor or something
{
	size_type theDist = 0;
	iterator	theRootIter = getRoot();
	while ((iChildIter != iParIter) and (iChildIter != theRootIter))
	{
		iChildIter = getParent (iChildIter);
		theDist++;
	}
	assert (iChildIter != theRootIter); // TO DO should throw
	return theDist;
}


void  MesaTree::speciate
(iterator iSplitIter, iterator& oChildIter1, iterator& oChildIter2)
{
	id_type /*theChildId1, theChildId2,*/ theParId;
	theParId = iSplitIter->first;
	oChildIter1 = insertChild (iSplitIter);
	oChildIter2 = insertChild (iSplitIter);
	setEdgeWeight (oChildIter1, 0.0);
	setEdgeWeight (oChildIter2, 0.0);
	makeDead (iSplitIter);
}


void  MesaTree::killLeaf (iterator& ioLeafIter)
//: make a leaf extinct
/*
This issue here is that the preferences may be set to preserve the root
or the root and its children. Therefore we have to detect these.
*/
{
	// Preconditions:
	assert (isLeaf (ioLeafIter));
	assert (isNodeAlive (ioLeafIter));
	
	// Main:
	switch (MesaGlobals::mPrefs.mPreserveNodes)
	{
		case kPrefPreserveNodes_Root:
			// if root, preserve it
			if (ioLeafIter == getRoot())
				return;
			break;

		case kPrefPreserveNodes_RootChildren:
			// if root or children, preserve it
			if (ioLeafIter == getRoot())
				return;
			if (getParent (ioLeafIter) == getRoot())
				return;
			break;

		case kPrefPreserveNodes_None:
			// nothing extra needs to be done
			break;
			
		default:
			// should never get here
			assert (false);
	}

	// add deceased to the roster of dead
	makeDead (ioLeafIter);
		
	// Postconditions:
	// invalidate iterator, just to be sure
	ioLeafIter = end();
}


/*
void MesaTree::collapseSingleton (iterator& ioNodeIt)
//: take a node that only has one child and delete it from the tree
// so that the child takes it's place.
{
	// Preconditions:
	assert (countChildren (ioNodeIt) == 1);
	
	// Main:
	iterator theChildIt = getChild (ioNodeIt, 0);
	weight_type theNewWeight = getEdgeWeight (ioNodeIt)
		+ getEdgeWeight (theChildIt);
	
	
	
	// Postconditions:
	ioNodeIt = end();
	validate ();
}
*/

void MesaTree::collapseNode (iterator ioNodeIt)
//: move an "only-child" node so it links to its (former) grandparent
/*
This is intended for phylogenies in which we literally don't see the dead,
we only see extant taxa. For example in ((A, B), C), if A dies, we want
the tree to collapse to (B, C). The parent of (A, B) ceases to exist: we
have no evidence of their existence. I'm uncertain whether we can cause
a cascades of collapses - one collapse leading to another cause another.
But anyway, we don't handle that. Note that under this scheme, distances
are additive.
*/
{
	// Preconditions:
	assert (not isRoot (ioNodeIt));
	assert (isOnlyChild (ioNodeIt));
	
	// Main:
	// this works regardless of whether the parent is root or not
	iterator theParentIt = getParent (ioNodeIt);
	weight_type theParentWt = getEdgeWeight (theParentIt);
	weight_type theChildWt = getEdgeWeight (ioNodeIt);
	replace (theParentIt, ioNodeIt); 
	setEdgeWeight (ioNodeIt, theParentWt + theChildWt);
	
	// Postconditions:
	if (not isRoot (ioNodeIt))
		assert (getParent (ioNodeIt) != theParentIt);
}


void MesaTree::collapseBranch (iterator ioNodeIt)
//: collapse as far back as you can
/*
This will collapse a node all the way back to the point where it isn't an
only child. For example, ((((A))), B) will collapse down to (A, B). We
recursively call collapseNode() until the node is no longer an only child,
or is the root.
*/
{
	// Preconditions:
	assert (not isRoot (ioNodeIt));
	assert (isOnlyChild (ioNodeIt));
	
	// Main:
	collapseNode (ioNodeIt);
	if (isRoot (ioNodeIt) or (not isOnlyChild (ioNodeIt)))
		return;
	else
		collapseBranch (ioNodeIt);
		
	// Postconditions:
	validate ();
}


void MesaTree::moveSubtree (iterator& ioSubtree, iterator& ioNewParent)
//: detach child from old parent and shift it to new parent
// Both nodes must, obviously, be valid. The moving subtree cannot be the
// root of the whole tree, nor can the new parent be within the subtree.
{
	// Preconditions: 
	assert (ioSubtree != end ());
	assert (ioNewParent != end ());
	assert (ioSubtree != getRoot ());
	assert (not isAncestorOf (ioSubtree, ioNewParent));
	
	// Main:
	deleteParentEdge (ioSubtree); // remove subtree from old parent
	newEdge (ioNewParent, ioSubtree); // add as child of new parent

	// Postconditions:
	assert (isParentOf (ioNewParent, ioSubtree));
}


// *** I/O ***************************************************************/


std::string	MesaTree::writeNewick (TranslationTable* iTranslatorP)
//: return a newick format representation of the tree
// if a translation table (mapping of names to numbers) is passed
// names are substituted for in the format
// CHANGE: if no node has a weight then no distance is printed.
{
	bool theDistance = false;
	for (iterator q = begin(); (q != end()) and (not theDistance); q++)
	{
		if (getEdgeWeight (q) != 0.0)
			theDistance = true;
	}
	
	iterator theRoot = getRoot();
	std::stringstream	theBuffer;
	writeNewickNode (theRoot->first, theBuffer, iTranslatorP, theDistance);
	return theBuffer.str ();
}

void MesaTree::writeNewickNode
(id_type iNodeId, std::stringstream& ioRep, TranslationTable* iTranslatorP, bool iDoDistances)
//: return a newick format representation of the given node
{
	// Preconditions & data prep:
	iterator q = findNode (iNodeId);
	assert (q != end());
	bool theNamesAreTranslated = (iTranslatorP == NULL);
	
	// Main:
	if (q->second.isLeaf())
	{
		// terminal, print name
		if (theNamesAreTranslated)
			ioRep << getLeafName (q);
		else
			ioRep << (*iTranslatorP)[getLeafName(q)];
	}
	else
	{
		// multiple node
		// open bracket
		ioRep << "(";

		// do child reps
		iterator theParent = getIter (iNodeId);
		size_type theNumChildren = countChildren (theParent);
		for (size_type i = 0; i < theNumChildren; i++)
		{
			iterator theChild = getChild (theParent, i);
			writeNewickNode (getId (theChild), ioRep, iTranslatorP, iDoDistances);
			if (i != (theNumChildren - 1))
				ioRep << ",";			
		}
		
		// close bracket
		ioRep << ")";
	}
	
	// do distance:
	if (iDoDistances)
		ioRep << ":" << getEdgeWeight(q);
}


// *** DEPRECATED & DEBUG ***********************************************/


void MesaTree::dump ()
{
#if SBL_DBG
	DBG_MSG ("MesaTree at " << (this) << " has contents: ");
	DBG_MSG ("Raw node collection:");				
	for (iterator p = begin(); p != end(); p++)
	{
		DBG_PREFIX << "Id " << p->first << "; ";
		DBG_PREFIX << "Parent " << p->second.mParent << "; ";
		size_type theNumChildren = countChildren (p);
		if (0 < theNumChildren)
		{
			DBG_STREAM << theNumChildren << " children:";
			for (size_type q = 0; q < theNumChildren; q++)
			{
				iterator theChild = getChild (p, q);
				DBG_STREAM << " " << theChild->first;
			}
		}
		else
		{
			DBG_STREAM << " name: " << p->second.mData.mName;
		}
		DBG_STREAM << std::endl;
	}
#endif // SBL_DBG
}


void MesaTree::setNodeName (id_type theNewId, std::string iName)
{
	iterator q = findNode (theNewId);
	assert (q != end());
	q->second.mData.mName = iName;
}

std::string MesaTree::getNodeLabel (MesaTree::id_type iTargetId)
//: return the name of the node with this id
{
	iterator theNodeIter = findNode (iTargetId);
	return getNodeLabelPhylo (theNodeIter); 
}

std::string MesaTree::getNodeNamebyId (MesaTree::id_type iTargetId)
	//: return the name of the node with this id
{
	iterator theNodeIter = findNode (iTargetId);
	
	// if there is no node with this id, return none
	if (theNodeIter == end())
		return std::string(""); 
	else
		return getNodeLabelPhylo (theNodeIter);			
}

void MesaTree::collectLeaveIds (id_type iTargetId, nodeidvec_t& iResultVec)
{
	iterator theNode = getIter (iTargetId);
	size_type theNumChildren = countChildren (theNode); 
	
	if (theNumChildren == 0)
	{
		// this is a leaf
		iResultVec.push_back (iTargetId);
	}
	else
	{
		for (size_type i = 0; i < theNumChildren; i++)
		{
			iterator theChild = getChild (theNode, i);
			collectLeaveIds (theChild->first, iResultVec);
		}
	}
}

MesaTree::id_type MesaTree::getLeafIdbyName (std::string& iName)
//: return the ID of the leaf with this name
{
	iterator q;
	for (q = begin(); q != end(); q++)
	{
		if (isLeaf (q) and (getLeafName (q) == iName))
			break;
	}
	
	if (q == end())
		return kTree_IdNone;
	else
		return q->first;
}

MesaTree::iterator MesaTree::getIter (const char* iNameStr)
{
	for (iterator q = begin(); q != end(); q++)
	{
		if (getNodeName (q) == iNameStr)
			return q;
	}
	assert (false);
	return end();
}


const char* MesaTree::getFirstTipName (MesaTree::id_type iTargetId)
//: traverse leftwise down the tree to the first tip and return it's name
{
	iterator theNode = findNode (iTargetId);
	
	if (isLeaf (theNode))
	{
		return (getLeafName (theNode)).c_str(); 
	}
	else
	{
		iterator theChild = getChild (theNode, 0);
		return getFirstTipName (theChild->first);
	}			
}


// *** END ***************************************************************/

