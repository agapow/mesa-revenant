/**************************************************************************
Analysis.h - an action that calculates some result and returns it

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef MSA_ANALYSIS_H
#define MSA_ANALYSIS_H


// *** INCLUDES

#include "Action.h"
#include "MesaTypes.h"
#include "MesaTree.h"
#include "CharComparator.h"
#include <vector>
#include <algorithm>
#include "StringUtils.h"


// *** CONSTANTS & DEFINES

// *** BASIC ANALYSIS ****************************************************/

class BasicAnalysis: public BasicAction
//: an action that runs some calculation over the data
{
public:
	// LIFECYCLE
	BasicAnalysis ()
		{}

	virtual ~BasicAnalysis ()
		{}

	// I/O
	const char*           describe (size_type iIndex);
	virtual const char*   describeAnalysis () = 0;
	size_type             deepSize ();
	void                  deleteElement (size_type iIndex);
	size_type             getDepth (size_type iIndex);

	// DEPRECIATED & DEBUG
	void	validate	() {}
};


// *** COUNT TAXA ********************************************************/

class CountExtantTaxaAnalysis: public BasicAnalysis
//: calculate the number of extant taxa in active tree
{
public:
	// LIFECYCLE
	CountExtantTaxaAnalysis ()
		: mRichCol (kColIndex_None)
		{}
	CountExtantTaxaAnalysis (int iRichIndex)
		: mRichCol (iRichIndex)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	// INTERNALS
private:
	int	mRichCol;
};


class CountAllTaxaAnalysis: public BasicAnalysis
//: calculate the total number of taxa, dead or alive, in active tree
{
public:
	// LIFECYCLE
	CountAllTaxaAnalysis ()
		: mRichCol (kColIndex_None)
		{}
	CountAllTaxaAnalysis (int iRichIndex)
		: mRichCol (iRichIndex)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	// INTERNALS
private:
	int	mRichCol;
};


// *** DIVERSITY *******************************************************/


class TreeInfoAnalysis: public BasicAnalysis
//: calculate the phylogenetic age of the active tree
{
public:
	// LIFECYCLE
	TreeInfoAnalysis (bool iCountNodes, bool iCountTips,
		bool iCountAlive, bool iCalcPaleo, bool iCalcAge)
		: mCountNodes (iCountNodes)
		, mCountTips (iCountTips)
		, mCountAlive (iCountAlive)
		, mCalcPaleo (iCalcPaleo)
		, mCalcAge (iCalcAge)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	bool mCountNodes, mCountTips, mCountAlive, mCalcPaleo, mCalcAge;
};


class NodeInfoAnalysis: public BasicAnalysis
//: calculate the information for all nodes in the tree
{
public:
	// LIFECYCLE
	NodeInfoAnalysis (nodetype_t iTargetNodetype, bool iCalcAges,
		bool iCalcTimeToParent, bool iCalcChildren, bool iCalcLeaves,
		bool iCalcSubtree, bool iCalcSiblings, bool iCalcHeight,
		bool iCalcTimeToRoot)
		: mTargetNodetype (iTargetNodetype)
		, mCalcAges (iCalcAges)
		, mCalcTimeToParent (iCalcTimeToParent)
		, mCalcChildren (iCalcChildren)
		, mCalcLeaves (iCalcLeaves)
		, mCalcSubtree (iCalcSubtree)
		, mCalcSiblings (iCalcSiblings)
		, mCalcHeight (iCalcHeight)
		, mCalcTimeToRoot (iCalcTimeToRoot)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	nodetype_t mTargetNodetype;
	bool mCalcAges, mCalcTimeToParent, mCalcChildren, mCalcLeaves,
		theSubtreeSz, mCalcSubtree, mCalcSiblings, mCalcHeight, mCalcTimeToRoot;
};

class NodeSelector
{
public:
	virtual void selectNodes (MesaTree* iTargetTree, nodearr_t& oSelectedNodes)
	{
		oSelectedNodes.clear();
		for (nodeiter_t p = iTargetTree->begin(); p != iTargetTree->end(); p++)
		{
			if (isNodeSelected (iTargetTree, p))
				oSelectedNodes.push_back (p);
		}
	}

	virtual ~NodeSelector ()
	{
	}

	virtual bool isNodeSelected (MesaTree* iTargetTree, nodeiter_t& iCurrNode)
	{
		MesaTree* theDummyP = iTargetTree;     // just to shut compiler up

		nodeiter_t theDummyIter = iCurrNode;   // just to shut compiler up
		assert (false);   // should never be called
		theDummyP = NULL;
		theDummyIter = theDummyIter;
		return true;
	}
};

class NodeSelectorAll: public NodeSelector
{
public:
	virtual bool isNodeSelected (MesaTree* iTargetTree, nodeiter_t& iCurrNode)
	{
		MesaTree* theDummyP = iTargetTree;     // just to shut compiler up
		theDummyP = theDummyP;
		nodeiter_t theDummyIter = iCurrNode;   // just to shut compiler up
		return true;
	}
};

class NodeSelectorTips: public NodeSelector
{
public:
	virtual bool isNodeSelected (MesaTree* iTargetTree, nodeiter_t& iCurrNode)
	{
		if (iTargetTree->isLeaf (iCurrNode))
			return true;
		else
			return false;
	}
};

class NodeSelectorAlive: public NodeSelector
{
public:
	virtual bool isNodeSelected (MesaTree* iTargetTree, nodeiter_t& iCurrNode)
	{
		if (iTargetTree->isNodeAlive (iCurrNode))
			return true;
		else
			return false;
	}
};

class NodeSelectorAbsNumTips: public NodeSelector
{
public:
	NodeSelectorAbsNumTips (int iTipsSelectedCount)
		: mTipsSelectedCount (iTipsSelectedCount)
	{}

	virtual void selectNodes (MesaTree* iTargetTree, nodearr_t& oSelectedNodes)
	{
		iTargetTree->getLiveLeaves (oSelectedNodes);
		std::random_shuffle (oSelectedNodes.begin(), oSelectedNodes.end());
		sbl::shuffle (oSelectedNodes.begin(), oSelectedNodes.end());
		oSelectedNodes.resize (mTipsSelectedCount);
	}

	int mTipsSelectedCount;
};

class NodeSelectorTipByTrait: public NodeSelector
{
public:
	NodeSelectorTipByTrait (CharComparator iTraitTest)
		: mTraitTest (iTraitTest)
	{}

	virtual bool isNodeSelected (MesaTree* iTargetTree, nodeiter_t& iCurrNode)
	{
		if (iTargetTree->isLeaf (iCurrNode) and mTraitTest.testCharacter (iCurrNode))
			return true;
		else
			return false;
	}

	CharComparator mTraitTest;
};

class XNodeInfoAnalysis: public BasicAnalysis
//: calculate the information over selected nodes
{
public:
	// LIFECYCLE
	XNodeInfoAnalysis (NodeSelector* iNodeSelectorP, bool iSelectionCount, bool iCalcAges,
		bool iCalcTimeToParent, bool iCalcChildren, bool iCalcLeaves,
		bool iCalcSubtree, bool iCalcSiblings, bool iCalcHeight,
		bool iCalcTimeToRoot)
		: mNodeSelectorP (iNodeSelectorP)
		, mSelectionCount (iSelectionCount)
		, mCalcAges (iCalcAges)
		, mCalcTimeToParent (iCalcTimeToParent)
		, mCalcChildren (iCalcChildren)
		, mCalcLeaves (iCalcLeaves)
		, mCalcSubtree (iCalcSubtree)
		, mCalcSiblings (iCalcSiblings)
		, mCalcHeight (iCalcHeight)
		, mCalcTimeToRoot (iCalcTimeToRoot)
		{}

	virtual ~XNodeInfoAnalysis ()
	{
		delete mNodeSelectorP;
	}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	NodeSelector* mNodeSelectorP;
	bool mSelectionCount, mCalcAges, mCalcTimeToParent, mCalcChildren, mCalcLeaves,
		theSubtreeSz, mCalcSubtree, mCalcSiblings, mCalcHeight, mCalcTimeToRoot;
};


/*
class ListNodesAnalysis: public BasicAnalysis
//: list nodes in the tree
{
public:
	// LIFECYCLE
	ListNodesAnalysis ()
		: BasicAnalysis ("list_nodes")
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};
*/



// *** DIVERSITY *******************************************************/

class GeneticDiversityAnalysis: public BasicAnalysis
//: calculate the genetic diversity of the active tree
{
public:
	// LIFECYCLE
	GeneticDiversityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class PhyloDiversityAnalysis: public BasicAnalysis
//: calculate the phylogenetic diversity of the active tree
{
public:
	// LIFECYCLE
	PhyloDiversityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class JackknifeGeneticDivAnalysis: public BasicAnalysis
//: jackknife the genetic diversity of the active tree
{
public:
	// LIFECYCLE
	JackknifeGeneticDivAnalysis ()
	{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class JackknifePhyloDivAnalysis: public BasicAnalysis
//: jackknife the phylogenetic diversity of the active tree
{
public:
	// LIFECYCLE
	JackknifePhyloDivAnalysis ()
	{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class BootstrapGeneticDivAnalysis: public BasicAnalysis
//: bootstrap the genetic diversity of the active tree
{
public:
	// LIFECYCLE
	BootstrapGeneticDivAnalysis (int iNumReps, int iNumSamples)
		: mNumReps (iNumReps), mNumSamples (iNumSamples)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	long   mNumReps;
	long   mNumSamples;
};


class BootstrapPhyloDivAnalysis: public BasicAnalysis
//: bootstrap the phylogenetic diversity of the active tree
{
public:
	// LIFECYCLE
	BootstrapPhyloDivAnalysis (int iNumReps, int iNumSamples)
		: mNumReps (iNumReps), mNumSamples (iNumSamples)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	long   mNumReps;
	long   mNumSamples;
};



class SimpsonDiversityAnalysis: public BasicAnalysis
{
public:
	// LIFECYCLE
	SimpsonDiversityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class ShannonWeinerDiversityAnalysis: public BasicAnalysis
{
public:
	// LIFECYCLE
	ShannonWeinerDiversityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class BrillouinDiversityAnalysis: public BasicAnalysis
{
public:
	// LIFECYCLE
	BrillouinDiversityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class PieDiversityAnalysis: public BasicAnalysis
{
public:
	// LIFECYCLE
	PieDiversityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class MacintoshDiversityAnalysis: public BasicAnalysis
{
public:
	// LIFECYCLE
	MacintoshDiversityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class MargelefDiversityAnalysis: public BasicAnalysis
{
public:
	// LIFECYCLE
	MargelefDiversityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class MenhinickDiversityAnalysis: public BasicAnalysis
{
public:
	// LIFECYCLE
	MenhinickDiversityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

// *** BALANCE *********************************************************/

class RichnessAnalysis: public BasicAnalysis
//: a richness analysis across all nodes
{
public:
	RichnessAnalysis (colIndex_t iRichIndex, bool iListSizes)
		: mRichCol (iRichIndex), mListSizes (iListSizes)
		{}

	colIndex_t   mRichCol;
	bool         mListSizes;
};


class FuscoAnalysis: public RichnessAnalysis
//: calculate the Fusco-Cronk imbalance of the tree (root)
{
public:
	// LIFECYCLE
	FuscoAnalysis
		(colIndex_t iRichCol, bool iListSize, bool iExtended, bool iCorrection)
		: RichnessAnalysis (iRichCol, iListSize)
		, mCorrection (iCorrection)
		, mExtended (iExtended)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	bool  mListSize, mCorrection, mExtended;
};


class SlowinskiGuyerAnalysis: public RichnessAnalysis
//: calculate the Slowinski-Guyer test for all nodes
{
public:
	// LIFECYCLE
	SlowinskiGuyerAnalysis (bool iListSizes, colIndex_t iRichIndex)
		: RichnessAnalysis (iRichIndex, iListSizes)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class ShaosNbarAnalysis: public BasicAnalysis
//: calculate Shao & Sokal's N bar imbalance
{
public:
	// LIFECYCLE
	ShaosNbarAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class ShaosSigmaSqAnalysis: public BasicAnalysis
//: calculate Shao & Sokal's sigma squared imbalance
{
public:
	// LIFECYCLE
	ShaosSigmaSqAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class CollessCAnalysis: public BasicAnalysis
//: calculate Colless' C imbalance score
{
public:
	// LIFECYCLE
	CollessCAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class B1Analysis: public BasicAnalysis
//: calculate Shao & Sokal's B1 analysis
{
public:
	// LIFECYCLE
	B1Analysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class B2Analysis: public BasicAnalysis
//: calculate Shao & Sokal's B1 analysis
{
public:
	// LIFECYCLE
	B2Analysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


// *** OTHER TREE SHAPE ANALYSES *****************************************/

class StemminessAnalysis: public BasicAnalysis
//: calculate Stemminess of tree
{
public:
	// LIFECYCLE
	StemminessAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class ResolutionAnalysis: public BasicAnalysis
//: calculate resolution of tree
{
public:
	// LIFECYCLE
	ResolutionAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};

class UltrametricAnalysis: public BasicAnalysis
//: is the tree ultrametric?
{
public:
	// LIFECYCLE
	UltrametricAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


// *** COMPARATIVE ANALYSES **********************************************/

class SiteComplementarityAnalysis: public BasicAnalysis
//: calculate complementarity across sites
{
public:
	// LIFECYCLE
	SiteComplementarityAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


// *** COMPARATIVE ANALYSES **********************************************/

class ComparativeAnalysis: public BasicAnalysis
//: basic comparative analysis
{
public:
	// LIFECYCLE
	ComparativeAnalysis ()
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class CaicAnalysis: public ComparativeAnalysis
{
public:
	// LIFECYCLE
	CaicAnalysis (colIndex_t iPredictorCol, traittype_t iPredictorType,
		std::vector<colIndex_t> iContCols)
		: mPredictorCol (iPredictorCol)
		, mPredictorType (iPredictorType)
		, mContCols (iContCols)
		{}

	// SERVICE
	void execute () {}

	// I/O
	const char* describeAnalysis () { return ""; }

	colIndex_t						mPredictorCol;
	traittype_t						mPredictorType;
	std::vector<colIndex_t>		mContCols;
};


// *** DEPRECIATED & DEBUG ***********************************************/

/*
class FuscoAllAnalysis: public RichnessAnalysis
//: calculate the Fusco imbalance across all feasible taxa/nodes
// Returns a stream of the the Fusco imbalance figures for all applicable
// nodes in the tree.
// TO DO: what if none of the nodes are usuable?
{
public:
	// LIFECYCLE
	FuscoAllAnalysis (bool iListSizes, int iRichIndex)
		: RichnessAnalysis ("calc_fusco_all", iRichIndex, iListSizes)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class FuscoWeightedAnalysis: public BasicAnalysis
//: calculate the weighted Fusco imbalance across all feasible taxa/nodes
// Returns the average of the Fusco imbalance, weighted by the size of the
// nodes it was calculated for.
{
public:
	// LIFECYCLE
	FuscoWeightedAnalysis ()
		: BasicAnalysis ("calc_fusco_weighted")
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();
};


class FuscoExtendedAnalysis: public BasicAnalysis
//: calculate the Fusco imbalance, extended to allow for polytomies
{
public:
	// LIFECYCLE
	FuscoExtendedAnalysis ()
		: BasicAnalysis ("calc_fusco_extended"), mRichCol (kColIndex_None)
		{}
	FuscoExtendedAnalysis (int iRichIndex)
		: BasicAnalysis ("calc_fusco_extended"), mRichCol (iRichIndex)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	// INTERNALS
private:
	int	mRichCol;
};


class FuscoExtendedAllAnalysis: public BasicAnalysis
//: calculate the extended Fusco imbalance across all nodes
{
public:
	// LIFECYCLE
	FuscoExtendedAllAnalysis (bool iListSizes)
		: BasicAnalysis ("calc_fusco_extended_all"), mRichCol (kColIndex_None),
		mListSizes (iListSizes)
		{}
	FuscoExtendedAllAnalysis (bool iListSizes, int iRichIndex)
		: BasicAnalysis ("calc_fusco_extended_all"), mRichCol (iRichIndex),
		mListSizes (iListSizes)
		{}

	// SERVICE
	void execute ();

	// I/O
	const char* describeAnalysis ();

	// INTERNALS
private:
	int	mRichCol;
	bool	mListSizes;
};
*/


// *** UTILITY FUNCTIONS *************************************************/

BasicAnalysis* castAsAnalysis (BasicAction* iActionP);



#endif
// *** END ***************************************************************/



