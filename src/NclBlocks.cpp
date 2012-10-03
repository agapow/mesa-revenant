/**************************************************************************
MesaDataBlock.cpp - encapsulating a Nexus DATA block

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>

**************************************************************************/


// *** INCLUDES

#include "NclBlocks.h"
#include "StringUtils.h"
#include "MesaUtils.h"
#include "StringScanner.h"
#include "xnexus.h"
#include "nexusdefs.h"

using std::string;
using std::stringstream;
using std::endl;
using std::cout;


// *** CONSTANTS & DEFINES


// *** UTILITY FUNCTIONS *************************************************/

string	makeUnknownCmdStr (nxsstring  iNexStr)
// TO DO: is "..." necessary?
{
	stringstream theBuffer;
	theBuffer << "Skipping unknown command (" << iNexStr << ") ...";
	return theBuffer.str();
}


// *** MY TAXA BLOCK *****************************************************/


void MyTaxaBlock::SkippingCommand (nxsstring s)
{
	stringstream theBuffer;
	theBuffer << "Skipping unknown command (" << s << ") ..." << endl;
	string theTmpStr = theBuffer.str();
	mProgressCb (kMsg_Progress, theTmpStr.c_str());
}


void MyTaxaBlock::summarise (ostream& ioOutStream)
{
	int theNumTaxa = GetNumTaxonLabels();
	assert (0 <= theNumTaxa);
	ioOutStream << "There " << ((theNumTaxa == 1) ? "is" : "are");
	ioOutStream << " " << GetNumTaxonLabels() <<
		((GetNumTaxonLabels() == 1) ? " taxon" : " taxa") << " listed." <<
		std::endl;
}
	

// *** MY CHARACTERS BLOCK ***********************************************/


void MyCharactersBlock::SkippingCommand (nxsstring s)
{
	string theTmpStr = makeUnknownCmdStr (s);
	mProgressCb (kMsg_Progress, theTmpStr.c_str());
}

void MyCharactersBlock::HandleMatrix (NexusToken& token)
{
	int i, j;

	if (ntax == 0)
	{
		errormsg = "Must precede ";
      errormsg += id;
      errormsg += " block with a TAXA block or specify NEWTAXA and NTAX in the DIMENSIONS command";
		throw XNexus (errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn());
	}

	if (ntaxTotal == 0)
		ntaxTotal = taxa.GetNumTaxonLabels();

	// We use >= rather than just > below because someone might have
	// ELIMINATEd all characters, and we should allow that (even though it
	// is absurd)
	assert (nchar >= 0);

	if (matrix != NULL)
		delete matrix;
	mContData.clear();

	if (GetDataType() != continuous)
	{
		matrix = new DiscreteMatrix (ntax, nchar);
	}
	
	// Allocate memory for (and initialize) the arrays activeTaxon and
	// activeChar.
	// All characters and all taxa are initially active.
	activeTaxon = new bool[ntax];
	for (i = 0; i < ntax; i++)
		activeTaxon[i] = true;
		
	activeChar = new bool[nchar];
	for (j = 0; j < nchar; j++)
		activeChar[j] = true;

   // The value of ncharTotal is normally identical to the value of nchar
   // specified in the CHARACTERS block DIMENSIONS command.  If an
   // ELIMINATE command is processed, however, nchar < ncharTotal.  Note
   // that the ELIMINATE command will have already been read by now, and
   // the ELIMINATEd character numbers will be stored in the IntSet
   // eliminated.
   //
   // Note that if an ELIMINATE command has been read, charPos will have
   // already been created; thus, we only need to allocate and initialize
   // charPos if user  did not specify an ELIMINATE command
	if (charPos == NULL)
		BuildCharPosArray();

	// The value of ntaxTotal equals the number of taxa specified in the
	// TAXA block, whereas ntax equals the number of taxa specified in
	// the DIMENSIONS command of the CHARACTERS block.  These two numbers
   // will be identical unless some taxa were left out of the MATRIX
   // command of the CHARACTERS block, in which case ntax < ntaxTotal.
	if (taxonPos != NULL)
		delete [] taxonPos;
  	taxonPos = new int[ntaxTotal];
   for (i = 0; i < ntaxTotal; i++)
      taxonPos[i] = -1;

	if (GetDataType() == continuous)
		handleContMatrix (token);
   else if (transposing)
      HandleTransposedMatrix (token);
   else
      HandleStdMatrix (token);

   // If we've gotten this far, presumably it is safe to
   // tell the ASSUMPTIONS block that were ready to take on
   // the responsibility of being the current character-containing
   // block (to be consulted if characters are excluded or included
   // or if taxa are deleted or restored)
   assumptionsBlock.SetCallback(this);

   // this should be the terminating semicolon at the end of the matrix command
	if (GetDataType() != continuous)
   	token.GetNextToken();
   if (!token.Equals(";")) {
      errormsg = "Expecting ';' at the end of the MATRIX command; found ";
      errormsg += token.GetToken();
      errormsg += " instead";
		throw XNexus (errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn());
   }

}


void MyCharactersBlock::handleContMatrix (NexusToken& token)
//: read in a matrix of continuous data and store it
{
	std::vector<double>	theRow;
	
	for(;;)
	{
		// grab the first token in the row
		string	theTokenVal;
		token.GetNextToken();
		theTokenVal = (token.GetToken()).c_str();
		
		// if reached the end of the matrix, break loop
		if (token.Equals (";"))
			break;

		// otherwise store the name, read and store the data
		mTaxaNames.push_back (theTokenVal);
		theRow.clear();		
		for (int i = 0; i < nchar; i++)
		{
			token.GetNextToken();
			theTokenVal = (token.GetToken()).c_str();
			theRow.push_back (sbl::toDouble (theTokenVal));
		}
		
		// store the row in the matrix
		mContData.appendRow (theRow);
	}

	assert (mTaxaNames.size() == mContData.size());
	for (sbl::ulong i = 1; i < mContData.size(); i++)
		assert (mContData[1].size() == mContData[i].size());
		
	// store names in continuous data matrix
	for (unsigned int i = 0; i < mTaxaNames.size(); i++)
		mContData.setRowName (i, mTaxaNames[i].c_str());
}


void MyCharactersBlock::assertValidIndex (int iRow, int iCol)
//: just a centralisation of the checks on correct indexing
{
	assert (0 <= iRow);
	assert (iRow <= ntax);
	assert (0 <= iCol);
	assert (iCol <= nchar);
}


// *** MY TREES BLOCK ****************************************************/


void MyTreesBlock::parseTrees ()
{
	for (int i = 0; i < GetNumTrees (); i++)
	{
		try
		{
			// create new tree
			MesaTree theNewTree;
			
			// initialise tree contents
			string	theTreeRep;
         if (translateList.empty())
            theTreeRep = GetTreeDescription (i);
         else
            theTreeRep = GetTranslatedTreeDescription (i);
			sbl::eraseAllSpace (theTreeRep);
			sbl::StringScanner	theScanner (theTreeRep);
			readNewickTree (&theScanner, theNewTree);
			
			// initialise the tree name
			string theTreeName = (GetTreeName (i)).c_str();
			theTreeName = nexifyString (theTreeName.c_str());
			theNewTree.setTreeName (theTreeName.c_str());
		
			// store it in container
			mTrees.push_back (theNewTree);
		}
		catch (...)
		{
			// any other errors, we run away screaming
			// DBG_MSG ("unknown tree error");
			throw;
		}
	}
}


void MyTreesBlock::readNewickTree (sbl::BasicScanner* iSrcScannerP, MesaTree& oTree)
//: read in and build the supplied tree from the scanner
// !! Note: this does not allow for single node trees.
// !! Changes: (00.3.13) allows for space before commas that seperate
// nodes. Like "A:0.234  ,  B:0.123"
// !! Changes: 00.3.17, introduced try-catch to allows cleaning up if
// there is failure part-way through building a tree.
{
	try
	{
		char	theNextChar;
		
		// 1. find start of tree
		if ((iSrcScannerP->ReadCharSkipSpace (theNextChar) == false) or
			(theNextChar != '('))
			throw sbl::ExpectedError (",");	

		// 2. start by placing root node in empty tree
		oTree.clear();
		MesaTree::id_type theRootId = (oTree.insertRoot ())->first;
		
		// 3. read in and link subnodes
		
		/*
		Here's where we allow singular nodes
		// read first one & link to root node
		readNewickNode (iSrcScannerP, oTree, theRootId);

		// read the following nodes - needs to be at least one more!
		if ((iSrcScannerP->ReadCharSkipSpace (theNextChar) == false) or
			(theNextChar != ','))
			throw sbl::ExpectedError ("',' [tree sub-node seperator]");	

		do
		{
			readNewickNode (iSrcScannerP, oTree, theRootId);
		
			// what's next?
			if (iSrcScannerP->ReadCharSkipSpace (theNextChar) == false)
				throw sbl::EndOfFileError ();	
		}
		while (theNextChar == ',');
		*/

		do
		{
			readNewickNode (iSrcScannerP, oTree, theRootId);
		
			// what's next?
			if (iSrcScannerP->ReadCharSkipSpace (theNextChar) == false)
				throw sbl::EndOfFileError ();	
		}
		while (theNextChar == ',');

		
		// end by finding top/root node closure
		if (theNextChar != ')')
			throw sbl::ExpectedError ("')' [tree end]");
			
		// is there a distance on the root node?
		if (*iSrcScannerP)
		{
			// if there's something left in the string
			double theDist = readNewickNodeDist (iSrcScannerP);
			oTree.setEdgeWeight (oTree.getRoot(), theDist);
		}
	}
	catch (...)
	{
		oTree.clear();
		throw;
	}
}


void MyTreesBlock::readNewickNode
(sbl::BasicScanner* iSrcScanner, MesaTree& oTree, MesaTree::id_type iParId)
// Reads a node from the scanner, places it in tree supplied and return
// the distance and new node ID. Responsibility for placing the connecting
// branch is thus devolved to the parent node, as is responsibility for
// catching errors. This function allows for space between the punctuation
// of the tree.
// !! Note: if there is no supplied distance, the constant for unknown
// distance is returned.
{
	// 1. move to start of tree and check
	char		theNextChar;
	if (iSrcScanner->PeekChar (theNextChar) == false)
		throw sbl::EndOfFileError ();		

	// 2. get code for this new node
	MesaTree::id_type theNewId = (oTree.insertChild (oTree.getIter (iParId)))->first;

	// 3. are we dealing with a tip or internal node?	
	if (theNextChar == '(')
	{
		// if it's an internal / complex node, read multiple subnodes
		// remember there must at least be two
		
		// consume opening bracket & do the first one
		iSrcScanner->ReadChar (theNextChar);
		
		/*
		so as to allow singular nodes
		readNewickNode (iSrcScanner, oTree, theNewId);

		// read the following - needs to be at least one more!
		if ((iSrcScanner->ReadCharSkipSpace (theNextChar) == false) or
			(theNextChar != ','))
			throw sbl::ExpectedError ("',' [tree sub-node seperator]");	
		*/
		do
		{
			// link in following nodes
			readNewickNode (iSrcScanner, oTree, theNewId);
		
			// what's next?
			if (iSrcScanner->ReadCharSkipSpace (theNextChar) == false)
				throw sbl::EndOfFileError ();	
		}
		while (theNextChar == ',');
		
		if (theNextChar != ')')
      {
          sbl::StringScanner* scnnr = (sbl::StringScanner*) iSrcScanner;
          string* buff = scnnr->mSrcStringP;
          std::string buffer = *buff;
          cout << "||" << theNextChar << "//" << scnnr->GetPosn() << endl;
          cout << buffer << endl;

			throw sbl::ExpectedError ("')' [tree node end]");
      }
	}
	else if ((std::isalnum(theNextChar)) or (theNextChar == '_'))
	{
		// it's a singular node (a tip), get the name
		string	theTipName;
		iSrcScanner->ReadToken (theTipName, ":,)");
		oTree.setNodeName (theNewId, theTipName);
	}
	else if (theNextChar == '\'')
	{
		// it's a quoted name, get it all
		iSrcScanner->ReadChar (theNextChar);
		string	theTipName;
		iSrcScanner->ReadUntil (theTipName, "\'", true);
		oTree.setNodeName (theNewId, theTipName);
	}
	else
	{
		// otherwise there's a problem
      sbl::StringScanner* scnnr = (sbl::StringScanner*) iSrcScanner;
      string* buff = scnnr->mSrcStringP;
      cout << "boo" << theNextChar << "bar" << scnnr->GetPosn() << endl;
      cout << *buff << endl;
		throw sbl::ExpectedError ("tree node");
	}
	
	
	// read distance, if there is any
	double theDist = readNewickNodeDist (iSrcScanner);
	oTree.setEdgeWeight (iParId, theNewId, theDist);
}


double MyTreesBlock::readNewickNodeDist (sbl::BasicScanner* iSrcScanner)
//: read a distance from a newick-format tree
// Given that it is at the appropriate point in a tree (just after a
// node) will look for and read the distance. If no distance is found,
// the const for UnknownDistance will be returned.
{
	char theNextChar;
	
	iSrcScanner->ConsumeSpace ();
	iSrcScanner->PeekChar (theNextChar);	
	if (theNextChar == ':')
	{
		iSrcScanner->ReadChar (theNextChar);
		string theDistance;
		iSrcScanner->ReadToken (theDistance, ",)");
		return sbl::String2Dbl (theDistance);
	}
	else
	{
		return 0.0;
	}
}



// *** CONTINUOUS BLOCK **************************************************/


void MyContBlock::summarise (ostream& ioOutStream)
{
	assert (GetDataType() == continuous);
	ioOutStream << "There " << ((nchar == 1) ? "is" : "are") << nchar <<
		" continuous trait" << ((nchar == 1) ? "" : "s") <<
		" listed for " << ntax << ((ntax == 1)? "taxon" : " taxa.") <<
		std::endl;
}


// *** DATA BLOCK ********************************************************/


void MyDataBlock::summarise (ostream& ioOutStream)
{
	assert (GetDataType() == standard);
	ioOutStream << "There " << ((nchar == 1) ? "is" : "are") << nchar <<
		" discrete trait" << ((nchar == 1) ? "" : "s") <<
		" listed for " << ntax << ((ntax == 1)? "taxon" : " taxa.") <<
		std::endl;
}

void MyDataBlock::Reset()
{
	MyCharactersBlock::Reset();
	newtaxa = true;
   taxa.Reset();
}	


// *** TREES BLOCK *******************************************************/


void MyTreesBlock::Read (NexusToken& token)
//: overriding the read so as to parse the trees into a useful format
{
	// call the inherited reading behaviour
	TreesBlock::Read (token);
	parseTrees ();
}

void MyTreesBlock::SkippingCommand (nxsstring s)
{
	string theTmpStr = makeUnknownCmdStr (s);
	mProgressCb (kMsg_Progress, theTmpStr.c_str());
}


// *** DEPRECATED & DEBUG ***********************************************/


/*
void MyCharactersBlock::mergeData (stringmatrix_t& iNewMatrix)
//: merge a table of imported data into the data
{
	// Preconditions:
	assert (GetNTax() == iNewMatrix.size());
	
	// Main:
	if (GetDataType() == standard)
		mergeDiscreteData (iNewMatrix);
	else if (GetDataType() == continuous)
		mergeContData (iNewMatrix);
	else
		assert (false);
		
}


void MyCharactersBlock::mergeDiscreteData (stringmatrix_t& iNewMatrix)
//: merge a table of imported data into the discrete data
{
	// Preconditions:
	assert (GetDataType() == standard);
	assert (matrix != NULL);
	
	// Main:
	// 1. resize data block to accomodate new characters
	int theOldSize = GetNChar();
	int theNumNewCols = iNewMatrix[0].size();
	int theNewSize = theOldSize + theNumNewCols;
	nchar = theNewSize;
	matrix->addCols (iNewMatrix[0].size());
	
	// 2. one-by-one add the new characters in
	for (int i = 0; i < ntax; i++)
	{
		for (int j = 0; j < theNumNewCols; j++)
		{
			//setDiscreteState (i, theOldSize + j, iNewMatrix[i][j]);
		}
	}
	
	// 3. add new character names
	for (int k = theOldSize; k < theNewSize; k++)
	{
		string theNewName = mCharNameGen.getNextName();
	}
	
	// Postconditions:
	

}




void MyCharactersBlock::mergeContData (stringmatrix_t& iNewMatrix)
//: merge a table of imported data into the continuous data
{
	// Preconditions:
	assert (GetDataType() == continuous);
	
	// Main:

}


int MyCharactersBlock::getDiscreteState (int iRow, int iCol)
{
	// Preconditions:
	assert (GetDataType() == standard);
	assertValidIndex (iRow, iCol);
	assert (matrix != NULL);
	assert (iRow < matrix->countRows ());
	assert (iCol < matrix->countCols ());
	
	// Main:
	return GetState (iRow,iCol);
}


double MyCharactersBlock::getContState (int iRow, int iCol)
{
	// Preconditions:
	assert (GetDataType() == continuous);
	assertValidIndex (iRow, iCol);
	
	// Main:
	return mContData[iRow][iCol];
}


void MyCharactersBlock::setDiscreteState (int iRow, int iCol, int iNewState)
{
	// Preconditions:
	assert (GetDataType() == standard);
	assertValidIndex (iRow, iCol);
	assert (matrix != NULL);
	assert (iRow < matrix->countRows ());
	assert (iCol < matrix->countCols ());
		
	// Main:
	matrix->SetState (iRow, iCol, iNewState);

}


void MyCharactersBlock::setContState (int iRow, int iCol, double iNewState)
{
	// Preconditions:
	assert (GetDataType() == continuous);
	assertValidIndex (iRow, iCol);
	
	// Main:
	mContData[iRow][iCol] = iNewState;
}

*/


// *** END ***************************************************************/



