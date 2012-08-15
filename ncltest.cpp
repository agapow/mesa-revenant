#include <iostream>#include "nexusdefs.h"#include "nexustoken.h"#include "nexus.h"#include "taxablock.h"#include "treesblock.h"#include "discretedatum.h"#include "discretematrix.h"#include "charactersblock.h"#include "allelesblock.h"#include "assumptionsblock.h"#include "datablock.h"#include "distancedatum.h"#include "distancesblock.h"class MyToken : public NexusToken{	ostream& out;	public:   	MyToken( istream& is, ostream& os ) : out(os), NexusToken(is) {}      void OutputComment( nxsstring& msg ) {			cout << msg << endl;			out << msg << endl;      }};class MyNexus : public Nexus{	public:      bool infile_exists;		ifstream inf;		ofstream outf;		MyNexus( char* infname, char* outfname );		~MyNexus();				bool FileExists( char* fn );		void ExecuteStarting() {}		void ExecuteStopping() {}		void OutputComment( nxsstring s ) {			cout << endl;			cout << s << endl;			outf << endl;			outf << s << endl;		}		void EnteringBlock( nxsstring blockName ) {			cout << "Reading \"" << blockName << "\" block..." << endl;			outf << "Reading \"" << blockName << "\" block..." << endl;		}		void ExitingBlock( nxsstring blockName ) {			cout << "Finished with \"" << blockName << "\" block." << endl;			outf << "Finished with \"" << blockName << "\" block." << endl;		}		void SkippingBlock( nxsstring blockName ) {			cout << "Skipping unknown block (" << blockName << ")..." << endl;			outf << "Skipping unknown block (" << blockName << ")..." << endl;		}		void SkippingDisabledBlock( nxsstring blockName )		{			cout << "Skipping disabled block (" << blockName << ")..." << endl;			outf << "Skipping disabled block (" << blockName << ")..." << endl;		}				void DebugReportBlock( NexusBlock& nexusBlock ) {         if( !nexusBlock.IsEmpty() ) {   			outf << endl;            outf << "********** Contents of the " << nexusBlock.GetID() << " block **********" << endl;            nexusBlock.Report(outf);         }		}		void NexusError( nxsstring& msg, ncl_streampos pos, long line, long col ) {			cerr << endl;			cerr << "Error found at line " << line;			cerr << ", column " << col;			cerr << " (file position " << pos << "):" << endl;			cerr << msg << endl;			outf << endl;			outf << "Error found at line " << line;			outf << ", column " << col;			outf << " (file position " << pos << "):" << endl;			outf << msg << endl;			exit(0);		}};MyNexus::MyNexus( char* infname, char* outfname ) 	: infile_exists(true), Nexus() {   if( !FileExists(infname) )      infile_exists = false;         if( infile_exists ) {		inf.open( infname );		outf.open( outfname );   }}MyNexus::~MyNexus() {	inf.close();	outf.close();}bool MyNexus::FileExists( char* fn ){	if( fn == NULL ) 		return false;			bool exists = false;		// I used to use the access function for this, but it doesn't	// seem to exist in the MacIntosh world, so I've reverted to this	// technique, which works for everything	//	FILE* f = fopen( fn, "r" );	if( f != NULL ) {		exists = true;		fclose(f);		f = NULL;	}		return exists;} class MyAllelesBlock : public AllelesBlock{	ostream& outf;public:	MyAllelesBlock( TaxaBlock& tb, AssumptionsBlock& ab, ostream& o )		: outf(o), AllelesBlock( tb, ab ) {}	void SkippingCommand( nxsstring s ) {		cout << "Skipping unknown command (" << s << ")..." << endl;		outf << "Skipping unknown command (" << s << ")..." << endl;	}};class MyCharactersBlock : public CharactersBlock{	ostream& outf;public:	MyCharactersBlock( TaxaBlock& tb, AssumptionsBlock& ab, ostream& o )		: outf(o), CharactersBlock( tb, ab ) {}	void SkippingCommand( nxsstring s ) {		cout << "Skipping unknown command (" << s << ")..." << endl;		outf << "Skipping unknown command (" << s << ")..." << endl;	}};class MyDataBlock : public DataBlock{	ostream& outf;public:	MyDataBlock( TaxaBlock& tb, AssumptionsBlock& ab, ostream& o )		: outf(o), DataBlock( tb, ab ) {}	void SkippingCommand( nxsstring s ) {		cout << "Skipping unknown command (" << s << ")..." << endl;		outf << "Skipping unknown command (" << s << ")..." << endl;	}};int main( int argc, char* argv[] ){	char infname[256];	char outfname[256];	strcpy( infname, "characters.nex" );	strcpy( outfname, "output.txt" );	if( argc > 1 )		strcpy( infname, argv[1] );	if( argc > 2 )		strcpy( outfname, argv[2] );	MyNexus nexus( infname, outfname );   if( !nexus.infile_exists ) {      ofstream outf( outfname );      outf << "Error: specified input file (";      outf << infname;      outf << ") does not exist." << endl;      outf.close();      cerr << "Error: specified input file (";      cerr << infname;      cerr << ") does not exist." << endl;      return 0;   }	TaxaBlock* taxa = new TaxaBlock();	AssumptionsBlock* assumptions = new AssumptionsBlock( *taxa );	TreesBlock* trees = new TreesBlock( *taxa );	MyAllelesBlock* alleles = new MyAllelesBlock( *taxa, *assumptions, nexus.outf );	MyCharactersBlock* characters = new MyCharactersBlock( *taxa, *assumptions, nexus.outf );	MyDataBlock* data = new MyDataBlock( *taxa, *assumptions, nexus.outf );	DistancesBlock* distances = new DistancesBlock( *taxa );	nexus.Add( taxa );	nexus.Add( assumptions );	nexus.Add( trees );	nexus.Add( alleles );	nexus.Add( characters );	nexus.Add( data );	nexus.Add( distances );	MyToken token( nexus.inf, nexus.outf );	nexus.Execute( token );   if( !taxa->IsEmpty() )   	taxa->Report( nexus.outf );   if( !trees->IsEmpty() )   	trees->Report( nexus.outf );   if( !alleles->IsEmpty() )   	alleles->Report( nexus.outf );   if( !characters->IsEmpty() )   	characters->Report( nexus.outf );   if( !data->IsEmpty() )   	data->Report( nexus.outf );   if( !distances->IsEmpty() )   	distances->Report( nexus.outf );   if( !assumptions->IsEmpty() )   	assumptions->Report( nexus.outf );	LabelList z;	nexus.outf << endl;   if( assumptions->GetNumTaxSets() > 0 )   {      assumptions->GetTaxSetNames(z);      LabelList::const_iterator zi;      for( zi = z.begin(); zi != z.end(); zi++ ) {         nexus.outf << "Taxset " << (*zi) << ": ";         IntSet& x = assumptions->GetTaxSet( *zi );         IntSet::const_iterator xi;         for( xi = x.begin(); xi != x.end(); xi++ )            nexus.outf << (*xi + 1) << " ";         nexus.outf << endl;      }   }   if( assumptions->GetNumCharSets() > 0 )   {      assumptions->GetCharSetNames(z);      LabelList::const_iterator zi;      for( zi = z.begin(); zi != z.end(); zi++ ) {         nexus.outf << "Charset " << (*zi) << ": ";         IntSet& x = assumptions->GetCharSet( *zi );         IntSet::const_iterator xi;         for( xi = x.begin(); xi != x.end(); xi++ )            nexus.outf << (*xi + 1) << " ";         nexus.outf << endl;      }   }   if( assumptions->GetNumExSets() > 0 )   {      assumptions->GetExSetNames(z);      LabelList::const_iterator zi;      for( zi = z.begin(); zi != z.end(); zi++ ) {         nexus.outf << "Exset " << (*zi) << ": ";         IntSet& x = assumptions->GetExSet( *zi );         IntSet::const_iterator xi;         for( xi = x.begin(); xi != x.end(); xi++ )            nexus.outf << (*xi + 1) << " ";         nexus.outf << endl;      }   }      cerr << endl;   cerr << "Done." << endl << endl;	return 0;}