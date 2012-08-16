/**************************************************************************
SystemAction.h - an action that acts on the data system rather than within

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

**************************************************************************/

#pragma once
#ifndef SYSTEMACTION_H
#define SYSTEMACTION_H


// *** INCLUDES

#include "Action.h"
#include "MesaTree.h"
#include "MesaPrefs.h"


// *** CONSTANTS & DEFINES

// *** CLASS DECLARATION *************************************************/

// *** RUN MACRO

class SystemAction: public BasicAction
{
public:
	// LIFECYCLE
	SystemAction ()
		{}
	
	// SERVICES
	void				execute ();
	virtual void	executeSystem () = 0;		
	size_type deepSize () { return 1; }
	void deleteElement (size_type iIndex)
		{ assert (false); iIndex = iIndex; }
	const char* describe (size_type iIndex)
		{ assert (iIndex == 0); return describeSysAction (); }
	virtual const char* describeSysAction () = 0;
};



// *** DUPLICATE TREE SYS ACTION *****************************************/

class DupTreeSysAction: public SystemAction
{
public:
	// LIFECYCLE


	// SERVICES
	void executeSystem ();
		
	// INTERNALS
private:
	const char* describeSysAction ();
};


// *** SAVE SYS ACTION ***************************************************/

enum saveFile_t
{
	kSaveFile_Nexus = 0,
	kSaveFile_Caic,
	
	kSaveFile_NumItems
};

class SaveSysAction: public SystemAction
{
public:
	// LIFECYCLE
	SaveSysAction (const char* iBaseFileCstr, saveFile_t iFileType = kSaveFile_Nexus)
		: mBaseFileName (iBaseFileCstr), mReps (0), mFileType (iFileType)
		{}

	// SERVICES
	void executeSystem ();
		
	// INTERNALS
private:
	std::string   mBaseFileName;
	int           mReps;          // how many times has this been called
	saveFile_t    mFileType;

	const char* describeSysAction ();
};


// *** SAVE SYS ACTION ***************************************************/

class SetLabelsSysAction: public SystemAction
{
public:
	// LIFECYCLE
	SetLabelsSysAction (pref_cladelabels_t iLabelSetting)
		: mLabelType (iLabelSetting)
		{}

	// SERVICES
	void executeSystem ();
		
	// INTERNALS
private:
	const char* describeSysAction ();
	pref_cladelabels_t  mLabelType;
};


// *** PRESERVE TAXA SYS ACTION ******************************************/

class PreserveTaxaSysAction: public SystemAction
{
public:
	// LIFECYCLE
	PreserveTaxaSysAction (pref_preservenodes_t iSetting)
		: mSetting (iSetting)
		{}

	// SERVICES
	void executeSystem ();
		
	// INTERNALS
private:
	const char*   describeSysAction ();
	
	pref_preservenodes_t   mSetting;
};



// *** TREE LENGTH MANIPULATION SYS ACTION *******************************/

enum treelenchange_t
{
	kTreeLenChange_Set = 0,
	kTreeLenChange_Add,
	kTreeLenChange_Multiply,
	kTreeLenChange_RandomFixed,
	kTreeLenChange_RandomFraction,
	
	kTreeLenChange_NumLimits
};

class SetTreeLengthSysAction: public SystemAction
{
public:
	// LIFECYCLE
	SetTreeLengthSysAction (double iNewLength = 0.0, treelenchange_t iChangeType = kTreeLenChange_Set)
		: mNewLength (iNewLength), mChangeType (iChangeType)
		{}

	// SERVICES
	void executeSystem ();
		
	// INTERNALS
private:
	const char* describeSysAction ();
	
	MesaTree::weight_type   mNewLength;
	treelenchange_t         mChangeType;
	
};


// *** TREE LENGTH MANIPULATION SYS ACTION *******************************/

class ConsolidateTaxaSysAction: public SystemAction
{
public:
	// LIFECYCLE
	// none needed

	// SERVICES
	void executeSystem ();
		
	// I/O
	const char* describeSysAction ();
};


// *** SHUFFLE TRAITS SYS ACTION *******************************/

class ShuffleTraitsSysAction: public SystemAction
{
public:
	// LIFECYCLE
	ShuffleTraitsSysAction (traittype_t iTraitType, colIndex_t iTraitIndex)
		: mTraitType (iTraitType), mTraitIndex (iTraitIndex)
		{}

	// SERVICES
	void executeSystem ();
		
	// I/O
	const char* describeSysAction ();

private:
	traittype_t mTraitType;
	colIndex_t mTraitIndex;
};



// *** DELETE DEAD TAXA SYS ACTION **************************************/

class DeleteDeadTaxaSysAction: public SystemAction
{
public:
	// LIFECYCLE
	// none needed

	// SERVICES
	void executeSystem ();
		
	// I/O
	const char* describeSysAction ();
};

class DeleteDeadTraitsSysAction: public SystemAction
{
public:
	// LIFECYCLE
	// none needed

	// SERVICES
	void executeSystem ();
		
	// I/O
	const char* describeSysAction ();
};


// *** COLLAPSE SINGLETONS SYS ACTION ************************************/

class CollapseSingletonsSysAction: public SystemAction
{
public:
	// LIFECYCLE
	// none needed

	// SERVICES
	void executeSystem ();
		
	// I/O
	const char* describeSysAction ();
};




// *** MAKE NEONTOLOGICAL SYS ACTION *************************************/

class MakeNeontSysAction: public SystemAction
{
public:
	// LIFECYCLE
	// none needed

	// SERVICES
	void executeSystem ();
		
	// I/O
	const char* describeSysAction ();
};


#endif
// *** END ***************************************************************/



