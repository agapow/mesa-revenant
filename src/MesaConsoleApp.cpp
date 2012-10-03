/**************************************************************************
MesaConsoleApp.cpp - a simple console application wrapper for the MESA model

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK) Silwood
  Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- This will 

Changes:
- (99.7.10) Created.
- (99.7.10) recrafted so as to inherit from ConsoleMenuApp and thus pass
  off most of the book-keeping.

To Do:
- Exceptions?

**************************************************************************/


// *** INCLUDES

#include "MesaConsoleApp.h"
#include "MesaVersion.h"
#include "MesaModel.h"
#include "MesaGlobals.h"
#include "Error.h"
#include "Sbl.h"
#include "StringUtils.h"
#include "Analysis.h"
#include "SystemAction.h"
#include "CharEvolRule.h"
#include "ComboMill.h"
#include "Action.h"
#include "Macro.h"
#include "ResultsDistiller.h"
#include "MesaCommands.h"
#include "ManipAction.h"
#include "XBounds.h"
#include <fstream>
#include <exception>
#include <sstream>
#include <iterator>
#include <cstdlib>
#include "callback.hpp"
#include <cmath>
#include <ctime>
#include <limits>

using sbl::cmdId_t;
using sbl::Split;
using sbl::StripFlankingWhitespace;
using sbl::eraseFlankingSpace;
using sbl::MissingFileError;
using sbl::FileOpenError;
using sbl::CommandMgr;
using sbl::kCmd_Quit;
using sbl::kCase_Upper;
using sbl::kCase_Lower;
using sbl::kCase_Mixed;
using sbl::isMemberOf;
using std::exception;
using std::ofstream;
using std::ifstream;
using std::bad_alloc;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::stringstream;
using std::string;
using std::back_insert_iterator;
using sbl::XBounds;
using sbl::kAnswerBounds_None;
using sbl::ComboMill;


// *** CONSTANTS & DEFINES

const bool kAnswer_Required = true;
const bool kAnswer_NotRequired = false;


// *** MAIN BODY *********************************************************/


// *** LIFECYCLE *********************************************************/


MesaConsoleApp::MesaConsoleApp ()
//: Ctor that inits all members and user/about info
{
	// be safe, init all members
	mModel = NULL;
	mLogStreamP = NULL;
	
	// info for about box
	mAppTitle = kMesaAppTitle;
	mAppVersion = kMesaVersion;
	mAppCredits = kMesaAuthor;
	
	mAppInfo.push_back(string(kMesaAuthorAddress));
	mAppInfo.push_back(string(kMesaAuthorEmail));
	mAppInfo.push_back(string(kMesaAuthorWeb));
	mAppInfo.push_back(string(kMesaVersionDate));
	
	SetPromptWidth (63);
	
	int theDefaultSeed = std::time (NULL) % std::numeric_limits<int>::max();
	std::srand(theDefaultSeed);
}


MesaConsoleApp::~MesaConsoleApp ()
{
	// clean up after yrself
	delete mModel;


	if (MesaGlobals::mPrefs.mLogging == kPrefLogging_Enabled)
	{
		assert (mLogStreamP != NULL);
		assert (mLogStreamP->is_open());
		mLogStreamP->close();
	}
}


// *** EVENT LOOP ********************************************************/


void MesaConsoleApp::LoadMenu ()
//: Load the commands that can be used at the start
{
	mMainCommands.mTitle = "File Menu";
	
	mMainCommands.AddCommand (kCmd_OpenFile, 'o', "Open a Nexus or Caic file");
	mMainCommands.AddCommand (kCmd_NewFile, 'n', "Create a new tree & data file");
	mMainCommands.AddCommand (kCmd_Import, 'm', "Import data into current file");
	mMainCommands.AddCommand (kCmd_ImportRich, "mr", "Import species richness data");
	mMainCommands.AddCommand (kCmd_SaveFile, 's', "Save the current file");
	mMainCommands.AddCommand (kCmd_CloseFile, 'w', "Close the current file");

	mMainCommands.AddCommand (kCmd_SysActionMenu, 'd', "Data Manipulation Menu ...");
	mMainCommands.AddCommand (kCmd_ExportData, 'e', "Export trait data");
	mMainCommands.AddCommand (kCmd_AnalysisMenu, 'a', "Instant Analysis Menu ...");
	mMainCommands.AddCommand (kCmd_TaskMenu, 'g', "Program and Run Queue Menu ...");

	mMainCommands.AddCommand (kCmd_Summarise, 'i', "Summarise file contents");
	mMainCommands.AddCommand (kCmd_Distill, 't', "Distill results file");
	mMainCommands.AddCommand (kCmd_PrefsMenu, 'f', "Preferences Menu ...");
	
	mMainCommands.AddCommand (kCmd_DEBUG, 'x', "Experimental");

	mMainCommands.AddCommand (sbl::kCmd_Quit, 'q', "Quit");	
	
	mMainCommands.SetConvertShortcut (false);
	SetCmdVisibility (false);
}


void MesaConsoleApp::ObeyCmd (cmdId_t iCmdId)
{
	try
	{
		cout << endl;
		switch (iCmdId)
		{
			case kCmd_DEBUG:
				doExperimentalMenu();
				break;
				
			// files
			case kCmd_OpenFile:
				doLoadFile ();
				break;

			case kCmd_NewFile:
				doNewFile ();
				break;

			case kCmd_SaveFile:
				doSave ();
				break;
			
			case kCmd_SysActionMenu:
				doManipMenu ();
				break;
								
			case kCmd_CloseFile:
				doClose ();
				break;

			case kCmd_ExportData:
				doExport ();
				break;
				
			case kCmd_Import:
				doImport ();
				break;

			case kCmd_ImportRich:
				doImportRich ();
				break; 

			case kCmd_Distill:
				doDistill ();
				break; 

			// submenu
			case kCmd_PrefsMenu:
				doPrefsMenu ();
				break;

			case kCmd_AnalysisMenu:
				doAnalysisMenu ();
				break;
				
			case kCmd_TaskMenu:
				doTaskMenu ();
				break;

			// prefs
				
			case kCmd_PrefVerbosity:
				doPrefsVerbosity ();
				break;

			case kCmd_PrefLogging:
				doPrefsLogging ();
				break;
			
			case kCmd_PrefCase:
				doPrefsCase ();
				break;

			
			case kCmd_PrefAnalysisOut:
				doPrefsAnalysis();
				break;
				
				
			// manipulation
			case kCmd_SelectTree:
				doSelectTree ();
				break;

			case kCmd_DuplicateTree:
				doDuplicateTree ();
				break;					

			case kCmd_DeleteTree:
				doDeleteTree ();
				break;					

			case kCmd_NewTrait:
				doNewTrait ();
				break;					

			case kCmd_DeleteTrait:
				doDeleteTrait ();
				break;					

			case kCmd_Summarise:
				doSummary ();
				break;

			case kCmd_DetailedReport:
				doDetailedReport ();
				break;

			default:
				DBG_MSG ("Recieved unknown cmd " << iCmdId);
				assert (false);
				break;
		}
	}
	catch (bad_alloc &theAllocError)
	{
		ReportError ("Memory allocation failed");
		Report ("Increase the memory alloted to this program.");
	}
	catch (exception &theException)
	{
		ReportError (theException.what());
	}
	catch (...)
	{
		ReportError ("Unidentified error");
	}		
}


bool MesaConsoleApp::UpdateCmd (cmdId_t iCmdId)
{
	bool theModelIsPresent = (mModel != NULL);
	bool theModelHasTrees = theModelIsPresent and (0 < mModel->countTrees());
	bool theModelHasMultipleTrees = theModelIsPresent and (1 < mModel->countTrees());
	bool theQueueHasActions = theModelIsPresent and (not mModel->mActionQueue.isEmpty());
	bool theModelHasTraits = theModelIsPresent and  (0 < (mModel->countDiscTraits() + mModel->countContTraits()));
	bool theModelHasContTraits = theModelHasTraits and (mModel->countContTraits() != 0);
	bool theModelHasDiscTraits = theModelHasTraits and (mModel->countDiscTraits() != 0);
	bool theModelHasSiteData = theModelHasTraits and (mModel->countSiteTraits() != 0);
	
	switch (iCmdId)
	{
		// it is always possible to quit & set prefs
		case kCmd_Return:
		case sbl::kCmd_Quit:
		case kCmd_Distill:
			return true;
			break;
		
		// loading is only allowed if there is no file already present
		case kCmd_NewFile:
		case kCmd_OpenFile:
			if (not theModelIsPresent)
				return true;
			else
				return false;
			break;

		// closing is only allowed if there is a model loaded
		case kCmd_RuleBiasedTraitSp_New:		
		case kCmd_PrefsMenu:
		case kCmd_PrefVerbosity:
		case kCmd_PrefLogging:
		case kCmd_PrefCase:
		case kCmd_PrefPadMatrix:
		case kCmd_PrefAnalysisOut:
		case kCmd_PrefSetRandSeed:
		case kCmd_PrefSetCladeLabels:
		case kCmd_PrefWriteTranslation:
		case kCmd_PrefWriteTaxa:
		case kCmd_PrefPreserveNodes:
		case kCmd_Summarise:
		case kCmd_DetailedReport:
		case kCmd_Import:
		case kCmd_ImportRich:
		case kCmd_SaveFile:
		case kCmd_CloseFile:
		case kCmd_NewTrait:
		case kCmd_RulePruneFixedNum:
		case kCmd_RulePrunFixedFrac:
		case kCmd_RulePrunProb:
		case kCmd_RulePrunIf:
			if (theModelIsPresent)
				return true;
			else
				return false;
			break;

		case kCmd_RuleBiasedTraitSp:		
		case kCmd_RuleBiasedTraitKill:
		case kCmd_RuleMassKillBiasedTrait:
		case kCmd_RulePrunByTrait:
			if (theModelHasContTraits)
				return true;
			else
				return false;
			break;
			
		case kCmd_RuleSymSpecTraitEv:
		case kCmd_RuleAsymSpecTraitEv:
		case kCmd_RuleGradualTraitEv:
		case kCmd_RuleTerminalTraitEv:
		case kCmd_RuleMassKillIf:
		case kCmd_ExportData:
		case kCmd_DeleteTrait:
		case 	kCmd_SysActionShuffleTraits:
			if (theModelHasTraits)
				return true;
			else
				return false;
			break;

		// if there are trees can duplicate and do analysis
		case kCmd_DeleteTree:
			if (theModelHasMultipleTrees)
				return true;
			else
				return false;
			break;
		
		case kCmd_RuleMassKillFixedNum:
		case kCmd_RuleMassKillPercent:
		case kCmd_RuleMassKillProb:
		case kCmd_SysActionMenu:
		case kCmd_AnalysisMenu:
		case kCmd_TaskMenu:
		case kCmd_DuplicateTree:
		case kCmd_SysActionDeleteDeadTaxa:
		case kCmd_SysActionCollapseSingletonsTaxa:
		case kCmd_SysActionMakeNeont:
		case kCmd_SysActionSetPreservation:
		case kCmd_RuleNull:		
		case kCmd_RuleMarkovSp:
      case kCmd_RuleLogisticSp:
      case kCmd_RuleLogisticKill:
		case kCmd_RuleLatentSp:
		case kCmd_RuleBiasedSp:
		case kCmd_RuleMarkovKill:
		case kCmd_RuleBiasedKill:
			if (theModelHasTrees)
				return true;
			else
				return false;
			break;
		
		case kCmd_SysActionDeleteDeadTraits:
			if (theModelHasTrees and theModelHasTraits)
				return true;
			else
				return false;
			break;
			
		// allow consolidation of tree only if there is no data and the
		// sizes have the same number of taxa
		case kCmd_SysActionConsolidateTaxa:
			if ((theModelHasMultipleTrees) and (not theModelHasTraits))
			{
				TreeWrangler::iterator q = mModel->mTreeData.begin();
				int theTreeSize = q->countLeaves();
				q++;
				for (; q != mModel->mTreeData.end(); q++)
				{
					if (q->countLeaves() != theTreeSize)
						return false;
				}
				return true;		
			}
			return false;
			break;
			
		// the action commands require at least one tree
		case kCmd_AnalExTaxa:
		case kCmd_AnalAllTaxa:
		case kCmd_AnalGeneticDiv: 		
		case kCmd_AnalPhyloDiv:
		case kCmd_AnalFusco:
		case kCmd_AnalFuscoAll:
		case kCmd_AnalFuscoWeighted:
		case kCmd_AnalFuscoExtended:
		case kCmd_AnalFuscoExtendedAll:
		case kCmd_AnalB1:
		case kCmd_AnalB2:
		case kCmd_AnalSlowinski:
		case kCmd_AnalShaosNbar:
		case kCmd_AnalShaosSigmaSq:
		case kCmd_AnalCollessC:
		case kCmd_AnalPhyloAgeTree:
		case kCmd_AnalNodeInfo:
		case kCmd_XAnalNodeInfo:
		// case kCmd_AnalListNodes:
		case kCmd_MacroRunOnce:
		case kCmd_MacroRunN:
		case kCmd_MacroRunAndRestore:
		case kCmd_EpochPopLimit:
		case kCmd_EpochTimeLimit:
		case kCmd_MacroRunTree:
		case kCmd_SysActionDupTree:
		case kCmd_SysActionSave:
		case kCmd_SysActionSetLabels:
		case kCmd_SysActionSetLengths:
		case kCmd_AnalStemminess:
		case kCmd_AnalCaic:
		case kCmd_AnalMacroCaic:
		case kCmd_AnalResolution:
		case kCmd_AnalUltrametric:
		case kCmd_QueueProgram:
			if (theModelHasTrees)
				return true;
			else
				return false;
			break;

		case kCmd_QueueGo:
		case kCmd_QueueGoTrees:
		case kCmd_QueueGoN:
		case kCmd_QueueGoRunAndRestore:
		case kCmd_QueueDelete:
		case kCmd_QueueList:
		case kCmd_QueueDeleteAll:
			if (theQueueHasActions)
				return true;
			else
				return false;
			break;
			
		// if there are multiple trees, some things become possible
		case kCmd_SelectTree:
			if (theModelHasMultipleTrees)
				return true;
			else
				return false;
			break;
		
		case kCmd_AnalShannonDiv:
		case kCmd_AnalSimpsonDiv:
		case kCmd_AnalJackknifePd:
		case kCmd_AnalJackknifeGd:
		case kCmd_AnalBootstrapPd:
		case kCmd_AnalBootstrapGd:
		case kCmd_AnalBrillDiv:
		case kCmd_XPruneSites:
		case kCmd_AnalSiteComp:
		case kCmd_AnalPieDiv:
		case kCmd_AnalMargelefDiv:
		case kCmd_AnalMacintoshDiv:
		case kCmd_AnalMehinickDiv:
			if (theModelHasSiteData)
				return true;
			else
				return false;
			break;
		
		
		case kCmd_DEBUG:
			return true;
			break;
			
		// shouldn't get here, complain if you do
		// return just to keep compiler quiet
		default:
			DBG_MSG ("Recieved unknown cmd " << iCmdId);
			assert (false);
			return false; 		
			break;
	}
	
	DBG_MSG ("fell through switch, something didn't return (cmd " << iCmdId << ")");
	assert (false);	// should never get here
	return false;		// just to shut compiler up
}

void MesaConsoleApp::updateCommandMgr (CommandMgr& ioManager)
//: update all the commands in a manager
// Just a way of passing off submenu command status onto the global function
{
	for (int i = 0; i < ioManager.countCommands(); i++)
	{
		cmdId_t theCmd = ioManager.getCommandId (i);
		bool theCmdIsActive = UpdateCmd (theCmd);
		ioManager.SetCommandActive	(theCmd, theCmdIsActive);
	}
}


// *** COMMANDS *********************************************************/


void MesaConsoleApp::doNewFile ()
//: create a new empty model
{
	// Preconditions:
	assert (mModel == NULL);
	
	// Main body:
	newModel();
	mModel->seedTree ();
	
	Report ("New file created with no traits and one tree stump");
}

void MesaConsoleApp::doLoadFile ()
//: Ask the user to name a file and load a model based on it.
// Error-handling is done here, but error-reporting takes place at a
// higher level.
{
	// Preconditions:
	assert (mModel == NULL);
	
	// Main body:
	try
	{
		// get & test path of input file
		string theInfilePath =
			askString ("What's the name of the input file");
		eraseFlankingSpace (theInfilePath);
		if (theInfilePath == "")
			return;
		if (not doesFileExist (theInfilePath.c_str()))
			// throw (exception ("The files does not exists or could not be opened"));
			throw sbl::MissingFileError ("couldn't locate the input file",
				theInfilePath.c_str());
				
		string theMsg ("Loading file \'");
		theMsg.append (theInfilePath);
		theMsg.append ("\'");
		printReport (theMsg.c_str());
	
		ifstream	theInStream (theInfilePath.c_str());
		if (not theInStream.is_open())
			throw FileOpenError ();
		
		newModel();
		assert (mModel != NULL);	
		mModel->mFilePath = theInfilePath;
		mModel->readModel (theInStream, theInfilePath);	
		
		printReport ("Finished loading file");
	}
	catch (...)
	{
		// do the necessary cleaning up and leave mainloop to report
		deleteModel ();
		throw;
	}
}


void MesaConsoleApp::doSave ()
//: Pipe the contents of the current model to a user-designated file
// TO DO: check if model is "dirty" and permit saving
{
	// Preconditions:
	assert (mModel != NULL);
	
	// Main body:
	try
	{
		if (askEitherOr ("Save as nexus or caic", 'n', 'c'))
		{
			ofstream	theOutStream;
			string theSavePath (askAndOpenOutFile (theOutStream,
				"Save the file as", mModel->mFilePath.c_str()));	
			if (not theOutStream.is_open())
				throw FileOpenError ();
			mModel->writeNexus (theOutStream);			
			printReport ("Finished writing file");
			mModel->mFilePath = theSavePath;
		}
		else
		{
			string theSavePath = askString ("Base name for caic files");
			
			// is there data?
			bool theDataIsPresent = (mModel->countContTraits() or
				mModel->countDiscTraits());

			ofstream thePhylFile ((theSavePath + kCaicPhylogenyFileSuffix).c_str());	
			ofstream theBlenFile ((theSavePath + kCaicBranchFileSuffix).c_str());	
			ofstream theDataFile;	
			if (theDataIsPresent)
			{
				theDataFile.open ((theSavePath + kCaicDataFileSuffix).c_str());
				assert (theDataFile);
			}	

			if (not (thePhylFile and theBlenFile))
				throw FileOpenError ();
			mModel->writeCaic (&thePhylFile, &theBlenFile, &theDataFile);
			thePhylFile.close();
			theBlenFile.close();
			if (theDataIsPresent)
				theDataFile.close();			
			printReport ("Finished writing files");
		}
	}
	catch (...)
	{
		// do the necessary cleaning up and leave mainloop to report
		throw;
	}
}


void MesaConsoleApp::doClose ()
//: Close the model so another file can be opened/loaded.
// TO DO: check if model is "dirty" and permit saving
{
	// Preconditions:
	assert (mModel != NULL);
	
	// Main body:
	deleteModel ();
	Report ("File closed");
}


void MesaConsoleApp::doSummary ()
//: summarise the contents of the loaded file
{
	// Preconditions:
	assert (mModel != NULL);
	
	// Main body:
	mModel->summarise (cout);
}


void MesaConsoleApp::doExport ()
//: export character data from file
{
	// Preconditions:
	assert (mModel != NULL);
	assert (mModel->countDiscTraits() or mModel->countContTraits());

	try
	{
		traittype_t theDataType = (traittype_t) askChoice ("Export both, discrete, or cont.", "bdc", 0);
		nodetype_t theTipType = (nodetype_t) askChoice ("Use all nodes, leaves, or living", "alv", 1);
		ofstream	theOutStream;
		string theSavePath (askAndOpenOutFile (theOutStream,
			"Export data into file"));
		if (not theOutStream.is_open())
			throw FileOpenError ();
		string theMsg ("Exporting data ...");	
		mModel->exportData (&theOutStream, theDataType, theTipType);			
		printReport ("Finished exporting data file");
	}
	catch (...)
	{
		// do the necessary cleaning up and leave mainloop to report
		throw;
	}

}


void MesaConsoleApp::doImport ()
//: import a tab delimited file into the current tree
{
	// Preconditions:
	assert (mModel != NULL);
	
	// Main body:
	try
	{
		// get & test path of input file
		string theInfilePath =
			askString ("What's the name of the file to be imported");
		eraseFlankingSpace (theInfilePath);
		if (theInfilePath == "")
			return;

		if (not doesFileExist (theInfilePath.c_str()))
			throw sbl::MissingFileError ("couldn't locate the input file",
				theInfilePath.c_str());
				
		string theMsg ("Importing file \'");
		theMsg.append (theInfilePath);
		theMsg.append ("\'");
		printReport (theMsg.c_str());
	
		ifstream	theInStream (theInfilePath.c_str());
		if (not theInStream.is_open())
			throw FileOpenError ();
			
		mModel->importTab (theInStream);
		
		printReport ("Finished importing file");
	}
	catch (exception& theException)
	{
		Report ("A problem has caused the import to fail.");
		// do the necessary cleaning up and leave mainloop to report
		throw;
	}

}


void MesaConsoleApp::doImportRich ()
//: import a tab delimited file into the current tree
{
	// Preconditions:
	assert (mModel != NULL);
	
	// Main body:
	try
	{
		// get & test path of input file
		string theInfilePath =
			askString ("What's the name of the species richness file");
		eraseFlankingSpace (theInfilePath);
		if (theInfilePath == "")
			return;

		if (not doesFileExist (theInfilePath.c_str()))
			throw sbl::MissingFileError ("couldn't locate the input file",
				theInfilePath.c_str());
				
		string theMsg ("Importing species richness file \'");
		theMsg.append (theInfilePath);
		theMsg.append ("\'");
		printReport (theMsg.c_str());
	
		ifstream	theInStream (theInfilePath.c_str());
		if (not theInStream.is_open())
			throw FileOpenError ();
			
		mModel->importRich (theInStream);
		
		printReport ("Finished importing file");
	}
	catch (exception& theException)
	{
		Report ("A problem has caused import to fail.");
		// do the necessary cleaning up and leave mainloop to report
		throw;
	}

}


void  MesaConsoleApp::doDistill ()
//: condense & transpose a user-selected results file to a summary
{
	// Main body:
	ifstream		theInFile;
	ofstream		theOutFile;
		
	try
	{
		// get & test path in & out files, and trim size, allow abort
		if (not askAndOpenInFile ("What's the name of the results file", theInFile))
			return;
		askAndOpenOutFile (theOutFile, "Save the distilled results as");
		if (not theOutFile)
			return;
		int theTrimCols = askIntegerWithMin ("Trim how many initial columns", 0);
		
		// do the work			
		ResultsDistiller	theDistiller (theTrimCols);
		ReportProgress ("Distilling file");
		theDistiller.distill (theInFile, theOutFile);
		printReport ("Finished");

		// clean up after self
		theInFile.close();
		theOutFile.close();
	}
	catch (exception& theException)
	{
		Report ("A problem has caused distillation to fail.");
		// do the necessary cleaning up and leave mainloop to report
		theInFile.close();
		theOutFile.close();

		throw;
	}
}


// *** MANIPULATIONS FXNS ************************************************/


void MesaConsoleApp::doManipMenu ()
{
	CommandMgr	theManipCmds ("Manipulate data menu");

	theManipCmds.AddCommand (kCmd_SelectTree, "Select active tree");		
	theManipCmds.AddCommand (kCmd_DuplicateTree, "Duplicate the active tree");		
	theManipCmds.AddCommand (kCmd_DeleteTree, "Delete a tree");
	theManipCmds.AddCommand (kCmd_NewTrait, "Add a new trait");
	theManipCmds.AddCommand (kCmd_DeleteTrait, "Delete a trait");
	theManipCmds.AddCommand (kCmd_SysActionSetLengths, "Set branchlengths");
	theManipCmds.AddCommand (kCmd_SysActionShuffleTraits, "Shuffle traits");
	theManipCmds.AddCommand (kCmd_SysActionConsolidateTaxa, "Consolidate taxa");	
	theManipCmds.AddCommand (kCmd_SysActionDeleteDeadTaxa, "Delete dead taxa");	
	theManipCmds.AddCommand (kCmd_SysActionDeleteDeadTraits, "Delete dead traits");
	theManipCmds.AddCommand (kCmd_RulePruneFixedNum, "prune (fixed number of taxa)");
	theManipCmds.AddCommand (kCmd_RulePrunFixedFrac, "prune (fixed fraction of taxa)");
	theManipCmds.AddCommand (kCmd_RulePrunProb, "prune (fixed prob for each taxa)");
	theManipCmds.AddCommand (kCmd_RulePrunByTrait, "prune (by continuous trait)");
	theManipCmds.AddCommand (kCmd_RulePrunIf, "prune (if taxa meets condition)");
	theManipCmds.AddCommand (kCmd_SysActionCollapseSingletonsTaxa, "Collapse singletons");	
	theManipCmds.AddCommand (kCmd_Summarise, "Summarise file contents");
	theManipCmds.AddCommand (kCmd_DetailedReport, "Report on the file contents");
	theManipCmds.AddCommand (kCmd_Return, 'r', "Return to main menu");

	theManipCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		// activate the right commands
		updateCommandMgr (theManipCmds);
		
		// get user command
		theUserCmd = AskUserCommand (&theManipCmds);
		
		switch (theUserCmd)
		{
			case kCmd_RulePruneFixedNum:
			case kCmd_RulePrunFixedFrac:
			case kCmd_RulePrunProb:
			case kCmd_RulePrunByTrait:
			case kCmd_RulePrunIf:
			case kCmd_SysActionSetLengths:
			case kCmd_SysActionShuffleTraits:
			case kCmd_SysActionConsolidateTaxa:
			case kCmd_SysActionDeleteDeadTaxa:
			case kCmd_SysActionDeleteDeadTraits:
			case kCmd_SysActionCollapseSingletonsTaxa:
			{
				BasicAction* theActionP = newAction (theUserCmd);
				theActionP->execute();
				delete theActionP;
			}
			break;

			case kCmd_Return:
				break;
				
			default:
				ObeyCmd (theUserCmd);			
		}
	}
	while (theUserCmd != kCmd_Return);	
}


void MesaConsoleApp::doSelectTree ()
{
	// Preconditions:
	assert (mModel != NULL);
	assert (1 < mModel->countTrees());
	
	// Main body:
	int theActiveTree = askIntegerWithBounds	("Select which tree as active",
		1, mModel->countTrees());
	mModel->selectTree (theActiveTree - 1);
}


void MesaConsoleApp::doDuplicateTree ()
{
	// Preconditions:
	assert (mModel != NULL);
	assert (0 < mModel->countTrees());
	
	// Main body:
	mModel->duplicateActiveTree();
	printReport ("Active tree duplicated");
}


void MesaConsoleApp::doDetailedReport ()
//: output a full and detailed report on the contents of the model
{
	// Preconditions:
	assert (mModel != NULL);
	
	// Main body:
	mModel->detailedReport(cout);
}


void MesaConsoleApp::doDeleteTrait ()
//: delete a continuous or discrete character
{
	// Preconditions:
	assert (mModel != NULL);
	
	// Main body:
	char theChoiceLetter = AskMultiChoice ("Continuous or discrete trait", "cd");
	traittype_t theDataType;
	int theNumChars;
	if (theChoiceLetter == 'd')
	{
		theDataType = kTraittype_Discrete;
		theNumChars = mModel->countDiscTraits ();
	}
	else
	{
		assert (theChoiceLetter = 'c');
		theDataType = kTraittype_Continuous;
		theNumChars = mModel->countContTraits ();
	}
	
	if (theNumChars == 0)
	{
		ReportError ("There are no traits of that type");
	}
	else 
	{
		string	theReportStr ("The trait \'");
		if (theNumChars == 1)
		{
			theReportStr += mModel->getTraitName (theDataType, 0);
			mModel->deleteChar (theDataType, 0);
		}
		else
		{
			int theIndex = askIntegerWithBounds ("Delete which trait", 1,
				theNumChars) - 1;
			theReportStr += mModel->getTraitName (theDataType, theIndex);
			mModel->deleteChar (theDataType, theIndex);
		} 
		theReportStr += "\' has been deleted";
		Report (theReportStr.c_str());
	} 
}


void MesaConsoleApp::doNewTrait ()
//: add a new continuous or discrete character
{
	// Preconditions:
	assert (mModel != NULL);
	
	// Main body:
	char theChoiceLetter = AskMultiChoice
		("Continuous or discrete trait", "cd");
	if (theChoiceLetter == 'd')
	{
		string	theNewVal =	askString ("Set the new trait to");
		mModel->newDiscChar (theNewVal.c_str());
	}
	else
	{
		assert (theChoiceLetter = 'c');
		mModel->newContChar (askDouble ("Set the new trait to"));
	}
}


void MesaConsoleApp::doDeleteTree ()
//: delete a tree from the wrangler
{
	// Preconditions:
	assert (mModel != NULL);
	assert (0 < mModel->countTrees());
	
	// Main body:
	int theNumTrees = mModel->countTrees();
	int theDelIndex;
	if (theNumTrees == 1)
		theDelIndex = 1;
	else
	theDelIndex = askIntegerWithBounds ("Delete which tree", 1, theNumTrees);
	mModel->deleteTree (theDelIndex - 1);
	
	// Postconditions:
	assert (mModel->countTrees() == (theNumTrees - 1));
}



// *** PREFERENCE FXNS ***************************************************/


void MesaConsoleApp::doPrefsMenu ()
//: Show current preferences and allow changing them
// This function is unbelievably long and awkward, but I can't think of any
// other way to do thing.
{
	CommandMgr	thePrefsCmds ("User preferences menu");

	thePrefsCmds.AddCommand (kCmd_PrefVerbosity, "Set verbosity");		
	thePrefsCmds.AddCommand (kCmd_PrefAnalysisOut, "Set saving of quick analyses");		
	thePrefsCmds.AddCommand (kCmd_PrefLogging, "Set logging");		
	thePrefsCmds.AddCommand (kCmd_PrefCase, "Set case of output files");		
	thePrefsCmds.AddCommand (kCmd_PrefSetCladeLabels, "Set clade labelling");		
	thePrefsCmds.AddCommand (kCmd_PrefPadMatrix, "Set padding of output matrix");		
	thePrefsCmds.AddCommand (kCmd_PrefPreserveNodes, "Set preservation of taxa");		
	thePrefsCmds.AddCommand (kCmd_PrefWriteTaxa, "Set writing of taxa block");		
	thePrefsCmds.AddCommand (kCmd_PrefWriteTranslation, "Set writing of translation cmd");		
	thePrefsCmds.AddCommand (kCmd_PrefTimeGrain, "Set granularity of simulation time");		
	thePrefsCmds.AddCommand (kCmd_PrefSetRandSeed, "Set random number seed");		
	thePrefsCmds.AddCommand (kCmd_Return, 'r', "Return to main menu");

	thePrefsCmds.SetCommandActive (true);
	thePrefsCmds.SetConvertShortcut (true);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		int theChoice;
		bool theOptionIsOn;
			
		// get user command
		theUserCmd = AskUserCommand (&thePrefsCmds);
		switch (theUserCmd)
		{
			case kCmd_Return:
			{
				return;
				break;
			}
				
			case kCmd_PrefTimeGrain:
			{
				cout << "Currently set to: " << MesaGlobals::mPrefs.mTimeGrain << endl;
				if (askYesNo ("Change"))
				{
					double theGrain = askDouble ("Set it to", 0.000006, 1.0);
					MesaGlobals::mPrefs.mTimeGrain = theGrain;
					cout << "Time granularity now set to: " << MesaGlobals::mPrefs.mTimeGrain << endl;
				}
				break;
			}

			case kCmd_PrefSetCladeLabels:
			{
				theChoice = int (MesaGlobals::mPrefs.mCladeLabels);
				cout << "Currently set to: " << kPrefCladeLabels_Cstrs[theChoice] << endl;
				theChoice = askChoice ("Label as phylo, CAIC, or hierachy", "pch",
					theChoice);
				MesaGlobals::mPrefs.mCladeLabels = (pref_cladelabels_t) theChoice;
				cout << "Now set to: " << kPrefCladeLabels_Cstrs[theChoice] << endl;
				break;
			}
				
			case kCmd_PrefPadMatrix:
			{
				theOptionIsOn = MesaGlobals::mPrefs.mPadNumericOutput;
				cout << "Prettyprint matrices in output files was set to" <<
					(theOptionIsOn? "true" : "false") << ", is now " <<
					(theOptionIsOn? "false" : "true") << "." << endl;
				MesaGlobals::mPrefs.mPadNumericOutput = (not theOptionIsOn);
				break;
			}

			case kCmd_PrefPreserveNodes:
			{
				theChoice = int (MesaGlobals::mPrefs.mPreserveNodes);
				cout << "Currently set to: " << kPrefPreserveNodes_Cstrs[theChoice] << endl;
				theChoice = askChoice ("Preserve none, root, root children", "nrc",
					theChoice);
				MesaGlobals::mPrefs.mPreserveNodes = (pref_preservenodes_t) theChoice;
				cout << "Now set to: " << kPrefPreserveNodes_Cstrs[theChoice] << endl;
				break;
			}
			
			case kCmd_PrefWriteTranslation:
			{
				theOptionIsOn = MesaGlobals::mPrefs.mWriteTransCmd;
				cout << "Writing a translation cmd was set to" <<
					(theOptionIsOn? "true" : "false") << ", is now " <<
					(theOptionIsOn? "false" : "true") << "." << endl;
				MesaGlobals::mPrefs.mWriteTransCmd = (not theOptionIsOn);
				break;
			}

			case kCmd_PrefWriteTaxa:
			{
				theOptionIsOn = MesaGlobals::mPrefs.mWriteTaxaBlock;
				cout << "Writing a taxa block was set to" <<
					(theOptionIsOn? "true" : "false") << ", is now " <<
					(theOptionIsOn? "false" : "true") << "." << endl;
				MesaGlobals::mPrefs.mWriteTaxaBlock = (not theOptionIsOn);
				break;
			}
								
			case kCmd_PrefSetRandSeed:
			{
				long theSeed = askInteger ("Set the random number seed to");
				setRandomSeed (theSeed);
				break;
			}
	

			default:
			{
				ObeyCmd (theUserCmd);			
			}
		}
	}
	while (theUserCmd != kCmd_Return);	
}

void MesaConsoleApp::doPrefsVerbosity ()
{
	char theCurrentState;
	switch (MesaGlobals::mPrefs.mVerbosity)
	{
		case kPrefVerbosity_Quiet:
			theCurrentState = 'q';
			break;
		case kPrefVerbosity_Normal:
			theCurrentState = 'n';
			break;
		case kPrefVerbosity_Loud:
			theCurrentState = 'l';
			break;
		default:
			assert ("default case should not be reached" == false);
	}
	
	char theAnswer = AskMultiChoice ("Quiet, normal or loud", "qnl",
		theCurrentState);
		
	switch (theAnswer)
	{
		case 'q':
			MesaGlobals::mPrefs.mVerbosity = kPrefVerbosity_Quiet;
			Report ("Verbosity set to quiet");
			break;
		case 'n':
			MesaGlobals::mPrefs.mVerbosity = kPrefVerbosity_Normal;
			Report ("Verbosity set to normal");
			break;
		case 'l':
			MesaGlobals::mPrefs.mVerbosity = kPrefVerbosity_Loud;
			Report ("Verbosity set to loud");
			break;
		default:
			assert ("default case should not be reached" == false);
	}
}


void MesaConsoleApp::doPrefsAnalysis ()
{
	char theCurrentState;
	switch (MesaGlobals::mPrefs.mAnalysisOut)
	{
		case kPrefAnalysisOut_AllScreen:
			theCurrentState = 's';
			break;
		case kPrefAnalysisOut_AllFile:
			theCurrentState = 'f';
			break;
		case kPrefAnalysisOut_Normal:
			theCurrentState = 'b';
			break;
		default:
			assert ("default case should not be reached" == false);
	}
	
	char theAnswer = AskMultiChoice ("Print to screen, file or both", "sfb",
		theCurrentState);
		
	switch (theAnswer)
	{
		case 's':
			MesaGlobals::mPrefs.mAnalysisOut = kPrefAnalysisOut_AllScreen;
			Report ("Quick analyses will print to the screen");
			break;
		case 'f':
			MesaGlobals::mPrefs.mAnalysisOut = kPrefAnalysisOut_AllFile;
			Report ("Quick analyses will save to a file");
			break;
		case 'b':
			MesaGlobals::mPrefs.mAnalysisOut = kPrefAnalysisOut_Normal;
			Report ("Quick analyses will print and save");
			break;
		default:
			assert ("default case should not be reached" == false);
	}
}

void MesaConsoleApp::doPrefsLogging ()
{
	char theCurrentState;
	switch (MesaGlobals::mPrefs.mLogging)
	{
		case kPrefLogging_Enabled:
			theCurrentState = 'e';
			break;
		case kPrefLogging_Disabled:
			theCurrentState = 'd';
			break;
		default:
			assert ("default case should not be reached" == false);
	}

	char theAnswer = AskMultiChoice ("Enabled or disabled", "ed",
		theCurrentState);

	switch (theAnswer)
	{
		case 'e':
			// if not already enabled, we are opening the log
			if (MesaGlobals::mPrefs.mLogging != kPrefLogging_Enabled)
			{
				string theFileName = askString (
					"What's the name of the log file");
				mLogStreamP = new ofstream (theFileName.c_str());
				if (not mLogStreamP->is_open())
				{
					delete mLogStreamP;
					throw MissingFileError ("the log file could not be opened", theFileName.c_str());
				}
				MesaGlobals::mPrefs.mLogging = kPrefLogging_Enabled;
				Report ("Logging enabled");
			}
			break;
		case 'd':
			// if already enabled, we are closing the log
			if (MesaGlobals::mPrefs.mLogging != kPrefLogging_Enabled)
			{
				MesaGlobals::mPrefs.mLogging = kPrefLogging_Disabled;
				if (mLogStreamP != NULL)
				{
					assert (mLogStreamP->is_open());
					mLogStreamP->close();
				}
				Report ("Logging disabled");
			}
			break;
		default:
			assert ("default case should not be reached" == false);
	}
}


void MesaConsoleApp::doPrefsCase ()
{
	char theCurrentState;
	switch (MesaGlobals::mPrefs.mCase)
	{
		case kCase_Upper:
			theCurrentState = 'u';
			break;
		case kCase_Lower:
			theCurrentState = 'l';
			break;
		case kCase_Mixed:
			theCurrentState = 'm';
			break;
		default:
			assert ("default case should not be reached" == false);
	}

	char theAnswer = AskMultiChoice ("Upper, lower or mixed case", "ulm",
		theCurrentState);

	switch (theAnswer)
	{
		case 'u':
			MesaGlobals::mPrefs.mCase = kCase_Upper;
			Report ("Output case set to upper");
			break;
			
		case 'l':
			MesaGlobals::mPrefs.mCase = kCase_Lower;
			Report ("Output case set to lower");
			break;
			
		case 'm':
			MesaGlobals::mPrefs.mCase = kCase_Mixed;
			Report ("Output case set to mixed");
			break;
		default:
			assert ("default case should not be reached" == false);
	}
}



// *** UTILITY FUNCTIONS *************************************************/


void MesaConsoleApp::newModel ()
//: Provides the necessary housekeeping to setup a model.
// Mostly here for historical reasons. Really doesn't do anything now.
{
	// Preconditions:
	assert (mModel == NULL);
	
	// Main body:
	progcallback_t theReportCb = makeFunctor ((progcallback_t*) NULL,
		*this, &MesaConsoleApp::callbackReport);
	mModel = new MesaModel (theReportCb);	
}

void MesaConsoleApp::deleteModel ()
//: Nuke the model from existence.
// As this may be called in an error condition, say during model creation,
// we do not test that the model is not NULL.

{
	delete mModel;
	mModel = NULL;
}

bool MesaConsoleApp::doesFileExist (const char* iFilePath)
//: Does a given filepath exist (is there a file there?)
// A mite clumsy, as it tests this by attempting to read the file there.
// Still, it's portable.
{
	// if invalid path
	if (iFilePath == NULL) 
		return false;
	
	// test if file can be read from
	ifstream	theTmpFile;
	theTmpFile.open (iFilePath);
	if (theTmpFile.is_open())
	{
		theTmpFile.close ();
		return true;
	}
	
	return false;
}


// *** TASK & ANALYSIS FUNCTIONS ******************************************/


bool MesaConsoleApp::askAndOpenReportFile (const char* iPromptCstr, ofstream& iOutFstrm)
//: prompt user for a report file name, open that file,tell reporter, return success
{
	askAndOpenOutFile (iOutFstrm, iPromptCstr);
	if (iOutFstrm)
	{
		mModel->mReporter.setFileStream (&iOutFstrm);
		return true;
	}
	else
	{
		mModel->mReporter.setFileStream (NULL);
		return false;
	}
}


void MesaConsoleApp::doAnalysisMenu ()
{
	// if output is to go to a file, get the name of it:
	ofstream theResultsFile;
	if (MesaGlobals::mPrefs.mAnalysisOut != kPrefAnalysisOut_AllScreen)
	{
		askAndOpenReportFile ("Save the analysis results in which file", theResultsFile);
	}

	// set up the menu
	CommandMgr	theAnalCmds ("Do quick analysis menu");
	
	theAnalCmds.AddCommand (kCmd_PrefAnalysisOut, 's', "Set saving of analyses");		
	loadAnalysisCmds (theAnalCmds);
	theAnalCmds.AddCommand (kCmd_Return, 'r', "Return to main menu");

	theAnalCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		// activate the right commands
		updateCommandMgr (theAnalCmds);
		
		// get user command
		theUserCmd = AskUserCommand (&theAnalCmds);
		
		// process the command
		switch (theUserCmd)
		{
			case kCmd_Return:
				// break from switch then do-loop ends
				break;
			
			case kCmd_PrefAnalysisOut:
			{
				cout << endl;
				bool theFileWasOpen = theResultsFile.is_open();
				doPrefsAnalysis();
				
				if (theFileWasOpen and (MesaGlobals::mPrefs.mAnalysisOut == kPrefAnalysisOut_AllScreen))
				{
					// if the file was open and is now closed, close it
					if (theResultsFile.is_open())
						theResultsFile.close();
					mModel->mReporter.setFileStream (NULL);
				}
				else if ((not theFileWasOpen) and (MesaGlobals::mPrefs.mAnalysisOut != kPrefAnalysisOut_AllScreen))
				{
					// the file was closed and must now be opened
					askAndOpenReportFile ("Save the analysis results in which file", theResultsFile);
				}
				else
				{
					// need not do anything for other possibilities:
					// 1. the file was open and still needs to be open
					// 2. the file was closed and need not be opened
				}
				break;
			}
			
			default:
				// if you reach this pt, it's an analysis
				BasicAction* theAnalysisP = newAction (theUserCmd);
				if (theAnalysisP != NULL)
				{
					theAnalysisP->execute();
					if (MesaGlobals::mPrefs.mAnalysisOut != kPrefAnalysisOut_AllScreen)
						Report ("Analysis saved to file");
					delete theAnalysisP;
				}
				break;
		}
	}
	while (theUserCmd != kCmd_Return);		
	
	// tidy up
	if (theResultsFile.is_open())
		theResultsFile.close();
	mModel->mReporter.setFileStream (NULL);
}


void MesaConsoleApp::doTaskMenu ()
{
	CommandMgr	theQueueCmds ("Manipulate the action queue");
	
	theQueueCmds.AddCommand (kCmd_QueueGo, "g", "Go! (run the queue)");		
	theQueueCmds.AddCommand (kCmd_QueueGoN, "gn", "Go! (run the queue N times)");		
	theQueueCmds.AddCommand (kCmd_QueueGoRunAndRestore, "gr", "Go! (run the queue & restore)");		
	theQueueCmds.AddCommand (kCmd_QueueGoTrees, "gt", "Go! (run the queue across all trees)");		
	theQueueCmds.AddCommand (kCmd_QueueList, "l", "List contents of the action queue");		
	theQueueCmds.AddCommand (kCmd_QueueDelete, "d", "Delete actions from the queue");		
	theQueueCmds.AddCommand (kCmd_QueueDeleteAll, "da", "Delete the whole queue");		
	theQueueCmds.AddCommand (kCmd_QueueProgram, "p", "Program the queue");		
	theQueueCmds.AddCommand (kCmd_Return, "r", "Return to main menu");

	theQueueCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		// activate the right commands
		updateCommandMgr (theQueueCmds);
		
		// get user command
		theUserCmd = AskUserCommand (&theQueueCmds);
		
		// process the command
		switch (theUserCmd)
		{
			case kCmd_QueueProgram:
			{
				doProgramQueue ();
				break;
			}
				
			case kCmd_Return:
			{
				// leave the switch and drop out of the while
				break;
			}
			
			case kCmd_QueueDeleteAll:
			{
				mModel->mActionQueue.clear();
				cout << endl;
				Report ("Action queue emptied");
				break;
			}
				
			case kCmd_QueueDelete:
			{
				cout << endl;
				long theNumActions = mModel->mActionQueue.size();
				long theTargetIndex;
				if (theNumActions == 1)
				{
					theTargetIndex = 1;
				}
				else
				{
					theTargetIndex = askIntegerWithBounds (
						"Delete which action", 1, theNumActions);
				}
				mModel->mActionQueue.deleteElement (theTargetIndex - 1);
				Report ("Action deleted from queue");
				break;
			}
				
			case kCmd_QueueGo:
			case kCmd_QueueGoTrees:
			case kCmd_QueueGoN:
			case kCmd_QueueGoRunAndRestore:
			{
				doRunQueue (theUserCmd);
				break;
			}

			case kCmd_QueueList:
			{
				cout << endl;
				// mModel->mActionQueue.detailedReport (&cout);
				ActionQueue* theQueueP = &(mModel->mActionQueue);
				long theSizeQueue = theQueueP->size();
				bool theMultipleActions = (1 < theSizeQueue);
				cout << "   There " << (theMultipleActions? "are" : "is") << 
					" " << theSizeQueue << " action" << (theMultipleActions? "s" : "")
					<< " programmed in the queue." << std::endl;
				for (long i = 0; i < theSizeQueue; i++)
				{
					long	theDepth = theQueueP->getDepth (i);
					cout << "  " << std::setw (4) << i + 1;
					for (long j = 0; j < theDepth; j++)
						cout << "   ";
					cout << theQueueP->describe (i) << std::endl;
				}
				break;
			}
				
			default:
			{
				assert (false);
			}
		}
	}
	while (theUserCmd != kCmd_Return);	
}

void MesaConsoleApp::loadQueueCmds (CommandMgr& ioCommands)
{
	loadMacroCmds (ioCommands);
	ioCommands.AddCommand (kCmd_SysActionSetLengths, "Set branchlengths");
	ioCommands.AddCommand (kCmd_SysActionShuffleTraits, "Shuffle traits");
	ioCommands.AddCommand (kCmd_SysActionConsolidateTaxa, "Consolidate taxa");	
	ioCommands.AddCommand (kCmd_SysActionDeleteDeadTaxa, "Delete dead taxa");	
	ioCommands.AddCommand (kCmd_SysActionDeleteDeadTraits, "Delete dead traits");
	ioCommands.AddCommand (kCmd_RulePruneFixedNum, "prune (fixed number of taxa)");
	ioCommands.AddCommand (kCmd_RulePrunFixedFrac, "prune (fixed fraction of taxa)");
	ioCommands.AddCommand (kCmd_RulePrunProb, "prune (fixed prob for each taxa)");
	ioCommands.AddCommand (kCmd_RulePrunByTrait, "prune (determined by trait)");
	ioCommands.AddCommand (kCmd_RulePrunIf, "prune (if taxa meets condition)");
	ioCommands.AddCommand (kCmd_SysActionCollapseSingletonsTaxa, "Collapse singletons");	
	loadAnalysisCmds (ioCommands);
	loadSystemCmds (ioCommands);
	ioCommands.AddCommand (kCmd_SysActionSetLengths, "Set branchlengths");
	ioCommands.AddCommand (kCmd_SysActionShuffleTraits, "Shuffle traits");
}

void MesaConsoleApp::doProgramQueue ()
{
	CommandMgr	theQueueCmds ("Program action queue (top level) with");
	
	theQueueCmds.AddCommand (kCmd_QueueList, "l", "List contents of the action queue");		
	theQueueCmds.AddCommand (kCmd_QueueDelete, "d", "Delete actions from the queue");		
	theQueueCmds.AddCommand (kCmd_QueueDeleteAll, "da", "Delete the whole queue");	
	loadQueueCmds (theQueueCmds);
	theQueueCmds.AddCommand (kCmd_Return, "r", "Return to queue manipulation menu");

	theQueueCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		// activate the right commands
		updateCommandMgr (theQueueCmds);
		// DBG_MSG ("HAS ANA: " << mModel->mActionQueue.containsAnalyses());
		// get user command
		theUserCmd = AskUserCommand (&theQueueCmds);
		
		// process the command
		switch (theUserCmd)
		{
			case kCmd_Return:
			{
				// leave the switch and drop out of the while
				break;
			}
			
			case kCmd_QueueDeleteAll:
			{
				mModel->mActionQueue.clear();
				cout << endl;
				Report ("Action queue emptied");
				break;
			}
				
			case kCmd_QueueDelete:
			{
				cout << endl;
				long theNumActions = mModel->mActionQueue.size();
				long theTargetIndex;
				if (theNumActions == 1)
				{
					theTargetIndex = 1;
				}
				else
				{
					theTargetIndex = askIntegerWithBounds (
						"Delete which action", 1, theNumActions);
				}
				mModel->mActionQueue.deleteElement (theTargetIndex - 1);
				Report ("Action deleted from queue");
				break;
			}
				
			case kCmd_QueueGo:
			case kCmd_QueueGoTrees:
			case kCmd_QueueGoN:
			case kCmd_QueueGoRunAndRestore:
			{
				doRunQueue (theUserCmd);
				break;
			}

			case kCmd_QueueList:
			{
				cout << endl;
				// mModel->mActionQueue.detailedReport (&cout);
				ActionQueue* theQueueP = &(mModel->mActionQueue);
				long theSizeQueue = theQueueP->size();
				bool theMultipleActions = (1 < theSizeQueue);
				cout << "   There " << (theMultipleActions? "are" : "is") << 
					" " << theSizeQueue << " action" << (theMultipleActions? "s" : "")
					<< " programmed in the queue." << std::endl;
				for (long i = 0; i < theSizeQueue; i++)
				{
					long	theDepth = theQueueP->getDepth (i);
					cout << "  " << std::setw (4) << i + 1;
					for (long j = 0; j < theDepth; j++)
						cout << "   ";
					cout << theQueueP->describe (i) << std::endl;
				}
				break;
			}
				
			default:
			{
				// for all other actions ...
				
				// ... program into queue with appropriate message ...
				BasicAction* theActionP = newAction (theUserCmd);
				mModel->mActionQueue.adoptAction(theActionP);

				string theReportStr ("Programmed \'");
				theReportStr += theActionP->describe(0);
				theReportStr += "\' into queue";
					
				Report (theReportStr.c_str());
				
				// ... if an epoch, recurse into programming queue with it
				EpochMacro* theNewEpoch = castAsEpoch (theActionP);
				if (theNewEpoch != NULL)
				{
					doTaskEpochMenu (theNewEpoch);
				}
				else
				{
					// ... if a macro, recurse into programming queue with it
					BasicMacro* theNewMacro = castAsMacro (theActionP);
					if (theNewMacro != NULL)
					{
						doTaskMacroMenu (theNewMacro);
					}
				}
			}
		}
	}
	while (theUserCmd != kCmd_Return);	
}



void MesaConsoleApp::doRunQueue (cmdId_t iUserCmd)
{
	ofstream	theResultsFile;
	if (mModel->mActionQueue.containsAnalyses())
	{
		cout << endl;
		askAndOpenReportFile ("Save the analysis results in which file", theResultsFile);
	}
	
	try
	{
		switch (iUserCmd)
		{
			case kCmd_QueueGo:
			{
				ReportProgress ("Running action queue once");
				mModel->mActionQueue.runOnce();
				Report ("Execution complete");
				break;
			}
					
			case kCmd_QueueGoTrees:
			{
				ReportProgress ("Running action queue across all trees");
				mModel->mActionQueue.runTrees ();
				Report ("Execution complete");
				break;
			}
					
			case kCmd_QueueGoN:
			{
				int theNumLoops = askIntegerWithMin (
					"Loop over queue how many times", 1);
				ReportProgress ("Running action queue");
				mModel->mActionQueue.runN (theNumLoops);
				Report ("Execution complete");
				break;
			}

			case kCmd_QueueGoRunAndRestore:
			{
				int theNumLoops = askIntegerWithMin (
					"Run & restore queue how many times", 1);
				ReportProgress ("Running action queue");
				mModel->mActionQueue.runAndRestore (theNumLoops);
				Report ("Execution complete");
				break;
			}

			default:
				assert (false);
		}
	}
	catch (...)
	{
		Report ("A problem has caused the queue to terminate.");
		throw;
		if (theResultsFile.is_open())
		{
			theResultsFile.close ();	
			mModel->mReporter.setFileStream (NULL);
		}
	}
	
	if (theResultsFile.is_open())
	{
		theResultsFile.close ();	
		mModel->mReporter.setFileStream (NULL);
	}
}


void MesaConsoleApp::doTaskMacroMenu (BasicMacro* iActionCntnrP)
//: program actions inside a macro
{
	string 	theMenuTitle ("Program macro (\'");
	string theTmpStr = (iActionCntnrP->describe(0));
	theTmpStr = theTmpStr.substr(0, 32);
	eraseFlankingSpace (theTmpStr);
	theMenuTitle += theTmpStr;
	theMenuTitle += " ...\')";
	
	CommandMgr	theQueueCmds (theMenuTitle.c_str());
	theQueueCmds.AddCommand (kCmd_QueueList, "l", "List contents of entire queue");		
	loadQueueCmds (theQueueCmds);
	theQueueCmds.AddCommand (kCmd_Return, "r", "Finish programming macro");
	theQueueCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		// activate the right commands
		updateCommandMgr (theQueueCmds);
		
		// get user command
		theUserCmd = AskUserCommand (&theQueueCmds);
		
		// process the command
		switch (theUserCmd)
		{
			case kCmd_Return:
				if (iActionCntnrP->size() == 0)
				{
					Report ("This macro is still empty");
					if (not AskYesNoQuestion ("Do you really want to stop programming it"))
						theUserCmd = kCmd_Null;
				}
				break;
			
			case kCmd_QueueDeleteAll:
			case kCmd_QueueDelete:
			case kCmd_QueueGo:
			case kCmd_QueueGoN:
			case kCmd_QueueGoRunAndRestore:
				// just in case
				assert (false);
				break;
				
			case kCmd_QueueList:
				cout << endl;
				mModel->mActionQueue.detailedReport (&cout);
				break;
				
			default:
				// for all other actions ...
				
				// ... program into queue with appropriate message ...
				BasicAction* theActionP = newAction (theUserCmd);
				iActionCntnrP->adoptAction (theActionP);

				string theReportStr ("Programmed \'");
				theReportStr += theActionP->describe(0);
				theReportStr += "\' into macro";
		
				Report (theReportStr.c_str());
				
				// ... if an epoch, recurse into programming queue with it
				EpochMacro* theNewEpoch = castAsEpoch (theActionP);
				if (theNewEpoch != NULL)
				{
					doTaskEpochMenu (theNewEpoch);
				}
				else
				{
					// ... if a macro, recurse into programming queue with it
					BasicMacro* theNewMacro = castAsMacro (theActionP);
					if (theNewMacro != NULL)
					{
						doTaskMacroMenu (theNewMacro);
					}
				}
		}
	}
	while (theUserCmd != kCmd_Return);		
}


void MesaConsoleApp::doTaskEpochMenu (EpochMacro* iEpochP)
{
	string 	theMenuTitle ("Program epoch (\'");
	string theTmpStr = (iEpochP->describe (0));
	theTmpStr = theTmpStr.substr(0, 32);
	eraseFlankingSpace (theTmpStr);
	theMenuTitle += theTmpStr;
	theMenuTitle += " ...\')";
	
	CommandMgr	theEpochCmds (theMenuTitle.c_str());	
	theEpochCmds.AddCommand (kCmd_QueueList, "l", "List contents of entire queue");
	loadRuleCmds (theEpochCmds);
	theEpochCmds.AddCommand (kCmd_Return, "r", "Finish programming epoch");
	theEpochCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		// activate the right commands
		updateCommandMgr (theEpochCmds);
				
		// get user command
		theUserCmd = AskUserCommand (&theEpochCmds);
		
		// process the command
		switch (theUserCmd)
		{
			case kCmd_Return:
				// break out of switch and hit end of do-while
				if (iEpochP->size() == 0)
				{
					Report ("This epoch is still empty");
					if (not AskYesNoQuestion ("Do you really want to stop programming it"))
						theUserCmd = kCmd_Null;
				}
				break;
			
			case kCmd_QueueList:
				cout << endl;
				mModel->mActionQueue.detailedReport (&cout);
				break;
				
			default:
				// for all other actions ...
				
				// ... program into queue with appropriate message ...
				BasicAction* theActionP = (BasicAction*) newRule (theUserCmd);
				iEpochP->adoptAction(theActionP);

				string theReportStr ("Programmed \'");
				theReportStr += theActionP->describe(0);
				theReportStr += "\' into epoch";
				Report (theReportStr.c_str());			
		}
	}
	while (theUserCmd != kCmd_Return);		
}


void MesaConsoleApp::loadMacroCmds (CommandMgr& ioCommands)
{
	ioCommands.AddCommand (kCmd_MacroRunOnce, "Run enclosed actions once only");		
	ioCommands.AddCommand (kCmd_MacroRunN, "Run enclosed actions multiple times");		
	ioCommands.AddCommand (kCmd_MacroRunAndRestore, "Run & restore enclosed actions multiple times");		
	ioCommands.AddCommand (kCmd_MacroRunTree, "Run enclosed actions over all trees");		
	ioCommands.AddCommand (kCmd_EpochPopLimit, "Run an evolutionary epoch (tree size limit)");		
	ioCommands.AddCommand (kCmd_EpochTimeLimit, "Run an evolutionary epoch (time limit)");		
}


void MesaConsoleApp::loadAnalysisCmds (CommandMgr& ioCommands)
{
	ioCommands.AddCommand (kCmd_AnalExTaxa, "Calculate number of extant taxa");		
	ioCommands.AddCommand (kCmd_AnalAllTaxa, "Calculate total number of taxa");		
	// ioCommands.AddCommand (kCmd_AnalListNodes, "List nodes");		
	ioCommands.AddCommand (kCmd_AnalGeneticDiv, "Calculate genetic diversity");		
	ioCommands.AddCommand (kCmd_AnalPhyloDiv, "Calculate phylogenetic diversity");		
	ioCommands.AddCommand (kCmd_AnalShannonDiv, "Calculate Shannon-Weiner index");
	ioCommands.AddCommand (kCmd_AnalSimpsonDiv, "Simpson index");
	ioCommands.AddCommand (kCmd_AnalBrillDiv, "Brillioun index");
	ioCommands.AddCommand (kCmd_AnalPieDiv, "PIE diversity index");
	ioCommands.AddCommand (kCmd_AnalMacintoshDiv, "Macintosh's index of diversity");
	ioCommands.AddCommand (kCmd_AnalMargelefDiv, "Margelef index");
	ioCommands.AddCommand (kCmd_AnalMehinickDiv, "Menhinick index");
	ioCommands.AddCommand (kCmd_AnalSiteComp, "Site-species complementarity");
	ioCommands.AddCommand (kCmd_AnalPhyloAgeTree, "Calculate tree information");		
	ioCommands.AddCommand (kCmd_AnalNodeInfo, "Calculate information over nodes of tree");		
	ioCommands.AddCommand (kCmd_XAnalNodeInfo, "Calculate information over nodes (exp)");		
	ioCommands.AddCommand (kCmd_AnalFusco, "Calculate Fusco imbalance");		
	ioCommands.AddCommand (kCmd_AnalSlowinski, "Calculate Slowinski-Guyer imbalance");	
	ioCommands.AddCommand (kCmd_AnalShaosNbar, "Calculate Shao's N-bar imbalance");
	ioCommands.AddCommand (kCmd_AnalShaosSigmaSq, "Calculate Shao's Sigma-squared imbalance");
	ioCommands.AddCommand (kCmd_AnalCollessC, "Calculate Colless' C imbalance");
	ioCommands.AddCommand (kCmd_AnalB1, "Calculate Shao's B1 balance");
	ioCommands.AddCommand (kCmd_AnalB2, "Calculate Shao's B2 balance");
	ioCommands.AddCommand (kCmd_AnalStemminess, "Calculate stemminess");
	ioCommands.AddCommand (kCmd_AnalResolution, "Calculate resolution");
	ioCommands.AddCommand (kCmd_AnalUltrametric, "Calculate ultrametricity");

	// ioCommands.AddCommand (kCmd_AnalCaic, "Calculate CAIC analysis");
	// ioCommands.AddCommand (kCmd_AnalMacroCaic, "Calculate MacroCAIC analysis");
}


void MesaConsoleApp::loadRuleCmds (CommandMgr& ioCommands)
{
	ioCommands.AddCommand (kCmd_RuleNull, "null (metronome) rule");		
	ioCommands.AddCommand (kCmd_RuleMarkovSp, "speciation (markovian)");	
	ioCommands.AddCommand (kCmd_RuleLogisticSp, "speciation (logistic)");	
	ioCommands.AddCommand (kCmd_RuleLatentSp, "speciation (with latency)");		
	ioCommands.AddCommand (kCmd_RuleBiasedSp, "speciation (biased by age)");		
	ioCommands.AddCommand (kCmd_RuleBiasedTraitSp, "speciation (biased by cont trait)");		
	ioCommands.AddCommand (kCmd_RuleBiasedTraitSp_New, "speciation (biased by trait, new)");		
	ioCommands.AddCommand (kCmd_RuleMarkovKill, "extinction (markovian)");		
	ioCommands.AddCommand (kCmd_RuleLogisticKill, "extinction (logistic)");		
	ioCommands.AddCommand (kCmd_RuleBiasedKill, "extinction (biased by age)");
	ioCommands.AddCommand (kCmd_RuleBiasedTraitKill, "extinction (biased by cont. trait)");
	ioCommands.AddCommand (kCmd_RuleMassKillFixedNum, "mass extinction (fixed number)");
	ioCommands.AddCommand (kCmd_RuleMassKillPercent, "mass extinction (fixed fraction)");
	ioCommands.AddCommand (kCmd_RuleMassKillProb, "mass extinction (fixed probability)");
	ioCommands.AddCommand (kCmd_RuleMassKillIf, "mass extinction (selected by trait)");
	ioCommands.AddCommand (kCmd_RuleMassKillBiasedTrait, "mass extinction (biased by cont. trait)");
//	ioCommands.AddCommand (kCmd_RuleMassKillRangeTrait, "mass extinction of fraction by trait");
	ioCommands.AddCommand (kCmd_RuleSymSpecTraitEv, "trait evol (symmetrical at speciation)");
	ioCommands.AddCommand (kCmd_RuleAsymSpecTraitEv, "trait evol (asymmetrical at speciation)");
	ioCommands.AddCommand (kCmd_RuleTerminalTraitEv, "trait evol (terminal)");
	ioCommands.AddCommand (kCmd_RuleGradualTraitEv, "trait evol (gradual)");
}


void MesaConsoleApp::loadSystemCmds (CommandMgr& ioCommands)
{
	ioCommands.AddCommand (kCmd_SysActionDupTree, "Duplicate active tree");		
	ioCommands.AddCommand (kCmd_SysActionSave, "Save current state");		
	ioCommands.AddCommand (kCmd_SysActionSetLabels, "Set labels");		
	ioCommands.AddCommand (kCmd_SysActionSetPreservation, "Set preservation of taxa");
	ioCommands.AddCommand (kCmd_SysActionConsolidateTaxa, "Consolidate taxa");	
	ioCommands.AddCommand (kCmd_SysActionMakeNeont, "Make trees neontological");	
	ioCommands.AddCommand (kCmd_SysActionDeleteDeadTaxa, "Delete dead taxa");
	ioCommands.AddCommand (kCmd_SysActionDeleteDeadTraits, "Delete dead traits");
	ioCommands.AddCommand (kCmd_SysActionCollapseSingletonsTaxa, "Collapse singletons");	
}


BasicAction* MesaConsoleApp::newAction (cmdId_t iUserCmd)
{
	// more concise to list all these variables up here
	BasicAction* theActionP = NULL;
	
	int      theChoice = -1;
	int		theRichCol = kColIndex_None;
	bool		theSizeListed = false;
	int		theLoops = 0;
   bool     theRestartIfDead = false;
	string	theBaseName;
	bool		theAdvanceEpoch = false;
   saveFile_t   theFileType;
	
	cout << endl;
	
	switch (iUserCmd)
	{
		case kCmd_AnalExTaxa:
		{
			theRichCol = askSppRichnessCol ();
			theActionP = (BasicAnalysis*) new CountExtantTaxaAnalysis (theRichCol);
			break;
		}

		case kCmd_AnalAllTaxa:
		{
			theRichCol = askSppRichnessCol ();
			theActionP = (BasicAnalysis*) new CountAllTaxaAnalysis (theRichCol);
			break;
		}

		case kCmd_AnalGeneticDiv:
		{
			theActionP = (BasicAnalysis*) new GeneticDiversityAnalysis;
			break;
		}

		case kCmd_AnalPhyloDiv:
		{
			theActionP = (BasicAnalysis*) new PhyloDiversityAnalysis;
			break;
		}

		case kCmd_AnalJackknifeGd:
		{
			theActionP = (BasicAnalysis*) new JackknifeGeneticDivAnalysis;
			break;
		}
			
		case kCmd_AnalJackknifePd:
		{
			theActionP = (BasicAnalysis*) new JackknifePhyloDivAnalysis;
			break;
		}
		
		case kCmd_AnalBootstrapGd:
		{
			long theNumReps = askLong ("How many bootstrap replicates",
				1, kAnswerBounds_None);
			long theNumSamples = askLong ("How many samples in each replicate",
				1, kAnswerBounds_None);
			theActionP = (BasicAnalysis*) new BootstrapGeneticDivAnalysis (theNumReps, theNumSamples);
		break;
		}
		
		case kCmd_AnalBootstrapPd:
		{
			long theNumReps = askLong ("How many bootstrap replicates",
				1, kAnswerBounds_None);
			long theNumSamples = askLong ("How many sample in each replicate",
				1, kAnswerBounds_None);
			theActionP = (BasicAnalysis*) new BootstrapPhyloDivAnalysis (theNumReps, theNumSamples);
		break;
		}
			
		case kCmd_AnalShannonDiv:
		{
			theActionP = (BasicAnalysis*) new ShannonWeinerDiversityAnalysis;
			break;
		}

		case kCmd_AnalSimpsonDiv:
		{
			theActionP = (BasicAnalysis*) new SimpsonDiversityAnalysis;
			break;
		}

		case kCmd_AnalBrillDiv:
		{
			theActionP = (BasicAnalysis*) new BrillouinDiversityAnalysis;
			break;
		}

		case kCmd_AnalPieDiv:
		{
			theActionP = (BasicAnalysis*) new PieDiversityAnalysis;
			break;
		}

		case kCmd_AnalMargelefDiv:
		{
			theActionP = (BasicAnalysis*) new MargelefDiversityAnalysis;
			break;
		}

		case kCmd_AnalMacintoshDiv:
		{
			theActionP = (BasicAnalysis*) new MacintoshDiversityAnalysis;
			break;
		}

		case kCmd_AnalMehinickDiv:
		{
			theActionP = (BasicAnalysis*) new MenhinickDiversityAnalysis;
			break;
		}
			
		case kCmd_AnalSiteComp:
		{
			theActionP = (BasicAnalysis*) new SiteComplementarityAnalysis;
			break;
		}
			
		case kCmd_AnalPhyloAgeTree:
		{
			bool theCountNodes = askYesNo ("Count all nodes");
			bool theCountTips = askYesNo ("Count leaves");
			bool theCountAlive = askYesNo ("Count extant taxa");
			bool theCalcPaleo = askYesNo ("Check if paleo- or neontological");
			bool theCalcAge = askYesNo ("Calculate phylogenetic age");
			theActionP = (BasicAnalysis*) new TreeInfoAnalysis (theCountNodes,
				theCountTips, theCountAlive, theCalcPaleo, theCalcAge);
			break;
		}

		case kCmd_AnalNodeInfo:
		{
			nodetype_t theTargetNodetype;
			bool theCalcAges, theCalcTimeToParent, theCalcChildren, theCalcLeaves,
				theCalcSubtree, theCalcSiblings,theCalcHeight, theCalcTimeToRoot;
			theTargetNodetype = 	nodetype_t (askChoice ("For all, leaves or internal",
				"ali", 0) + kNodetype_All);
			theCalcAges = askYesNo ("Calculate node age");
			theCalcTimeToParent = askYesNo ("Calculate time to parent");
			theCalcChildren = askYesNo ("Count immediate children");
			theCalcLeaves = askYesNo ("Count leaves subtended");
			theCalcSubtree = askYesNo ("Calculate size of subtree subtended");
			theCalcSiblings = askYesNo ("Count number of siblings");
			theCalcHeight = askYesNo ("Calculate height of node");
			theCalcTimeToRoot = askYesNo ("Calculate time to root node");
			theActionP = (BasicAnalysis*) new NodeInfoAnalysis (theTargetNodetype,
				theCalcAges, theCalcTimeToParent, theCalcChildren, theCalcLeaves,
				theCalcSubtree, theCalcSiblings, theCalcHeight, theCalcTimeToRoot);
			break;
		}

		case kCmd_XAnalNodeInfo:
		{
			Report ("This analysis will calculate various metrics over a subset of nodes "
				"determined by the user.");
			NodeSelector* theNodeSelectorP = askNodeSelector ();
				
			bool theSelectionCount, theCalcAges, theCalcTimeToParent, theCalcChildren, theCalcLeaves,
				theCalcSubtree, theCalcSiblings,theCalcHeight, theCalcTimeToRoot;
			theSelectionCount = askYesNo ("Count number of nodes in selection");
			theCalcAges = askYesNo ("Calculate node age");
			theCalcTimeToParent = askYesNo ("Calculate time to parent");
			theCalcChildren = askYesNo ("Count immediate children");
			theCalcLeaves = askYesNo ("Count leaves subtended");
			theCalcSubtree = askYesNo ("Calculate size of subtree subtended");
			theCalcSiblings = askYesNo ("Count number of siblings");
			theCalcHeight = askYesNo ("Calculate height of node");
			theCalcTimeToRoot = askYesNo ("Calculate time to root node");
			
			theActionP = (BasicAnalysis*) new XNodeInfoAnalysis (theNodeSelectorP, theSelectionCount,
				theCalcAges, theCalcTimeToParent, theCalcChildren, theCalcLeaves,
				theCalcSubtree, theCalcSiblings, theCalcHeight, theCalcTimeToRoot);
		break;
		}
			
		/*
		case kCmd_AnalListNodes:
			theActionP = (BasicAnalysis*) new ListNodesAnalysis;
			break;
		*/
		
		case kCmd_AnalStemminess:
		{
			theActionP = (BasicAnalysis*) new StemminessAnalysis;
			break;
		}

		case kCmd_AnalCaic:
		{
			int theDiscreteData = askDiscCol (kAnswer_NotRequired);
			int theContPredictorCol;
			if (theDiscreteData == kColIndex_None)
			{
				theContPredictorCol = askContTraitCol ("Select the continuous "
					"predictor column");
			}
			else
			{
				theContPredictorCol = kColIndex_None;
			}
			vector<int> theContCols;
			int theNextContCol;
			while ((theNextContCol = askContCol (kAnswer_NotRequired)) != kColIndex_None)
				theContCols.push_back (theNextContCol);
			/* theActionP =  (BasicAnalysis*) new CaicAnalysis (theDiscreteData,
				theContPredictorCol, theContCols); */
		break;
		}

		case kCmd_AnalMacroCaic:
		{
			assert (false);
			break;
		}

			
		case kCmd_AnalFusco:
		{
			theRichCol = askSppRichnessCol ();
			bool theSizeListed = AskYesNoQuestion ("List the node size");
			bool theExtendedTreatment = AskYesNoQuestion ("Use extended calculation");
			bool theCorrection = AskYesNoQuestion ("Use I' correction");
			theActionP =  (BasicAnalysis*) new FuscoAnalysis (theRichCol,
				theSizeListed, theExtendedTreatment, theCorrection);
			break;
		}

		/*
		case kCmd_AnalFuscoAll:
		
		case kCmd_AnalFuscoExtendedAll:
			theRichCol = askSppRichnessCol ();
			theSizeListed = AskYesNoQuestion ("List the node size");
			theActionP = (BasicAnalysis*) new FuscoExtendedAllAnalysis (theSizeListed, theRichCol);
			break;

		case kCmd_AnalFuscoWeighted:
			theActionP = (BasicAnalysis*) new FuscoWeightedAnalysis;
			break;

		case kCmd_AnalFuscoExtended:
			theRichCol = askSppRichnessCol ();
			theActionP = (BasicAnalysis*) new FuscoExtendedAnalysis (theRichCol);
			break;
		*/
		
		case kCmd_AnalSlowinski:
		{
			theRichCol = askSppRichnessCol ();
			theSizeListed = AskYesNoQuestion ("List the node size");
			theActionP = (BasicAnalysis*) new SlowinskiGuyerAnalysis (theSizeListed, theRichCol);
			break;
		}

		case kCmd_AnalShaosNbar:
		{
			theActionP = (BasicAnalysis*) new ShaosNbarAnalysis;
			break;
		}
			
		case kCmd_AnalShaosSigmaSq:
		{
			theActionP = (BasicAnalysis*) new ShaosSigmaSqAnalysis;
			break;
		}

		case kCmd_AnalCollessC:
		{
			theActionP = (BasicAnalysis*) new CollessCAnalysis;
			break;
		}

		case kCmd_AnalB2:
		{
			theActionP = (BasicAnalysis*) new B2Analysis;
			break;
		}

		case kCmd_AnalB1:
		{
			theActionP = (BasicAnalysis*) new B1Analysis;
			break;
		}

		case kCmd_AnalResolution:
		{
			theActionP = (BasicAnalysis*) new ResolutionAnalysis;
			break;
		}

		case kCmd_AnalUltrametric:
		{
			theActionP = (BasicAnalysis*) new UltrametricAnalysis;
			break;
		}

		case kCmd_MacroRunOnce:
		{
			theActionP = new RunOnceMacro;
			break;
		}

		case kCmd_MacroRunN:
		{
			theLoops	= askIntegerWithMin ("Run how many times", 1);
			theActionP = new RunNMacro (theLoops);
			break;
		}

		case kCmd_MacroRunAndRestore:	
		{
			theLoops	= askIntegerWithMin ("Run & restore  how many times", 1);
			theActionP = new RunAndRestoreMacro (theLoops);
			break;
		}

		case kCmd_MacroRunTree:
		{
			theActionP = new TreeMacro;
			break;
		}

		case kCmd_EpochPopLimit:
		{
			theChoice = askChoice ("Count all nodes, leaves or extant taxa", "alx");
			nodetype_t theNodeType = (nodetype_t) theChoice;
			theLoops	= askIntegerWithMin ("Evolve until this number reaches", 2);
			theAdvanceEpoch = askYesNo ("Advance until next event");
 			theRestartIfDead = askYesNo ("Restart the epoch if all taxa die");
			theActionP = new EpochPopLimit (theLoops, theAdvanceEpoch, theNodeType, theRestartIfDead);
			break;
		}

		case kCmd_EpochTimeLimit:
		{
			mesatime_t theTimeLimit	= askDouble ("Evolve until time reaches", 0, kAnswerBounds_None);
 			theRestartIfDead = askYesNo ("Restart the epoch if all taxa die");
			theActionP = new EpochTimeLimit (theTimeLimit, theRestartIfDead);
			break;
		}

		case kCmd_SysActionSetPreservation:
		{
			theChoice = askChoice ("Preserve no nodes, the root, root & children", "nrc",
					theChoice);
			pref_preservenodes_t thePreserveChoice = (pref_preservenodes_t) theChoice;
			cout << "Set to: " << kPrefPreserveNodes_Cstrs[thePreserveChoice] << endl;
			theActionP = (BasicAction*) new PreserveTaxaSysAction (thePreserveChoice);
			break;
		}
			
		case kCmd_SysActionConsolidateTaxa:
		{
			theActionP = (BasicAction*) new ConsolidateTaxaSysAction;
			break;
		}

		case kCmd_SysActionDeleteDeadTaxa:
		{
			theActionP = (BasicAction*) new DeleteDeadTaxaSysAction;
			break;
		}

		case kCmd_SysActionDeleteDeadTraits:
		{
			theActionP = (BasicAction*) new DeleteDeadTraitsSysAction;
			break;
		}

		case kCmd_SysActionMakeNeont:
		{
			theActionP = (BasicAction*) new MakeNeontSysAction;
			break;
		}
		
		case kCmd_SysActionCollapseSingletonsTaxa:
		{
			theActionP = (BasicAction*) new CollapseSingletonsSysAction;
			break;
		}

		case kCmd_SysActionDupTree:
		{
			theActionP = (BasicAction*) new DupTreeSysAction;
			break;
		}

		case kCmd_SysActionShuffleTraits:
		{
			traittype_t   theTraitType;
			colIndex_t    theTraitIndex = -2; // illegal init
			
			if (askYesNo ("Shuffle all traits"))
				theTraitType = kTraittype_All;
			else
				askSelectTrait (theTraitType, theTraitIndex);
				
			theActionP = (BasicAction*) new ShuffleTraitsSysAction (theTraitType, theTraitIndex);
		break;
		}
		
		case kCmd_SysActionSave:
		{
			theBaseName = askString ("Name the savefile (max. 16 char)");
			theFileType = kSaveFile_Caic;
			if (askEitherOr ("Save as nexus or caic", 'n', 'c'))
				theFileType = kSaveFile_Nexus;
			theActionP = (BasicAction*) new SaveSysAction (theBaseName.c_str(), theFileType);
			break;
		}
			
		case kCmd_SysActionSetLengths:
		{
			/// @todo change to a less terse question
			Report ("Transform branch lengths:");
			treelenchange_t theChange = (treelenchange_t) askChoice
				("Transform branchlengths by setting, multiplying, adding, "
				"changing by a random number (a fixed amount or fraction of "
				"the current length)", "smaxr", 1);
			double theNewLength;
			if (theChange == kTreeLenChange_Add)
				theNewLength = askDouble ("Factor");
			else
			{
				do 
					theNewLength = askDouble ("Factor");
				while (theNewLength <= 0.0);
			}
			theActionP = (BasicAction*) new SetTreeLengthSysAction (theNewLength, theChange);
		break;
		}

		case kCmd_RulePruneFixedNum:
		{
			int theFixedNum = askIntegerWithMin ("Prune how many taxa", 0);
			theActionP = (BasicAction*) new PruneFixedNumAction (theFixedNum);		
		break;
		}
		 
		case kCmd_RulePrunFixedFrac:
		{
			double thePercent = askDouble ("Prune what fraction of taxa", 0.0, 100.0);
			theActionP = (BasicAction*) new PruneFixedFracAction (thePercent);				
		break;
		}
		 
		case kCmd_RulePrunProb:
		{
			double theProb = askDouble ("Prune taxa with probability", 0.0, 100.0);
			theActionP = (BasicAction*) new PruneByProbAction (theProb);			
		break;
		}
		
		case kCmd_RulePrunByTrait:
		{
			colIndex_t theCharCol = askContTraitCol ("Select the continuous trait to be pruned on");
			double theParamA, theParamB, theParamC;
			askRate (theParamA, theParamB, theParamC,
				"Enter 3 params (A*x^B + C) for calculating prob. of pruning:");
			theActionP = (BasicAction*) new PruneByTraitAction (theCharCol,
				theParamA, theParamB, theParamC);		
		break;
		}
		
		case kCmd_RulePrunIf:
		{
			CharComparator theTest = askSppTest ("Define how taxa are to be selected for pruning");
			theActionP = (BasicAction*) new PruneIfAction (theTest);		
		break;
		}

		case kCmd_SysActionSetLabels:
		{
			pref_cladelabels_t theLabelPref; 	
			char theNodeChoice = askMultiChoice ("Label clades phylo, caic, series", "pcs");
			switch (theNodeChoice)
			{
				case 'p':
					theLabelPref = kPrefCladeLabels_Phylo;
					break;
				case 'c':
					theLabelPref = kPrefCladeLabels_Caic;
					break;
				case 's':
					theLabelPref = kPrefCladeLabels_Series;
					break;
				default:
					assert (false);
			}				
			theActionP = (BasicAction*) new SetLabelsSysAction (theLabelPref);
			break;
		}

		default:
		{
			assert (false);
			break;
		}
	}
	
	return theActionP;
}


EvolRule* MesaConsoleApp::newRule (cmdId_t iUserCmd)
{
	EvolRule*   theRuleP = NULL;
	double		theRate = 0.0;
	double      theFreqA, theFreqB, theFreqC;
	double      thePeriod;
	colIndex_t  theCharCol;
	SchemeArr   theSchemeArr;
	CharComparator theTest;
	SymmetricSceRule* theSRuleP = NULL;
   int         capacity;
	
	cout << endl;

	switch (iUserCmd)
	{
		case kCmd_RuleMassKillIf:
		{
			askRate (theFreqA);
			theTest = askSppTest ("Define species to be killed");
			theRuleP = (EvolRule*) new MassKillIf (theFreqA, theTest);
			break;
		}
			
		case kCmd_RuleMassKillFixedNum:
		{
			askRate (theFreqA);
			int theFixedNum = askIntegerWithMin ("Kill how many species", 0);
			theRuleP = (EvolRule*) new MassKillFixedNumRule (theFreqA, theFixedNum);
			break;
		}
			
		case kCmd_RuleMassKillPercent:
		{
			askRate (theFreqA);
			double thePercent = askDouble ("Kill what percent of species", 0.0, 100.0);
			theRuleP = (EvolRule*) new MassKillFixedNumRule (theFreqA, thePercent);		
			break;
		}
			
		case kCmd_RuleMassKillProb:
		{
			askRate (theFreqA);
			double theProb = askDouble ("Kill species with what probability", 0.0, 100.0);
			theRuleP = (EvolRule*) new MassKillFixedNumRule (theFreqA, theProb);	
			break;
		}

		case kCmd_RuleMassKillBiasedTrait:
		{
			askRate (theFreqA);
			theCharCol = askContTraitCol (NULL);
			askRate (theFreqA, theFreqB, theFreqC,
				"Enter 3 parameters (A*x^B + C) for determining probability:");
			theRuleP = (EvolRule*) new MassKillTraitBiasedRule (theRate, theCharCol, theFreqA,
				theFreqB, theFreqC);
			break;
		}
			

/* TO DO: kill top/bottom x%			
		case kCmd_RuleMassKillRangeTrait:
			traittype_t   theCharType;
			colIndex_t   theCharIndex = -1;
			askSelectTrait (theCharType, theCharIndex);
			theRuleP = (EvolRule*) new MassKillFixedNumRule (theFreqA, theProb);	
	
			break;
*/
			
		case kCmd_RuleNull:
		{
			askRate (theRate);
			theRuleP = (EvolRule*) new NullRule (theRate);
			break;
		}
			
		case kCmd_RuleMarkovSp:
		{
			askRate (theRate);
			theRuleP = (EvolRule*) new MarkovSpRule (theRate);
			break;
		}

      case kCmd_RuleLogisticSp:
		{
			askRate (theRate);
         capacity = askIntegerWithMin ("Carrying capacity / limit", 1);
			theRuleP = (EvolRule*) new LogisticSpRule (theRate, capacity);
			break;
		}

      case kCmd_RuleLogisticKill:
		{
			askRate (theRate);
         capacity = askIntegerWithMin ("Carrying capacity / limit", 1);
			theRuleP = (EvolRule*) new LogisticKillRule (theRate, capacity);
			break;
		}
         
		case kCmd_RuleBiasedSp:
		{
			askRate (theFreqA, theFreqB, theFreqC);
			theRuleP = (EvolRule*) new AgeBiasedSpRule (theFreqA, theFreqB,
				theFreqC);
			break;
		}
			
		case kCmd_RuleLatentSp:
		{
			askRate (theFreqA);
			thePeriod = AskFloatQuestion ("What is the latency period");
			assert (0.0 < thePeriod);
			theRuleP = (EvolRule*) new LatentSpRule (theFreqA, thePeriod);
			break;
		}
			
		case kCmd_RuleBiasedTraitSp:
		{
			theCharCol = askContTraitCol (NULL);
			askRate (theFreqA, theFreqB, theFreqC);
			theRuleP = (EvolRule*) new CharBiasedSpRule (theCharCol, theFreqA, theFreqB,
				theFreqC);
			break;
		}

		case kCmd_RuleBiasedTraitSp_New: // for testing purposes
		{
			cout << "You will now select the rate of speciation & how it varies." << endl;
			XBasicRate* theRateP = askRate_New (); 
			theRuleP = (EvolRule*) new CharBiasedSpRule_New (theRateP);
			break;
		}

		case kCmd_RuleBiasedTraitKill:
		{
			theCharCol = askContTraitCol (NULL);
			askRate (theFreqA, theFreqB, theFreqC);
			theRuleP = (EvolRule*) new CharBiasedKillRule (theCharCol, theFreqA, theFreqB,
				theFreqC);
			break;
		}
			
		case kCmd_RuleMarkovKill:
		{
			askRate (theFreqA);
			theRuleP = (EvolRule*) new MarkovKillRule (theFreqA);
			break;
		}
			
		case kCmd_RuleBiasedKill:
		{
			askRate (theFreqA, theFreqB, theFreqC);
			theRuleP = (EvolRule*) new BiasedKillRule (theFreqA, theFreqB,
				theFreqC);
			break;
		}

		case kCmd_RuleSymSpecTraitEv:
		{
			theSRuleP = new SymmetricSceRule ();
			askForSchemes (theSchemeArr, "Enter char evol scheme for symmetrical speciation rule");
			theSRuleP->adoptScheme (theSchemeArr);
			theRuleP = (EvolRule*) theSRuleP;
			break;
		}
			
		case kCmd_RuleAsymSpecTraitEv: 
		{
			AsymmetricSceRule* theARuleP = new AsymmetricSceRule ();
			askForSchemes (theSchemeArr, "Enter char evol schemes for asymmetrical speciation rule (left)");
			theARuleP->adoptScheme (0, theSchemeArr);
			askForSchemes (theSchemeArr, "Enter char evol schemes for asymmetrical speciation rule (right)");
			theARuleP->adoptScheme (1, theSchemeArr);
			theRuleP = (EvolRule*) theARuleP;
			break;
		}
			
		case kCmd_RuleTerminalTraitEv:
		{
			TerminalCharEvolRule* thePRuleP = new TerminalCharEvolRule ();
			askForSchemes (theSchemeArr, "Enter char evol scheme for terminal rule");
			theSRuleP->adoptScheme (theSchemeArr);
			theRuleP = (EvolRule*) theSRuleP;
			break;
		}
			
		case kCmd_RuleGradualTraitEv:
		{
			GradualCharEvolRule* theTRuleP = new GradualCharEvolRule ();
			askForSchemes (theSchemeArr, "Enter char evol schemes for gradual evolution rule");
			theTRuleP->adoptScheme (theSchemeArr);
			theRuleP = (EvolRule*) theTRuleP;
			break;
		}
				
	
		default:
		{
			assert (false);
			break;
		}
	}
	
	return theRuleP;
}


// *** DEPRECATED FUNCTIONS *********************************************/


void MesaConsoleApp::validate ()
//: Just some scaffolding for testing the app
{
}


colIndex_t MesaConsoleApp::askForSite (const char* ikPrompt)
{
	// Preconditions:
	assert (mModel->countSiteTraits() == 0);
	
	// Main:
	// print menu
	if (ikPrompt != NULL)
		Report (ikPrompt);
	else
		Report ("Select a site:");

	vector<colIndex_t> thePossibleSites;
	colIndex_t theNumConTraits = mModel->countContTraits();
	for (colIndex_t i = 0; i < theNumConTraits; i++)
	{
		if (mModel->isSiteTrait (i))
		{
			cout << "   " << i << ") " << mModel->getSiteName (i) << endl;
			thePossibleSites.push_back (i);
		}
	}
	
	// get and check answer
	colIndex_t theAnswer = (int) askInteger ("Select a site");
	if (isMemberOf (theAnswer, thePossibleSites.begin(), thePossibleSites.end()))
		return theAnswer;
	else
	{
		ReportError ("That isn't a valid site.");
		return askForSite (ikPrompt);
	}
}

/**
Ask user to select multiple sites.

@param   oSiteIndexes   vector of site indices

@todo   ensure at least one site chosen
@note   returns true (0-based) index of sites, not user (1-based) index
@note   sites can be chosen multiple times
*/
void MesaConsoleApp::askForSites (vector<colIndex_t>& oSiteIndexes, const char* ikPrompt)
{
	// Preconditions:
	assert (mModel->countSiteTraits() != 0);
	
	// Main:
	// collate possible answers
	const colIndex_t kStopSelecting = -1;
	vector<colIndex_t> thePossibleSites;
	colIndex_t theNumConTraits = mModel->countContTraits();
	thePossibleSites.push_back (kStopSelecting);
	for (colIndex_t i = 0; i < theNumConTraits; i++)
	{
		if (mModel->isSiteTrait (i))
			thePossibleSites.push_back (i);
	}
	
	// print menu
	if (ikPrompt != NULL)
		Report (ikPrompt);
	else
		Report ("Select multiple sites:");
	
	colIndex_t theAnswer = kStopSelecting;
	do
	{
		Report ("Select a site:");
		for (colIndex_t i = 0; i < theNumConTraits; i++)
		{
			if (mModel->isSiteTrait (i) and
			   (not isMemberOf (i, oSiteIndexes.begin(), oSiteIndexes.end())))
			{
				cout << "   " << i << ") site " << mModel->getSiteName (i) << endl;
			}
		}
		
		theAnswer = (int) askInteger ("Select a site (or -1 to stop selecting)");
		if (theAnswer == kStopSelecting)
			break;
		else if (isMemberOf (theAnswer, thePossibleSites.begin(), thePossibleSites.end()))
			oSiteIndexes.push_back (theAnswer);
		else 
			ReportError ("That isn't a valid site.");
	}
	while (theAnswer != kStopSelecting);
	
	cout << "You have selected sites ";
	for (vector<colIndex_t>::iterator p = oSiteIndexes.begin(); p != oSiteIndexes.end();
		p++)
	{
		cout << mModel->getSiteName (*p) << " ";
	}
	cout << endl;
}

#include "ComboMill.h"


void MesaConsoleApp::doXPruneSites ()
{
	cout << endl;
	Report ("PRESERVE - eliminate sites and then prune extinct taxa:");
	cout << endl;
	Report ("These set of calculations allow you to test the efficiacy of "
		"different reserve schemes in preserving diversity. Using a set of "
		"site abundance data (i.e. the number of examples of a given taxa "
		"seen at different sites), sites can be omitted and the impact on "
		"biodiversity observed via changes in various diversity metrics. "
		"You can specifically choose the sites to be omitted or "
		"systematically omit combinations of sites.");
	cout << endl; 
	
	// select the sites to be pruned
	typedef vector<colIndex_t>   indexarr_t;
	indexarr_t                   theSiteIndexes; // array of sites
	vector<indexarr_t>           theSiteArrays;  // array of site arrays
	
	if (askEitherOr ("Pick sites to be omitted or test site combinations", 'p', 'c'))
	{
		askForSites (theSiteIndexes, "Select sites to be omitted");
		theSiteArrays.push_back (theSiteIndexes);
	}
	else
	{
		int theNumSites = mModel->countSiteTraits();
		Report ("This will test the effects of omitting all combinations "
			"of sites of a given size (e.g. deleting all possible pairs of "
			"sites, deleting every set of 3 to 5 sites etc). Warning: if you "
			"pick a wide range of combinations, it will take a long time to "
			"test them all.");
		int theMin = askIntegerWithBounds ("Omit at least how many sites",
			1, theNumSites - 1);
		int theMax = 0;
		if (theMin < (theNumSites - 1))
		{
			theMax = askIntegerWithBounds ("Omit at most how many sites",
				theMin, theNumSites - 1);
		}
		else
		{
			theMax = theMin;
		}
		mModel->mContData.listSiteTraits (theSiteIndexes);
		ComboMill<indexarr_t> theSiteComboMill (theSiteIndexes.begin(), theSiteIndexes.end());
		indexarr_t theComboArr (theSiteIndexes);
		indexarr_t::iterator theSetIter = theComboArr.begin();
		
		theSiteComboMill.firstKJ (theMin,theMax);	
		while (not theSiteComboMill.isLast())
		{
			theSiteComboMill.getCurrent (theComboArr.begin(), theSetIter);
			// PrintContainer (theComboArr.begin(), theSetIter);
			theSiteArrays.push_back (indexarr_t (theComboArr.begin(), theSetIter));
			theSiteComboMill.nextKJ (theMin,theMax);
		}
	}
	cout << endl;
	
	bool theRootIsLeft = askYesNo ("Preserve root of tree (pruning will not reroot)");
	bool theListExtinctTaxa = askYesNo ("List extinct taxa");

	// select the analyses to be done before and after pruning
	vector<BasicAnalysis*> theAnalysisPtrs;
	CommandMgr	theAnalCmds ("Select analyses to be done before & after pruning");
	loadAnalysisCmds (theAnalCmds);
	theAnalCmds.AddCommand (kCmd_AnalJackknifePd, "Jackknife estimate of phylogenetic diversity");		
	theAnalCmds.AddCommand (kCmd_AnalJackknifeGd, "Jackknife estimate of genetic diversity");		
	theAnalCmds.AddCommand (kCmd_AnalBootstrapPd, "Bootstrap estimate of phylogenetic diversity");
	theAnalCmds.AddCommand (kCmd_AnalBootstrapGd, "Bootstrap estimate of genetic diversity");
	theAnalCmds.AddCommand (kCmd_Return, 'r', "Finished");
	theAnalCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		// activate the right commands, get user command & process
		updateCommandMgr (theAnalCmds);		
		theUserCmd = AskUserCommand (&theAnalCmds);
		BasicAnalysis* theAnalysisP = NULL;
		
		switch (theUserCmd)
		{
			case kCmd_Return:
				// break from switch then do-loop ends
				break;
										
			default:
				// if you reach this pt, it's an analysis
				theAnalysisP = (BasicAnalysis*) newAction (theUserCmd);
				assert (theAnalysisP != NULL);
				theAnalysisPtrs.push_back (theAnalysisP);
				break;
		}
	}
	while (theUserCmd != kCmd_Return);			
	
	// do analyses before pruning
	cout << endl;	
	Report ("Executing analyses before omitting sites:");
	for (int i = 0; i < theAnalysisPtrs.size(); i++)
		(theAnalysisPtrs[i])->execute();
	
	// save data before pruning so it can be restored
	mModel->backupData();
	
	
	for (int a = 0; a < theSiteArrays.size(); a++)
	{
		// prune sites (i.e. reduce them to population zero)
		/// @todo have it print out names and not just numbers
		cout << endl;	
		cout << "Omitting " << theSiteArrays[a].size() <<  " site(s):";
		for (vector<colIndex_t>::iterator p = theSiteArrays[a].begin();
			p != theSiteArrays[a].end(); p++)
		{
			cout << " " << mModel->getSiteName (*p);
		}
		cout << endl;
		/// As per RHCs wishes, print out a list of those sites not culled
		cout << "Retaining " << (theSiteIndexes.size() - theSiteArrays[a].size()) <<  " site(s):";
		for (indexarr_t::iterator q = theSiteIndexes.begin();
			q != theSiteIndexes.end(); q++)
		{
			if (not isMemberOf (*q, theSiteArrays[a].begin(), theSiteArrays[a].end()))
				cout << " " << mModel->getSiteName (*q);
		}
		cout << endl;
		
		colIndex_t theNumTaxa = mModel->countTaxa();
		for (colIndex_t i = 0; i < theNumTaxa; i++)
		{
			for (vector<colIndex_t>::iterator p = theSiteArrays[a].begin();
				p != theSiteArrays[a].end(); p++)
			{
				mModel->mContData.at (i, *p) = 0.0;
			}
		}
		
		// prune taxa with no individuals left
		colIndex_t theNumContTraits = mModel->countContTraits();
		vector<string> thePrunedNames;
		for (colIndex_t i = 0; i < theNumTaxa; i++)
		{
			bool theTaxaIsDead = true;
			for (colIndex_t j = 0; j < theNumContTraits; j++)
			{
				if (mModel->isSiteTrait (j) and (0.0000 < mModel->mContData.at (i, j)))
				{
					theTaxaIsDead = false;
					break;
				}
			}
			
			if (theTaxaIsDead == true)
				thePrunedNames.push_back (mModel->mContData.getRowName (i));
		}

		if (theListExtinctTaxa)
		{
			if (thePrunedNames.size() == 0)
			{
				Report ("No taxa have been made extinct.");
			}
			else
			{
				string theExtinctTaxa;
				for (int k = 0; k < thePrunedNames.size(); k++)
				{
					theExtinctTaxa += " ";
					theExtinctTaxa += thePrunedNames[k];
				}
				Report ("The following taxa are extinct and will be pruned:");
				Report (theExtinctTaxa.c_str());
			}
		}
		else
		{
			string theReportStr ("Number of extinct taxa: ");
			theReportStr += sbl::toString (thePrunedNames.size());
			Report (theReportStr.c_str()); 
		}
				
		BasicAction* thePruning = (BasicAction*) new PruneByName (thePrunedNames, theRootIsLeft);
		thePruning->execute();
		delete thePruning;
		
		// mModel->detailedReport(cout);
		// do previous analyses
		Report ("Executing analyses after pruning:");
		for (int i = 0; i < theAnalysisPtrs.size(); i++)
			(theAnalysisPtrs[i])->execute();
		
		// Cleanup:
		// restore data after manipulations
		mModel->restoreData();
	}
	
	// delete analyses
	for (int i = 0; i < theAnalysisPtrs.size(); i++)
		delete theAnalysisPtrs[i];
}


void MesaConsoleApp::doExperimentalMenu ()
{
	CommandMgr	theExperimentalCmds ("Experimental functions");
	
	theExperimentalCmds.AddCommand (kCmd_XPruneSites, "Omit sites & test diversity");		
		
	theExperimentalCmds.AddCommand (kCmd_Return, "r", "Return to main menu");

	theExperimentalCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		// activate the right commands
		updateCommandMgr (theExperimentalCmds);
		
		// get user command
		theUserCmd = AskUserCommand (&theExperimentalCmds);
		
		// process the command
		switch (theUserCmd)
		{
			case kCmd_XPruneSites:
				doXPruneSites ();
				break;
				
			case kCmd_Return:
				// leave the switch and drop out of the while
				break;
				
			default:
				assert (false);
		}
	}
	while (theUserCmd != kCmd_Return);	
}


void MesaConsoleApp::doDEBUG ()
{
	XBasicRate* theRateP = askRate_New ();

/*
	MesaTree* thePtr = getActiveTreeP();
	for (MesaTree::id_type i = 0; i < 10; i++)
		MesaTree::iterator p = thePtr->getIter (i);
	MesaTree::iterator q = thePtr->getOldestNode ();
	thePtr->validate(q);
	q = thePtr->getNextOldestNode (q);
	thePtr->validate(q);

	DBG_MSG ("starting iteration");
	for (q = thePtr->getOldestNode();
	     q != thePtr->end();
	     q = thePtr->getNextOldestNode (q))
	{
		DBG_MSG ("id no is " << q->first);
	}
	DBG_MSG ("ending iteration");
	

	DBG_MSG ("testing triparam");
	double theParam = 0.00001;
	for (int i = 0; i < 3; i++, theParam *= 100)
	{
		DBG_MSG ("Char = " << theParam);
		DBG_MSG ("params=(.1, -1.0, .1) : " << calcRateFromTriParameter (.1, -1.0, .1, theParam));
		DBG_MSG ("params=(1, 1, 1,) : " << calcRateFromTriParameter (1, 1, 1, theParam));
		DBG_MSG ("params=(0.1, -0.0001, 0.1) : " << calcRateFromTriParameter (0.1, -0.0001, 0.1, theParam));
		DBG_MSG ("params=(0.1, -5, 0.1) : " << calcRateFromTriParameter (0.1, -5, 0.1, theParam));
	}
	

	DBG_MSG (kCmd_RuleMassKillFixedNum);
	DBG_MSG (kCmd_RuleMassKillPercent);
	DBG_MSG (kCmd_RuleMassKillProb);
	DBG_MSG (kCmd_RuleMassKillIf);
	DBG_MSG (kCmd_SysActionMakeNeont);


	cout.precision (6);
	cout << .00000001;
		

	double theAnswer;
	theAnswer = askDouble ("Well");
	DBG_MSG ("The Answer is " << theAnswer);
	theAnswer = askDouble ("Well", 25.0, sbl::kNoBound);
	DBG_MSG ("The Answer is " << theAnswer);
	theAnswer = askDouble ("Well", sbl::kNoBound, -2);
	DBG_MSG ("The Answer is " << theAnswer);
	theAnswer = askDouble ("Well", 1, 10);
	DBG_MSG ("The Answer is " << theAnswer);
*/

//	DBG_MSG (MesaGlobals::mRng.gaussian (10, 2));

/*
	if (mModel != NULL)
		mModel->mActionQueue.dump();
*/		
/*
	SchemeArr  theArr;
	askForSchemes (theArr, "testing the schem loader");

	CharStateSet theStates = askForStates ("testing the charstateset");
	theStates.validate ();
	theStates.dump ();
*/
}



// *** END ***************************************************************/

