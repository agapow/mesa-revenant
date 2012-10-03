/**************************************************************************
NclReader.h - loads data from a filestream into an NCL reader

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef NCLREADER_H
#define NCLREADER_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaTypes.h"
#include "MesaGlobals.h"
#include "Error.h"
#include <fstream>
#include <sstream>
#include "nexusdefs.h"
#include "nexus.h"
#include "nxsstring.h"


// *** CONSTANTS & DEFINES


// *** CLASS DECLARATION *************************************************/

class NclReader: public Nexus
{
public:
	// Lifecycle
	NclReader (const progcallback_t& ikProgressCb)
		: mProgressCb (ikProgressCb)
		{}
	~NclReader ()
		{}
				
	// Services	
	void	read	(std::ifstream& iInStream, const progcallback_t&
		ikProgressCb);

	// Inherited
	void ExecuteStarting () {} // override pure abstract
	void ExecuteStopping () {} // override pure abstract

	// Reporting
	// Uses global states to determine behaviour
	
	void OutputComment (nxsstring s)
	// only do this if normal or greater verbosity
	{
		if (kPrefVerbosity_Normal <= MesaGlobals::mPrefs.mVerbosity)
			mProgressCb (kMsg_Progress, s.c_str());
	}

	void EnteringBlock (nxsstring blockName)
	// only do this if normal or greater verbosity
	{
		if (kPrefVerbosity_Normal <= MesaGlobals::mPrefs.mVerbosity)
		{
			std::stringstream theBuffer;
			theBuffer << "Reading \"" << blockName << "\" block ...";
			std::string theTmpStr = theBuffer.str();
			mProgressCb (kMsg_Progress, theTmpStr.c_str());
		}
	}
	
	void ExitingBlock (nxsstring blockName)
	// only do this if loud or greater verbosity
	{
		if (kPrefVerbosity_Loud <= MesaGlobals::mPrefs.mVerbosity)
		{
			std::stringstream theBuffer;
			theBuffer << "Finished with \"" << blockName << "\" block.";
			std::string theTmpStr = theBuffer.str();
			mProgressCb (kMsg_Progress, theTmpStr.c_str());
		}
	}
	
	void SkippingBlock (nxsstring blockName)
	// only do this if normal or greater verbosity
	{
		if (kPrefVerbosity_Normal <= MesaGlobals::mPrefs.mVerbosity)
		{
			std::stringstream theBuffer;
			theBuffer << "Skipping unknown block (" << blockName << ") ...";
			std::string theTmpStr = theBuffer.str();
			mProgressCb (kMsg_Progress, theTmpStr.c_str());
		}
	}

	void SkippingDisabledBlock (nxsstring blockName)
	// only do this if normal or greater verbosity
	{
		if (kPrefVerbosity_Normal <= MesaGlobals::mPrefs.mVerbosity)
		{
			std::stringstream theBuffer;
			theBuffer << "Skipping disabled block (" << blockName << ") ...";
			std::string theTmpStr = theBuffer.str();
			mProgressCb (kMsg_Progress, theTmpStr.c_str());
		}
	}

/*		
	void DebugReportBlock (NexusBlock& nexusBlock)
	{
#ifdef SBL_DBG
      if (!nexusBlock.IsEmpty())
      {
			DBG_MSG ("");
         DBG_MSG ("*** Contents of the " << nexusBlock.GetID() << " block **********");
         nexusBlock.Report(DBG_STREAM);
      }
#endif
	}
*/

	void NexusError (nxsstring& msg, streampos_t pos, long line, long col)
	// CHANGE: (01.2.16) We abort at 
	{
		sbl::ParseError theError (msg.c_str());
		theError.mLineNum = line;
		throw theError;
		
		pos = pos; // just to keep compiler quiet
		col = col; 
		
		/*
		std::stringstream theBuffer;
		theBuffer << "Error found at line " << line << ", column " << col <<
			" (file position " << pos << "):" << msg;
		std::string theTmpStr = theBuffer.str();
		mProgressCb (kMsg_Error, theTmpStr.c_str());
		*/
	}
	
	
	// Depreciated & Debug
	void	validate	();

	// Internals
private:
	progcallback_t	mProgressCb;
};



#endif
// *** END ***************************************************************/



