/**************************************************************************
Analysis.cpp - an action that calculates some result and returns it

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

**************************************************************************/


// *** INCLUDES

#include "Sbl.h"
#include "Numerics.h"
#include "Analysis.h"
#include "ActionUtils.h"
#include "TreeWrangler.h"
#include "MesaGlobals.h"
#include "TaxaTraitMatrix.h"
#include "Reporter.h"
#include "ReporterPrefix.h"
#include "ExecutionError.h"
#include "StringUtils.h"
#include "ManipAction.h"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

using std::pow;
using std::string;
using std::stringstream;
using std::map;
using std::swap;
using std::vector;
using std::ceil;
using sbl::toString;
using std::max_element;
using std::min_element;
using std::max;
using std::abs;
using std::sqrt;


// *** CONSTANTS & DEFINES

typedef BasicAnalysis::size_type   size_type;


// *** UTILITY FXNS ******************************************************/

template <typename ITER>
void calcStdDev (ITER iStart, ITER iStop, double& oMean, double& oStdDev)
{
	// calculate the mean and collect numbers
	vector<double>	theDeviations;
	oMean = 0.0;

	for (ITER q = iStart; q != iStop; q++)
	{
		oMean += double (*q);
		theDeviations.push_back (double (*q));
	}

	oMean = oMean / double (theDeviations.size());

	// convert numbers to sq deviations
	for (long i = 0; i < theDeviations.size(); i++)
	{
		theDeviations[i] -= oMean;
		theDeviations[i] *= theDeviations[i];
	}

	// produce stddev from sqrt of sum of dev / size - 1
	double theSumDev = 0.0;
	for (long i = 0; i < theDeviations.size(); i++)
	{
		theSumDev += theDeviations[i];
	}
	oStdDev = theSumDev / (theDeviations.size() - 1);
	oStdDev = pow(oStdDev, .5);
}



// *** BASIC ANALYSIS ****************************************************/

const char* BasicAnalysis::describe (size_type iIndex)
//: return a description of the analyse
{
	// Preconditions:
	assert (iIndex == 0);
	iIndex = iIndex; // just to shut compiler up

	// Main:
	static string theBuffer;
	theBuffer = "analyse: ";
	theBuffer += describeAnalysis ();
	return theBuffer.c_str();
}

size_type BasicAnalysis::deepSize ()
{
	return 1;
}

void BasicAnalysis::deleteElement (size_type iIndex)
{
	assert (false);
	iIndex = iIndex; // to shut compiler up
}

size_type BasicAnalysis::getDepth (size_type iIndex)
{
	assert (iIndex == 0);
	return 0;
}


// *** AGE *************************************************************/

void TreeInfoAnalysis::execute ()
//: calculate the age of the tree
// NOTE: we make no guarantees if the tree isn't ultrametric
// NOTE: does not include the root / outgroup distance
{
	ReporterPrefix	thePrefix ("tree information");

	// dumbass checks
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();
	assert (theTreeP != NULL);
	if (theTreeP->isEmpty())
	{
		MesaGlobals::mReporterP->printNotApplicable ("tree is empty");
		return;
	}


	// print out results
	if (mCountNodes)
		MesaGlobals::mReporterP->print (long (theTreeP->countNodes()), "total size");

	if (mCountTips)
		MesaGlobals::mReporterP->print (long (theTreeP->countLeaves()), "terminal taxa");

	if (mCountAlive)
		MesaGlobals::mReporterP->print (long (theTreeP->countAliveLeaves()), "extant taxa");

	/**
	@change   Previously tree age was calculated as the distance from the
	   first living tip (effectively a random one) to the root, plus the
	   distance on the root. I now realise the error of my ways (what do we
	   do with imported trees that have no living / dead status) and so we
	   now calculate it the slower but better way - the longest distance
	   from any tip to the root. Root distance is not included.
	*/
	if (mCalcAge)
		// MesaGlobals::mReporterP->print (theTreeP->getTreeAge(), "phylogenetic age");
		MesaGlobals::mReporterP->print (theTreeP->getPhyloAge(), "phylogenetic age");

	if (mCalcPaleo)
	{
		if (theTreeP->isTreePaleo())
			MesaGlobals::mReporterP->print ("paleontological", "paleo/neo");
		else if (theTreeP->isTreeNeo())
			MesaGlobals::mReporterP->print ("neontological", "paleo/neo");
		else
			MesaGlobals::mReporterP->print ("other", "paleo/neo");
	}
}


const char* TreeInfoAnalysis::describeAnalysis ()
{
	return "tree information";
}


void NodeInfoAnalysis::execute ()
//: calculate the age of every node
// TO DO: cache lengths to save time
{
	ReporterPrefix	thePrefix ("node information");

	// dumbass checks
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();
	assert (theTreeP != NULL);
	if (theTreeP->isEmpty() or
		((mTargetNodetype == kNodetype_Internal) and (theTreeP->countInternalNodes() == 0)))
	{
		MesaGlobals::mReporterP->printNotApplicable ("tree is too small");
		return;
	}

	stringvec_t		theLabels;
	vector<double>	theAges, theTimesToParent, theTimesToRoot;
	vector<int>		theChildrenCnt, theLeaveCnt, theSubtreeSz, theSiblingCnt, theHeights;

	// for every node

	// for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	for (nodeiter_t q = theTreeP->getOldestNode(); q != theTreeP->end(); q = theTreeP->getNextOldestNode (q))
	{
		// is this one of the nodes we're looking at?
		bool theNodeIsTarget = false;
		switch (mTargetNodetype)
		{
			case kNodetype_All:
				theNodeIsTarget = true;
				break;
			case kNodetype_Tips:
				if (theTreeP->isLeaf(q))
					theNodeIsTarget = true;
				break;
			case kNodetype_Internal:
				if (theTreeP->isInternal(q))
					theNodeIsTarget = true;
				break;
			default:
				assert (false);
		}
		if (theNodeIsTarget == false)
			continue;

		// do analysis
		theLabels.push_back (getNodeLabel (q));
		if (mCalcAges)
			theAges.push_back (theTreeP->getTimeSinceNodeOrigin (q));
		// if (mCalcTimeToParent)
			theTimesToParent.push_back (theTreeP->getEdgeWeight (q));
		if (mCalcChildren)
			theChildrenCnt.push_back (theTreeP->countChildren (q));
		if (mCalcLeaves)
			theLeaveCnt.push_back (theTreeP->countLeaves (q));
		if (mCalcSubtree)
			theSubtreeSz.push_back (theTreeP->sizeOfSubtree (q));
		if (mCalcSiblings)
			theSiblingCnt.push_back (theTreeP->countSiblings (q));
		if (mCalcHeight)
			theHeights.push_back (theTreeP->getHeight (q));
		if (mCalcTimeToRoot)
			theTimesToRoot.push_back (theTreeP->getTimeFromNodeToRoot (q));
	}

	// print out results
	bool theAgesInTree = true;
	vector<double>::iterator s = max_element (theTimesToParent.begin(), theTimesToParent.end());
	if (*s == 0.0)
		theAgesInTree = false;

	if (mCalcAges)
	{
		if (theAgesInTree)
			MesaGlobals::mReporterP->print (theAges, "age of nodes");
		else
			MesaGlobals::mReporterP->printNotApplicable ("no distances in tree", "age of nodes");
	}

	if (mCalcTimeToParent)
	{
		if (theAgesInTree)
			MesaGlobals::mReporterP->print (theTimesToParent, "time since parent");
		else
			MesaGlobals::mReporterP->printNotApplicable ("no distances in tree", "time since parent");
	}

	if (mCalcChildren)
		MesaGlobals::mReporterP->print (theChildrenCnt, "number of children");

	if (mCalcLeaves)
		MesaGlobals::mReporterP->print (theLeaveCnt, "number of leaves");

	if (mCalcSubtree)
		MesaGlobals::mReporterP->print (theSubtreeSz, "number of nodes");

	if (mCalcSiblings)
		MesaGlobals::mReporterP->print (theSiblingCnt, "number of siblings");

	if (mCalcHeight)
		MesaGlobals::mReporterP->print (theHeights, "height of node");

	if (mCalcTimeToRoot)
		MesaGlobals::mReporterP->print (theTimesToRoot, "time to root");

	// dumbass check on answer
	MesaGlobals::mReporterP->print (theLabels, "node label");
}


const char*  NodeInfoAnalysis::describeAnalysis ()
// TO DO: make this better
{
	return "information over nodes";
}

void XNodeInfoAnalysis::execute ()
//: calculate the age of every node
// TO DO: cache lengths to save time
{
	ReporterPrefix	thePrefix ("node information");

	// dumbass checks
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();
	assert (theTreeP != NULL);

	// grab the nodes
	nodearr_t theSelectedNodes;
	mNodeSelectorP->selectNodes (theTreeP, theSelectedNodes);

	stringvec_t		theLabels;
	vector<double>	theAges, theTimesToParent, theTimesToRoot;
	vector<int>		theChildrenCnt, theLeaveCnt, theSubtreeSz, theSiblingCnt, theHeights;

	// for every node

	// for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	nodearr_t::iterator p;
	for (p = theSelectedNodes.begin(); p != theSelectedNodes.end(); p++)
	{
		nodeiter_t q = *p;

		// do analysis
		// theLabels.push_back (getNodeLabel (q));
		if (mCalcAges)
			theAges.push_back (theTreeP->getTimeSinceNodeOrigin (q));
		// if (mCalcTimeToParent)
			theTimesToParent.push_back (theTreeP->getEdgeWeight (q));
		if (mCalcChildren)
			theChildrenCnt.push_back (theTreeP->countChildren (q));
		if (mCalcLeaves)
			theLeaveCnt.push_back (theTreeP->countLeaves (q));
		if (mCalcSubtree)
			theSubtreeSz.push_back (theTreeP->sizeOfSubtree (q));
		if (mCalcSiblings)
			theSiblingCnt.push_back (theTreeP->countSiblings (q));
		if (mCalcHeight)
			theHeights.push_back (theTreeP->getHeight (q));
		if (mCalcTimeToRoot)
			theTimesToRoot.push_back (theTreeP->getTimeFromNodeToRoot (q));
	}

	// print out results
	if (mSelectionCount)
			MesaGlobals::mReporterP->print (long (theSelectedNodes.size()), "nodes in selection");

	bool theAgesInTree = true;
	vector<double>::iterator s = max_element (theTimesToParent.begin(), theTimesToParent.end());
	if ((s == theTimesToParent.end()) or (*s == 0.0))
		theAgesInTree = false;

	if (mCalcAges)
	{
		if (theAgesInTree)
			MesaGlobals::mReporterP->print (theAges, "age of nodes");
		else
			MesaGlobals::mReporterP->printNotApplicable ("no distances in tree", "age of nodes");
	}

	if (mCalcTimeToParent)
	{
		if (theAgesInTree)
			MesaGlobals::mReporterP->print (theTimesToParent, "time since parent");
		else
			MesaGlobals::mReporterP->printNotApplicable ("no distances in tree", "time since parent");
	}

	if (mCalcChildren)
		MesaGlobals::mReporterP->print (theChildrenCnt, "number of children");

	if (mCalcLeaves)
		MesaGlobals::mReporterP->print (theLeaveCnt, "number of leaves");

	if (mCalcSubtree)
		MesaGlobals::mReporterP->print (theSubtreeSz, "number of nodes");

	if (mCalcSiblings)
		MesaGlobals::mReporterP->print (theSiblingCnt, "number of siblings");

	if (mCalcHeight)
		MesaGlobals::mReporterP->print (theHeights, "height of node");

	if (mCalcTimeToRoot)
		MesaGlobals::mReporterP->print (theTimesToRoot, "time to root");

	// dumbass check on answer
	MesaGlobals::mReporterP->print (theLabels, "node label");
}


const char*  XNodeInfoAnalysis::describeAnalysis ()
// TO DO: make this better
{
	return "information over nodes (exp)";
}

/*
void ListNodesAnalysis::execute ()
//: list all the nodes in the tree
{
	ReporterPrefix	thePrefix ("list nodes");

	// dumbass checks
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();
	assert (theTreeP != NULL);
	if (theTreeP->isEmpty())
	{
		MesaGlobals::mReporterP->printNotApplicable ("tree is empty");
		return;
	}

	// for every node list label
	stringvec_t	theLabels;
	nodeiter_t q;
	for (q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		theLabels.push_back (getNodeLabel (q));
	}

	MesaGlobals::mReporterP->print (theLabels);
}


string ListNodesAnalysis::initDesc ()
{
	return string ("list all the nodes in the tree");
}

*/

// *** COUNT TAXA ********************************************************/

void CountExtantTaxaAnalysis::execute ()
//: count the number of tips in the active tree
// Note that if a spp richness column is provided we assume all richness
// refer to tips. Hence the answer is the sum of all tip richnesses.
{
	ReporterPrefix	thePrefix ("count extant taxa");

	assert ((MesaGlobals::mTreeDataP->getActiveTreeP()) != NULL);
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	int theNumLeaves = 0;

	//: if there are species richness to be added
	if (mRichCol != kColIndex_None)
	{
		nodeiter_t	q;
		for (q = theTreeP->begin(); q != theTreeP->end(); q++)
		{
			if (q->second.isLeaf())
			{
				int theRichness =	(int) MesaGlobals::mContDataP->getData ((theTreeP->getLeafName (q)).c_str(), mRichCol);
				if (theRichness < 0)
				{
					throw ExecutionError ("negative species richness");
				}
				theNumLeaves += theRichness;
			}
		}
	}
	// otherwise just count the tips.
	else
	{
		theNumLeaves = (int) theTreeP->countLeaves ();
	}

	MesaGlobals::mReporterP->print (theNumLeaves);
}


const char* CountExtantTaxaAnalysis::describeAnalysis ()
{
	return "number of extant taxa";
}


void CountAllTaxaAnalysis::execute ()
//: count the total number of ndoes in the active tree
// Note that if a spp richness column is provided we assume the richness
// count all nodes beyond the resolved tips.
{
	ReporterPrefix	thePrefix ("count all taxa");

	assert ((MesaGlobals::mTreeDataP->getActiveTreeP()) != NULL);
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	int theAnswer = (int) theTreeP->countNodes();

	//: if there are species richness to be added
	if (mRichCol != kColIndex_None)
	{
		nodeiter_t	q;
		for (q = theTreeP->begin(); q != theTreeP->end(); q++)
		{
			if (q->second.isLeaf())
			{
				int theRichness =	(int) MesaGlobals::mContDataP->getData ((theTreeP->getLeafName (q)).c_str(), mRichCol);
				if (theRichness < 0)
				{
					throw ExecutionError ("negative species richness");
				}
				theAnswer += theRichness;
			}
		}

	}

	MesaGlobals::mReporterP->print (theAnswer);
}


const char* CountAllTaxaAnalysis::describeAnalysis ()
{
	return "number of extant and extinct taxa";
}


// *** DIVERSITY *******************************************************/

/**
Calculate genetic (allelic) diversity over the tree.

Note that the distance on the root is not included. Also note that if
the distances between nodes are large, then GD will tend towards 1.0.
For example, if all branchlengths are 0.7, in a tree of 20 tips, the
GD is effectively 1.0.
*/
void GeneticDiversityAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("genetic diversity");

	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();
	double		theAnswer = theTreeP->calcGeneticDiversity();

	if (theAnswer == 0.0)
		MesaGlobals::mReporterP->printNotApplicable ("non-allelic distances in the tree");
	else if (theAnswer == 1.0)
		MesaGlobals::mReporterP->printNotApplicable ("no distances in the tree");
	else
		MesaGlobals::mReporterP->print (theAnswer);
}


const char* GeneticDiversityAnalysis::describeAnalysis ()
{
	return "genetic diversity";
}



void PhyloDiversityAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("phylogenetic diversity");

	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();
	double theAnswer = theTreeP->calcPhyloDiversity();

	if (theAnswer == 0.0)
		MesaGlobals::mReporterP->printNotApplicable ("no distances in the tree");
	else
		MesaGlobals::mReporterP->print (theAnswer);
}


const char* PhyloDiversityAnalysis::describeAnalysis ()
{
	return "phylogenetic diversity";
}


void JackknifeGeneticDivAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("genetic diversity over sites");

	assert ((MesaGlobals::mTreeDataP->getActiveTreeP()) != NULL);
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// save the current tree so we can prune with impunity
	MesaTree theOrigTree = *theTreeP;

	// prune by sites
	BasicAction* thePruner = new PruneByAbundanceAction;
	thePruner->execute();

	// calculate PD of original tree
	double theOrigDiv = theTreeP->calcGeneticDiversity();
	if (theOrigDiv == 0.0)
		MesaGlobals::mReporterP->printNotApplicable ("non-allelic distances in tree");
	else if (theOrigDiv == 1.0)
		MesaGlobals::mReporterP->printNotApplicable ("no distances in tree");
	else
	{
		MesaGlobals::mReporterP->print (theOrigDiv, "GD");

		// restore tree
		*theTreeP = theOrigTree;

		// do jackknifing ...
		// ... get list of sites
		vector<colIndex_t> theSiteIndexes;
		MesaGlobals::mContDataP->listSiteTraits (theSiteIndexes);
		int theNumSites = theSiteIndexes.size();

		// ... get list of tips, ditto
		stringvec_t theTipNames;
		theTreeP->getTaxaNames (theTipNames);
		int theNumTaxa = theTipNames.size();

		// ... get number of observations
		long theNumObservations = 0;
		for (int i = 0; i < theNumSites; i++)
		{
			for (int j = 0; j < theNumTaxa; j++)
				theNumObservations += getContData (theTipNames[j].c_str(), theSiteIndexes[i]);
		}

		/*
		... examine every site to see if one fulfills the requirement that
		every site but 1 has an abundance of zero and the remaining site has
		an abundance of 1.
		*/
		long   theNumJackknifes = 0;
		double theSumDiversity = 0.0;
		double theTotalSqDiffs = 0.0;
		for (int i = 0; i < theNumTaxa; i++)
		{
			bool theTaxaIsJackknifable = true;
			long theNumSitesWithZero = 0;
			int  theIndexOfJackknifeSite = -1;

			// walk along sites for every taxa, count all the 0s, record the 1
			for (int j = 0; j < theNumSites; j++)
			{
				long theAbundance = getContData (theTipNames[i].c_str(), theSiteIndexes[j]);
				if (2 <= theAbundance)
				{
					theTaxaIsJackknifable = false;
					break;
				}
				else if (theAbundance == 1)
				{
					if (theIndexOfJackknifeSite != -1)
					{
						theTaxaIsJackknifable = false;
						break;
					}
					else
					{
						theIndexOfJackknifeSite = j;
					}
				}
				else if  (theAbundance == 0)
				{
					theNumSitesWithZero++;
				}
				else
				{
					// should never get here
					assert (false);
				}
			}

			if ((theTaxaIsJackknifable) and
			    ((unsigned int) (theNumSitesWithZero + 1) == theSiteIndexes.size()) and
				 (theIndexOfJackknifeSite != -1))
			{
				theNumJackknifes++;
				// set jackknifed site to 0
				referContState (theTipNames[i].c_str(), theIndexOfJackknifeSite) = 0;
				// prune tree
				thePruner->execute();
				// calc diversity
				double theNewDiv = theTreeP->calcGeneticDiversity();
				theSumDiversity += theNewDiv;
				// restore tree
				*theTreeP = theOrigTree;
				// restore site
				referContState (theTipNames[i].c_str(), theIndexOfJackknifeSite) = 1;
				// do calculations
				double theDiff = theOrigDiv - theNewDiv;
				theTotalSqDiffs += theDiff * theDiff;
			}
		}

		if (theTotalSqDiffs == 0.0)
		{
			MesaGlobals::mReporterP->printNotApplicable ("no appreciable error", "jackknifed error");
		}
		else
		{
			double theMean = (theSumDiversity + ((theNumObservations -
				theNumJackknifes) * theOrigDiv)) / theNumObservations;
			MesaGlobals::mReporterP->print (theMean, "jackknife estimate of mean");
			MesaGlobals::mReporterP->print (sqrt (theTotalSqDiffs *
				double (theNumObservations - 1.0) / double (theNumObservations)),
				"jackknife estimate of error");
			MesaGlobals::mReporterP->print (theNumObservations,
				"number of samples");
		}
	}

	// put it all back as it was before
	*theTreeP = theOrigTree;
	delete thePruner;
}


const char* JackknifeGeneticDivAnalysis::describeAnalysis ()
{
	return "jackknife estimate over sites of genetic diversity";
}


void JackknifePhyloDivAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("phylogenetic diversity over sites");

	assert ((MesaGlobals::mTreeDataP->getActiveTreeP()) != NULL);
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// save the current tree so we can prune with impunity
	MesaTree theOrigTree = *theTreeP;

	// prune by sites
	BasicAction* thePruner = new PruneByAbundanceAction;
	thePruner->execute();

	// calculate PD of original tree
	double theOrigDiv = theTreeP->calcPhyloDiversity();
	if (theOrigDiv == 1.0)
		MesaGlobals::mReporterP->printNotApplicable ("no distances in tree");
	else
	{
		MesaGlobals::mReporterP->print (theOrigDiv, "PD");

		// restore tree
		*theTreeP = theOrigTree;

		// do jackknifing ...
		// ... get list of sites
		vector<colIndex_t> theSiteIndexes;
		MesaGlobals::mContDataP->listSiteTraits (theSiteIndexes);
		int theNumSites = theSiteIndexes.size();

		// ... get list of tips, ditto
		stringvec_t theTipNames;
		theTreeP->getTaxaNames (theTipNames);
		int theNumTaxa = theTipNames.size();

		// ... get number of observations
		long theNumObservations = 0;
		for (int i = 0; i < theNumSites; i++)
		{
			for (int j = 0; j < theNumTaxa; j++)
				theNumObservations += getContData (theTipNames[j].c_str(), theSiteIndexes[i]);
		}

		/*
		... examine every site to see if one fulfills the requirement that
		every site but 1 has an abundance of zero and the remaining site has
		an abundance of 1.
		*/
		long   theNumJackknifes = 0;
		double theSumDiversity = 0.0;
		double theTotalSqDiffs = 0.0;
		for (int i = 0; i < theNumTaxa; i++)
		{
			bool theTaxaIsJackknifable = true;
			long theNumSitesWithZero = 0;
			int  theIndexOfJackknifeSite = -1;

			// walk along sites for every taxa, count all the 0s, record the 1
			for (int j = 0; j < theNumSites; j++)
			{
				long theAbundance = getContData (theTipNames[i].c_str(), theSiteIndexes[j]);
				if (2 <= theAbundance)
				{
					theTaxaIsJackknifable = false;
					break;
				}
				else if (theAbundance == 1)
				{
					if (theIndexOfJackknifeSite != -1)
					{
						theTaxaIsJackknifable = false;
						break;
					}
					else
					{
						theIndexOfJackknifeSite = j;
					}
				}
				else if  (theAbundance == 0)
				{
					theNumSitesWithZero++;
				}
				else
				{
					// should never get here
					assert (false);
				}
			}

			if ((theTaxaIsJackknifable) and
			    ((unsigned int) (theNumSitesWithZero + 1) == theSiteIndexes.size()) and
				 (theIndexOfJackknifeSite != -1))
			{
				theNumJackknifes++;
				// set jackknifed site to 0
				referContState (theTipNames[i].c_str(), theIndexOfJackknifeSite) = 0;
				// prune tree
				thePruner->execute();
				// calc diversity
				double theNewDiv = theTreeP->calcPhyloDiversity();
				theSumDiversity += theNewDiv;
				// restore tree
				*theTreeP = theOrigTree;
				// restore site
				referContState (theTipNames[i].c_str(), theIndexOfJackknifeSite) = 1;
				// do calculations
				double theDiff = theOrigDiv - theNewDiv;
				theTotalSqDiffs += theDiff * theDiff;
			}
		}

		if (theTotalSqDiffs == 0.0)
		{
			MesaGlobals::mReporterP->printNotApplicable ("no appreciable error", "jackknifed error");
		}
		else
		{
			double theMean = (theSumDiversity + ((theNumObservations -
				theNumJackknifes) * theOrigDiv)) / theNumObservations;
			MesaGlobals::mReporterP->print (theMean, "jackknife estimate of mean");
			MesaGlobals::mReporterP->print (sqrt (theTotalSqDiffs *
				double (theNumObservations - 1.0) / double (theNumObservations)),
				"jackknifed error");
			MesaGlobals::mReporterP->print (theNumObservations,
				"number of samples");
		}
	}

	// put it all back as it was before
	*theTreeP = theOrigTree;
	delete thePruner;
}


const char* JackknifePhyloDivAnalysis::describeAnalysis ()
{
	return "jackknife estimate over sites of phylogenetic diversity";
}

// internal class for holding site/taxa combinations
class abund_t
{
public:
	colIndex_t   mSite;
	string       mName;
};


void BootstrapPhyloDivAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("phylogenetic diversity over sites");

	assert ((MesaGlobals::mTreeDataP->getActiveTreeP()) != NULL);
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// save the current tree so we can prune with impunity
	MesaTree theOrigTree = *theTreeP;

	// prune by sites
	BasicAction* thePruner = new PruneByAbundanceAction;
	thePruner->execute();
	MesaTree theWorkingTree = *theTreeP;

	// calculate PD of original tree
	double theOrigDiv = theTreeP->calcPhyloDiversity();
	if (theOrigDiv == 1.0)
		MesaGlobals::mReporterP->printNotApplicable ("no distances in tree");
	else
	{
		MesaGlobals::mReporterP->print (theOrigDiv, "PD");

		// restore tree
		*theTreeP = theOrigTree;

		// do bootstrap ...
		// ... get list of sites
		vector<colIndex_t> theSiteIndexes;
		MesaGlobals::mContDataP->listSiteTraits (theSiteIndexes);
		int theNumSites = theSiteIndexes.size();

		// ... get list of tips, ditto
		stringvec_t theTipNames;
		theTreeP->getTaxaNames (theTipNames);
		int theNumTaxa = theTipNames.size();

		// ... get number of observations, collect
		vector<abund_t>   theSiteAbundances;
		vector<double>    theFrequencies;
		long theNumObservations = 0;
		for (int i = 0; i < theNumSites; i++)
		{
			for (int j = 0; j < theNumTaxa; j++)
			{
				long theNumAtSite = getContData (theTipNames[j].c_str(), theSiteIndexes[i]);
				if (0 < theNumAtSite)
				{
					theNumObservations += theNumAtSite;
					theFrequencies.push_back (double (theNumAtSite));
					abund_t   theTmpData;
					theTmpData.mName = theTipNames[j];
					theTmpData.mSite = theSiteIndexes[i];
					theSiteAbundances.push_back (theTmpData);
				}
			}
		}
		assert (theSiteAbundances.size() == theFrequencies.size());

		// convert frequency array to cumulative freq/probability
		double thePrevAbundance = 0.0;
		for (unsigned int i = 0; i < theFrequencies.size(); i++)
		{
			thePrevAbundance += theFrequencies[i];
			theFrequencies[i] = thePrevAbundance / double (theNumObservations);
		}
		assert (long (thePrevAbundance) == theNumObservations);
		*(theFrequencies.end() - 1) = 1.0;

		/*
		do the actual bootstrapping
		*/
		double theSumDiversity = 0.0;
		double theTotalSqDiffs = 0.0;
		ContTraitMatrix theOrigTraits = *(MesaGlobals::mContDataP);
		for (int i = 0; i < mNumReps; i++)
		{
			// set all abundances to zero
			for (int j = 0; j < theNumSites; j++)
			{
				for (int k = 0; k < theNumTaxa; k++)
				{
					referContState (theTipNames[k].c_str(), theSiteIndexes[j]) = 0.0;
				}
			}

			// rebuild them with sampling
			for (int m = 0; m < mNumSamples; m++)
			{
				// pick site to sample
				double theChoice = MesaGlobals::mRng.UniformFloat();
				unsigned int theSampleIndex = 0;
				for (; theSampleIndex < theFrequencies.size(); theSampleIndex++)
				{
					if (theChoice <= theFrequencies[theSampleIndex])
						break;
				}
				assert (theSampleIndex < theFrequencies.size());

				referContState (theSiteAbundances[theSampleIndex].mName.c_str(),
					theSiteAbundances[theSampleIndex].mSite) += 1.0;
			}


			// prune tree
			thePruner->execute();
			// calc diversity
			double theNewDiv = theTreeP->calcPhyloDiversity();
			theSumDiversity += theNewDiv;
			// restore tree
			*theTreeP = theOrigTree;
			// do calculations
			double theDiff = theOrigDiv - theNewDiv;
			theTotalSqDiffs += theDiff * theDiff;
		}

		*(MesaGlobals::mContDataP) = theOrigTraits;

		double theMean = theSumDiversity / double (mNumReps);
		MesaGlobals::mReporterP->print (theMean, "bootstrap estimate of mean");
		MesaGlobals::mReporterP->print (sqrt (theTotalSqDiffs) / double (mNumReps - 1),
			"bootstrap estimate of std error");
	}

	// put it all back as it was before
	*theTreeP = theOrigTree;
	delete thePruner;
}


const char* BootstrapPhyloDivAnalysis::describeAnalysis ()
{
	return "bootstrap estimate over sites of phylogenetic diversity";
}


void BootstrapGeneticDivAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("genetic diversity over sites");

	assert ((MesaGlobals::mTreeDataP->getActiveTreeP()) != NULL);
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// save the current tree so we can prune with impunity
	MesaTree theOrigTree = *theTreeP;

	// prune by sites
	BasicAction* thePruner = new PruneByAbundanceAction;
	thePruner->execute();
	MesaTree theWorkingTree = *theTreeP;

	// calculate PD of original tree
	double theOrigDiv = theTreeP->calcGeneticDiversity();
	if (theOrigDiv == 1.0)
		MesaGlobals::mReporterP->printNotApplicable ("no distances in tree");
	else
	{
		MesaGlobals::mReporterP->print (theOrigDiv, "GD");

		// restore tree
		*theTreeP = theOrigTree;

		// do bootstrap ...
		// ... get list of sites
		vector<colIndex_t> theSiteIndexes;
		MesaGlobals::mContDataP->listSiteTraits (theSiteIndexes);
		int theNumSites = theSiteIndexes.size();

		// ... get list of tips, ditto
		stringvec_t theTipNames;
		theTreeP->getTaxaNames (theTipNames);
		int theNumTaxa = theTipNames.size();

		// ... get number of observations, collect
		vector<abund_t>   theSiteAbundances;
		vector<double>    theFrequencies;
		long theNumObservations = 0;
		for (int i = 0; i < theNumSites; i++)
		{
			for (int j = 0; j < theNumTaxa; j++)
			{
				long theNumAtSite = getContData (theTipNames[j].c_str(), theSiteIndexes[i]);
				if (0 < theNumAtSite)
				{
					theNumObservations += theNumAtSite;
					theFrequencies.push_back (double (theNumAtSite));
					abund_t   theTmpData;
					theTmpData.mName = theTipNames[j];
					theTmpData.mSite = theSiteIndexes[i];
					theSiteAbundances.push_back (theTmpData);
				}
			}
		}
		assert (theSiteAbundances.size() == theFrequencies.size());

		// convert frequency array to cumulative freq/probability
		double thePrevAbundance = 0.0;
		for (unsigned int i = 0; i < theFrequencies.size(); i++)
		{
			thePrevAbundance += theFrequencies[i];
			theFrequencies[i] = thePrevAbundance / double (theNumObservations);
		}
		assert (long (thePrevAbundance) == theNumObservations);
		*(theFrequencies.end() - 1) = 1.0;

		/*
		do the actual bootstrapping
		*/
		double theSumDiversity = 0.0;
		double theTotalSqDiffs = 0.0;
		ContTraitMatrix theOrigTraits = *(MesaGlobals::mContDataP);
		for (int i = 0; i < mNumReps; i++)
		{
			// set all abundances to zero
			for (int j = 0; j < theNumSites; j++)
			{
				for (int k = 0; k < theNumTaxa; k++)
				{
					referContState (theTipNames[k].c_str(), theSiteIndexes[j]) = 0.0;
				}
			}

			// rebuild them with sampling
			for (int m = 0; m < mNumSamples; m++)
			{
				// pick site to sample
				double theChoice = MesaGlobals::mRng.UniformFloat();
				unsigned int theSampleIndex = 0;
				for (; theSampleIndex < theFrequencies.size(); theSampleIndex++)
				{
					if (theChoice <= theFrequencies[theSampleIndex])
						break;
				}
				assert (theSampleIndex < theFrequencies.size());

				referContState (theSiteAbundances[theSampleIndex].mName.c_str(),
					theSiteAbundances[theSampleIndex].mSite) += 1.0;
			}


			// prune tree
			thePruner->execute();
			// calc diversity
			double theNewDiv = theTreeP->calcGeneticDiversity();
			theSumDiversity += theNewDiv;
			// restore tree
			*theTreeP = theOrigTree;
			// do calculations
			double theDiff = theOrigDiv - theNewDiv;
			theTotalSqDiffs += theDiff * theDiff;
		}

		*(MesaGlobals::mContDataP) = theOrigTraits;

		double theMean = theSumDiversity / double (mNumReps);
		MesaGlobals::mReporterP->print (theMean, "bootstrap estimate of mean");
		MesaGlobals::mReporterP->print (sqrt (theTotalSqDiffs) / double (mNumReps - 1),
			"bootstrap estimate of std error");
	}

	// put it all back as it was before
	*theTreeP = theOrigTree;
	delete thePruner;
}


const char* BootstrapGeneticDivAnalysis::describeAnalysis ()
{
	return "bootstrap estimate over sites of genetic diversity";
}

void BrillouinDiversityAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("Brillouin index");

	// get the necessary data structures
	MesaTree* theTreeP = getActiveTreeP();
	ContTraitMatrix* theContDataP = MesaGlobals::mContDataP;
	assert (theTreeP != NULL);
	assert (theContDataP != NULL);
	assert (0 < theContDataP->countSiteTraits());

	// get the indices of the sites and tree leaves
	vector<colIndex_t> theIndices;
	theContDataP->listSiteTraits (theIndices);
	vector<nodeiter_t> theLeaves;
	theTreeP->getLeaves (theLeaves);
	assert (2 <= theLeaves.size());
	assert (1 <= theIndices.size());

	// collect abundances for each species
	vector<long> theAbundances;
	for (unsigned int i = 0; i < theLeaves.size(); i++)
	{
		long theTaxaAbundance = 0;
		for (unsigned int j = 0; j < theIndices.size(); j++)
		{
			theTaxaAbundance += (long) getContData (theLeaves[i], theIndices[j]);
		}
		assert (0 <= theTaxaAbundance);
		// TODO: check for dumbasses that have negative abundances
		theAbundances.push_back (theTaxaAbundance);
	}

	// calculate total abundance & sum of ln (factorial)
	long theTotal = 0;
	long theTotalLnFactorial = 0;
	for (unsigned int k = 0; k < theAbundances.size(); k++)
	{
		long theNextAbundance = theAbundances[k];
		if (0 < theNextAbundance)
		{
			theTotal += theNextAbundance;
			double theFactorial = factorialGosper (theNextAbundance);
			assert (0.0 < theFactorial);
			theTotalLnFactorial += logE (theFactorial);
		}
		assert (0 <= theTotal);
	}
	if (theTotal == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("all taxa have zero abundance");
		return;
	}

	double theAnswer = (logE (factorialGosper (theTotal)) - theTotalLnFactorial)
		/ theTotal;

	MesaGlobals::mReporterP->print (theAnswer);
}


const char* BrillouinDiversityAnalysis::describeAnalysis ()
{
	return "Brillouin index of species diversity";
}


void PieDiversityAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("PIE index");

	// get the necessary data structures
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);
	ContTraitMatrix* theContDataP = MesaGlobals::mContDataP;
	assert (theContDataP != NULL);
	assert (0 < theContDataP->countSiteTraits());

	// get the indices of the sites and tree leaves
	vector<colIndex_t> theIndices;
	theContDataP->listSiteTraits (theIndices);
	vector<nodeiter_t> theLeaves;
	theTreeP->getLeaves (theLeaves);
	assert (2 <= theLeaves.size());
	assert (1 <= theIndices.size());

	// collect abundances for each species & total abundance
	long theTotalAbundance = 0;
	vector<long> theAbundances;
	for (unsigned int i = 0; i < theLeaves.size(); i++)
	{
		long theTaxaAbundance = 0;
		for (unsigned int j = 0; j < theIndices.size(); j++)
		{
			theTaxaAbundance += (long) getContData (theLeaves[i], theIndices[j]);
		}
		assert (0 <= theTaxaAbundance);
		// TODO: check for dumbasses that have negative abundances
		theAbundances.push_back (theTaxaAbundance);
		theTotalAbundance += theTaxaAbundance;
	}
	if (theTotalAbundance == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("all taxa have zero abundance");
		return;
	}

	// calculate sum of squared proportions
	double theSumSqPorps = 0.0;
	for (unsigned int k = 0; k < theAbundances.size(); k++)
	{
		double thePorpoprtion = theAbundances[k] / theTotalAbundance;
		theSumSqPorps += thePorpoprtion * thePorpoprtion;
	}

	// calculate correction factor
	double theNumSpecies = theAbundances.size();
	double theCorrrection = theNumSpecies / (theNumSpecies + 1);

	// calculate answer
	double theAnswer = theCorrrection * (1.0 - theSumSqPorps);
	MesaGlobals::mReporterP->print (theAnswer);
}


const char* PieDiversityAnalysis::describeAnalysis ()
{
	return "PIE diversity index";
}


void MacintoshDiversityAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("Macintosh's index");

	// get the necessary data structures
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);
	ContTraitMatrix* theContDataP = MesaGlobals::mContDataP;
	assert (theContDataP != NULL);
	assert (0 < theContDataP->countSiteTraits());

	// get the indices of the sites and tree leaves
	vector<colIndex_t> theIndices;
	theContDataP->listSiteTraits (theIndices);
	vector<nodeiter_t> theLeaves;
	theTreeP->getLeaves (theLeaves);
	assert (2 <= theLeaves.size());
	assert (1 <= theIndices.size());

	// collect abundances for each species & total abundance
	long theTotalAbundance = 0;
	vector<long> theAbundances;
	for (unsigned int i = 0; i < theLeaves.size(); i++)
	{
		long theTaxaAbundance = 0;
		for (unsigned int j = 0; j < theIndices.size(); j++)
		{
			theTaxaAbundance += (long) getContData (theLeaves[i], theIndices[j]);
		}
		assert (0 <= theTaxaAbundance);
		// TODO: check for dumbasses that have negative abundances
		theAbundances.push_back (theTaxaAbundance);
		theTotalAbundance += theTaxaAbundance;
	}
	if (theTotalAbundance == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("all taxa have zero abundance");
		return;
	}

	// calculate sum of squared proportions
	double theSumSqPorps = 0.0;
	for (unsigned int k = 0; k < theAbundances.size(); k++)
	{
		double thePorpoprtion = theAbundances[k] / theTotalAbundance;
		theSumSqPorps += thePorpoprtion * thePorpoprtion;
	}

	// calculate answer
	double theAnswer = std::sqrt (theSumSqPorps);
	MesaGlobals::mReporterP->print (theAnswer);
}


const char* MacintoshDiversityAnalysis::describeAnalysis ()
{
	return "Macintosh's index";
}


void MargelefDiversityAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("Margelef diversity index");

	// get the necessary data structures
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);
	ContTraitMatrix* theContDataP = MesaGlobals::mContDataP;
	assert (theContDataP != NULL);
	assert (0 < theContDataP->countSiteTraits());

	// get the indices of the sites and tree leaves
	vector<colIndex_t> theIndices;
	theContDataP->listSiteTraits (theIndices);
	vector<nodeiter_t> theLeaves;
	theTreeP->getLeaves (theLeaves);
	assert (2 <= theLeaves.size());
	assert (1 <= theIndices.size());

	// collect abundances for each species & total abundance
	long theTotalAbundance = 0;
	for (unsigned int i = 0; i < theLeaves.size(); i++)
	{
		for (unsigned int j = 0; j < theIndices.size(); j++)
		{
			theTotalAbundance += (long) getContData (theLeaves[i], theIndices[j]);
		}
		assert (0 <= theTotalAbundance);
	}
	if (theTotalAbundance == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("all taxa have zero abundance");
		return;
	}

	// calculate answer
	double theAnswer = (theLeaves.size() - 1) / logE (theTotalAbundance);
	MesaGlobals::mReporterP->print (theAnswer);
}


const char* MargelefDiversityAnalysis::describeAnalysis ()
{
	return "Margelef diversity index";
}


void MenhinickDiversityAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("Menhinick diversity index");

	// get the necessary data structures
	MesaTree* theTreeP = getActiveTreeP();
	assert (theTreeP != NULL);
	ContTraitMatrix* theContDataP = MesaGlobals::mContDataP;
	assert (theContDataP != NULL);
	assert (0 < theContDataP->countSiteTraits());

	// get the indices of the sites and tree leaves
	vector<colIndex_t> theIndices;
	theContDataP->listSiteTraits (theIndices);
	vector<nodeiter_t> theLeaves;
	theTreeP->getLeaves (theLeaves);
	assert (2 <= theLeaves.size());
	assert (1 <= theIndices.size());

	// collect abundances for each species & total abundance
	long theTotalAbundance = 0;
	for (unsigned int i = 0; i < theLeaves.size(); i++)
	{
		for (unsigned int j = 0; j < theIndices.size(); j++)
		{
			theTotalAbundance += (long) getContData (theLeaves[i], theIndices[j]);
		}
		assert (0 <= theTotalAbundance);
	}
	if (theTotalAbundance == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("all taxa have zero abundance");
		return;
	}

	// calculate answer
	double theAnswer = theLeaves.size() / std::sqrt (theTotalAbundance);
	MesaGlobals::mReporterP->print (theAnswer);
}


const char* MenhinickDiversityAnalysis::describeAnalysis ()
{
	return "Menhinick diversity index";
}

void SimpsonDiversityAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("Simpson index of diversity");

	// get the necessary data structures
	MesaTree* theTreeP = getActiveTreeP();
	ContTraitMatrix* theContDataP = MesaGlobals::mContDataP;
	assert (theTreeP != NULL);
	assert (theContDataP != NULL);
	assert (0 < theContDataP->countSiteTraits());

	// get the indices of the sites and tree leaves
	vector<colIndex_t> theIndices;
	theContDataP->listSiteTraits (theIndices);
	vector<nodeiter_t> theLeaves;
	theTreeP->getLeaves (theLeaves);
	assert (2 <= theLeaves.size());
	assert (1 <= theIndices.size());

	// collect abundances for each species
	vector<conttrait_t> theAbundances;
	for (unsigned int i = 0; i < theLeaves.size(); i++)
	{
		conttrait_t theTaxaAbundance = 0.0;
		for (unsigned int j = 0; j < theIndices.size(); j++)
		{
			theTaxaAbundance += getContData (theLeaves[i], theIndices[j]);
		}
		assert (0.0 <= theTaxaAbundance);
		// TODO: check for dumbasses that have negative abundances
		theAbundances.push_back (theTaxaAbundance);
	}

	// calculate total abundance & sum of porportions^2
	conttrait_t theTotal = 0.0;
	for (unsigned int k = 0; k < theAbundances.size(); k++)
	{
		theTotal += theAbundances[k];
		assert (0.0 <= theTotal);
	}
	if (theTotal == 0.0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("all taxa have zero abundance");
		return;
	}

	conttrait_t theSumPorpSq = 0.0;
	for (unsigned int m = 0; m < theAbundances.size(); m++)
	{
		conttrait_t thePorp = (theAbundances[m] / theTotal);
		theSumPorpSq += (thePorp * thePorp);
		assert (theSumPorpSq <= 1.0);
	}

	// calculate the final answer
	conttrait_t theAnswer = 1.0 - theSumPorpSq;
	MesaGlobals::mReporterP->print (theAnswer);
}


const char* SimpsonDiversityAnalysis::describeAnalysis ()
{
	return "Simpson index of diversity";
}



/**
calculate Shannon Weiner diversity index.

Note that you can use any log value in this, so I've elected to use the
original log2
*/
void ShannonWeinerDiversityAnalysis::execute ()
// only differs from SImpson in two places
{
	ReporterPrefix	thePrefix ("Shannon-Weiner diversity");

	// get the necessary data structures
	MesaTree* theTreeP = getActiveTreeP();
	ContTraitMatrix* theContDataP = MesaGlobals::mContDataP;
	assert (theTreeP != NULL);
	assert (theContDataP != NULL);
	assert (0 < theContDataP->countSiteTraits());

	// get the indices of the sites and tree leaves
	vector<colIndex_t> theIndices;
	theContDataP->listSiteTraits (theIndices);
	vector<nodeiter_t> theLeaves;
	theTreeP->getLeaves (theLeaves);
	assert (2 <= theLeaves.size());
	assert (1 <= theIndices.size());

	// collect abundances for each species
	vector<conttrait_t> theAbundances;
	for (unsigned int i = 0; i < theLeaves.size(); i++)
	{
		conttrait_t theTaxaAbundance = 0.0;
		for (unsigned int j = 0; j < theIndices.size(); j++)
		{
			theTaxaAbundance += getContData (theLeaves[i], theIndices[j]);
		}
		assert (0.0 <= theTaxaAbundance);
		// TODO: check for dumbasses that have negative abundances
		theAbundances.push_back (theTaxaAbundance);
	}

	// calculate total abundance & sum of porportions^2
	conttrait_t theTotal = 0.0;
	for (unsigned int k = 0; k < theAbundances.size(); k++)
	{
		theTotal += theAbundances[k];
		assert (0.0 <= theTotal);
	}
	if (theTotal == 0.0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("all taxa have zero abundance");
		return;
	}

	conttrait_t theSumPorpLogPorp = 0.0;
	for (unsigned int m = 0; m < theAbundances.size(); m++)
	{
		if (0.0 < theAbundances[m])
		{
			// just in case something has an abundance of 0
			conttrait_t thePorp = (theAbundances[m] / theTotal);
			theSumPorpLogPorp += thePorp * log2 (thePorp);
		}
	}

	// calculate the final answers
	conttrait_t theAnswer = - theSumPorpLogPorp;
	conttrait_t theMax = log2 (theTotal);
	conttrait_t theEvenness = theAnswer / theMax;

	MesaGlobals::mReporterP->print (theAnswer, "index");
	MesaGlobals::mReporterP->print (theMax, "maximum");
	MesaGlobals::mReporterP->print (theEvenness, "evenness");
}


const char* ShannonWeinerDiversityAnalysis::describeAnalysis ()
{
	return "Shannon-Weiner diversity";
}


// *** IMBALANCE *********************************************************/

// *** NORMAL FUSCO
void FuscoAnalysis::execute ()
//: calculate Fusco's I imbalance score on the active tree
// This is the Fusco we are using at the moment with the correction built in
{
// bool , mExtended;
	ReporterPrefix	thePrefix ("fusco imbalance");
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	if (theTreeP->isEmpty())
	{
		MesaGlobals::mReporterP->printNotApplicable ("empty tree");
		return;
	}


	// and a cache to save calculating answers repeatedly
	typedef map<MesaTree::id_type, int>		richnesscache_t;
	richnesscache_t								theRichnessCache;

	// where we'll store the answers
	vector<string>   theLabels;
	vector<double>   theAnswers;
	vector<double>   theWeights;
	vector<int>      theSizes;

	// for each node: if you get a sensible balance store it & the labels
	for (nodeiter_t r = theTreeP->begin(); r != theTreeP->end(); r++)
	{
		// if not bifurcating
		if (theTreeP->countChildren (r) != 2)
			continue;

		// else calculate the parameters
		long theBigTips, theSmallTips;
		if (mRichCol != kColIndex_None)
		{
			int theResultVec[2];

			for (nodeidvec_t::size_type i = 0; i < 2; i++)
			{
				nodeidvec_t	theLeaveVec;

				theResultVec [i] = 0;
				nodeiter_t theChild = theTreeP->getChild (r, i);
				theTreeP->collectLeaveIds (theChild->first, theLeaveVec);

				nodeidvec_t::iterator	q;
				for (q = theLeaveVec.begin(); q != theLeaveVec.end(); q++)
				{
					// find out the richness for this tip
					int theRichness;

					// if it not in the cache calculate it
					richnesscache_t::iterator r = theRichnessCache.find (*q);
					if (r == theRichnessCache.end())
					{
						theRichness =	(int) MesaGlobals::mContDataP->getData ((theTreeP->getNodeLabel (*q)).c_str(), mRichCol);
						assert (theRichness == (int) getContData (
							(theTreeP->getNodeLabel (*q)).c_str(), mRichCol));
						assert (theRichness = getRichnessData (*q, mRichCol));
					}
					else
					{
						theRichness = r->second;
					}

					assert (0 <= theRichness);
					theResultVec [i] += theRichness;
				}
			}

			theBigTips = theResultVec[0];
			theSmallTips = theResultVec[1];
		}
		else
		{
			theBigTips = theTreeP->countLeaves (theTreeP->getChild (r, 0));
			theSmallTips = theTreeP->countLeaves (theTreeP->getChild (r, 1));
		}

		assert (0 < theBigTips);
		assert (0 < theSmallTips);
		long theTotalTips = theBigTips + theSmallTips;

		// if insufficient tips
		if (theTotalTips < 4)
			continue;

		double theImbalance;
		// if utterly balanced
		if (theBigTips == theSmallTips)
		{
			theImbalance = 0.0;
		}
		else
		{
			// otherwise calculate
			if (theBigTips < theSmallTips)
				swap (theBigTips, theSmallTips);

			long theMin = ceil (double (theTotalTips) / 2.0);
			long theMax = theTotalTips - 1;
			theImbalance = double (theBigTips - theMin) / double (theMax - theMin);
			// this where we correct
			if ((mCorrection) and ((theTotalTips % 2) == 0))
				theImbalance *= double (theTotalTips - 1) / double (theTotalTips);
		}

		// calculate weight
		if (not mCorrection)
		{
			double theWeight;
			if ((theTotalTips % 2) != 0) // if S is odd
			{
				theWeight = 1.0;
			}
			else // if S is even
			{
				assert ((theTotalTips % 2) == 0);
				theWeight = double (theTotalTips - 1) / double (theTotalTips);
				if (theImbalance == 0.0)
					theWeight *= 2.0;
			}

			theWeights.push_back (theWeight);
		}

		// stuff results into vector
		theLabels.push_back (getNodeLabel (r));
		theAnswers.push_back (theImbalance);
		if (mListSizes)
			theSizes.push_back (theTotalTips);
	}

	// if no meaningful answers produced
	if (theAnswers.size() == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("analysis not possible at any node");
		return;
	}

	// produce the answer string
	assert (theLabels.size() == theAnswers.size());
	MesaGlobals::mReporterP->print (theLabels, "node");
	MesaGlobals::mReporterP->print (theAnswers, "imbalance");
	if (not mCorrection)
		MesaGlobals::mReporterP->print (theWeights, "weight");
	if (mListSizes)
		MesaGlobals::mReporterP->print (theSizes, "node size");
}

const char* FuscoAnalysis::describeAnalysis ()
{
	static string theBuffer;

	theBuffer = "Fusco imbalance";
	if (0 <= mRichCol)
	{
		theBuffer += " using species richness column ";
		theBuffer += toString (mRichCol + 1);
		theBuffer += "\'";
		theBuffer += MesaGlobals::mContDataP->getColName (mRichCol);
		theBuffer += "\'";
	}
	return theBuffer.c_str();
}


// *** SLOWINSKI-GUYER
void SlowinskiGuyerAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("Slowinski-Guyer imbalance");

	assert (MesaGlobals::mTreeDataP != NULL);
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP ();

	// where we'll store the answers
	vector<string>	theLabels;
	vector<bool>	theAnswers;
	vector<bool>	thePVals;
	vector<int> theSizes;

	nodeiter_t q;
	for (q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		if (2 == theTreeP->countChildren (q))
		{
			long theSmallTips = countLeaves (theTreeP->getChild (q, 0), mRichCol);
			long theBigTips = countLeaves (theTreeP->getChild (q, 1), mRichCol);
			long theTotalTips = theBigTips + theSmallTips;
			if (theBigTips < theSmallTips)
				swap (theBigTips, theSmallTips);

			theLabels.push_back (getNodeLabel (q));
			theAnswers.push_back (0.9 <= (double (theBigTips) / double (theTotalTips)));
			thePVals.push_back (0.05 >= (2.0 * double (theSmallTips) / double (theTotalTips - 1)));
			if (mListSizes)
				theSizes.push_back (theTotalTips);

		}
	}

	// if no meaningful answers produced
	if (theAnswers.size() == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("analysis not possible at any node");
		return;
	}

	// produce the answer string
	assert (theLabels.size() == theAnswers.size());
	MesaGlobals::mReporterP->print (theLabels, "node");
	MesaGlobals::mReporterP->print (theAnswers, "imbalanced?");
	MesaGlobals::mReporterP->print (thePVals, "significant?");
	if (mListSizes)
		MesaGlobals::mReporterP->print (theSizes, "node size");

}

const char* SlowinskiGuyerAnalysis::describeAnalysis ()
{
	return "Slowinski-Guyer imbalance";
}



// *** SHAO/SOKAL N BAR
void ShaosNbarAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("Shao & Sokal's Nbar imbalance");

	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// handle simple case of a single node
	if (theTreeP->countNodes() <= 1)
	{
		MesaGlobals::mReporterP->printNotApplicable ("tree is too small");

		return;
	}

	// clauclate Nbar
	long	theSumHt = 0;
	long	theNumLeaves = 0;

	for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		if (theTreeP->isLeaf (q))
		{
			theNumLeaves++;
			MesaTree::size_type theHt = theTreeP->getHeight (q);
			theSumHt += theHt; // number of intervening nodes
		}
	}

	double theAnswer = double (theSumHt) / double (theNumLeaves);

	// calculate the expected
	double theExpected = 0.0;
	for (int i = 2; i <= theNumLeaves; i++)
		theExpected += 1 / double (i);
	theExpected *= 2.0;

	// output results
	MesaGlobals::mReporterP->print (theAnswer, "observed");
	MesaGlobals::mReporterP->print (theExpected, "expected");

}

const char* ShaosNbarAnalysis::describeAnalysis ()
{
	return "Shao & Sokal's N-bar imbalance";
}


// *** SHAO/SOKAL SIGMA SQUARED
void ShaosSigmaSqAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("sigma squared imbalance");

	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// handle simple case of a single node
	if (theTreeP->countNodes() <= 1)
	{
		MesaGlobals::mReporterP->printNotApplicable ("tree is too small");
		return;
	}

	// calculate N for each leaf
	vector<int>	theNVec;
	for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		if (theTreeP->isLeaf (q))
		{
			MesaTree::size_type theHt = theTreeP->getHeight (q);
			theNVec.push_back (theHt - 1); // number of intervening nodes
		}
	}
	long theNumLeaves = theNVec.size();

	// calculate Nbar
	double theNbar = 0.0;
	for (long i = 0; i < theNumLeaves; i++)
	{
		theNbar += double (theNVec[i]);
	}
	theNbar /= double (theNumLeaves);

	// sum the squared diffs
	double theSumSqDiffs = 0.0;
	for (long i = 0; i < theNumLeaves; i++)
	{
		double theDiff = theNVec[i] - theNbar;
		theSumSqDiffs += (theDiff * theDiff);
	}

	// calculate sigma squared
	double theAnswer = theSumSqDiffs / double (theNumLeaves);

	// output results
	MesaGlobals::mReporterP->print (theAnswer);
}

const char* ShaosSigmaSqAnalysis::describeAnalysis ()
{
	return "Shao & Sokal's sigma-squared imbalance";
}


// *** COLLESS' C
void CollessCAnalysis::execute ()
// TO DO: currently we ignore polytomies. Should we assume they are
// balanced?
{
	ReporterPrefix	thePrefix ("Colless' C imbalance");

	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// for every internal node
	bool theTreeHasNoPolytomies = true;
	long theTotal = 0;

	nodeiter_t q = theTreeP->begin();
	while ((q != theTreeP->end()) and theTreeHasNoPolytomies)
	{
		if (not theTreeP->isLeaf (q))
		{
			if (theTreeP->countChildren (q) != 2)
			{
				theTreeHasNoPolytomies = false;
				break;
			}
			else
			{
				nodeiter_t theLeftSubtreeIter = theTreeP->getChild (q, 0);
				long theBigTips = theTreeP->countLeaves (theLeftSubtreeIter);
				nodeiter_t theRightSubtreeIter = theTreeP->getChild (q, 1);
				long theSmallTips = theTreeP->countLeaves (theRightSubtreeIter);

				theTotal += abs (theBigTips - theSmallTips);
			}
		}

		q++;
	}

	// output results
	if (not theTreeHasNoPolytomies)
		MesaGlobals::mReporterP->printNotApplicable ("tree contains polytomies");
	else
	{
		int theNumTips = theTreeP->countLeaves();
		if (theNumTips <= 2)
			MesaGlobals::mReporterP->printNotApplicable ("tree is too small");
		else
		{
			double theAnswer = (2.0 / double ((theNumTips - 1) * (theNumTips - 2))) *
				theTotal;
			MesaGlobals::mReporterP->print (theAnswer);
		}
	}
}

const char* CollessCAnalysis::describeAnalysis ()
{
	return "Colless' C imbalance";
}


// *** SHAO & SOKAL's B1
void B1Analysis::execute ()
{
	ReporterPrefix	thePrefix ("B1 balance");

	MesaTree* theTreeP = getActiveTreeP();

	double theAnswer = 0.0;

	// for every internal node
	for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		// if a tip or root, don't process
		if (theTreeP->isRoot (q) or theTreeP->isLeaf (q))
			continue;

		// find out the subtended tips
		nodeidvec_t theLeafIds;
		theTreeP->collectLeaveIds (q->first, theLeafIds);

		// how far is it from the tips to this node?
		long theMaxDist = 1;
		for (unsigned int i = 0; i < theLeafIds.size(); i++)
		{
			nodeiter_t theChildIter = theTreeP->getIter (theLeafIds[i]);
			long theDist = theTreeP->getDistance (theChildIter, q);
			if (theMaxDist < theDist)
				theMaxDist = theDist;
		}

		theAnswer += 1.0 / double (theMaxDist);
	}

	// output results
	MesaGlobals::mReporterP->print (theAnswer);

}

const char* B1Analysis::describeAnalysis ()
{
	return "Shao & Sokal's B1 balance";
}


// *** SHAO & SOKAL's B2
void B2Analysis::execute ()
{
	ReporterPrefix	thePrefix ("B2 balance");

	MesaTree* theTreeP = getActiveTreeP();

	double theAnswer = 0.0;

	// for every internal node
	for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		if (theTreeP->isLeaf (q))
		{
			long theHt = theTreeP->getHeight (q);
			if (theHt != 0)
				theAnswer += double (theHt) / pow (2.0, double (theHt));
		}
	}

	// output results
	MesaGlobals::mReporterP->print (theAnswer);
}

const char* B2Analysis::describeAnalysis ()
{
	return "Shao & Sokal's B2 balance";
}


// *** OTHER TREE SHAPE ANALYSES ******************************************/

void StemminessAnalysis::execute ()
/*
Note this assumes that the tree is ultrametric. If not - well, you're
fairly fucked.
CHANGE (01.6.18): isInternal() now correctly calls the root internal
and thus we must exclude them.
CHANGE (01.10.16): I'm worrying about the calculation of node age in an
efficient and correct manner, especially in a paleontological tree.
Since age was defined the time from a node to a living tip, nodes with
no extant descendants run into some problems. Thus we now calculate
the age of a node based on the tree age (the distance from the root to
a living tip less the distance from the ndoe to the root). This should
give a more consistent answer.
TO DO: get rid of the counting of internal nodes.
*/
{
	ReporterPrefix	thePrefix ("Stemminess");

	// For every internal node, divide the length of the branch above
	// a node by the age of its parent (i.e. the time from when the
	// parent splits to the present), then calculate the sum of these
	// So as to efficiently and correctly calculate the node age (time
	// since splitting), get the age of the tree (less the branchlength
	// on the root) and subtract the time from the node to the root.

	MesaTree*    theTreeP = getActiveTreeP();
	double       theAnswer = 0.0;
	int          theNumInternalNodes = 0;
	nodeiter_t   theRootIter = theTreeP->getRoot();

	// get age of root
	mesatime_t theRootAge = theTreeP->getRootAge();

	// for every internal node that is not the root
	for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		if (theTreeP->isInternal(q) and (q != theRootIter)) // 01.6.18
		{
			theNumInternalNodes++;

			nodeiter_t theParent = theTreeP->getParent (q);

			/*
			// 01.10.16
			time_t theParAge = theTreeP->getNodeAge (theParentIter);
			time_t theBranchLen = theTreeP->getEdgeWeight (q);
			*/

			mesatime_t theBranchLen = theTreeP->getTimeFromNodeToParent (q);
			mesatime_t theParAge = theRootAge -
				theTreeP->getTimeFromNodeToRoot (theParent);
			assert (0 <= theBranchLen);
			assert (0 <= theParAge);

			// get steminess of this node
			MesaTree::weight_type theStemminess;
			if ((theParAge == 0.0) or (theBranchLen == 0.0))
				theStemminess = 0.0;
			else
				theStemminess = theBranchLen / theParAge;
			assert (0.0 <= theStemminess);

			// add to running total
			theAnswer += theStemminess;
			assert (0.0 <= theAnswer);
		}
	}

	// calculate results
	bool theTreeHasNoLengths = (theAnswer == 0.0);
	if (not theTreeHasNoLengths)
		theAnswer /= MesaTree::weight_type (theNumInternalNodes);

	// Postconditions & return:
	assert (0 <= theNumInternalNodes);
	assert (theNumInternalNodes = theTreeP->countInternalNodes() - 1);
	assert (0.0 <= theAnswer);

	if (theTreeHasNoLengths)
		MesaGlobals::mReporterP->printNotApplicable ("tree has no lengths");
	else if (theNumInternalNodes == 0)
		MesaGlobals::mReporterP->printNotApplicable ("tree too small");
	else
		MesaGlobals::mReporterP->print (theAnswer);
}

const char* StemminessAnalysis::describeAnalysis ()
{
	return "stemminess";
}


void ResolutionAnalysis::execute ()
//: calculate the resolution of the tree using Colless' 1980 measure
// Note that we check to see the tree is rooted here, which is moot in reality
// CHANGE:
// 01.6.18: now the root is correctly identified as an internal node
{
	ReporterPrefix	thePrefix ("resolution");

	MesaTree* theTreeP = getActiveTreeP();

	double theNumInternalBranches = theTreeP->countInternalNodes() - 1;
	int theCorrection;
	if (theTreeP->isTreeRooted())
		theCorrection = 2;
	else
		theCorrection = 3;
	double theMaxIntBranches = theTreeP->countLeaves() - theCorrection;

	if (theMaxIntBranches <= 0.0)
		MesaGlobals::mReporterP->printNotApplicable ("tree too small");
	else
		MesaGlobals::mReporterP->print (theNumInternalBranches / theMaxIntBranches);
}

const char* ResolutionAnalysis::describeAnalysis ()
{
	return "resolution";
}


void UltrametricAnalysis::execute ()
//: calculate that it is ultrametric within limits
{
	ReporterPrefix	thePrefix ("ultrametric");

	MesaTree* theTreeP = getActiveTreeP();
	if (theTreeP->countNodes () <= 1)
	{
		MesaGlobals::mReporterP->printNotApplicable ("tree too small");
		return;
	}

	vector <MesaTree::weight_type> theTipLengths;
	for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		if (theTreeP->isLeaf (q))
		{
			theTipLengths.push_back (theTreeP->getTimeFromNodeToRoot (q));
		}
	}

	// now we have a list of lengths tip to root
	MesaTree::weight_type theMin, theMax, theDiff;
	theMin = *(min_element (theTipLengths.begin(), theTipLengths.end()));
	theMax = *(max_element (theTipLengths.begin(), theTipLengths.end()));
	theDiff = (theMax - theMin) / theMax;

	if (theMin == theMax)
	{
		MesaGlobals::mReporterP->printNotApplicable ("tree has no lengths");
		return;
	}

	if (theDiff < 0.001)
		MesaGlobals::mReporterP->print (true);
	else
		MesaGlobals::mReporterP->print (false);
}


const char* UltrametricAnalysis::describeAnalysis ()
{
	return "ultrametricity";
}


// *** SITE ANALYSES *************************************************/

void SiteComplementarityAnalysis::execute ()
//: calculate that it is ultrametric within limits
{
	ReporterPrefix	thePrefix ("complementarity");

	// get the indices of the sites and tree leaves
	vector<colIndex_t> theSiteIndices;
	ContTraitMatrix* theContDataP = MesaGlobals::mContDataP;
	theContDataP->listSiteTraits (theSiteIndices);
	if (theSiteIndices.size() == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("no site abundance data");
		return;
	}

	vector<nodeiter_t> theLeaves;
	MesaTree* theTreeP = getActiveTreeP();
	theTreeP->getLeaves (theLeaves);
	if (theLeaves.size() == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("only one extant species");
		return;
	}

	// for every species
	long theTotalSpp, theTotalUniques;
	theTotalSpp = 0;
	theTotalUniques = 0;
	for (unsigned int i = 0; i < theLeaves.size(); i++)
	{
		long theSitesWithSpps = 0;
		for (unsigned int j = 0; j < theSiteIndices.size(); j++)
		{
			if (0 < getContData (theLeaves[i], theSiteIndices[j]))
				theSitesWithSpps++;
			if (2 <= theSitesWithSpps)
				break;
		}

		if (theSitesWithSpps == 1)
			theTotalUniques++;
		if (1 <= theSitesWithSpps)
			theTotalSpp++;
	}

	assert (theTotalUniques <= theTotalSpp);

	// collect abundances for each species & total abundance
	double theComp;
	if (theTotalUniques == 0)
		theComp = 0.0;
	else
		theComp = (double) theTotalUniques / (double) theTotalSpp;

	// print answer
	MesaGlobals::mReporterP->print (theComp);
}


const char* SiteComplementarityAnalysis::describeAnalysis ()
{
	return "species complementarity across sites";
}


// *** COMPARATIVE ANALYSES ******************************************/

void ComparativeAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("Stemminess");

	MesaTree* theTreeP = getActiveTreeP();

	double theAnswer = 0.0;
	int theNumInternalNodes = 0;

	// for every internal node:
	// divide the length of the branch by the age of it's parent
	// calculate the sum of these
	for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
	{
		if (theTreeP->isInternal (q))
		{
			theNumInternalNodes++;
			// get age of parent
			nodeiter_t theParentIter = theTreeP->getParent (q);
			MesaTree::weight_type theParAge = theTreeP->getNodeAge (theParentIter);
			// get branch length
			MesaTree::weight_type theBranchLen = theTreeP->getEdgeWeight (q);
			// get steminess of this node
			MesaTree::weight_type theStemminess = theBranchLen / theParAge;
			// add to running total
			theAnswer += theStemminess;
		}
	}

	// calculate results
	theAnswer /= MesaTree::weight_type (theNumInternalNodes);

	// Postconditions & return:
	assert (0 <= theNumInternalNodes);
	assert (0.0 <= theAnswer);

	// output results
	if (theNumInternalNodes == 0)
		MesaGlobals::mReporterP->printNotApplicable ("tree too small");
	else
		MesaGlobals::mReporterP->print (theAnswer);
}

const char* ComparativeAnalysis::describeAnalysis ()
{
	return "comparative analysis";
}


// *** UTILITY FUNCTIONS *************************************************/

BasicAnalysis* castAsAnalysis (BasicAction* iActionP)
//: return the parameter cast as an analysis is possible, otherwise nil
// Necessary if we can't get the dynamic cast to work as desired. May
// need to place ids in actions and test them.
{
	BasicAnalysis* theAnalysisP = NULL;
	theAnalysisP = dynamic_cast<BasicAnalysis*> (iActionP);
	return theAnalysisP;
}




// *** DEPRECATED & DEBUG ***********************************************/

/*
// *** FUSCO ALL
void FuscoAllAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("fusco imbalance");

	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// grab all the node ids from the tree
	nodeidvec_t  theNodeIds;
	theTreeP->getNodeVec (theNodeIds);

	// where we'll store the answers
	vector<string>	theLabels;
	vector<double>	theAnswers;
	vector<int> theSizes;

	// and a cache to save calculating answers repeatedly
	map<MesaTree::id_type, int>  theRichnessCache;

	// for each node:
	// if you get a sensible answer store it & the labels
	for (long j = 0; j < theNodeIds.size(); j++)
	{
		// grab all the node ids from the tree
		nodeidvec_t  theChildIds;
		theTreeP->getNodeChildrenVec (theNodeIds[j], theChildIds);

		// if not bifurcating
		if (theChildIds.size() != 2)
			continue;

		// else calculate the parameters
		long theBigTips, theSmallTips;
		if (mRichCol != kColIndex_None)
		{
			int			theResultVec[2];

			for (nodeidvec_t::size_type i = 0; i < 2; i++)
			{
				nodeidvec_t	theLeaveVec;
				theResultVec [i] = 0;
				theTreeP->collectLeaveIds (theChildIds[i], theLeaveVec);
				nodeidvec_t::iterator	q;
				for (q = theLeaveVec.begin(); q != theLeaveVec.end(); q++)
				{
					// find out the richness for this tip
					int theRichness;

					// if it not in the cache calculate it
					map<MesaTree::id_type, int>::iterator r;
					r = theRichnessCache.find (*q);
					if (r == theRichnessCache.end())
					{
						theRichness =	(int) MesaGlobals::mContDataP->getData ((theTreeP->getNodeLabel (*q)).c_str(), mRichCol);
						assert (theRichness == (int) getContData (
							(theTreeP->getNodeLabel (*q)).c_str(), mRichCol));
						assert (theRichness = getRichnessData (*q, mRichCol));
					}
					else
					{
						theRichness = r->second;
					}

					assert (0 <= theRichness);
					theResultVec [i] += theRichness;
				}
			}

			theBigTips = theResultVec[0];
			theSmallTips = theResultVec[1];
		}
		else
		{
			theBigTips = theTreeP->countLeaves (theChildIds[0]);
			theSmallTips = theTreeP->countLeaves (theChildIds[1]);
		}

		assert (0 < theBigTips);
		assert (0 < theSmallTips);
		long theTotalTips = theBigTips + theSmallTips;

		// if insufficient tips
		if (theTotalTips < 4)
			continue;

		double theImbalance;
		// if utterly balanced
		if (theBigTips == theSmallTips)
		{
			theImbalance = 0.0;
		}
		else
		{
			// otherwise calculate
			if (theBigTips < theSmallTips)
				swap (theBigTips, theSmallTips);

			long theMin = ceil (double (theTotalTips) / 2.0);
			long theMax = theTotalTips - 1;
			theImbalance = double (theBigTips - theMin) / double (theMax - theMin);
		}

		theLabels.push_back (theTreeP->getNodeLabel (theNodeIds[j]));
		theAnswers.push_back (theImbalance);
		if (mListSizes)
			theSizes.push_back (theTotalTips);
	}

	// if no meaningful answers produced
	if (theAnswers.size() == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("analysis not possible at any node");
		return;
	}

	// produce the answer string
	assert (theLabels.size() == theAnswers.size());
	MesaGlobals::mReporterP->print (theLabels, "node");
	MesaGlobals::mReporterP->print (theAnswers, "imbalance");
	if (mListSizes)
		MesaGlobals::mReporterP->print (theSizes, "node size");

}

string FuscoAllAnalysis::initDesc ()
{
	return string ("calculate Fusco imbalance");
}


// *** FUSCO WEIGHTED
void FuscoWeightedAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("weighted fusco imbalance");

	assert (MesaGlobals::mTreeDataP);
	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP ();

	// grab all the node ids from the tree
	nodeidvec_t  theNodeIds;
	theTreeP->getNodeChildrenVec (theTreeP->getRootId(), theNodeIds);

	// if not bifurcating
	if (theNodeIds.size() != 2)
	{
		MesaGlobals::mReporterP->printNotApplicable ("root is not bifuracting");
		return;
	}

	// collect the fusco index for all applicable nodes
	double	theTotalAnswer = 0.0;
	int 		theTotalLeaves = 0;
	for (nodeidvec_t::size_type i = 0; i < theNodeIds.size(); i++)
	{
		// double theAnswer = theTreeP->calcFusco(theNodeIds[i]);
		int  theNumTips = theTreeP->countLeaves(theNodeIds[i]);
		if (0 <= theNumTips)
		{
			//theTotalAnswer += theAnswer * theNumTips;
			theTotalLeaves += theNumTips;
		}
	}

	// produce the answer string
	assert (0.0 <= theTotalAnswer);
	assert (0 < theTotalLeaves);
	if (theTotalAnswer == 0.0)
		MesaGlobals::mReporterP->printNotApplicable ("unknown defect in tree");
	else
		MesaGlobals::mReporterP->print (theTotalAnswer / theTotalLeaves);
}

string FuscoWeightedAnalysis::initDesc ()
{
	return string ("calculate Fusco-plus imbalance");
}


// *** EXTENDED FUSCO
void FuscoExtendedAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("extended fusco imbalance");

	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// grab all the node ids from the tree
	nodeidvec_t  theChildIds;
	theTreeP->getNodeChildrenVec (theTreeP->getRootId(), theChildIds);

	// if not bifurcating
	if (theChildIds.size() < 2)
	{
		MesaGlobals::mReporterP->printNotApplicable ("root has fewer than 2 children");
		return;
	}

	// else calculate the parameters (theBiggestChild & the total)
	long theNumChildren = theChildIds.size();
	long theBigTips = 0;
	long theTotalTips = 0;

	// if richness column
	if (mRichCol != kColIndex_None)
	{
		// for every child of the root
		for (long i = 0; i < theNumChildren; i++)
		{
			// collect the richness of its leaves
			nodeidvec_t	theLeaveVec;
			theTreeP->collectLeaveIds (theChildIds[i], theLeaveVec);
			long theCurrRich = 0;
			nodeidvec_t::iterator	q;
			for (q = theLeaveVec.begin(); q != theLeaveVec.end(); q++)
			{
				int theRichness =	(int) MesaGlobals::mContDataP->getData ((theTreeP->getNodeLabel (*q)).c_str(), mRichCol);
				assert (theRichness == (int) getContData (
					(theTreeP->getNodeLabel (*q)).c_str(), mRichCol));
				assert (0 <= theRichness);
				theCurrRich += theRichness;
			}

			if (theBigTips < theCurrRich)
				theBigTips = theCurrRich;
			theTotalTips += theCurrRich;
		}
	}
	else
	// if no richness column
	{
		// for every child of the root
		for (long i = 0; i < theNumChildren; i++)
		{
			// count the leaves underneath
			long theCurrRich = theTreeP->countLeaves (theChildIds[i]);

			if (theBigTips < theCurrRich)
				theBigTips = theCurrRich;
			theTotalTips += theCurrRich;
		}
	}

	// so by now you have the total and the biggest subbranch
	assert (0 < theBigTips);
	assert (0 < theTotalTips);
	assert (theBigTips < theTotalTips);

	// calc min & max
	long theMax = theTotalTips + 1 - theNumChildren;
	long theMin = ceil (double (theTotalTips) / double (theNumChildren));

	// check for boundary conditions
	if (theMax == theMin)
		MesaGlobals::mReporterP->printNotApplicable ("insufficient leaves");
	if (theTotalTips < 4)
		MesaGlobals::mReporterP->printNotApplicable ("insufficient leaves");

	// otherwise it's good
	double theImbalance = double (theBigTips - theMin) / double (theMax - theMin);
	MesaGlobals::mReporterP->print (theImbalance);

}

string FuscoExtendedAnalysis::initDesc ()
{
	return string ("calculate extended Fusco imbalance");
}


// *** EXTENDED FUSCO ALL
void FuscoExtendedAllAnalysis::execute ()
{
	ReporterPrefix	thePrefix ("extended fusco imbalance");

	MesaTree* theTreeP = MesaGlobals::mTreeDataP->getActiveTreeP();

	// grab all the node ids from the tree
	nodeidvec_t  theNodeIds;
	theTreeP->getNodeVec (theNodeIds);

	// where we'll store the answers
	vector<string>	theLabels;
	vector<double>	theAnswers;
	vector<int> theSizes;

	// and a cache to save caclulating answers repeatedly
	map<MesaTree::id_type, int>  theRichnessCache;

	// for each node:
	// if you get a sensible answer store it & the labels
	for (long j = 0; j < theNodeIds.size(); j++)
	{
		// grab all the node ids from the tree
		nodeidvec_t  theChildIds;
		theTreeP->getNodeChildrenVec (theNodeIds[j], theChildIds);

		// if not bifurcating
		if (theChildIds.size() < 2)
			continue;

		// else calculate the parameters
		long theNumChildren = theChildIds.size();
		long theBigTips = 0;
		long theTotalTips = 0;

		// for every child of the current tip
		for (nodeidvec_t::size_type i = 0; i < theNumChildren; i++)
		{
			int theRichness = 0;

			// if you have a spp richness
			if (mRichCol != kColIndex_None)
			{
				nodeidvec_t	theLeaveVec;
				theTreeP->collectLeaveIds (theChildIds[i], theLeaveVec);
				nodeidvec_t::iterator	q;
				for (q = theLeaveVec.begin(); q != theLeaveVec.end(); q++)
					theRichness +=	(int) MesaGlobals::mContDataP->getData ((theTreeP->getNodeLabel (*q)).c_str(), mRichCol);
			}
			// if not
			else
			{
				theRichness = theTreeP->countLeaves (theChildIds[i]);
			}

			// accumulate the total of tips & check for the biggest
			assert (0 <= theRichness);
			theTotalTips += theRichness;
			if (theBigTips < theRichness)
				theBigTips = theRichness;
		}

		assert (0 < theBigTips);
		assert (theBigTips < theTotalTips);

		// if insufficient tips
		if (theTotalTips < 4)
			continue;

		// if utterly balanced
		if ((theBigTips * theNumChildren) == theTotalTips)
			continue;

		// otherwise calculate
		long theMin = ceil (double (theTotalTips) / 2.0);
		long theMax = theTotalTips - 1;
		double theImbalance = double (theBigTips - theMin) / double (theMax - theMin);

		theLabels.push_back (theTreeP->getNodeLabel (theNodeIds[j]));
		theAnswers.push_back (theImbalance);
		if (mListSizes)
			theSizes.push_back (theTotalTips);
	}

	// if no meaningful answers produced
	if (theAnswers.size() == 0)
	{
		MesaGlobals::mReporterP->printNotApplicable ("analysis not possible at any node");
		return;
	}

	// produce the answer string
	assert (theLabels.size() == theAnswers.size());
	MesaGlobals::mReporterP->print (theLabels, "node");
	MesaGlobals::mReporterP->print (theAnswers, "imbalance");
	if (mListSizes)
		MesaGlobals::mReporterP->print (theSizes, "node size");

}

string FuscoExtendedAllAnalysis::initDesc ()
{
	return string ("calculate extended Fusco imbalance");
}

*/

// *** END ***************************************************************/



