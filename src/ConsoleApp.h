/**
@file
@author   Paul-Michael Agapow
          Dept. Biology, University College London, WC1E 6BT, UK.
          <mail://p.agapow@ic.ac.uk> 
          <http://www.agapow.net/code/sibil/>
*/

#pragma once
#ifndef SBL_CONSOLEAPP_H
#define SBL_CONSOLEAPP_H


// *** INCLUDES

#include "Sbl.h"
#include "Error.h"
#include <string>
#include <vector>

SBL_NAMESPACE_START


// *** CONSTANTS & DEFINES

/**
The type of bound allowed for user input.

Although this is an enum, there is currently only one allowable value.
This is because if there is a boundary, the actual boundary value will
be passed instead of this type. Having this type allows us to have
overloaded functions of the type:
@code
	askDouble (iPrompt);
	askDouble (iPrompt, kAnswerBounds_None, iUpperBound);
	askDouble (iPrompt, iLowerBound, kAnswerBounds_None);
	askDouble (iPrompt, iLowerBound, iUpperBound);
@endcode
*/
enum answerbounds_t
{ 
   kAnswerBounds_None
};


// *** DECLARATIONS *

/**
A simple text console based application & UI functions.
   
Derive from this to make bog-standard C++ apps. A simple example of its
use follows. Note that Startup() and Quit() are not overridden and thus
the default behaviour for the 

// A very simple console application demonstrating the I/0 functions
// Could override Startup and Quit but have chosen to accept default
// behaviour for startup box & quit message.
class TestConsoleApp: public ConsoleApp
{
public:
	
	//  _must_ override
	void	Run					()
	{
		Report ("This is a test and demonstration of ConsoleApp");
		Report ("Now I'll ask a question");
		int theAnswer = AskIntWithBoundsQuestion
			("What's a even number between 7 and 11", 7, 11);
		if ((theAnswer != 8) and (theAnswer != 10))
			ReportError ("That's not correct");
	}
};

@note   Where questions or reports are longer than a single line, this
        object is able to wrap them nicely.

@todo   Start eliminating the deprecated function.
@todo   Sort out the jumble of multichoice function.
*/
class ConsoleApp
{
public:
/// @name LIFECYCLE
//@{
	ConsoleApp	();
	~ConsoleApp	();
//@}
		
/// @name SERVICES
//@{
	virtual void	Startup				();		// can override
	virtual void	Run					() = 0;	// _must_ override in derived class
	virtual void	Quit					();		// can override
//@}

/// @name I/O
//@{
	// Aboutbox display
	virtual void	ShowAboutBox		();		// can override
	void				PrintAboutString	(std::string& iContentString);
	void				PrintAboutBorder	();


	void			Report				(const char* iPrompt);
	void			ReportRaw 			(const char* iPrompt);
	void			ReportError 		(const char* iPrompt);
	void			ReportError			(Error& Error);
	void			ReportFatalError 	(const char* iPrompt);
	void			ReportProgress		(const char* iPrompt);
	
	void			askString (const char* iPrompt, char* oAnswer);
	std::string		askString (const char* iPrompt);

	double		askDoubleWithBounds	(const char* iPrompt, double iLower, double iUpper);

	double		askDouble (const char* iPrompt);
	double		askDouble (const char* iPrompt, answerbounds_t iNoLowerBound, double iUpperBound);
	double		askDouble (const char* iPrompt, double iLowerBound, answerbounds_t iNoUpperBound);
	double		askDouble (const char* iPrompt, double iLowerBound, double iUpperBound);

	long		askLong (const char* iPrompt);
	long		askLong (const char* iPrompt, answerbounds_t iNoLowerBound, long iUpperBound);
	long		askLong (const char* iPrompt, long iLowerBound, answerbounds_t iNoUpperBound);
	long		askLong (const char* iPrompt, long iLowerBound, long iUpperBound);
						
	char			askChar 	(const char* iPrompt);
	bool			askYesNo 	(const char* iPrompt);
	bool			askYesNo (const char* iPromptCStr, bool iCurrentState);
	bool			askEitherOr (const char *iPrompt, char iChoice1, char iChoice2);

	int			askChoice (const char *iPrompt, char *iChoiceStr, int iCurrChoice = -1);
//@}

/// @name DEPRECATED & DEBUG
//@{
   
	void			AskStringQuestion (const char* iPrompt, char* oAnswer);
	std::string	AskStringQuestion (const char* iPrompt);
   
	float			AskFloatQuestion	(const char* iPrompt);
	long			askInteger 	(const char* iPrompt);
	long			askIntegerWithMax 		(const char* iPrompt, long iMax);
	long			askIntegerWithMin 		(const char* iPrompt, long iMin);
	long			askIntegerWithBounds	(const char* iPrompt, long iMin, long iMax);
   long			AskIntQuestion 	(const char* iPrompt);
	long			AskIntWithMaxQuestion 		(const char* iPrompt, long iMax);
	long			AskIntWithMinQuestion 		(const char* iPrompt, long iMin);
	long			AskIntWithBoundsQuestion	(const char* iPrompt, long iMin, long iMax);
	char			AskCharQuestion 	(const char* iPrompt);
	bool			AskYesNoQuestion 	(const char* iPrompt);
	bool			AskYesNoQuestion (const char* iPromptCStr, bool iCurrentState);
	bool			AskEitherOrQuestion (const char *iPrompt, char iChoice1, char iChoice2);
	char			AskMultiChoice		(const char *iPrompt, char *iChoiceStr);
	char			AskMultiChoice		(const char *iPrompt, char *iChoiceStr, char iCurrChoice);
	char			askMultiChoice		(const char *iPrompt, char *iChoiceStr);
	char			askMultiChoice		(const char *iPrompt, char *iChoiceStr, char iCurrChoice);
//@}
	
protected:
	// for aboutbox, should define in derived class
	std::string			mAppTitle;
	std::string			mAppVersion;
	std::string			mAppCredits;
	std::vector<std::string>	mAppInfo;

	// can use in derived class for custom about boxes
	void		SetCols					(unsigned int iColsWidth);
	void		SetBox					(unsigned int iBoxWidth);
	void		SetPromptWidth			(unsigned int iPromptWidth);
	void		SetCmdVisibility		(bool iCmdsVisible);
	void		CalcAboutBoxIndent	();

	// screen layour parameters
	bool		mCmdsVisible;

	// UI helper functions
	void			printQuestion		(std::string *iPromptString);
	void			printQuestion		(std::string &iPromptString);
	void			printQuestion		(const char* iPromptCstr);
   
	std::string	getAnswer ();

private:
	// screen layout parameters
	unsigned int	mScreenColsWidth;
	unsigned int	mAboutBoxWidth;
	unsigned int	mAboutBoxIndent;
	unsigned int	mPromptWidth;

	long     askLongHelper (const char* iPrompt, long iLower, long iUpper,
	            bool iLowerSet, bool iUpperSet);
	double   askDoubleHelper (const char* iPrompt, double iLower, double iUpper,
               bool iLowerSet, bool iUpperSet);
};



SBL_NAMESPACE_STOP
#endif
// *** END ***************************************************************/



