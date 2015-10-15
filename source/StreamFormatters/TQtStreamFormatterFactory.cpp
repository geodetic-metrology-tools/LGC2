// TQtStreamFormatterFactory.h: 
//
/** A Factory producing the different TQtStreamFormatter objects.

    Patterns:
	A singleton to ensure only one Factory exists.

 
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations
#include  "TQtFreeFormatTStream.h"
#include  "TAQtStreamFormatter.h"
#include  "TQtStreamFormatterFactory.h"
//
//////////////////////////////////////////////////////////////////////


//ClassImp(TQtStreamFormatterFactory)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

TQtStreamFormatterFactory*  TQtStreamFormatterFactory::fFactory = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TQtStreamFormatterFactory::TQtStreamFormatterFactory()
{
}



TQtStreamFormatterFactory::~TQtStreamFormatterFactory()
{
}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TQtStreamFormatterFactory *TQtStreamFormatterFactory::instance()
{
	if( fFactory == 0 )
	{
		fFactory = new TQtStreamFormatterFactory();
	}

	return fFactory;
}


TAQtStreamFormatter *TQtStreamFormatterFactory::getFormatter( TAQtStreamFormatter::ETextFormat  format, 
															 QTextStream*  stream) 
{	// Function to obtain a pointer instance of the requested
	// text stream formatter
	
	TAQtStreamFormatter *formatter = 0;

	switch( format )
	{
	case TAQtStreamFormatter::kFreeFormat:
		formatter = new TQtFreeFormatTStream( stream );
		break;

	default:
		// undefined angle filter requested
		assert( formatter != 0 );
		break;
	}

	return formatter;
}


string  TQtStreamFormatterFactory::descriptionOf( enum TAQtStreamFormatter::ETextFormat code )  const
{	// Get a description of a given text formatter
	
	string  description;

	switch( code )
	{
	case TAQtStreamFormatter::kFreeFormat:
		description = "Free Format";
		break;

	default:
		break;
	}

	return  description;
}


TAQtStreamFormatter::ETextFormat  TQtStreamFormatterFactory::whichDataSet( const string& description )  const
{	// Get a text formatter enumeration from a given description
	
	TAQtStreamFormatter::ETextFormat  code;

	if( description == "Free Format" )
	{
		code = TAQtStreamFormatter::kFreeFormat;
	}
	else
	{
		code = TAQtStreamFormatter::kNull;
	}

	return code;
}





//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

