/**************************************************************************
ManipAction.h - an action that alters the data system

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef MANIPACTION_H
#define MANIPACTION_H


// *** INCLUDES

#include "Action.h"
#include "ActionUtils.h"
#include "SblDebug.h"
#include "MesaTree.h"
#include "CharComparator.h"
#include "MesaGlobals.h"
#include "EvolRule.h"
#include "TaxaTraitMatrix.h"


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

// *** RUN MACRO

class ManipAction: public BasicAction
{
public:
	// LIFECYCLE
	ManipAction ()
		{}

	// SERVICES
	void				execute ()
	{ executeManip(); }

	virtual void	executeManip () = 0;
	size_type deepSize ()
		{ return 1; }
	void deleteElement (size_type iIndex)
		{ assert (false); iIndex = iIndex; }
	const char* describe (size_type iIndex)
		{ assert (iIndex == 0); return describeManipAction (); }
	virtual const char* describeManipAction () = 0;
};


/**************************************************************************/

/**
An action to depopulate targetted sites.

This was added not because of any user demand but mostly because the
functionality was needed by several modules.
*/
class RazeSiteAction: public ManipAction
{
public:
		RazeSiteAction (colIndex_t iSite)
		{
			mSites.push_back (iSite);
		}
		RazeSiteAction (colIndexArr_t iSiteArr)
		{
			mSites = iSiteArr;
		}

		void executeManip ()
		{
			colIndex_t theNumTaxa = MesaGlobals::mContDataP->countTaxa();
			colIndex_t theNumCols = MesaGlobals::mContDataP->countCols();

			// for every site indicated ...
			for (colIndexArr_t::iterator p = mSites.begin();
					p != mSites.end(); p++)
			{
				assert (0 <= *p);
				assert (*p < theNumCols);

				// ... for every taxa, raze it to one
				for (colIndex_t i = 0; i < theNumTaxa; i++)
					MesaGlobals::mContDataP->at (i, *p) = 0.0;
			}
		}

private:
	colIndexArr_t   mSites;

	 /** @todo   Finish description of action. */
	const char* describeManipAction ()
	{return ""; }

};


/**
Should the path to the root be pruned?

In the case of pruning there is an awkward problem when taxa are pruned so as
leave the root of the tree with only one descendent (i.e. the root node becomes
a singleton). In this case do you trim the path to the root (thus re-rooting
the tree) or do you leave it? On one hand, the surviving taxa still preserve
the root branch in their relationship with some unidentified external taxa. In
case of PD this also means that a single surving taxa still has some worth /
information value (sensu Wilson). On the other hand, sensu Crozier, it could be
argued that a single taxa has no diversity (variance), and therefore the path
to root should be trimmed (as the presence of, say, frogs does not raise the
importance of a surving bird population). Hence we provide a flag for the
pruning operations to choose either option.
*/
typedef bool pruneLeaveRootPath_t;


class PruneAction: public ManipAction
{
public:
	// LIFECYCLE
	PruneAction (pruneLeaveRootPath_t iLeaveRootPath = true)
		: mLeaveRootPath (iLeaveRootPath)
		{}

	// SERVICES
	/**
	Actually do the manipulation (i.e. prune the tree).

	This is where most of the action actually occurs. It calls the
	abstract function selectTargets() (provided by derived classe)
	to select the nodes, actually kills them, then prunes the tree
	down. All the various prune functions only differ in how the
	nodes are selected.

	@todo   move implementation out of header file
	*/
	void executeManip ()
	{
		// Preconditions:
		MesaTree* theTreeP = getActiveTreeP();
		assert (theTreeP != NULL);
		theTreeP->validate();

		// Main:
		// gather nodes to be pruned ...
		nodearr_t   theTargetNodes;
		selectTargets (theTargetNodes);

		for (nodearr_t::iterator q = theTargetNodes.begin();
		   q != theTargetNodes.end (); q++)
			theTreeP->pruneBranch (*q);


		if (mLeaveRootPath == false)
		{
			nodeiter_t theRootIter = theTreeP->getRoot();
			while (theTreeP->countChildren (theRootIter) == 1)
			{
				nodeiter_t theChildIter = theTreeP->getChild (theRootIter, 0);
				theTreeP->deleteNode (theRootIter);
				theTreeP->setRoot (theChildIter);
				theRootIter = theChildIter;
			}
		}

		/*
		// ... kill them ...
		for (nodearr_t::iterator q = theTargetNodes.begin();
		   q != theTargetNodes.end (); q++)
			theTreeP->killLeaf (*q);

		// ... delete their subtrees ...
		while (theTreeP->countLeaves () != theTreeP->countAliveLeaves ())
		{
			for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end();)
			{
				nodeiter_t theCurrNode = q;
				q++;
				if (theTreeP->isLeaf (theCurrNode) and
				    (not theTreeP->isNodeAlive (theCurrNode)))
				{
					theTreeP->pruneLeaf (theCurrNode);
					theTreeP->validate();
				}
			}
		}
		*/
		// Postconditions:
		assert (theTreeP->countLeaves() == theTreeP->countAliveLeaves ());
		theTreeP->validate();
	}

	virtual void selectTargets (nodearr_t& oTargetNodes) = 0;

	// INTERNALS
private:
	pruneLeaveRootPath_t mLeaveRootPath;
	const char* describeManipAction ()
	{ /** @todo */ return ""; }
};


/**
An action to prune the phylogeny based on abundance.

Once again, this was added not because of any user demand but mostly because
the functionality was needed by several modules. This examines the site
abundance data and - if it's 0 - prunes the active phylogeny.

@todo Maybe this should accept specific sites.

*/
class PruneByAbundanceAction: public PruneAction
{
public:
	PruneByAbundanceAction (pruneLeaveRootPath_t iLeaveRootPath = true)
		: PruneAction (iLeaveRootPath)
		{}

	// SERVICES
	void selectTargets (nodearr_t& oTargetNodes)
	{
		// select taxa with no individuals left
		ContTraitMatrix* theTraitsP = MesaGlobals::mContDataP;
		colIndex_t theNumTraits = theTraitsP->countCols();
		nodearr_t  theTips;
		MesaTree* theTreeP = getActiveTreeP();
		theTreeP->getLeaves (theTips);
		// DBG_VAL (theTreeP->countLeaves());


		// for every taxa ...
		for (nodearr_t::size_type i = 0; i < theTips.size(); i++)
		{
			nodeiter_t theNodeIt = theTips[i];
			bool theTaxaIsDead = true;

			for (colIndex_t j = 0; j < theNumTraits; j++)
			{
				/*
				DBG_MSG ("node i: " << i << "; col j: " << j);
				DBG_MSG ("name: " << theTreeP->getNodeName (theNodeIt));
				std::cout.flush();
				DBG_MSG ("getcontdata i,j: " << getContData (theNodeIt, j));
				*/
				if (theTraitsP->isSiteTrait (j) and (0.0000 < getContData (theNodeIt, j)))
				{
					theTaxaIsDead = false;
					break;
				}
			}

			if (theTaxaIsDead == true)
				oTargetNodes.push_back (theNodeIt);
		}

		// DBG_VAL (oTargetNodes.size());
	}

	// INTERNALS
private:
	const char* describeManipAction ()
	{
		static char theBuffer[64];
		std::sprintf (theBuffer, "prune (taxa with 0 abundance)");
		return theBuffer;
	}
};


// *** *****************************************/

/// mass prune (fixed number)
class PruneFixedNumAction: public PruneAction
{
public:
	// LIFECYCLE
	PruneFixedNumAction (int iKillNum, pruneLeaveRootPath_t iLeaveRootPath = true)
		: PruneAction (iLeaveRootPath), mKillNum (iKillNum)
		{}


	// SERVICES
	void selectTargets (nodearr_t& oTargetNodes)
	{
		// get the live nodes, shuffle them and take the first N
		MesaTree* theTreeP = getActiveTreeP ();
		theTreeP->getLiveLeaves (oTargetNodes);
		random_shuffle (oTargetNodes.begin(), oTargetNodes.end());
		if ((unsigned int) mKillNum < oTargetNodes.size())
			oTargetNodes.resize (mKillNum);
	}

	// INTERNALS
private:
	const char* describeManipAction ()
	{
		static char theBuffer[64];
		std::sprintf (theBuffer, "prune (kill %i taxa)", mKillNum);
		return theBuffer;
	}

	int   mKillNum;
};

/// mass prune (fixed fraction)
class PruneFixedFracAction: public PruneAction
{
public:
	// LIFECYCLE
	PruneFixedFracAction (double iPercent, pruneLeaveRootPath_t iLeaveRootPath = true)
		: PruneAction (iLeaveRootPath), mKillFrac (iPercent)
		{}

	// SERVICES
	void selectTargets (nodearr_t& oTargetNodes)
	{
		MesaTree* theTreeP = getActiveTreeP ();
		theTreeP->getLiveLeaves (oTargetNodes);
		nodearr_t::size_type theKillNum = nodearr_t::size_type (oTargetNodes.size() * mKillFrac);
		random_shuffle (oTargetNodes.begin(), oTargetNodes.end());
		oTargetNodes.resize (theKillNum);
	}

	// INTERNALS
private:
	const char* describeManipAction ()
	{
		static char theBuffer[64];
		std::sprintf (theBuffer, "prune (%.4g%% of taxa)", mKillFrac * 100);
		return theBuffer;
	}

	double   mKillFrac;
};

/// mass prune (fixed probability)
class PruneByProbAction: public PruneAction
{
public:
	PruneByProbAction (double iProb, pruneLeaveRootPath_t iLeaveRootPath = true)
		: PruneAction (iLeaveRootPath), mProb (iProb)
		{}

	// SERVICES
	void selectTargets (nodearr_t& oTargetNodes)
	{
		// get the live nodes, shuffle them and take the first N percent
		MesaTree* theTreeP = getActiveTreeP ();
		theTreeP->getLiveLeaves (oTargetNodes);
		nodearr_t::iterator q;
		for (q = oTargetNodes.begin(); q != oTargetNodes.end(); )
		{
			if (MesaGlobals::mRng.UniformFloat () <= mProb)
				q++;
			else
				oTargetNodes.erase (q);
		}
	}

	// INTERNALS
private:
	const char* describeManipAction ()
	{
		static char theBuffer[64];
		std::sprintf (theBuffer, "prune (%.4g%% of pruning any taxa)", mProb);
		return theBuffer;
	}

	double   mProb;
};

/// mass prune (prob biased by trait)
class PruneByTraitAction: public PruneAction
{
public:
	// LIFECYCLE
	PruneByTraitAction (colIndex_t iTraitCol, double iParamA,
	   double iParamB, double iParamC, pruneLeaveRootPath_t iLeaveRootPath = true)
		: PruneAction (iLeaveRootPath)
		, mTraitIndex (iTraitCol)
		, mTriParamA (iParamA)
		, mTriParamB (iParamB)
		, mTriParamC (iParamC)
		{}

	// SERVICES
	void selectTargets (nodearr_t& oTargetNodes)
	{
		MesaTree* theTreeP = getActiveTreeP ();
		theTreeP->getLiveLeaves (oTargetNodes);
		nodearr_t::iterator q;
		for (q = oTargetNodes.begin(); q != oTargetNodes.end(); )
		{
			conttrait_t theTraitVal = getContData (*q, mTraitIndex);
			double theProb = calcProbFromTriParameter (mTriParamA, mTriParamB,
				mTriParamC, theTraitVal);

			if (MesaGlobals::mRng.UniformFloat () <= theProb)
				q++;
			else
				oTargetNodes.erase (q);
		}
	}

	// INTERNALS
private:
	const char* describeManipAction ()
	{
		static char theBuffer[64];
		std::sprintf (theBuffer,
			"prune (biased by trait %i, params %.4g, %.4g, %.4g)",
			mTraitIndex, mTriParamA, mTriParamB, mTriParamC);
		return theBuffer;
	}

	colIndex_t   mTraitIndex;
	double       mTriParamA;
	double       mTriParamB;
	double       mTriParamC;
};

/// mass prune (taxa that meet a set condition)
class PruneIfAction: public PruneAction
{
public:
	// LIFECYCLE
	PruneIfAction (CharComparator& iSppTest, pruneLeaveRootPath_t iLeaveRootPath = true)
		: PruneAction (iLeaveRootPath), mSppTest (iSppTest)
		{}

	// SERVICES
	void selectTargets (nodearr_t& oTargetNodes)
	{
		// get the live nodes, shuffle them and take the first N percent
		MesaTree* theTreeP = getActiveTreeP ();
		theTreeP->getLiveLeaves (oTargetNodes);
		nodearr_t::iterator q;
		for (q = oTargetNodes.begin(); q != oTargetNodes.end(); )
		{
			if (mSppTest.testCharacter (*q))
				q++;
			else
				oTargetNodes.erase (q);
		}
	}

	// INTERNALS
private:
	const char* describeManipAction ()
	{
		static char theBuffer[64];
		std::sprintf (theBuffer, "prune (taxa %s)", mSppTest.describe());
		return theBuffer;
	}

	CharComparator   mSppTest;
};


/// mass prune (on a list of names)
class PruneByName: public PruneAction
{
public:
	// LIFECYCLE
	PruneByName (std::vector<std::string> iNames, pruneLeaveRootPath_t iLeaveRootPath = true)
		: PruneAction (iLeaveRootPath), mNames (iNames)
		{}

	// SERVICES
	void selectTargets (nodearr_t& oTargetNodes)
	{
		MesaTree* theTreeP = getActiveTreeP ();
		for (std::vector<std::string>::size_type i = 0; i < mNames.size(); i++)
		{
			nodeiter_t theNode = theTreeP->getIter (mNames[i].c_str());
			if ((theNode != theTreeP->end()) and theTreeP->isLeaf (theNode))
				oTargetNodes.push_back (theNode);
		}
	}

	// INTERNALS
private:
	const char* describeManipAction ()
	{
		static char theBuffer[64];
		std::sprintf (theBuffer, "prune (a list of taxa names)");
		return theBuffer;
	}

	std::vector<std::string> mNames;
};


#endif
// *** END ***************************************************************/



