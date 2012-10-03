/**************************************************************************
NclReader.cpp - loads data from a stream into an NCL reader

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK)
- <mail://pma@agapow.net>
- <http://www.agapow.net/software/mesa>

About:
- Hides the process of parsing from the user. It is lent a set of blocks
  to look for in the stream and fill up. (Note that it doesn't own these
  blocks.)
  
Changes:
- 00.10.10: recreated

To Do:
- 

**************************************************************************/


// *** INCLUDES

#include "NclReader.h"
#include "NclBlocks.h"
#include <iostream>
#include "nexusdefs.h"
#include "nexustoken.h"
#include "nexus.h"

using std::ifstream;


// *** CONSTANTS & DEFINES


// *** HELPER CLASS ******************************************************/

class MesaToken : public NexusToken
{
public:
	MesaToken (istream& iInput, const progcallback_t& ikCommentCb)
		: NexusToken (iInput), mCommentCb (ikCommentCb)
		{}
		
   void OutputComment (nxsstring& msg)
   {
		mCommentCb (kMsg_Comment, msg.c_str());
   }
   
private:
	progcallback_t	mCommentCb;
};


// *** MAIN BODY *********************************************************/

// *** LIFECYCLE *********************************************************/


// *** ACCESSORS *********************************************************/



// *** SERVICES **********************************************************/

void NclReader::read
(ifstream& iInStream, const progcallback_t& ikProgressCb)
//: Given a filepath, parse all the data within
{
	assert (iInStream.is_open());
	MesaToken token (iInStream, ikProgressCb);
	Execute (token);
}


// *** INTERNALS *********************************************************/

// *** DEPRECATED & TEST FUNCTIONS **************************************/

void NclReader::validate ()
{
//	cout << "*** Testing NclReader class" << endl;
	
//	cout << "*** Finished testing NclReader class" << endl;
}

// *** END ***************************************************************/

