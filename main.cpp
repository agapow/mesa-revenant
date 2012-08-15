/**************************************************************************
main.cpp - main body for the MESA application

Credits:
- From SIBIL, the Silwood Biocomputing Library.
- By Paul-Michael Agapow, 2002, Dept. Biology, Imperial College at
  Silwood Park, Ascot, Berks, SL5 7PY, UNITED KINGDOM.
- <mail://mesa@agapow.net> <http://www.agapow.net/software/mesa//>

About:
- This doesn't need to be very long, it just creates the objects the app
  is made of and lets them rip.

Changes:
- 99.7.7: Created.
- 00.9.20: Due to change in application structure, have changed calling
  code below.

To Do:
- is this the best way to catch errors that get this high?
- highest level exceptions?
- a load prefs at any point?

**************************************************************************/


// *** INCLUDES

#include "MesaConsoleApp.h"
#include "Sbl.h"
#include <iostream>

#ifdef MESA_USEWINSIOUX
	#include <WinSioux.h>
#endif
#ifdef MESA_USEMACSIOUX
	#include <sioux.h>
#endif


// *** CONSTANTS & DEFINES


// *** MAIN BODY *********************************************************/

int main()
{	
#ifdef MESA_USEMACSIOUX
	// init SIOUX appearance, for Mac only
	SIOUXSettings.rows = 75;
	SIOUXSettings.columns = 80;
	SIOUXSettings.toppixel = 45;
	SIOUXSettings.leftpixel = 10;
	SIOUXSettings.autocloseonquit = true;
	SIOUXSettings.asktosaveonclose = true;
	SIOUXSetTitle("\pMesa console");	
#endif

	// set appropriate file types
	// _fcreator = 'XCEL';
	// _ftype = 'TEXT';
	
	try
	{
		MesaConsoleApp	theApp;

		theApp.Startup ();
		theApp.Run ();
		theApp.Quit ();
	}
	catch (sbl::Error theError)
	{
		std::cout << "An error (" << theError.what() <<
			") has caused the program to quit!" << std::endl;
		return 1;
	}
	catch (std::exception theException)
	{
		std::cout << "An error (" << theException.what() <<
			") has caused the program to quit!" << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "An unexpected problem has caused the program " <<
			"to quit!" << std::endl;
		return 1;
	}
	return 0;
}

// *** END ***************************************************************/

