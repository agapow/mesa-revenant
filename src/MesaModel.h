/**************************************************************************
MesaModel.h - the Mesa domain model and data structures

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- An encapsulation of the Mesa engine.
- Ideally this should be completely divorcable from any interface or
  application loop so as to be portable to other platforms and other
  modes of use (e.g client/server).

**************************************************************************/

#pragma once
#ifndef MESAMODEL_H
#define MESAMODEL_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include "TaxaTraitMatrix.h"
#include "TreeWrangler.h"
#include "MesaGlobals.h"
#include "ActionQueue.h"
#include "Reporter.h"
#include <fstream>


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

class MesaModel
{
public:
	// LIFECYCLE
	MesaModel (progcallback_t& ikProgressCb)
		: mReporter (ikProgressCb)
		, mFilePath ("")
		, mProgressCb (ikProgressCb)
	{
		// set up the access for actions & tasks
		MesaGlobals::mDiscDataP = &mDiscData;
		MesaGlobals::mContDataP = &mContData;
		MesaGlobals::mTreeDataP = &mTreeData;	
		MesaGlobals::mReporterP = &mReporter;
	}

	~MesaModel ()
	{}
	
	// ACCESSORS
	int countTrees ();
	int countContTraits ();
	int countDiscTraits ();
	int countSiteTraits ();
	int countTaxa ();
	
	bool isSiteTrait (colIndex_t iIndex);
	
	std::string getSiteName (int iIndex);
	std::string getTraitName (traittype_t iDataType, int iIndex);

	CharStateSet& getDiscStates ()
		{ return mDiscData.mStates; }
	
	// MUTATORS
	void	selectTree (int iTreeIndex);
	void	duplicateActiveTree ();
	void	deleteTree (int iTreeIndex);
	void	deleteChar (traittype_t iDataType, int iIndex);
	void	newDiscChar (const char*  iNewVal);
	void	newContChar (double iNewVal);
	void	seedTree ();
	
	void backupData ();
	void restoreData ();
	
	// I/O		
	void exportData (std::ostream* theOutStream, traittype_t theDataType, nodetype_t theTipType);			
	fileformat_t sniffFormat (std::ifstream& iInStream);
	void	readModel	(std::ifstream& iInStream, std::string& ikFileName);
	void  readCaic (std::ifstream& iInStream,  const std::string& ikFileName);
	void	readNexus (std::ifstream& iInStream);
	void	writeNexus	(std::ofstream& iOutStream);
	void	writeCaic	(std::ostream* iPhylStrm, std::ostream* iBlenStrm, std::ostream* iDataStrm);
	void	importTab	(std::ifstream& theInStream);
	void	importRich	(std::ifstream& theInStream);
	
	// REPORTING
	void 	summarise	(std::ostream& ioOutStream);
	void 	detailedReport	(std::ostream& ioOutStream);
	
	// PUBLIC DATA
	Reporter						mReporter;
	// the data containers
	ContTraitMatrix			mContData;
	DiscTraitMatrix			mDiscData;
	TreeWrangler				mTreeData;
	std::string					mFilePath;
	
	// DEPRECIATED & DEBUG
	void	validate	();

	ActionQueue					mActionQueue;
	
	// INTERNALS
private:
	progcallback_t				mProgressCb;
	
	
	// I/O
	void mergeContData		(stringmatrix_t& iNewData);
	void mergeDiscreteData	(stringmatrix_t& iNewData);
	
	// for backup
	TreeWrangler				mBackupTreeData;
	ContTraitMatrix			mBackupContData;
	DiscTraitMatrix			mBackupDiscData;
};



#endif
// *** END ***************************************************************/



