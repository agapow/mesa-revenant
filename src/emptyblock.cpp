// This is a template for writing block classes.  Look at the top
// of the header file "emptyblock.h" for detailed steps to follow.

#include <iostream>
#include <stdio.h>

#include "nexusdefs.h"
#include "xnexus.h"
#include "nexustoken.h"
#include "nexus.h"
#include "emptyblock.h"

using std::endl;

/**
 * @class      EmptyBlock
 * @file       emptyblock.h
 * @file       emptyblock.cpp
 * @author     Paul O. Lewis
 * @copyright  Copyright � 1999. All Rights Reserved.
 * @see        Nexus
 * @see        NexusBlock
 * @see        NexusReader
 * @see        NexusToken
 * @see        XNexus
 *
 * This class handles reading and storage for the Nexus block EMPTY.
 * It overrides the member functions Read and Reset, which are abstract
 * virtual functions in the base class NexusBlock.
 *
 * <P> Below is a table showing the correspondence between the elements of
 * an EMPTY block and the variables and member functions that can be used
 * to access each piece of information stored.
 *
 * <p><table border=1>
 * <tr>
 *   <th> Nexus command
 *   <th> Nexus subcommand
 *   <th> Data Members
 *   <th> Member Functions
 * <tr>
 *   <td> DIMENSIONS
 *   <td> NTAX
 *   <td> int <a href="#ntax">ntax</a>
 *   <td> int <a href="#GetNtax">GetNtax()</a>
 * </table>
 */

/**
 * @constructor
 *
 * Performs the following initializations:
 * <table>
 * <tr><th align="left">Variable <th> <th align="left"> Initial Value
 * <tr><td> id             <td>= <td> "EMPTY"
 * </table>
 */
EmptyBlock::EmptyBlock()
{
	id = "EMPTY";
}

/**
 * @destructor
 *
 * Nothing needs to be done.
 */
EmptyBlock::~EmptyBlock()
{
}

/**
 * @method CharLabelToNumber [int:protected]
 * @param s [nxsstring] the character label to be translated to character number
 *
 * The code here is identical to the base class version (simply returns 0),
 * so the code here should either be modified or this derived version
 * eliminated altogether.  Under what circumstances would you need to
 * modify the default code, you ask?  This function should be modified
 * to something meaningful if this derived class needs to construct and
 * run a SetReader object to read a set involving characters.  The SetReader
 * object may need to use this function to look up a character label
 * encountered in the set.  A class that overrides this method should
 * return the character index in the range [1..nchar]; i.e., add one to the
 * 0-offset index.
 */
int EmptyBlock::CharLabelToNumber( nxsstring s )
{
	s = s; // PMA 
   return 0;
}

/**
 * @method HandleEndblock [void:protected]
 * @param token [NexusToken&] the token used to read from in
 * @throws XNexus
 *
 * Called when the END or ENDBLOCK command needs to be parsed
 * from within the EMPTY block.  Basically just checks to make
 * sure the next token in  the data file is a semicolon.
 */
void EmptyBlock::HandleEndblock( NexusToken& token )
{
	// get the semicolon following END or ENDBLOCK token
   //
	token.GetNextToken();

	if( !token.Equals(";") ) {
		errormsg = "Expecting ';' to terminate the END or ENDBLOCK command, but found ";
      errormsg += token.GetToken();
      errormsg += " instead";
		throw XNexus( errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn() );
	}
}

/**
 * @method Read [void:protected]
 * @param token [NexusToken&] the token used to read from in
 * @throws XNexus
 *
 * This function provides the ability to read everything following
 * the block name (which is read by the Nexus object) to the end or
 * endblock statement. Characters are read from the input stream
 * in. Overrides the pure virtual function in the base class.
 */
void EmptyBlock::Read( NexusToken& token )
{
   isEmpty = false;

   // this should be the semicolon after the block name
   //
	token.GetNextToken();
	if( !token.Equals(";") ) {
		errormsg = "Expecting ';' after ";
      errormsg += id;
      errormsg += " block name, but found ";
      errormsg += token.GetToken();
      errormsg += " instead";
		throw XNexus( errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn() );
	}

	for(;;)
	{
		token.GetNextToken();

		if( token.Equals("END") ) {
			HandleEndblock( token );
			break;
		}
		else if( token.Equals("ENDBLOCK") ) {
			HandleEndblock( token );
			break;
		}
		else
      {
			SkippingCommand( token.GetToken() );
         do {
            token.GetNextToken();
         } while( !token.AtEOF() && !token.Equals(";") );

         if( token.AtEOF() ) {
				errormsg = "Unexpected end of file encountered";
				throw XNexus( errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn() );
         }
		}
   }
}

/**
 * @method Reset [void:protected]
 *
 * Sets ntax and nchar to 0 in preparation for reading a new EMPTY block.
 * Overrides the pure virtual function in the base class.
 */
void EmptyBlock::Reset()
{
   isEmpty = true;
}

/**
 * @method Report [void:public]
 * @param out [ostream&] the output stream to which to write the report
 *
 * This function outputs a brief report of the contents of this EMPTY block.
 * Overrides the pure virtual function in the base class.
 */
void EmptyBlock::Report( ostream& out )
{
	out << endl;
	out << id << " block contains...";
}

/**
 * @method SkippingCommand [virtual void:public]
 * @param commandName [nxsstring] the name of the command being skipped
 *
 * This function is called when an unknown command named commandName is
 * about to be skipped.  This version of the function (which is identical
 * to the base class version) does nothing (i.e., no warning is issued
 * that a command was unrecognized).  Modify this virtual function to
 * provide such warnings to the user (or eliminate it altogether since
 * the base class version already does what this does).
 */
/*
void EmptyBlock::SkippingCommand( nxsstring )
{
}
*/

/**
 * @method TaxonLabelToNumber [int:protected]
 * @param s [nxsstring] the taxon label to be translated to a taxon number
 *
 * The code here is identical to the base class version (simply returns 0),
 * so the code here should either be modified or this derived version
 * eliminated altogether.  Under what circumstances would you need to
 * modify the default code, you ask?  This function should be modified
 * to something meaningful if this derived class needs to construct and
 * run a SetReader object to read a set involving taxa.  The SetReader
 * object may need to use this function to look up a taxon label
 * encountered in the set.  A class that overrides this method should
 * return the taxon index in the range [1..ntax]; i.e., add one to the
 * 0-offset index.
 */
/*
int EmptyBlock::TaxonLabelToNumber( nxsstring s )
{
   return 0;
}
 */


