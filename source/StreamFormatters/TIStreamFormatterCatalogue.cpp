// TIStreamFormatterCatalogue.h: 
//
// A catalogue for obtaining TQTAStreamFormatter objects.
//
//
// Patterns:
// A singleton to ensure only one catalogue exists.
// 
//
// Copyright 2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations
#include  "TFreeFormatTStream.h"
#include  "TAStreamFormatter.h"
#include  "TIStreamFormatterCatalogue.h"
//
//////////////////////////////////////////////////////////////////////


//ClassImp(TIStreamFormatterCatalogue)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TIStreamFormatterCatalogue::TIStreamFormatterCatalogue()
{
}



TIStreamFormatterCatalogue::~TIStreamFormatterCatalogue()
{
}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TIStreamFormatterCatalogue *TIStreamFormatterCatalogue::instance()
{
	if( fCatalogue == 0 )
	{
		fCatalogue = new TIStreamFormatterCatalogue();
	}

	return fCatalogue;
}


TAStreamFormatter *TIStreamFormatterCatalogue::getFormatter( TAStreamFormatter::ETextFormat  format) 
{	// Function to obtain a pointer instance of the requested
	// text stream formatter
	
	TAStreamFormatter *formatter = 0;

	switch( format )
	{
	case TIStreamFormatterCatalogue::kFreeFormat:
		formatter = new TFreeFormatTStream();
		break;

	default:
		// undefined angle filter requested
		assert( formatter != 0 );
		break;
	}

	return formatter;
}


TAStreamFormatter *TIStreamFormatterCatalogue::getFormatter( TAStreamFormatter::ETextFormat  format, const string& str) 
{	// Function to obtain a pointer instance of the requested
	// text stream formatter
	
	TAStreamFormatter *formatter = 0;

	switch( format )
	{
	case TIStreamFormatterCatalogue::kFreeFormat:
		formatter = new TFreeFormatTStream(str);
		break;

	default:
		// undefined angle filter requested
		assert( formatter != 0 );
		break;
	}

	return formatter;
}


TAStreamFormatter *TIStreamFormatterCatalogue::getFormatter( TAStreamFormatter::ETextFormat  format, TAStreamFormatter::EIOType io, const char* filename) 
{	// Function to obtain a pointer instance of the requested
	// text stream formatter
	
	TAStreamFormatter *formatter = 0;

	switch( format )
	{
	case TIStreamFormatterCatalogue::kFreeFormat:
		formatter = new TFreeFormatTStream(io, filename);
		break;

	default:
		// undefined angle filter requested
		assert( formatter != 0 );
		break;
	}

	return formatter;
}





/*
TAStreamFormatter *TIStreamFormatterCatalogue::getFormatter( TIStreamFormatterCatalogue::ETextFormat  format, QTextStream*  stream) 
{	// Function to obtain a pointer instance of the requested
	// text stream formatter
	
	TAStreamFormatter *formatter = 0;

	switch( format )
	{
	case TIStreamFormatterCatalogue::kFreeFormat:
		formatter = new TFreeFormatTStream( stream );
		break;

	default:
		// undefined angle filter requested
		assert( formatter != 0 );
		break;
	}

	return formatter;
}
*/




//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

