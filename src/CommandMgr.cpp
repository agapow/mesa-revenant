/**************************************************************************
CommandMgr.h - a managed collection of commands and associated information

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- This class manages a group of commands (or actually, any set of states
  that can be activated, deactivated, ordered etc.) For a start, it maps
  shortcuts (accelerators) to the correct command, stores the command
  title, and whether it is active (available) or not.
- A number of commands are predefined. By convetion these have negative
  numbers. User defined commands should be strictly positive.

To Do:
- obviously, much more can be stored here, like command descriptions,
  groupings and flags, and functions to do when command is called.

Changes:
- (099.7.14) created.
- (00.9.20) renamed from "group" to "manager".
- (01.2.16) changed to cope with multiletter shortcuts & depreciate the
  single character commands.

**************************************************************************/


// *** INCLUDES

#include "CommandMgr.h"
#include "StringUtils.h"
#include <cctype>

using std::isalpha;
using std::islower;
using std::isupper;
using std::toupper;
using std::tolower;
using std::string;
using std::vector;
using sbl::toString;


SBL_NAMESPACE_START

// *** CONSTANTS & DEFINES


// *** MAIN BODY *********************************************************/

// *** LIFECYCLE *********************************************************/

CommandMgr::CommandMgr ()
{
	init ("");
}

CommandMgr::CommandMgr (const char* iTitleCstr)
{
	init (iTitleCstr);
}

void CommandMgr::init (const char* iTitleCstr)
{
	mTitle = iTitleCstr;
	mDoCaseConversion = true;
	mMaxNumberedCmds = 0;
}

CommandMgr::~CommandMgr ()
{
}

// *** ACCESS ************************************************************/

void CommandMgr::SetConvertShortcut ( bool iConvert )
{
	mDoCaseConversion = iConvert;
}


int CommandMgr::GetNumCommands ()
{
	return countCommands();
}

cmdInfo_t* CommandMgr::GetCommand ( int thePosn )
//: return a pointer to the command at position i
{
	assert ((0 <= thePosn) && ((unsigned int) thePosn < mCommands.size()));
	return (&(mCommands[(unsigned long) thePosn]));
}

int CommandMgr::GetCommandWithShortcut (const char* iShortcutCstr)
//: return the command associated with the shortcut given
// If the command does not exist or is inactive, return 0.
// CHANGE 01.2.16
{
	// Preconditions:
	int theNumCommands = GetNumCommands();
	assert (0 < theNumCommands);

	// Main:
	// if we allow character conversion reduce the string to lower case
	string theSearchStr (iShortcutCstr);
	if (mDoCaseConversion)
	{
		for (string::iterator q = theSearchStr.begin(); q != theSearchStr.end(); q++)
			*q = char (std::tolower(int(*q)));
	}

	// go through the group and look for the shortcut
	for (int i = 0; i < theNumCommands; i++)
	{
		if (mCommands[i].mActive == true)
		{
			string theTestStr = mCommands[i].mShortcut;
			if (mDoCaseConversion)
			{
				for (string::iterator q = theTestStr.begin(); q != theTestStr.end(); q++)
					*q = char (std::tolower(int(*q)));
			}

			if (theTestStr == theSearchStr)
				return mCommands[i].mCmd;
		}
	}

	// if the command was not found
	return kCmd_Null;
}


void CommandMgr::SetCommandActive (cmdId_t iTargetCmd, bool iActivity)
{
	int theNumCommands = GetNumCommands();
	assert (0 < theNumCommands);

	// go through the group and look for the shortcut
	for (int i = 0; i < theNumCommands; i++)
	{
		if (mCommands[i].mCmd == iTargetCmd)
		{
			mCommands[i].mActive = iActivity;
			return;
		}
	}

	// if the command was not found
	DBG_MSG ("Didn't find cmd " << iTargetCmd << " to set it to " <<
		iActivity);
	validate ();
	assert (false);
}

// activates (or inactivates) all commands
void CommandMgr::SetCommandActive ( bool iActivity )
{
	int theNumCommands = GetNumCommands();

	for (int i = 0; i < theNumCommands; i++)
	{
			mCommands[i].mActive = iActivity;
	}
}


// *** MAIN INTERFACE ****************************************************/

void CommandMgr::AddCommand
(cmdId_t iCmdId, const char* iShortcutCstr, const char *iTitleCstr)
// TO DO: should check that cmdNum & shortcut aren't already present
{
	// Preconditions:
	assert (kCmd_Null != iCmdId);

	cmdInfo_t theNewCmd;

	theNewCmd.mCmd = iCmdId;
	theNewCmd.mShortcut = iShortcutCstr;
	theNewCmd.mMenuStr = iTitleCstr;
	theNewCmd.mActive = false;

	mCommands.push_back (theNewCmd);
}

void CommandMgr::AddCommand
(cmdId_t iCmdId, const char *iTitleCstr)
// TO DO: should check that cmdNum & shortcut aren't already present
{
	// Preconditions:
	assert (kCmd_Null != iCmdId);

	cmdInfo_t theNewCmd;

	theNewCmd.mCmd = iCmdId;
	theNewCmd.mShortcut = toString (++mMaxNumberedCmds);
	theNewCmd.mMenuStr = iTitleCstr;
	theNewCmd.mActive = false;

	mCommands.push_back (theNewCmd);
}


// *** INTERNALS *********************************************************/

// *** DEPRECATED & TEST FUNCTIONS **************************************/

void CommandMgr::validate()
{
	DBG_MSG("*** Validating CommandMgr class");


	DBG_MSG("*** Finished validating CommandMgr class");
}


void CommandMgr::AddCommand
(cmdId_t iCmdId, char iShortcut, const char *iTitleCstr)
{
	// Preconditions:
	/*
	assert (kCmd_Null != iCmdId);

	cmdInfo_t theNewCmd;

	theNewCmd.mCmd = iCmdId;
	theNewCmd.mShortcut = iShortcut;
	theNewCmd.mMenuStr = iTitle;
	theNewCmd.mActive = false;

	mCommands.push_back (theNewCmd);
	*/
	string theShortcutBuffer (1, iShortcut);
	AddCommand (iCmdId, theShortcutBuffer.c_str(), iTitleCstr);
}

// return the command associated with the shortcut given. If the command
// does not exist or is inactive, return 0.
int CommandMgr::GetCommandWithShortcut (char iShortcut)
// CHANGE 01.2.16
{
	char*	theShortcutBuffer = "x";
	theShortcutBuffer[0] = iShortcut;
	assert (theShortcutBuffer[1] == '\0');
	return GetCommandWithShortcut (theShortcutBuffer);
}



SBL_NAMESPACE_STOP

// *** END ***************************************************************/

