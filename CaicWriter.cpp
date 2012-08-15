/**************************************************************************
CaicWriter.cpp - formats & pours data to output stream

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>

To Do:
- richness file
- unknown data
- the branchlength 2 problem
  
**************************************************************************/


// *** INCLUDES

#include "CaicWriter.h"
#include <algorithm>

using std::sort;
using std::endl;


// *** CONSTANTS & DEFINES

// *** CLASS DEFINITION **************************************************/

void CaicWriter::writeData
(ContTraitMatrix& iContWrangler, DiscTraitMatrix& iDiscWrangler)
{
	// Preconditions:
	
	// Main:
	// gather & sort species names
	stringvec_t theSppNames;
	ContTraitMatrix::size_type theNumContRows = iContWrangler.countRows ();
	ContTraitMatrix::size_type theNumDiscRows = iDiscWrangler.countRows ();
	if (theNumContRows < theNumDiscRows)
	{
		for (long i = 0; i < theNumDiscRows; i++)
		{
			theSppNames.push_back (iDiscWrangler.getRowName (i));
		}
	}
	else
	{
		for (long i = 0; i < theNumContRows; i++)
		{
			theSppNames.push_back (iContWrangler.getRowName (i));
		}
	}	
	sort (theSppNames.begin(), theSppNames.end());

	// print out data
	ContTraitMatrix::size_type theNumContCols = iContWrangler.countCols ();
	DiscTraitMatrix::size_type theNumDiscCols = iDiscWrangler.countCols ();
	
	// print out col titles
	(*mDataStreamP) << "Spp names";
	for (long i = 0; i < theNumContCols; i++)
		(*mDataStreamP) << "\t" << iContWrangler.getColName (i);
	for (long i = 0; i < theNumDiscCols; i++)
		(*mDataStreamP) << "\t" << iDiscWrangler.getColName (i);
	(*mDataStreamP)  << endl;
	
	// print out data
	stringvec_t::iterator p;
	for (p = theSppNames.begin(); p != theSppNames.end(); p++)
	{
		// print out spp name
		(*mDataStreamP) << *p;
		
		// print out cont data
		for (long i = 0; i < theNumContCols; i++)
		{
			/*
			std::string theContStr = iContWrangler.getData (p->c_str(), i);
			if (theContStr == "?")
				theContStr = "-9";
			(*mDataStreamP) << "\t" << theContStr;
			*/
			(*mDataStreamP) << "\t" << iContWrangler.getData (p->c_str(), i);;
		}
		// print out disc data
		for (long i = 0; i < theNumDiscCols; i++)
		{
			// TO DO: need to translate disc data. Big prob
			// see if all numerical?
			std::string theDiscStr = iDiscWrangler.getData (p->c_str(), i);
			// if (theDiscStr == "?")
			// 	theDiscStr = "-9";
			(*mDataStreamP) << "\t" << theDiscStr;
		}
		
		(*mDataStreamP) << endl;
	}

}


void CaicWriter::writeTrees (TreeWrangler& iWrangler)
{
// TO DO: make sure minimum length is 2.0

	MesaTree* theTreeP = iWrangler.getActiveTreeP ();
	
	// gather & sort the caic codes
	stringvec_t theCaicCodes;
	for (nodeiter_t q = theTreeP->begin(); q != theTreeP->end(); q++)
		theCaicCodes.push_back (theTreeP->getNodeLabelCaic (q));
	sort (theCaicCodes.begin(), theCaicCodes.end());

	// get data for nodes
	stringvec_t::iterator p;
	for (p = theCaicCodes.begin(); p != theCaicCodes.end(); p++)
	{
		// do branch-length file
		// write code
		(*mBlenStreamP) << p->c_str() << "\t";
		nodeiter_t theCurrNode = theTreeP->getNodeByCaicCode (p->c_str());
		// write distance to parent
		if (theTreeP->isRoot (theCurrNode))
			(*mBlenStreamP) << "0";
		else
			(*mBlenStreamP) << theTreeP->getTimeFromNodeToParent (theCurrNode);
		// write age
		(*mBlenStreamP) << "\t"; 
		// (*mBlenStreamP) << theTreeP->getTimeSinceNodeOrigin (theCurrNode) << endl; 
		(*mBlenStreamP) << theTreeP->getTimeSinceNodeTerminus (theCurrNode) << endl; 
		
		// do phyl file
		if (theTreeP->isLeaf (theCurrNode))
		{
			(*mPhylStreamP) << p->c_str() << endl;
			(*mPhylStreamP) << theTreeP->getLeafName(theCurrNode) << endl;
		}
	}
	
}


// *** END ***************************************************************/
