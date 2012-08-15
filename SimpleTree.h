/**
@file
@author   Paul-Michael Agapow
          Dept. Biology, University College London, WC1E 6BT, UK.
          <mail://p.agapow@ic.ac.uk> 
          <http://www.agapow.net/code/sibil/>
          2003
*/

#pragma once
#ifndef SBL_SIMPLETREE_H    
#define SBL_SIMPLETREE_H    


// *** INCLUDES

#include "Sbl.h"
#include <list>
#include <map>
#include <utility>
#include <iterator>
#include <vector>
#include <algorithm>

SBL_NAMESPACE_START


// *** CONSTANTS & DEFINES

const int     kTree_IdUnknown  = 0;    ///< id of an unknown node
const int     kTree_IdNone     = -1;   ///< id of a null / non-existant node
const double  kTree_DefaultWt  = 0.0;  ///< default edge weight
	
	
// *** DECLARATION 

/**
A user (programmer) friendly tree data-structure.

@param   X   the type of data to be stored in each node of the tree

Not guaranteed to be very fast, but reasonably idiot-proof, this is a
hierachical container. Note that as opposed to other tree forms (e.g AVL
and red-black trees) the value of a datum does not determine its position
in the tree. Rather we are interested in the hierachy represented. This
tree is mostly intended as a basis for phylogenetic trees. 

There is a general problem as regards the accidental use of invalidated
iterators, for example obtaining an iterator to a (then) valid node, 
performing an operation that deletes that node and then trying to access
the now invalid node. This can be an issue with many containers but
because for trees structure _is_ data, manipulations will produce this
problem more often. Therefore mutating operations that invalid their
target iterator will usually take it as a reference and after using it
set it to end(). There is also heavy use of validate() throughout this
template which if left comopiled in will use of invalid iterators in a
timely fashion but result in code slowdown. SBL_DBG should be defined
for this module during development but switched off for release
  
By convention, nothing you can do with the public mutators should be
able to invalid that tree (as long as sensible argument are provided).
In contrast, the private newX() and deleteX() functions are the raw
interface to tree structure and only worry about local consistency
(e.g. the target nodes are connnected correctly) and leave higher
level aspects of the tree (does it have a root, is it fully connected)
temporarily incorrect. For example, deleteSubtree() disconnects a node
from it's parent and remove it and all it's descendants. pruneSubtree()
checks if the subtree starts at the root and if so clears the tree and
sets the root accordingly.

Note how the functions that dump a collection of ids (GetNodeIds etc.)
require an insert/output iterator, which is done like this:
@code
  #include <iterator>
  
  nodeIdCollection  theNodeIds;
  std::back_insert_iterator<nodeIdCollection> theOutputIter (theNodeIds);
  getNodeChildren (theOutputIter);
@endcode

*/
template <typename X = int>
class SimpleTree
{
public:
// PUBLIC TYPE INTERFACE
	typedef double   weight_type;   ///< the branch weights/lengths type
	typedef X        nodedata_type; ///< the type of data stored in nodes
	
protected:
	typedef int      id_type;       ///< the node identifiers type

private:
// PRIVATE TYPE INTERFACE

	/// the internal data structure used to represent topology
	class Node: public std::vector<id_type>
	{
	public:
   /// @name TYPE INTERFACE
   //@{
		typedef std::vector<id_type>   base_type;
		typedef base_type::iterator    iterator;
		typedef base_type::size_type   size_type;   
		
		id_type                        mParent;
		nodedata_type                  mData;
		weight_type                    mWeight;
   //@}
         
   /// @name LIFECYCLE
   //@{
		Node ()
			: mParent (kTree_IdNone)
			///<default ctor.
			{};
		
		Node (id_type iParId)
			: mParent (iParId)
			///<ctor that sets the parent as per the argument.
			{};
   //@}
                  
   /// @name ACCESSORS
   //@{
		inline size_type countChildren () const
			{ return size(); }
		
		bool isLeaf ()
			///<is this node a terminal (i.e has no children)?
			{ return (countChildren() == 0); }

		inline id_type getParentId ()
			{ return mParent; }
			
		inline weight_type getWeight ()
			{ return mWeight; }

		inline nodedata_type* getDataP ()
			{ return &mData; }
   //@}
         
   /// @name MUTATORS
   //@{
		inline void setParentId (id_type iNewId)
			{ mParent = iNewId; }
			
		inline void setWeight (weight_type iNewWt)
			{ mWeight = iNewWt; }

		inline void addChild (id_type iChildId)
			///<Add a child of the given ID to this node
			{ push_back (iChildId); }

		inline void removeChild (id_type iChildId)
			///<Remove a child of the given ID from this node
			{ 
				for (iterator q = begin (); q != end (); q++)
				{
					if (*q == iChildId)
					{
						erase (q);
						return;
					}
				}
			}
   //@}
		
		id_type getChildId (size_type iIndex)
		{
			assert (0 <= iIndex);
			assert (iIndex < size());
			iterator q = base_type::begin();
			std::advance (q, iIndex);
			return *q;
		}
		
   /// @name DEPRECATED & DEBUG
   //@{
		id_type operator [] (size_type iIndex)
			{ return getChildId (iIndex); }   
   //@}
		
	};
   // end of Node declaration
   
	typedef Node                          value_type;
	typedef std::map<id_type,value_type>  nodecontainer_type;
	
	
// MORE PUBLIC TYPE INTERFACE
 public:
	/// type of node index 
	typedef typename nodecontainer_type::size_type        size_type;
	/// iterator over nodes
	typedef typename nodecontainer_type::iterator         iterator;
	/// constant iterator over nodes
	typedef typename nodecontainer_type::const_iterator   const_iterator;
	
	
/// LIFECYCLE
//@{
	SimpleTree ();
//@}

/// ACCESSORS
///@{
	size_type isEmpty () const;
	size_type countNodes () const;
	size_type countNodes (iterator iSubtreeIter);
	size_type sizeOfSubtree (iterator iSubtreeIter);
	size_type countChildren (iterator& iNodeIter);
	size_type countEdges ();
	size_type countEdges (iterator iSubtreeIter);
	size_type countLeaves ();
	size_type countLeaves (iterator iSubtreeIter);
	size_type countInternalNodes ();
	size_type countSiblings (iterator iNodeIter);

	bool   isRoot (iterator& iNodeIter);
	bool   isLeaf (iterator& iNodeIter);
	bool   isInternal (iterator& iNodeIter);
	bool   isOnlyChild (iterator iNodeIter);
	bool   isAncestorOf (iterator iOldIter, iterator iNewIter);
	bool   isDescendantOf (iterator iNewIter, iterator iOldIter);   
	bool   isParentOf (iterator iParent, iterator iChild);
	
	iterator getRoot ();
	iterator getChild (iterator& iNodeIter, size_type iIndex);
	iterator getParent (iterator iNodeIter);
	
	size_type getHeight (iterator iNodeIter);
	weight_type getWeightToParent (iterator iNodeIter);
	weight_type getEdgeWeight (iterator iNodeIter);
///@}
	
	void setEdgeWeight (iterator iNodeIter, weight_type iNewWt);
	void setEdgeWeight (id_type iParId, id_type theNewId, double theDist)
	{
		iterator q = findNode (theNewId);
		assert (q != end());
		assert (q->second.mParent == iParId);
		
		q->second.mWeight = theDist;
	}

	nodedata_type* getNodeDataP (const iterator& iNodeIter) const;


// TRAVERSAL & SEARCH
	iterator            begin ();
	iterator            end ();
	const_iterator      begin () const;
	const_iterator      end () const;


// TREE-BUILDING & MANIPULATION
	iterator   insertRoot (const nodedata_type& iNewData = nodedata_type(),
	              weight_type iNewWeight = kTree_DefaultWt);
	iterator   insertChild (iterator iParentId,
	              const nodedata_type& iNewData = nodedata_type(),
	              weight_type iNewWeight = kTree_DefaultWt);
	iterator   pruneSubtree (iterator& iSubtreeIter);
	iterator   pruneBranch (iterator& iSubtreeIter);
	iterator   pruneLeaf (iterator& iLeafIter);
	void       clear ();
	void       replace (iterator& iOldIter, iterator& iNewIter);

	
/*
	template <class INSERTITER>
	void
	getNodeChildren (iterator iTargetIter, INSERTITER& iOutputIter)
	///<fill a container with the id's of this nodes children
	// TO DO: zap this sucker
	{
		// Preconditions:
		validate (iTargetIter);
		
		// Main:
		Node::iterator theChildBegin = iTargetIter->second.begin();
		Node::iterator theChildEnd = iTargetIter->second.end();
		
		while (theChildBegin != theChildEnd)
		{
			*iOutputIter = *theChildBegin;
			iOutputIter++;
			theChildBegin++;
		}
	}
*/

	template <class INSERTITER>
	void
	getLeafIds (INSERTITER& iOutputIter)
	///<fill a container with the id's of the leaves
	// TO DO: zap this sucker
	{
		size_type theNumLeaves = 0;
		for (const_iterator q = begin(); q != end(); q++)
		{
			if (q->second.isLeaf())
			{
				*iOutputIter = q->first;
				iOutputIter++;
			}
		}
	}

/// DEPRECATED & DEBUG
///@{
	/*
	void pruneNode (id_type iTargetId)
	///<delete this node and all below it
	{
		iterator theNodeIter = findNode (iTargetId);
		(void) pruneSubtree (theNodeIter);
	}

	value_type& operator [] (id_type iId)
		///<get a reference to the node with this id
		// TO DO: depreciate or make internal
		{ return mNodes[iId]; }

	iterator findNode (id_type iTargetId)
		///<return an iterator to a key/node pair
		// TO DO: depreciate or make internal
		{ 
			assertValidIndex (iTargetId);
			return mNodes.find (iTargetId);
		}

	value_type* findNodeP (id_type iTargetId)
	///<return a pointer to a node
		// TO DO: depreciate or make internal
	{
		iterator q = findNode (iTargetId);
		assert (q != end());
		return &(q->second);
	}
	*/
	
	void   validate ();
	void   validate (iterator iTestIter);
	void   assertValidIndex (id_type iIndex)
	{
		assert (0 < iIndex);
		assert (iIndex <= mMaxId);
	}
	bool   isIdPresent (id_type iTargetId);   
	void   dump ();
		
	/*
	size_type countLeaves (id_type iStartNode)
	///<how many leaves from this node?
	{
		// collect the id's of immediate children
		std::vector<id_type>  theNodeIds;
		std::back_insert_iterator< std::vector<id_type> > theOutputIter (theNodeIds);
		getNodeChildren (iStartNode, theOutputIter);
		
		// add the number for each child
		size_type theNumLeaves = 0;
		for (size_type i = 0; i < theNodeIds.size(); i++)
		{
			theNumLeaves += countLeaves (theNodeIds[i]);
		}
		
		// return 
		if (theNumLeaves == 0)
			return 1; // because you must be a leaf
		return theNumLeaves;
	}

	size_type countDescendants (id_type iStartNode)
	///<how many descendants from this node?
	// does not include this node itself
	{
		return  sizeOfSubtree (findNode (iStartNode)) - 1;
	}
	*/
///@}

// INTERNALS

iterator getIter (id_type iId)
{
	return findNode (iId);
}


id_type getId (iterator& iIter)
{
	return iIter->first;
}

iterator getOldestNode ()
{
	if (countNodes() == 0)
		return end();
	else
	{
		for (id_type i = 0; i < mMaxId; i++)
		{
			iterator p = getIter (i);
			if (p != end())
			{
				assert (p == getRoot());
				return p;
			}
		}   
	}
	
	// Postconditions & return:
	assert (false);   // should never get here
	return end();     // just to shut compiler up
}


iterator getNextOldestNode (iterator iCurrIter)
{
	for (id_type i = iCurrIter->first + 1; i <= mMaxId; i++)
	{
		iterator p = getIter (i);
		if (p != end())
		{
			return p;
		}
	}
	return end();
}


	iterator   newNode (const nodedata_type& iNewData,
	              weight_type iNewWeight);
	iterator   deleteNode (iterator& iTargetIter);
	iterator   deleteSubtree (iterator& iSubtreeIter);
	void       setRoot (iterator iNodeIter);
	void       newEdge (iterator iParent, iterator iChild);
	void       deleteEdge (iterator iParent, iterator iChild);
	void       deleteParentEdge (iterator iChild);
	void       deleteChildrenEdges (iterator iParent);
	iterator   findNode (id_type iTargetId)
	{ 
		return mNodes.find (iTargetId);
	}

private:   
	nodecontainer_type   mNodes;  // where nodes are stored
	id_type              mRootId;   // id of the root node
	id_type              mMaxId;   
	id_type              getNextId ()    { return (++mMaxId); }

	
	void         init ();

}; 


// *** DEFINITIONS ********************************************************

// *** LIFECYCLE ***


/// Default ctor.
template <typename X>
SimpleTree<X>::SimpleTree ()
{
	init();
}


// *** ACCESSORS ***

// *** COUNT/SIZE

/// Does the tree contain any nodes?
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::isEmpty () const
{
	return (mNodes.size () == 0);
}


/// How many nodes in the tree?
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::countNodes () const
{
	return mNodes.size ();
}


/**
How many descendant nodes from this node?

Note that this includes this node, so the answer is always at least 1.
*/
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::countNodes (iterator iSubtreeIter)
{
	// Preconditions:
	validate (iSubtreeIter);
	
	// Main:
	size_type theNumDescendants = 1;
	size_type theNumChildren = countChildren (iSubtreeIter);   
	
	// for each child add one and their number of descendants
	for (size_type i = 0; i < theNumChildren; i++)
	{
		iterator theChild = getChild (iSubtreeIter, i);
		theNumDescendants += countDescendants (theChild) + 1;
	}
	
	// Postconditions & return:
	assert (0 <= theNumDescendants);
	return theNumDescendants;
}


/// how many branches in the tree?
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::countEdges ()
{
	// Main:
	size_type theNumBranches = (isEmpty() ? 0 : countNodes() - 1);
	
	// Postconditions & return:
	assert (0 <= theNumBranches);
	return theNumBranches;
}


/// how many branches in the subtree?
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::countEdges (iterator iSubtreeIter)
{
	// Preconditions:
	validate (iSubtreeIter);
	
	// Main:
	size_type theNumBranches = (isEmpty() ? 0 : countNodes(iSubtreeIter) - 1);
	
	// Postconditions & return:
	assert (0 <= theNumBranches);
	return theNumBranches;
}


/// How many leaves/tips in the entire tree?
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::countLeaves ()
{
	// Main:
	size_type theNumLeaves = 0;
	for (iterator q = begin(); q != end(); q++)
	{
		if (isLeaf (q))
			theNumLeaves++;
	}

	// Postconditions & return:
	assert (0 <= theNumLeaves);
	return theNumLeaves;
}


/// Return the number of leaves on this subtree
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::countLeaves (iterator iSubtreeIter)
{
	// Preconditions:
	validate (iSubtreeIter);
	
	// Main:
	int theNumLeaves = 0;
	int theNumChildren = countChildren (iSubtreeIter);
	
	if (theNumChildren == 0)
	{
		theNumLeaves = 1;
	}
	else
	{
		// add up children
		for (int i = 0; i < theNumChildren; i++)
		{
			iterator q = getChild (iSubtreeIter, i);
			theNumLeaves += countLeaves (q);
		}
	}
	
	// Postconditions & return:
	assert (0 <= theNumLeaves);
	return theNumLeaves;
}


/**
Return the number of internal nodes on this subtree

@warning   The root is correctly identified as an internal node.
*/
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::countInternalNodes ()
{
	// Main:
	size_type theNumIntNodes = 0;
	for (iterator q = begin(); q != end(); q++)
	{
		if (isInternal (q))
			theNumIntNodes++;
	}
	return theNumIntNodes;
	
	// Postconditions & return:
	assert (0 <= theNumIntNodes);
	return theNumIntNodes;
}


/// How many immediate children does this node have
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::countChildren (iterator& iNodeIter)
{
	// Main:
	size_type theNumChildren = iNodeIter->second.countChildren();
	
	// Postconditions & return:
	assert (0 <= theNumChildren);
	return theNumChildren;
}


/// How many sisters does this node have?
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::countSiblings (iterator iNodeIter)
{
	// Preconditions:
	validate (iNodeIter);
	
	// Main:
	size_type theNumSiblings;
	if (isRoot (iNodeIter))
	{
		theNumSiblings = 0;
	}
	else
	{
		iterator q = getParent (iNodeIter);
		assert (q != end());
		theNumSiblings = q->second.size() - 1;
	}
	
	// Postconditions & return:
	assert (0 <= theNumSiblings);
	return theNumSiblings;
}


// *** IDENTITY

/// Is the first node the parent of the second?
template <typename X>
bool
SimpleTree<X>::isParentOf (iterator iParent, iterator iChild)
{
	return (iParent == getParent (iChild));
}


/// Is the first node an ancestor (above/higher) than the second?
template <typename X>
bool
SimpleTree<X>::isAncestorOf (iterator iOldIter, iterator iNewIter)
{
	// Preconditions:
	validate (iOldIter);
	validate (iNewIter);
	
	// Main:
	iterator theRoot = getRoot ();   // save calculation later
	if ((iNewIter == theRoot) or (iNewIter == iOldIter))
		return false;                     // handle simple case
	
	// walk up the tree
	while (true)
	{
		iNewIter = getParent (iNewIter); 
		if (iNewIter == iOldIter)
			return true;
		if (iNewIter == getRoot())
			return false;         
	}
	
	assert (false);   // shouldn't get here
	return false;     // just to shut compiler up
}


/**
Is the first node an descendant of (subtended from) the second?

It is necessary have this function as isDescendantOf() is not the simple
complement of isAncestorOf(). (They might be the same node!)
*/
template <typename X>
bool
SimpleTree<X>::isDescendantOf (iterator iNewIter, iterator iOldIter)
{
	if (iNewIter == iOldIter)   // handle simple case
		return false;         
	return (not isAncestorOf (iOldIter, iNewIter));
}


/// Does this node have no sisters?
template <typename X>
bool
SimpleTree<X>::isOnlyChild (iterator iNodeIter)
{
	// Preconditions:
	validate (iNodeIter);
	
	// Main:
	return (countSiblings (iNodeIter) == 0);
}


template <typename X>
bool
SimpleTree<X>::isRoot (iterator& iNodeIter) // TO DO: const
{
	// Preconditions:
	// validate (iNodeIter);
	
	// Main:
	if (iNodeIter->first == mRootId)
		return true;
	else
		return false;
}


template <typename X>
bool
SimpleTree<X>::isLeaf (iterator& iNodeIter)
///<is this node a leaf
{
	// Preconditions:
	validate (iNodeIter);
	
	// Main:
	return iNodeIter->second.isLeaf(); 
}


/**
Is this node internal to the tree?

// CHANGE: (01.6.18) now the root is correctly identified as an internal node
*/
template <typename X>
bool
SimpleTree<X>::isInternal (iterator& iNodeIter)
{
	// Preconditions:
	validate (iNodeIter);
	
	// Main:
	return (not isLeaf (iNodeIter));
}


// *** RELATIONSHIPS

/// Return an iterator to the root node
template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::getRoot ()
{
	return findNode (mRootId);
}


/// Return an iterator to the ith child of this node
template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::getChild (iterator& iNodeIter, size_type iIndex)
{
	// Preconditions:
	validate (iNodeIter);
	assert (0 <= iIndex);
	assert (iIndex < countChildren (iNodeIter));
	
	// Main:      
	id_type theChildId =  iNodeIter->second.getChildId (iIndex);
	iterator q = findNode (theChildId);
	
	// Postconditions & return:
	validate (q);
	return q;
}


template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::getParent (iterator iNodeIter)
{
	// Preconditions:
	validate (iNodeIter);
	
	// Main:
	if (isRoot (iNodeIter))
		return end();
	else
		return findNode (iNodeIter->second.mParent);
}


// *** TOPOLOGY

/**
Return the height of this node.

By convention the height of an empty tree is -1, and height is
otherwise the number of steps to the root.
*/
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::getHeight (iterator iNodeIter)
{
	// Preconditions:
	validate (iNodeIter);
	
	// Main:
	if (isRoot (iNodeIter))
		return 0;
	return (1 + getHeight (getParent (iNodeIter)));
}


/// Return the length/weight of the branch between this node and its parent
template <typename X>
typename SimpleTree<X>::weight_type
SimpleTree<X>::getWeightToParent (iterator iNodeIter)
{
	// Preconditions:
	validate (iNodeIter);
	
	// Main:
	weight_type theWt = getEdgeWeight (iNodeIter)
	
	// Postconditions & return:
	assert (0.0 <= theWt);
	return theWt;
}


template <typename X>
typename SimpleTree<X>::weight_type
SimpleTree<X>::getEdgeWeight (iterator iNodeIter)
///<return the length/weight of the branch between this node and its parent
{
	// Preconditions:
	validate (iNodeIter);
	
	// Main:
	weight_type theWt = iNodeIter->second.getWeight();
	
	// Postconditions & return:
	assert (0.0 <= theWt);
	return theWt;
}


template <typename X>
void
SimpleTree<X>::setEdgeWeight (iterator iNodeIter, weight_type iNewWt)
{
	// Preconditions:
	validate (iNodeIter);
	assert (0.0 <= iNewWt);
	
	// Main:
	iNodeIter->second.setWeight (iNewWt);
}


template <typename X>
typename SimpleTree<X>::nodedata_type*
SimpleTree<X>::getNodeDataP (const iterator& iNodeIter) const
{
	// Preconditions:
	assert (iNodeIter != end());
	
	// Return:
	return &(iNodeIter->mData);
}
		
		
// *** TRAVERSAL & SEARCH

template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::begin ()
{
	return mNodes.begin ();
}


template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::end ()
{
	return mNodes.end ();
}


template <typename X>
typename SimpleTree<X>::const_iterator
SimpleTree<X>::begin () const
{
	return mNodes.begin ();
}


template <typename X>
typename SimpleTree<X>::const_iterator
SimpleTree<X>::end () const
{
	return mNodes.end ();
}


// TREE-BUILDING & MANIPULATION

/// Add a new node to tree at root with this data
template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::insertRoot (const nodedata_type& iNewData, weight_type iNewWeight)
{
	// Preconditions: 
	assert (mRootId == kTree_IdNone);
	assert (isEmpty ());
	
	// Main:
	iterator theNewRoot = newNode (iNewData, iNewWeight);
	mRootId = theNewRoot->first;
	
	// Return:
	return getRoot();
}


/// Add a new node to tree at root with this data
template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::insertChild
(iterator iParentIter, const nodedata_type& iNewData, weight_type iNewWeight)
{
	// Preconditions: 
	assert (getRoot() != end());
	validate (iParentIter);
	assert (not isEmpty ());

	// Main:
	iterator theNewIter = newNode (iNewData, iNewWeight);
	newEdge (iParentIter, theNewIter);
	
	// Return:
	validate (theNewIter);
	return theNewIter;
}


/// Delete this subtree and the branch that leads to it, returning the parent
template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::pruneSubtree (iterator& iSubtreeIter)
{
	// Preconditions: 
	validate (iSubtreeIter);
	
	// Main:
	iterator theParIter;
	if (iSubtreeIter == getRoot())
	{
		clear();
		theParIter = end();
	}
	else
	{
		theParIter = deleteSubtree (iSubtreeIter);
	}
	
	// Postconditions & return:
	iSubtreeIter = end();
	// validate ();
	return theParIter;
}


/// Prune the non-branching path leading to this tip, return origin of branch
template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::pruneBranch (iterator& ioLeafIter)
{
	// Preconditions: 
	assert (isLeaf (ioLeafIter));
	validate (ioLeafIter);
	
	// Main:
	// trek up from the tip until you have a sibling (i.e. you are at
	// the start of the branch) or hit the root.
	for (;;)
	{
		if (isRoot (ioLeafIter))
			break;
	
		if (isOnlyChild (ioLeafIter))
			ioLeafIter = getParent (ioLeafIter);
		else
			break;         
	}
	
	// identify the origin of the branch & cut the branch off
	iterator theStemIter = getParent (ioLeafIter);
	pruneSubtree (ioLeafIter);
	
	// Return origin:
	ioLeafIter = end();
	// validate ();
	return (theStemIter);
}


/**
Delete this leaf and all references to it

Must be a leaf, tree is left in consistent state afterwards
*/
template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::pruneLeaf (iterator& iLeafIter)
{
	// Preconditions:
	assert (isLeaf (iLeafIter));
	
	// Main:
	iterator theParent = getParent (iLeafIter);
	deleteNode (iLeafIter);
	
	// Postconditions & return:
	iLeafIter = end();
	// validate();
	return theParent;
}


/// Empty the tree
template <typename X>
void
SimpleTree<X>::clear ()
{
	mNodes.clear ();
	init ();
}


/**
Replace the old node with the new one.

@todo   note this does not retain the relative order of the replaced node
and it's siblings, the shifted node always becomes the last child of the
old nodes parent.
*/
template <typename X>
void
SimpleTree<X>::replace (iterator& iOldIter, iterator& iNewIter)
{
	// Preconditions:
	// new node does not replace one of its descendants & thus become
	// its own descendant. Also cannot be root.
	validate (iNewIter);
	validate (iOldIter);
	assert (not isAncestorOf (iNewIter, iOldIter));
	assert (iNewIter != getRoot ());
	
	// Main:
	// disconnect moving subtree from tree
	deleteParentEdge (iNewIter);
	iterator theRoot = getRoot ();
	if (iOldIter == theRoot)
	{
		// if it is to become the root, make it so
		setRoot (iNewIter);
		DBG_MSG ("root reset")
	}
	else
	{
		// otherwise give it the right parent
		iterator theNewParent = getParent (iOldIter);
		newEdge (theNewParent, iNewIter);
	}
	// clip out the old crap
	// iterator theOldParent = pruneSubtree (iOldIter);
	deleteNode (iOldIter);
	
	// Postconditions:
	// iNewIter still points to the shifted node
	iOldIter = end();
	validate (iNewIter);
	// validate ();   
}


// *** INTERNALS
// These are the raw building-block methods for tree construction &
// manipulation. They are will maintain tree consistency (correctness)
// locally only - their actions make leave the tree incorrect in a
// global sense (e.g. without a root or disconnected). Hence conditions
// are more sparsely used here.

/// General initialisation function for use in ctor.
template <typename X>
void
SimpleTree<X>::init ()
{
	mMaxId = 0;
	mRootId = kTree_IdNone;
	
	// Postconditions:
	// validate ();
}


/// Make this node the root, don't guarantee any other consistency
template <typename X>
void
SimpleTree<X>::setRoot (iterator iNodeIter)
{
	// Preconditions:
	// validate (iNodeIter);
	
	// Main:
	mRootId = iNodeIter->first;
	iNodeIter->second.mParent = kTree_IdNone;
}


/// Add an edge from a parent to child 
template <typename X>
void
SimpleTree<X>::newEdge (iterator iParent, iterator iChild)
{
	// Preconditions:
	// validate (iParent);
	// validate (iChild);
	
	// Main:
	iParent->second.addChild (iChild->first);
	iChild->second.mParent = iParent->first;
}


/// Remove an edge between a parent to child 
template <typename X>
void
SimpleTree<X>::deleteEdge (iterator iParent, iterator iChild)
{
	// Preconditions:
	// validate (iParent);
	// validate (iChild);
	
	// Main:
	iParent->second.removeChild (iChild->first);
	iChild->second.mParent = kTree_IdNone;
}


/// Remove an edge between a parent to child 
template <typename X>
void
SimpleTree<X>::deleteParentEdge (iterator iChildIter)
{
	// Preconditions:
	// validate (iChildIter);
	
	// Main:
	iterator theParentIter = getParent (iChildIter);
	assert (theParentIter != end());
	deleteEdge (theParentIter, iChildIter);
}


/// Remove edges between a parent and all children
template <typename X>
void
SimpleTree<X>::deleteChildrenEdges (iterator iParent)
{
	// Preconditions:
	assert (iParent != end());
	
	// Main:
	while (0 < countChildren (iParent))
	{
		iterator theChildIter = getChild (iParent, 0);
		deleteEdge (iParent, theChildIter);
	}
}


/// Add a new node to tree with this data and return an iterator to it
template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::newNode (const nodedata_type& iNewData, weight_type iNewWeight)
{
	// Main:
	// allocate ID for new node
	id_type theNewId = getNextId ();
	assert (isIdPresent (theNewId) == false);

	// construct new node
	Node   theNewNode;
	theNewNode.mData = iNewData;
	theNewNode.mWeight = iNewWeight;
	mNodes[theNewId] = theNewNode;
	iterator theNodeIter = findNode (theNewId);
	
	// Postconditions & return:
	// validate (theNodeIter);
	return theNodeIter;
}


/**
Delete just this node and all references to it,  return parent

May therefore leave the tree in an invalid state, ie. unconnected.
*/
template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::deleteNode (iterator& iTargetIter)
{
	// Preconditions:
	// validate (iTargetIter);
	
	// Main:
	iterator theParentIter;
	// remove references in children
	deleteChildrenEdges (iTargetIter);      
	// remove references in parent
	theParentIter = getParent (iTargetIter);
	if (theParentIter != end())
		deleteEdge (theParentIter, iTargetIter);
	// physically delete this node
	mNodes.erase (iTargetIter);
	
	// Postconditions & return:
	iTargetIter = end();
	// can't vaidate because Parent may be end() and tree may be in invalid state
	// validate (theParentIter);
	return theParentIter;
}


template <typename X>
typename SimpleTree<X>::iterator
SimpleTree<X>::deleteSubtree (iterator& iSubtreeIter)
{
	// Preconditions: 
	// validate (iSubtreeIter);
	
	// Main:
	// note that this way is not efficient, but it is simple & robust
	iterator theParIter = getParent (iSubtreeIter);   
	while (0 < countChildren (iSubtreeIter))
	{
		iterator theChild = getChild (iSubtreeIter, 0);
		pruneSubtree (theChild);
	}
	deleteNode (iSubtreeIter);
	
	// Postconditions & return:
	iSubtreeIter = end();
	return theParIter;
}


// *** DEPRECIATED

/// How many descendant nodes from (and including) this node?
template <typename X>
typename SimpleTree<X>::size_type
SimpleTree<X>::sizeOfSubtree (iterator iSubtreeIter)
{
	// Preconditions:
	// validate (iSubtreeIter);
	
	// Main:
	// Main:
	size_type theNumDescendants = 1;
	size_type theNumChildren = countChildren (iSubtreeIter);   
	
	// for each child add one and their number of descendants
	for (size_type i = 0; i < theNumChildren; i++)
	{
		iterator theChild = getChild (iSubtreeIter, i);
		theNumDescendants += sizeOfSubtree (theChild) + 1;
	}
	
	// Postconditions & return:
	assert (0 <= theNumDescendants);
	return theNumDescendants;
}


// *** DEBUG FUNCTIONS

#ifdef SBL_DBG
/**
Check that an iterator points to a valid (existing) node

It would be nice to check connections in this function (do you and
your parent know each other, do you and your children know each other)
but as these function validate their argument iterators, this would
lead to an infinite loop. So this is done in validate tree.
*/
	template <typename X>
	void
	SimpleTree<X>::validate (iterator iTestIter)
	{
		// DBG_MSG ("SimpleTree is validating iter");
		if (iTestIter == end())
		{
			DBG_MSG ("here failing validate");
			assert (iTestIter != end());
		}
		assert (0 < iTestIter->first);
		assert (iTestIter->first <= mMaxId);
		assert (isIdPresent (iTestIter->first));
	}


	template <typename X>
	void
	SimpleTree<X>::validate ()
	{
		DBG_MSG ("SimpleTree is validating tree");
		
		// assert there is a root, or there are no nodes
		assert ((mRootId == kTree_IdNone) or isIdPresent (mRootId));
		assert ((getRoot() != end()) or isEmpty ());
		
		// speed up later calculations
		iterator theRootIter = getRoot ();
		
		// for each node ...
		for (iterator p = begin(); p != end(); p++)
		{
			// validate node/iter
			validate (p);
			
			// do you and your parent know each other
			if (p != getRoot())
			{
				iterator theParent = getParent (p);
				bool theParentKnowsChild = false;
				for (size_type i = 0; (i < countChildren (theParent)) and (not theParentKnowsChild); i++)
				{
					if (p == getChild (theParent, i))
						theParentKnowsChild = true;
				}
				assert (theParentKnowsChild);
			}
			
			// do you and your children know each other
			for (size_type i = 0; i < countChildren (p); i++)
			{
				iterator theChildIter = getChild (p, i);
				assert (p == getParent (theChildIter));
			}
						
			// ... assert the path up from here ends at the root
			iterator theCurrNode = p;
			iterator theCurrParent = getParent (p);
			while (theCurrParent != end())
			{
				theCurrNode = theCurrParent;
				theCurrParent = getParent (theCurrParent);
			
			}         
		}      
	}
			

	template <typename X>
	void
	SimpleTree<X>::dump ()
	{
		DBG_MSG ("SimpleTree at " << (this) << " has contents: ");
		DBG_MSG ("MaxId " << mMaxId << "; Root: " << mRootId);
		DBG_MSG ("Raw node collection:");            
		for (iterator p = begin(); p != end(); p++)
		{
			DBG_PREFIX << "Id " << p->first << "; ";
			DBG_PREFIX << "Parent " << p->second.mParent << "; ";
			size_type theNumChildren = countChildren (p);
			DBG_STREAM << theNumChildren << " children:";
			if (0 < theNumChildren)
			{
				for (size_type i = 0; i < theNumChildren; i++)
				{
					iterator theChild = getChild (p, i);
					DBG_STREAM << " " << theChild->first;
				}
			}
			else
			{
				DBG_STREAM << " -";
			}
			DBG_STREAM << std::endl;
		}
	}

	/**
	Does this node exist / Is it part of the tree?
	
	Mainly for debugging.
	*/
	template <typename X>
	bool
	SimpleTree<X>::isIdPresent (id_type iTargetId)
	{
		if (findNode (iTargetId) == end())
			return false;
		else
			return true;
	}
	
#else
	template <typename X>
	void
	SimpleTree<X>::validate ()
	{}

	template <typename X>
	void
	SimpleTree<X>::validate (iterator iTestIter)
	{}

	template <typename X>
	void
	SimpleTree<X>::dump ()
	{}

	template <typename X>
	bool
	SimpleTree<X>::isIdPresent (id_type iTargetId)
	{ return false; /* just to shut compiler up */ }
#endif

// testing function for SimpleTree
#if 0

	void testSimpleTree ();

	void testSimpleTree ()
	{
		DBG_MSG ("*** Testing SimpleTree ...");   
		
		DBG_MSG ("** Create empty tree");   
		sbl::SimpleTree<int>   theNewTree;
		DBG_MSG ("** is the tree empty? " << theNewTree.isEmpty());   
		DBG_MSG ("** Dump empty tree");   
		DBG_BLOCK (theNewTree.dump());
		DBG_MSG ("** Validating empty tree");   
		DBG_BLOCK (theNewTree.validate());
	
		DBG_MSG ("** Fill tree ((D,(X,Y,Z))),B)");   
		SimpleTree<int>::iterator theFirstNode = theNewTree.insertRoot ();
		SimpleTree<int>::iterator the2ndNode = theNewTree.insertChild (theFirstNode);
		SimpleTree<int>::iterator the3rdNode = theNewTree.insertChild (the2ndNode);
		SimpleTree<int>::iterator the4thNode = theNewTree.insertChild (the2ndNode);
		SimpleTree<int>::iterator the5thNode = theNewTree.insertChild (theFirstNode);
		SimpleTree<int>::iterator the6thNode = theNewTree.insertChild (the4thNode);
		SimpleTree<int>::iterator the7thNode = theNewTree.insertChild (the4thNode);
		SimpleTree<int>::iterator the8thNode = theNewTree.insertChild (the4thNode);
		DBG_MSG ("** the number of nodes in the tree? " << theNewTree.countNodes ());   
		DBG_MSG ("** the number of branches in the tree? " << theNewTree.countEdges ());   
		DBG_MSG ("** the size of the tree? " << theNewTree.countNodes ());
		SimpleTree<int>::iterator theRoot = theNewTree.getRoot();
		DBG_MSG ("** the root of the tree? " << theRoot->first);   
		DBG_MSG ("** Dump tree");   
		DBG_BLOCK (theNewTree.dump());
		DBG_MSG ("** Validating tree");   
		DBG_BLOCK (theNewTree.validate());
		
		/*
		DBG_MSG ("** Pruning the 2nd node ...");   
		theNewTree.pruneSubtree (the2ndNode);
		DBG_MSG ("** the number of nodes in the tree? " << theNewTree.countNodes ());
		*/
		theNewTree.replace (theFirstNode, the4thNode);   
		DBG_MSG ("** Dump tree");   
		DBG_BLOCK (theNewTree.dump());
		DBG_MSG ("** Validating tree");   
		DBG_BLOCK (theNewTree.validate());

		DBG_MSG ("** Pruning the root ...");   
		theNewTree.pruneSubtree (the7thNode);
		DBG_MSG ("** the number of nodes in the tree? " << theNewTree.countNodes ());   
		DBG_MSG ("** Dump tree");   
		DBG_BLOCK (theNewTree.dump());
		DBG_MSG ("** Validating tree");   
		DBG_BLOCK (theNewTree.validate());

		DBG_MSG ("*** Finished testing SimpleTree.");   
	}
#endif

SBL_NAMESPACE_STOP

#endif
// *** END ***************************************************************/

