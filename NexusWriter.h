/**************************************************************************
NexusWriter.h - formats & pours data to output stream

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- takes Mesa objects and squirts appropriate Nexus description to output
  file. The actual output stream is shielded by a wrapper that does all
  the necessary case conversion.
  
**************************************************************************/

#pragma once
#ifndef NEXUSWRITER_H
#define NEXUSWRITER_H


// *** INCLUDES

#include "Sbl.h"
#include "MesaGlobals.h"
#include "TaxaTraitMatrix.h"
#include "TreeWrangler.h"
#include "ConvertingStreamWrapper.h"
#include <fstream>


// *** CONSTANTS & DEFINES

class CharStateSet;


// *** CLASS DECLARATION *************************************************/

class NexusWriter
{
public:
	// Lifecycle
	NexusWriter (std::ofstream& iOutStream)
		//: ctor accepts open stream to write to as parameter
		: mOutStream (iOutStream, MesaGlobals::mPrefs.mCase),
		mLiteral (false)
		{ writeHeader(); }
		
	~NexusWriter ()
		//: dtor tidies up
		// output should be closed by destruction of streamwrapper
		{}
				
	// Services
	void	writeDiscData (DiscTraitMatrix& iWrangler);
	void	writeContData (ContTraitMatrix& iWrangler);
	void	writeTrees (TreeWrangler& iWrangler, bool iWriteTransCmd);
	void	writeTaxa (stringvec_t& iTaxaNames);

	// Nexus writing commands
	void	writeHeader ();
	void	writeBeginBlock (const char* iBlockString);
	void	writeEndBlock ();	

	void	writeDimensionsCmd (int iNumTaxa, int iNumChar);
	void	writeCharStateLabels (stringvec_t& iLabelList);
	void	writeFormatCmd (const char* iFormatString, CharStateSet* iStatesP = NULL);
	void	writeMatrixCmd (DiscTraitMatrix& iWrangler);
	void	writeMatrixCmd (ContTraitMatrix& iWrangler);
	void	writeTranslationCmd (TranslationTable* iTableP);
	void	writeTreesCmd (TreeWrangler& iWrangler, TranslationTable* iTableP);
	
	// Depreciated & Debug
	void	validate	()
		{ /* nothing as yet */ }

	// Internals
private:
	ConvertingStreamWrapper		mOutStream;
	bool								mLiteral;

/*
	void write (MyTaxaBlock* iTaxaP, MyCharactersBlock* iDiscreteP,
		MyCharactersBlock* mContinuousP, MyTreesBlock* mTreesP);
		
	// Block writing
	void writeTaxa (MyTaxaBlock* iTaxaP);	
	void writeDiscrete (MyCharactersBlock* iDiscreteP);	
	void writeContinuous (MyCharactersBlock* mContinuousP);	
	void writeTrees (MyTreesBlock* mTreesP);
	
	// Command writing
	void writeBeginBlock (NexusBlock* iBlockP);
	void writeDimensionsCmd (TaxaBlock* iTaxaP);
	void writeDimensionsCmd (CharactersBlock* iCharactersP);
	void writeFormatCmd (CharactersBlock* iCharactersP);
	void writeCharLabelsCmd (CharactersBlock* iCharactersP);
	void writeMatrixCmd (CharactersBlock* iCharactersP);
	void writeTaxlabelsCmd (TaxaBlock* iTaxaP);
	void writeTranslationCmd (MyTreesBlock* iTreesP);
	void writeTreesCmd (MyTreesBlock* iTreesP);
*/

	friend NexusWriter& operator << (NexusWriter& iWriter, literal& iFormatCmd);
	
};


#endif
// *** END ***************************************************************/



