/**************************************************************************
MesaConsoleApp.cpp - a simple console application wrapper for the MESA model

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2000-2012, Health Protection Agency (UK) Silwood
  Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa/>

About:
- This will 

Changes:
- (99.7.10) Created.
- (99.7.10) recrafted so as to inherit from ConsoleMenuApp and thus pass
  off most of the book-keeping.

To Do:
- Exceptions?

**************************************************************************/


// *** INCLUDES

#include "MesaConsoleApp.h"
#include "MesaModel.h"
#include "MesaGlobals.h"
#include "Error.h"
#include "StringUtils.h"
#include "Analysis.h"
#include "SystemAction.h"
#include "CharEvolRule.h"
#include "Action.h"
#include "Macro.h"
#include "ResultsDistiller.h"
#include "MesaCommands.h"
#include "ManipAction.h"
#include "XBounds.h"
#include <fstream>
#include <exception>
#include <sstream>
#include <iterator>
#include "callback.hpp"
#include <cmath>

using sbl::cmdId_t;
using sbl::Split;
using sbl::StripFlankingWhitespace;
using sbl::eraseFlankingSpace;
using sbl::MissingFileError;
using sbl::FileOpenError;
using sbl::CommandMgr;
using sbl::kCmd_Quit;
using sbl::kCase_Upper;
using sbl::kCase_Lower;
using sbl::kCase_Mixed;
using sbl::isMemberOf;
using std::exception;
using std::ofstream;
using std::ifstream;
using std::bad_alloc;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::stringstream;
using std::string;
using std::back_insert_iterator;
using sbl::XBounds;
using sbl::kAnswerBounds_None;


// *** CONSTANTS & DEFINES

const bool kAnswer_Required = true;
const bool kAnswer_NotRequired = false;


// *** IO FUNCTIONS *******************************************************/


// *** INPUT ***



void MesaConsoleApp::askForSchemes
(SchemeArr& iSchemeArr, const char* iPrompt)
//: get user to define an array of schemes
{
	// Postconditions:
	assert (iSchemeArr.size() == 0);

	// Main:
	string 	theMenuTitle (iPrompt);
	
	CommandMgr	theSchemeCmds (theMenuTitle.c_str());	
	theSchemeCmds.AddCommand (kCmd_SchemeList, "l", "List the current schemes");
	theSchemeCmds.AddCommand (kCmd_SchemeDelete, "d", "Delete a scheme");
	theSchemeCmds.AddCommand (kCmd_SchemeNull, "Add null scheme");
//	theSchemeCmds.AddCommand (kCmd_SchemeBinary, "Add discrete binary scheme");
	theSchemeCmds.AddCommand (kCmd_SchemeDiscMarkov, "Add discrete markovian scheme");
	theSchemeCmds.AddCommand (kCmd_SchemeDiscRankedMarkov, "Add discrete ranked markovian scheme");
	theSchemeCmds.AddCommand (kCmd_SchemeContBrownian, "Add continuous brownian scheme");
	theSchemeCmds.AddCommand (kCmd_SchemeContLogNormal, "Add continuous log-normal scheme");
	theSchemeCmds.AddCommand (kCmd_Return, "r", "Finish defining schemes");

	theSchemeCmds.SetCommandActive (true);
	theSchemeCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		long theSize = iSchemeArr.size();
		long theColIndex = -1;
		CharStateSet theStates;
		
		// activate the right commands
		bool theHasDiscChars = (mModel->countDiscTraits() != 0);
//		theSchemeCmds.SetCommandActive (kCmd_SchemeBinary, theHasDiscChars);
		theSchemeCmds.SetCommandActive (kCmd_SchemeDiscMarkov, theHasDiscChars);
		theSchemeCmds.SetCommandActive (kCmd_SchemeDiscRankedMarkov, theHasDiscChars);
		
		bool theHasContChars = (mModel->countContTraits() != 0);
		theSchemeCmds.SetCommandActive (kCmd_SchemeContBrownian, theHasContChars);
		theSchemeCmds.SetCommandActive (kCmd_SchemeContLogNormal, theHasContChars);

		bool theSchemeIsNonEmpty = (iSchemeArr.size() != 0);
		theSchemeCmds.SetCommandActive (kCmd_SchemeList, theSchemeIsNonEmpty);
		theSchemeCmds.SetCommandActive (kCmd_SchemeDelete, theSchemeIsNonEmpty);
		theSchemeCmds.SetCommandActive (kCmd_Return, theSchemeIsNonEmpty);

		// get user command
		theUserCmd = AskUserCommand (&theSchemeCmds);
		
		// process the command
      long theTarget = 0;

		switch (theUserCmd)
		{
			case kCmd_Return:
				// goto end of switch
				break;
			
			case kCmd_SchemeList:
			{
				cout << endl;
				cout << "The schemes currently collected are:" << endl;
				for (long i = 0; i < theSize; i++)
					cout << "   " << i + 1 << ". " << (iSchemeArr[i])->describe() << endl;
				break;
			}
				
			case kCmd_SchemeDelete:
			{
				if (theSize == 1)
					theTarget = 1;
				else
					theTarget = askIntegerWithBounds ("Delete which scheme", 1, theSize);
				assert (1 <= theTarget);
				assert (theTarget <= theSize);
				iSchemeArr.remove (theTarget - 1);
				break;
			}
				
			case kCmd_SchemeNull:
			{
				iSchemeArr.adopt (new NullCeScheme ());
				Report ("Null scheme added");			
				break;
			}
				
			case kCmd_SchemeDiscRankedMarkov:
			{
				theColIndex = askDiscTraitCol ("Choose a discrete trait");
				theStates = askForStates ("Edit states for ranked markovian evolution");
				double theUpFreq = askDouble ("What is the upwards rate", 0.0, 1.0);
				double theDownFreq = askDouble ("What is the downwards rate", 0.0, (1.0 - theUpFreq));
				iSchemeArr.adopt (new RankedMarkovianCeScheme (theColIndex, theStates,
					theUpFreq, theDownFreq));
				break;
			}
				
			case kCmd_SchemeDiscMarkov:
			{
				theColIndex = askDiscTraitCol ("Choose a discrete trait");
				theStates = askForStates ("Edit states for markovian evolution");
				double theFreq = askDouble ("What is the transition rate", 0, kAnswerBounds_None);
				iSchemeArr.adopt (new MarkovianCeScheme (theColIndex, theStates, theFreq));
				break;
			}
			
			/*
			case kCmd_SchemeBinary:
				theColIndex = askDiscTraitCol ("Choose a discrete character");
				theStates.addState ((askString ("What is the first state")).c_str());
				theStates.addState ((askString ("What is the second state")).c_str());
				// TO DO: fix later for goofy input
				double theFirstFreq = askDoubleWithBounds ("What is the forward rate", 0.0, 1.0);
				double theSecondFreq = askDoubleWithBounds ("What is the reverse rate", 0.0, (1.0 - theFirstFreq));
				iSchemeArr.adopt (new BinaryCeScheme (theColIndex, theStates,
					theFirstFreq, theSecondFreq));
				break;
			*/
			
			case kCmd_SchemeContBrownian:
			{
				theColIndex = askContTraitCol ("Choose a continuous trait");
				double theMean = askDouble ("Enter the mean change");
				double theStdDev = std::abs (askDouble ("Enter the std dev of change", 0, kAnswerBounds_None));
				bool thePunctuational = askYesNo ("Use punctuational (all distances equal)");
				contcharrange_t theRange;
				if (askYesNo ("Use an upper bound"))
					theRange.setUpper (askDouble ("Enter the upper bound"));
				if (askYesNo ("Use an lower bound"))
				{
					double theLower;
					do
					{
						theLower = askDouble ("Enter the lower bound");
					}
					while (theRange.hasUpper() and (theRange.getUpper () < theLower));
					theRange.setLower (theLower);
				}
				evolbound_t theBoundBehaviour = kEvolBound_Ignore;
				if (theRange.hasUpper() or theRange.hasLower())
					theBoundBehaviour = (evolbound_t) askChoice
						("Replace or truncate at boundaries", "rt");
				iSchemeArr.adopt (new ContBrownianScheme (long (theColIndex),
					theMean, theStdDev, thePunctuational, theRange, theBoundBehaviour));
				break;
			}

			case kCmd_SchemeContLogNormal:
			{
				theColIndex = askContTraitCol ("Choose a continuous trait");
				double theMean = askDouble ("Enter the mean change");
				double theStdDev = std::abs (askDouble ("Enter the std dev of change", 0, kAnswerBounds_None));
				// bool thePunctuational = askYesNo ("Use punctuational (all distances equal)");
				contcharrange_t theRange;
				if (askYesNo ("Use an upper bound"))
					theRange.setUpper (askDouble ("Enter the upper bound"));
				if (askYesNo ("Use an lower bound"))
				{
					double theLower;
					do
					{
						theLower = askDouble ("Enter the lower bound");
					}
					while (theRange.hasUpper() and (theRange.getUpper () < theLower));
					theRange.setLower (theLower);
				}
				evolbound_t theBoundBehaviour = kEvolBound_Ignore;
				if (theRange.hasUpper() or theRange.hasLower())
					theBoundBehaviour = (evolbound_t) askChoice
						("Replace or truncate at boundaries", "rt");
				iSchemeArr.adopt (new ContLogNormalScheme (long (theColIndex),
					theMean, theStdDev, false, theRange, theBoundBehaviour));
				break;
			}

								
			default:
				assert (false); // shouldn't reach here
		}
	}
	while (theUserCmd != kCmd_Return);

	// Postconditions:
	assert (iSchemeArr.size() != 0);
}


CharStateSet MesaConsoleApp::askForStates (const char* iPrompt)
//: get user to define an array of states
{
	// Main:
	CharStateSet theStates;
	if (mModel)
		theStates = mModel->getDiscStates ();

	string 	theMenuTitle (iPrompt);
	
	CommandMgr	theStateCmds (theMenuTitle.c_str());	
	theStateCmds.AddCommand (kCmd_StateList, "l", "List the current states");
	theStateCmds.AddCommand (kCmd_StateDelete, "d", "Delete a state");
	theStateCmds.AddCommand (kCmd_StateAdd, "a", "Add a new state");
	theStateCmds.AddCommand (kCmd_StateSort, "s", "Sort the states");
	theStateCmds.AddCommand (kCmd_Return, "r", "Finish editing states");

	theStateCmds.SetCommandActive (true);
	theStateCmds.SetConvertShortcut (false);

	// event loop
	cmdId_t	theUserCmd;
	do 
	{
		long theSize = theStates.size();
		string theNewState;
		long theTarget;
		
		// activate the right commands
		if (theSize < 2)
			theStateCmds.SetCommandActive (kCmd_Return, false);
		else
			theStateCmds.SetCommandActive (kCmd_Return, true);

		if (theSize <= 1)
			theStateCmds.SetCommandActive (kCmd_StateSort, false);
		else
			theStateCmds.SetCommandActive (kCmd_StateSort, true);
		 
		if (theSize == 0)
		{
			theStateCmds.SetCommandActive (kCmd_StateList, false);
			theStateCmds.SetCommandActive (kCmd_StateDelete, false);
		}
		else
		{
			theStateCmds.SetCommandActive (kCmd_StateList, true);
			theStateCmds.SetCommandActive (kCmd_StateDelete, true);
		}
		
		// get user command
		theUserCmd = AskUserCommand (&theStateCmds);
		
		// process the command
		switch (theUserCmd)
		{
			case kCmd_Return:
				// goto end of switch
				break;
				
			case kCmd_StateList:
			{
				cout << endl;
				cout << "The states currently collected are:" << endl;
				for (long i = 0; i < theSize; i++)
					cout << "   " << i + 1 << ". " << theStates[i] << endl;
				break;
			}

			case kCmd_StateDelete:
			{
				theTarget = 0;
				if (theSize == 1)
					theTarget = 1;
				else
					theTarget = askIntegerWithBounds ("Delete which state", 1, theSize);
				assert (1 <= theTarget);
				assert (theTarget <= theSize);
				theStates.deleteState (theTarget - 1);
				break;
			}
				
			case kCmd_StateAdd:
			{
				theNewState = askString("Enter a single trait for the new state");
				sbl::eraseFlankingSpace (theNewState);
				if (theNewState.size() != 1)
					ReportError ("States symbols must consist of a single trait");
				else if (theNewState == "")
					ReportError ("States must consist of non-blank trait");
				else if (theStates.isMember (theNewState))
					ReportError ("That state is already defined");
				else
				{
					theStates.addState (theNewState.c_str());
					Report ("A new state has been added");
				}
				break;
			}
				
			case kCmd_StateSort:
			{
				theStates.sort();
				Report ("The states have been sorted");
				break;
			}
								
			default:
				assert (false); // shouldn't reach here
		}
	}
	while (theUserCmd != kCmd_Return);

	// Postconditions:
	assert (2 <= theStates.size());	
	return theStates;		
}


const char* MesaConsoleApp::askAndOpenOutFile
(ofstream& iOutFstrm, const char* iPromptCstr, const char* iDefPath)
//: prompt user for a file name and open that file, return path
// If no name is provided and a default is, use that name.
{
	// build the prompt, using default name if provided
	string thePrompt (iPromptCstr);
	if (iDefPath != "")
	{
		thePrompt.append (" [default \'");
		thePrompt.append (iDefPath);
		thePrompt.append ("\']");
	}

	// ask user for file name
	static string theFileName; // so const char* can be returned with impunity
	theFileName = askString (thePrompt.c_str());
	
	// process name
	eraseFlankingSpace (theFileName);
	if (theFileName == "")
		theFileName = iDefPath;
		
	if (theFileName == "")
	{
		// if no name ask again
		return askAndOpenOutFile (iOutFstrm, iPromptCstr, iDefPath);
	}
	else
	{
		// open stream & return
		iOutFstrm.open (theFileName.c_str());
		return (theFileName.c_str());
	}
}


bool MesaConsoleApp::askAndOpenInFile
(const char* iPromptCstr, ifstream& iInFstream)
//: prompt user for a file name and open that file, return success
{
	// get the path of the file
	string theFilePath = askString (iPromptCstr);
	eraseFlankingSpace (theFilePath);
	// allow user abort
	if (theFilePath == "")
		return false;
		
	// does the file exist
	if (not doesFileExist (theFilePath.c_str()))
		throw sbl::MissingFileError ("couldn't locate the input file",
			theFilePath.c_str());

	// open the filestream
	iInFstream.open (theFilePath.c_str());
	if (iInFstream.is_open())
		return true;
	else
		return false;
}


bool MesaConsoleApp::askIntegerOrNone
(const char* iPromptCstr, int iMin, int iMax, int& iRetValue)
//: ask the user for an integer (within the bounds) or a null answer
// Note that as opposed to the other user question fxns, the result is
// returned in a referenced argument. If the null/default answer is
// chosen, false is returned.
// TO DO: clean up answer, accept 'none', '-', etc.
{
	// Preconditions:
	assert (iMin <= iMax);
	
	// Main:
	// form question and grab answer
	string				theAnswerStr;

	// construct prompt
	stringstream 		thePromptStrm;
	thePromptStrm << iPromptCstr << "? [" << iMin;
	if (iMin != iMax)
		thePromptStrm << "-" << iMax;
	thePromptStrm << " or none]";
	
	// print question
	printQuestion ((thePromptStrm.str()).c_str());
	
	// get & cleanup answer
	getline (cin, theAnswerStr);
	eraseFlankingSpace (theAnswerStr);
	
	// is it a null answer?
	if (theAnswerStr == "")
		return false;
		
	// otherwise it must be an integer
	// so check that it is so
	if (not sbl::isWhole (theAnswerStr))
	{
		ReportError("That's not an integer");
		return (askIntegerOrNone (iPromptCstr, iMin, iMax, iRetValue));	
	}	 	

	// if it is an integer, do the conversion & check it is within bounds
	iRetValue = sbl::toLong (theAnswerStr.c_str());
	
	if ((iMin <= iRetValue) and (iRetValue <= iMax))
	{
		return true;
	}
	else
	{
		ReportError("That's outside the possible range");
		return (askIntegerOrNone (iPromptCstr, iMin, iMax, iRetValue));	
	}
}



int MesaConsoleApp::askSppRichnessCol ()
//: ask the user for which species richness column to use
// GOTCHA: this function automagically translates from the 1-based user
// view of columns to the 0-based model view. 
{
	int	theNumContChars = mModel->countContTraits();
	int	theRichCol;
	// if the user can choose a spp richness column and does, return it
	if ((0 < theNumContChars) and 
		askIntegerOrNone ( "Using spp. richness data from column", 1,
			theNumContChars, theRichCol))
		return theRichCol - 1;
	else
		return kColIndex_None;
}

int MesaConsoleApp::askContCol (bool iAnswer)
//: ask the user to select a column or none
// GOTCHA: this function automagically translates from the 1-based user
// view of columns to the 0-based model view. 
{
	int	theNumContChars = mModel->countContTraits();
	// if the user can choose a spp richness column and does, return it
	if (0 < theNumContChars)
	{
		int	theColIndex;
		if (iAnswer == kAnswer_NotRequired)
		{
			if (not askIntegerOrNone ("Select a continuous data column",
				1, theNumContChars, theColIndex))
				return kColIndex_None;
		}
		else
		{
			theColIndex = askIntegerWithBounds ("Select a continuous data column",
				1, theNumContChars);
		}
		return theColIndex - 1;
	}
	else
		return kColIndex_None;
}


NodeSelector* MesaConsoleApp::askNodeSelector (const char* iPrompt)
//: ask the user to define a test/condition to select nodes
{
	// setup prompt
	string thePrompt (iPrompt);
	if (thePrompt == "")
		thePrompt = "Select nodes for operations";
	thePrompt += " - all, all tips, absolute number of tips, tips by trait "
		"value, alive";
	
	// select which type of selector
	switch (askChoice (thePrompt.c_str(), "atbrl"))
	{
		case 0:
		{
			return new NodeSelectorAll ();
		}
		break;
		
		case 1:
		{
			return new NodeSelectorTips ();
		}
		break;
		
		case 2:
		{
			int theSelectionCount = askIntegerWithMin ("Select how many tips", 1);
			return new NodeSelectorAbsNumTips (theSelectionCount);
		}
		break;
		
		case 3:
		{
			CharComparator theTraitTest = askSppTest ("Select nodes by traits how");
			return new NodeSelectorTipByTrait (theTraitTest);
		}
		break;
		
		case 4:
		{
			return new NodeSelectorAlive ();
		}
		break;
		
		default:
			// this should never happen
			assert (false);
			return NULL;
	}
}


CharComparator MesaConsoleApp::askSppTest (const char* iPrompt)
//: ask the user to define a species test/condition
{
	// setup prompt
	string thePrompt (iPrompt);
	if (thePrompt == "")
		cout << "Define a test for selecting taxa:";
	else
		cout << thePrompt;
	cout << ":" << endl;
	
	// get left hand trait
	traittype_t   theCharType;
	colIndex_t   theCharIndex = -1;
	askSelectTrait (theCharType, theCharIndex);
	
	// get comparator
	cout << "Select a trait that is (less than, less than or equal ...):" << endl;
	comparator_t theComp = (comparator_t) (askIntegerWithBounds ("<, <=, =, !=, >, >=", 1, 6) - 1);
	
	// get right-hand-side
	CharComparator theTest;
	if (theCharType == kTraittype_Continuous)
	{
		conttrait_t theRhsVal = askDouble ("Enter a continuous value trait will be compared to");
		theTest = CharComparator (theCharIndex, theComp, theRhsVal);
	}
	else
	{
		assert (theCharType == kTraittype_Discrete);
		disctrait_t theRhsVal = askString ("Enter a discrete value the trait will be compared to");
		theTest = CharComparator (theCharIndex, theComp, theRhsVal);
	}
	
	// Postconditions & return:
	cout << "Select taxa " << theTest.describe() << endl;
	return theTest;
}
	
	
	
void MesaConsoleApp::askSelectTrait (traittype_t& ioCharType, colIndex_t& ioCharIndex)
{
	// Preconditions:
	long theNumContChars = mModel->countContTraits();
	long theNumDiscChars = mModel->countDiscTraits();
	assert (theNumContChars or theNumDiscChars);

	// Main:
	// select type of character
	if (theNumContChars and (not theNumDiscChars))
	{
		// there is only cont data
		ioCharType = kTraittype_Continuous;
	}
	else if ((not theNumContChars) and theNumDiscChars)
	{
		// there is only disc data
		ioCharType = kTraittype_Discrete;
	}
	else if (theNumContChars and theNumDiscChars)
	{
		// otherwise choose
		char theChoiceLetter = AskMultiChoice ("Select continuous or discrete trait", "cd");
		if (theChoiceLetter == 'd')
		{
			ioCharType = kTraittype_Discrete;
		}
		else if (theChoiceLetter == 'c')
		{
			ioCharType = kTraittype_Continuous;
		}
		else
			assert (false);
	}
	else
	{
		assert (false); // shouldn't get here
	}
	
	// and many columns could if be?
	long theNumChars = -1;
	if (ioCharType == kTraittype_Continuous)
		theNumChars = theNumContChars;
	else
		theNumChars = theNumDiscChars;

	// select the column
	if (theNumChars == 1)
		ioCharIndex = 0;
	else
		ioCharIndex = askIntegerWithBounds ("Select a trait", 1,
			theNumChars) - 1;

	// Postconditions:
	assert (0 <= ioCharIndex);
	assert ((ioCharType == kTraittype_Continuous) or (ioCharType == kTraittype_Discrete));
}



int MesaConsoleApp::askDiscCol (bool iAnswer)
//: ask the user to select a column or none
// GOTCHA: this function automagically translates from the 1-based user
// view of columns to the 0-based model view. 
{
	int	theNumDiscChars = mModel->countDiscTraits();
	// if the user can choose a spp richness column and does, return it
	if (0 < theNumDiscChars)
	{
		int	theColIndex;
		if (iAnswer == kAnswer_NotRequired)
		{
			if (not askIntegerOrNone ( "Select a discrete data column",
				1, theNumDiscChars, theColIndex))
				return kColIndex_None;
		}
		else
		{
			theColIndex = askIntegerWithBounds ( "Select a discrete data column",
				1, theNumDiscChars);
		}
		return theColIndex - 1;
	}
	else
		return kColIndex_None;
}



int MesaConsoleApp::askContTraitCol (const char* iPrompt)
//: ask the user for which continuous character to use
// if there is only one continuous char return that
// GOTCHA: this function automagically translates from the 1-based user
// view of columns to the 0-based model view. 
{
	// Preconditions:
	int theNumContChars = mModel->countContTraits();
	assert (0 < theNumContChars);
	
	// Main:
	if (theNumContChars == 1)
	{
		return 0;
	}
	else
	{
		if (iPrompt == NULL)
			iPrompt = "Using continuous trait";
		int theColIndex =	askIntegerWithBounds (iPrompt, 1, theNumContChars);
		return theColIndex - 1;
	}
}



int MesaConsoleApp::askDiscTraitCol (const char* iPrompt)
//: ask the user for which discrete character to use
// if there is only one discrete char return that
// GOTCHA: this function automagically translates from the 1-based user
// view of columns to the 0-based model view. 
{
	int	theNumDiscChars = mModel->countDiscTraits();
	
	if (theNumDiscChars == 1)
		return 0;
	else
	{
		if (iPrompt == NULL)
			iPrompt = "Using discrete trait";
		int theAnswer =	askIntegerWithBounds (iPrompt, 1, theNumDiscChars);
		return theAnswer - 1;
	}
}



XBasicRate* MesaConsoleApp::askRate_New ()
//: ask the user for a rate
// TO DO: let them choose time etc.
{
	XBasicRate*      theNewRateP = NULL;
	dependentVar_t   theValueType;
	int              theValueQualifier;
	char             theChoice;
	bool             theModelHasContTraits, theModelHasDiscTraits, theModelHasBothTraits,
	                    theModelHasNeitherTrait;
	
	theModelHasContTraits = (0 < mModel->countContTraits());
	theModelHasDiscTraits = (0 < mModel->countDiscTraits());
	theModelHasBothTraits = theModelHasContTraits and theModelHasDiscTraits;
	theModelHasNeitherTrait = (not theModelHasContTraits) and (not theModelHasDiscTraits);

	if (theModelHasNeitherTrait)
		theChoice = 'c';
	else if (theModelHasBothTraits)
		theChoice = askMultiChoice ("Constant, linear, log, tri-param, normal, discrete",
		"clgtnd", 'c');
	else if (theModelHasContTraits)
		theChoice = askMultiChoice ("Constant, linear, log, tri-param, normal", "clgtn", 'c');
	else if (theModelHasDiscTraits)
		theChoice = askMultiChoice ("Constant, discrete", "cd", 'c');
	else
		assert (false); // should never get here
				
	switch (theChoice)
	{
		case 'c':
		{
			sayMsg ("The rate will remain constant.");
			double theRate = askDouble ("Enter rate of event", 0.000001, kAnswerBounds_None);
			theNewRateP = new XFlatRate (theRate);
		}
		break;
		
		case 'l':
		{
			sayMsg ("The rate will vary as slope * value + intercept.");
			double theSlope = askDouble ("Enter the slope");
			double theIntercept = askDouble ("Enter the intercept");
			askDependentVar (theValueType, theValueQualifier);
			theNewRateP = new XLinearDependentRate (theSlope, theIntercept, theValueType,
				theValueQualifier);
		}
		break;
	
		case 'g':
		{
			sayMsg ("The rate will vary as the log (base) of the value.");
			double theLogBase = askIntegerWithMin ("Enter the base of the log", 2);
			askDependentVar (theValueType, theValueQualifier);
			theNewRateP = new XLogDependentRate (theLogBase, theValueType,
				theValueQualifier);
		}
		break;
	
		case 't':
		{
			sayMsg ("The rate will vary as A * value^B + C.");
			double theParamA = askDouble ("What is the 1st parameter (A)");
			double theParamB = askDouble ("What is the 2nd parameter (B)");
			double theParamC = askDouble ("What is the 3rd parameter (C)");
			askDependentVar (theValueType, theValueQualifier);
			theNewRateP = new XTriParamDependentRate (theParamA, theParamB, theParamC,
				theValueType, theValueQualifier);
		}
		break;
	
		case 'n':
		{
			sayMsg ("The rate will vary as a bell curve around a mean.");
			double theMaxRate = askDouble ("What is the rate at the mean");
			double theMinRate = askDouble ("What is the background rate (far from the mean)");
			double theMean = askDouble ("Where (for what input value) is the mean");
			double theInflexion = askDouble ("How far from the mean does the curve fall by 1/2");
			askDependentVar (theValueType, theValueQualifier);
			theNewRateP = new XNormalDependentRate (theMaxRate, theMinRate, theMean, theInflexion,
				theValueType, theValueQualifier);
		}
		break;

		case 'd':
		{
			sayMsg ("The rate will have constant values for different discrete trait states.");
			theValueQualifier = askDiscTraitCol ("Vary rate based on which discrete trait");
			Report ("I will now ask you for a series of associated trait values and rates. "
				"Finally I will ask you for a rate associated with any unspecified trait values.");
			std::string theStateValue;
			double theRateValue;
			DiscTraitRateArr theTraitRatePairs;
			theStateValue = askString ("Name a trait value [or nothing to finish]");
			while (theStateValue != "")
			{
				theRateValue = askDouble ("What is the rate associated with this value", 0.000001,
					kAnswerBounds_None);
				theTraitRatePairs.storePair (theStateValue,theRateValue);
				theStateValue = askString ("Name a trait value [or nothing to finish]");
			}
			theRateValue = askDouble ("What is the rate associated all other values", 0.000001,
					kAnswerBounds_None);	
			theTraitRatePairs.storeDefault (theRateValue);
			theNewRateP = new XDiscreteRate (theTraitRatePairs, theValueQualifier);
		}
		break;
			
		default:
			// should never get here
			DBG_MSG ("The choice was " << theChoice);
			assert (false);	
	}
	
	theNewRateP->finishInit();
	
	// Postconditions & return:
	assert (theNewRateP != NULL);
	return theNewRateP;
}



void MesaConsoleApp::askDependentVar (dependentVar_t& oValueType, int& oValueQualifier)
{
	// NOTE: at the moment we only allow cont char so ...
	// TO DO: let them choose time etc.
	oValueType = kDependentVariable_ContTrait;
	oValueQualifier = askContTraitCol ("Vary rate based on which continuous trait");
}



void MesaConsoleApp::sayMsg (const char* ikMsg)
//: print message to console, ending with a newline
{
	cout << ikMsg << endl;
}



void MesaConsoleApp::askRate (mesatime_t& iFreq)
//: ask the user for a rate
{
	do
	{
		iFreq = askDouble ("Enter rate of event", 0.000001, kAnswerBounds_None);
	}
	while (iFreq <= 0.0);
}



void MesaConsoleApp::askRate
(double& iFreqA, double& iFreqB, double& iFreqC, const char* iPromptCstr)
//: ask the user for frequency parameters in the eqn (ax^b + c)
{
	if (iPromptCstr == NULL)
		cout << "Enter three parameters (A*x^B + C) for determining rate:" << endl;
	else
		cout << iPromptCstr << endl;

	// grab an answer from the user
	iFreqA = askDouble ("What is the 1st parameter (A)");
	iFreqB = askDouble ("What is the 2nd parameter (B)");
	iFreqC = askDouble ("What is the 3rd parameter (C)");

/*
	string theRawAnswer = askString ("With what rate parameters [3 values, 0.0+]");
	eraseFlankingSpace (theRawAnswer);
	stringvec_t theSplitStrings;
	back_insert_iterator<stringvec_t> theOutIter (theSplitStrings);
	sbl::split (theRawAnswer, theOutIter);
	
	// if we can't get three strings from the answer
	if (theSplitStrings.size() != 3)
	{
		ReportError ("There must be three parameters");
		askRate (iFreqA, iFreqB, iFreqC);
		return;
	}
	
	// if we can't convert the answer to a float
	vector <double> theRawNumbers;
	for (long i = 0; i < theSplitStrings.size(); i++)
	{
		if (not (sbl::isFloat (theSplitStrings[i]) or sbl::isWhole (theSplitStrings[i])))
		{
			ReportError ("Must be a floating point number");
			askRate (iFreqA, iFreqB, iFreqC);
			return;
		}
		else
			theRawNumbers.push_back (sbl::toDouble (theSplitStrings[i]));
	}
	
	// stick the converted answer in the parameter
	iFreqA = theRawNumbers[0];
	iFreqB = theRawNumbers[1];
	iFreqC = theRawNumbers[2];
*/
}



// *** OUTPUT ***

// OVERRIDES
// Modifying base function to allow centralised logging

void MesaConsoleApp::Report (const char* iMsg)
{
	ConsoleApp::Report (iMsg);
	printLog (iMsg);
}

void MesaConsoleApp::ReportError (const char* iMsg)
{
	ConsoleApp::ReportError (iMsg);
	printLog (iMsg);
}


// LOCAL IO
// To format, modify and recieve messages from callbacks
// Some of these will evetually go

void MesaConsoleApp::printReport (const char* ikMsg)
//: Outputs messages recieved from callbacks
// Used for progress-report style messages during long procedures.
{
	//string theBuffer(""); // in case we want to reformat the message
	//theBuffer += ikMsg;
	// Report (theBuffer.c_str());
	Report (ikMsg);
}



void MesaConsoleApp::callbackReport (msg_t iMessageType, const char* ikMsg)
{
	switch (iMessageType)
	{
		case kMsg_Progress:
			Report (ikMsg);
			break;
			
		case kMsg_Comment:
			printComment (ikMsg);
			break;
			
		case kMsg_Error:
			ReportError (ikMsg);
			break;
			
		case kMsg_Analysis:
			printAnalysis (ikMsg);
			break;

		default:
			assert (iMessageType == false);
	}
}



void MesaConsoleApp::printComment (const char* ikMsg)
//: Outputs the nexus internal comments recieved while loading the file.
// Breaks the comments up into lines if need be, prefixes them and sends
// them to the screen and log. 
// TO DO: incorporate logging
{
//	Report (ikMsg);
	string theBuffer (ikMsg);
	vector<string>	theSplitStrings;
	Split (theBuffer, theSplitStrings, '\n');							
	for (vector<string>::size_type i = 0; i < theSplitStrings.size(); i++)
	{
		string theCurrLine (theSplitStrings[i]);
		(void) StripFlankingWhitespace	(theCurrLine);							
		theCurrLine.insert (0, ">   ");
		Report (theCurrLine.c_str());
	}
}



void MesaConsoleApp::printAnalysis (const char* ikMsg)
//: receives output from analysis events
{
	// should reach here if anything but file only (print to screen)
	assert (MesaGlobals::mPrefs.mAnalysisOut != kPrefAnalysisOut_AllFile);
	cout << ikMsg << endl;
}

void MesaConsoleApp::printLog (const char* ikMsg)
//: Pipes statements to the log, if it is 
// TO DO: incorporate logging
{
	if (MesaGlobals::mPrefs.mLogging == kPrefLogging_Enabled)
	{
		assert (mLogStreamP->is_open());
		(*mLogStreamP) << ikMsg << endl;
	}
}



// *** END ***************************************************************/

