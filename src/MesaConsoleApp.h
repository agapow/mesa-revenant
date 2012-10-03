/**************************************************************************
MesaConsoleApp.h - a simple console application wrapper for the MESA model

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- All fairly self obvious.

Changes:
- (00.9.20) Changed to derive from the more compact ConsoleMenuApp.

**************************************************************************/

#pragma once
#ifndef MESACONSOLEAPP_H
#define MESACONSOLEAPP_H


// *** INCLUDES

#include "ConsoleMenuApp.h"
#include "CommandMgr.h"
#include "MesaTypes.h"
#include "Epoch.h"
#include "CharEvolScheme.h"
#include "CharComparator.h"
#include "CharStateSet.h"
#include "XRate.h"
#include <fstream>


// *** CONSTANTS & DEFINES

class MesaModel;
class BasicAction;
class BasicMacro;
class NodeSelector;

// *** CLASS DECLARATION *************************************************/

class MesaConsoleApp: public sbl::ConsoleMenuApp
{
public:
	// Lifecycle
	MesaConsoleApp		();
	~MesaConsoleApp 	();

	// Services		
	void	LoadMenu ();								// obligatory override
	bool	UpdateCmd (sbl::cmdId_t iCmdId );	// obligatory override
	void	ObeyCmd (sbl::cmdId_t iCmdId );		// obligatory override
				
	// Depreciated & Debug
	void   validate ();
	void   doDEBUG ();

	// Internals
private:
	// Members
	MesaModel*					mModel;
	std::ofstream*				mLogStreamP;
	
	// Commands
	void 	doClose ();
	void	doSave ();
	void	doLoadFile ();
	void	doSummary ();
	void  doExport ();
	void	doImport ();
	void	doImportRich ();
	void	doDistill ();

	// submenus
	void	doPrefsMenu ();
	void	doAnalysisMenu ();
	void	doManipMenu ();
	void	doQuickActionMenu ();

	// action menu
	void	doTaskMenu ();
	void	doTaskMacroMenu (BasicMacro* iMacroP);
	void	doTaskEpochMenu (EpochMacro* iEpochP);
	void	doRunQueue (sbl::cmdId_t iUserCmd);
	void  doProgramQueue ();
	
	void	loadQueueCmds (sbl::CommandMgr& ioCommands);
	void	loadAnalysisCmds (sbl::CommandMgr& ioCommands);
	void	loadMacroCmds (sbl::CommandMgr& ioCommands);
	void	loadSystemCmds (sbl::CommandMgr& ioCommands);
	void	loadRuleCmds (sbl::CommandMgr& ioCommands);
	
	BasicAction*	newAction (sbl::cmdId_t iUserCmd);
	EvolRule* 		newRule (sbl::cmdId_t iUserCmd);

	// prefs settings
	void	doPrefsVerbosity ();
	void	doPrefsLogging ();
	void	doPrefsCase ();
	void	doPrefsAnalysis ();

	// manip fxns
	void	doDetailedReport ();
	void	doSelectTree ();
	void	doDuplicateTree ();
	void	doDeleteTree ();
	void	doNewTrait ();
	void	doDeleteTrait ();		
	void	doNewFile ();
	
	// experimental functions
	void  doExperimentalMenu ();
	colIndex_t  askForSite (const char* ikPrompt);
	void  askForSites (std::vector<colIndex_t>& oSiteIndexes, const char* ikPrompt);
	void  doXPruneSites ();

	// Utility Functions
	void	deleteModel ();
	void	newModel ();
	void	updateCommandMgr (sbl::CommandMgr& ioManager);
	bool	doesFileExist (const char* iFilePath);

	//bool	askAndOpenOutFile (const char* iPromptCstr, std::ofstream& iOutFstrm);
	bool	askAndOpenReportFile (const char* iPromptCstr, std::ofstream& iOutFstrm);
	
	// IO functions
	const char* askAndOpenOutFile (std::ofstream& iOutFstrm, const char* iPromptCstr,
		const char* iDefPath = "");
		
	bool	askAndOpenInFile (const char* iPromptCstr, std::ifstream& iInFstream);
	int	askContTraitCol (const char* iPrompt);
	int   askDiscTraitCol (const char* iPrompt);
	bool	askIntegerOrNone (const char* iPromptCStr, int iMin, int iMax, int& iRetValue);
	int	askContCol (bool iAnswer = true);
	int	askDiscCol (bool iAnswer = true);

	CharComparator askSppTest (const char* iPrompt = "");
	NodeSelector* askNodeSelector (const char* iPrompt = "");
	void askSelectTrait (traittype_t& ioCharType, colIndex_t& ioCharIndex);
	
	void	askForSchemes (SchemeArr& ISchemeArr, const char* iPrompt);
	CharStateSet askForStates (const char* iPrompt);

	int	askSppRichnessCol ();
	void 	askRate (double& iFreq);
	void 	askRate (double& iFreqA, double& iFreqB, double& iFreqC, const char* iPromptCstr = NULL);

	void	Report (const char* iMsg);
	void	ReportError (const char* iMsg);

	void	callbackReport (msg_t iMessageType, const char* ikMsg);
	void	printReport (const char* ikMsg);
	void	printComment (const char* ikMsg);
	void	printAnalysis (const char* ikMsg);
	void	printLog (const char* ikMsg);
	
	// New / development
	XBasicRate* askRate_New ();
	void askDependentVar (dependentVar_t& oValueType, int& oValueQualifier);
	void sayMsg (const char* ikMsg);
	
	
};


#endif
// *** END ***************************************************************/



