/**************************************************************************
ConsoleMenuApp - simple menu oriented console application

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- Derive from this to make bog-standard, C++ apps.

To Do:
- remove direct access to CommandMgr

**************************************************************************/


// *** INCLUDES

#include "ConsoleMenuApp.h"

#include <iostream>
#include <string>

using std::string;
using std::cout;
using std::endl;


SBL_NAMESPACE_START

// *** CONSTANTS & DEFINES

// *** MAIN BODY *********************************************************/

// *** LIFECYCLE *********************************************************/

// using defaults for all


// *** APPLICATION FLOW **************************************************/
void ConsoleMenuApp::Run ()
{
	LoadMenu(); // call user to load menu up	
	
	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		// update status of all commands
		for (int i = 0; i < mMainCommands.GetNumCommands (); i++)
		{
			cmdInfo_t* theCurrCmdP = mMainCommands.GetCommand (i);
			theCurrCmdP->mActive = UpdateCmd (theCurrCmdP->mCmd);
		}

		// ask the user for a command
		theUserCmd = AskUserCommand (&mMainCommands);
		
		// interpret it
		if (theUserCmd != kCmd_Quit)
		{
			try
			{
				ObeyCmd (theUserCmd);
			}
			catch (Error &theError)
			{
				ReportError (theError);
			}
			catch (...)
			{
				ReportError ("Unidentified error");
			}
		}
	}
	while (theUserCmd != kCmd_Quit);

	// exit runtime sequence
}


// *** UI FUNCTIONS ******************************************************/
cmdId_t ConsoleMenuApp::AskUserCommand (CommandMgr* iCommandMgr, const char* iMenuTitle)
{
	assert (iCommandMgr != 0);
	int theNumCommands = iCommandMgr->GetNumCommands();
	assert (0 <= theNumCommands);
	
	// draw menu
	string theMenuTitle;
	if (iMenuTitle != NULL)
		theMenuTitle = iMenuTitle;
	else if (iCommandMgr->mTitle != "")
		theMenuTitle = iCommandMgr->mTitle;
	else
		theMenuTitle = "Command Menu";
	
	cout << endl << theMenuTitle << ":" << endl;
	for (int i = 0; i < theNumCommands; i++)
	{
		cmdInfo_t *theCommandPtr = iCommandMgr->GetCommand (i);
		assert (theCommandPtr);
		PrintMenuChoice (theCommandPtr);
	}
	
	// ask for choice & check
	cmdId_t theReturnedCmd = iCommandMgr->GetCommandWithShortcut
		(askString ("What is your command"));
		
	// loop until we get a decent answer
	while (theReturnedCmd == kCmd_Null)
	{
		ReportError ("That's not a valid command");
		theReturnedCmd = iCommandMgr->GetCommandWithShortcut
			(askString ("What is your command"));
	}

	return theReturnedCmd;
}


void ConsoleMenuApp::PrintMenuChoice ( cmdInfo_t *iCommandPtr )
{
	if (iCommandPtr->mActive == true)
	{
		cout << "   " << iCommandPtr->mShortcut;
		cout << "> " << iCommandPtr->mMenuStr << endl;
	}
	else
	{
		if (mCmdsVisible)
			cout << "   -> " << iCommandPtr->mMenuStr << endl;
	}
}



// *** DEPRECATED & DEBUG FUNCTIONS *************************************/


SBL_NAMESPACE_STOP

// *** END ***************************************************************/

