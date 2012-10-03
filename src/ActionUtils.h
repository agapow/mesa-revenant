/**************************************************************************
ActionUtils.h - utility functions for the simulation / action classes

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef ACTIONUTILS_H
#define ACTIONUTILS_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include "MesaTree.h"


// *** CONSTANTS & DEFINES

//class BasicAction;
//class BasicMacro;


// *** FUNCTIONS *********************************************************/

// BasicMacro* castAsMacro (BasicAction* iActionP);

conttrait_t	  getContData (const char* iTaxaName, int iColIndex);
conttrait_t   getContData (nodeiter_t iNodeIter, int iColIndex);
disctrait_t   getDiscData (const char* iTaxaName, int iColIndex);
disctrait_t	  getDiscData (nodeiter_t iNodeIter, int iColIndex);

disctrait_t&  referDiscState (const char* iTaxaName, int iColIndex);
disctrait_t&  referDiscState (nodeiter_t& iNode, int iColIndex);
conttrait_t&  referContState (const char* iTaxaName, int iColIndex);
conttrait_t&  referContState (nodeiter_t& iNode, int iColIndex);

void setContData (nodeiter_t iNodeIter, int iColIndex, conttrait_t& iNewVal);
void setDiscData (nodeiter_t iNodeIter, int iColIndex, disctrait_t& iNewVal);

int			getRichnessData (int iLeafId, int iRichCol);
int			getRichnessData (nodeiter_t iLeafIter, int iRichCol);

int			countLeaves (nodeiter_t iNodeIter, int iRichCol);

void			pushReportPrefix (const char* iPrefCstr);
void			popReportPrefix ();

const char* getNextFakeName ();

MesaTree*	getActiveTreeP ();
void			speciate (nodeiter_t iLeafIter);

std::string getNodeLabel (nodeiter_t iNodeIter);



#endif
// *** END ***************************************************************/



