/**************************************************************************
ConsoleMenuApp - simple menu oriented console application

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- Derive from this to make bog-standard, C++ apps using menus.

**************************************************************************/

#pragma once
#ifndef CONSOLEMENUAPP_H
#define CONSOLEMENUAPP_H


// *** INCLUDES

#include "Sbl.h"
#include "ConsoleApp.h"
#include "CommandMgr.h"

SBL_NAMESPACE_START


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

/**
A console application with simple textual menus.
*/
class ConsoleMenuApp: public ConsoleApp
{
public:
	// Lifecycle
	ConsoleMenuApp		() {};	// use defaults
	~ConsoleMenuApp	() {};
		
	// Services		
	virtual void	Run			();							// main event loop
	
	virtual void	LoadMenu		() = 0;						// _must_ override
	virtual bool	UpdateCmd	(cmdId_t iCmdId) = 0;	// _must_ override
	virtual void	ObeyCmd		(cmdId_t iCmdId) = 0;	// _must_ override
	
	// UI functions
	cmdId_t		AskUserCommand		(CommandMgr *iCommandMgr, const char* iMenuTitle = NULL);
	void			PrintMenuChoice	(cmdInfo_t *iCommandPtr);

protected:
	CommandMgr	mMainCommands;	// main event loop commands
};


SBL_NAMESPACE_STOP
#endif
// *** END ***************************************************************/



