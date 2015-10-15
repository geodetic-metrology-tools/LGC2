// TStreamFormatterFactory.h: 
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
#include  "TFreeFormatTStream.h"
#include  "TSeparatedFormatTStream.h"
#include  "TFixedColFormatTStream.h"
#include  "TAStreamFormatter.h"
#include  "TStreamFormatterFactory.h"
//
//////////////////////////////////////////////////////////////////////


//ClassImp(TStreamFormatterFactory)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

TStreamFormatterFactory*  TStreamFormatterFactory::fFactory = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TStreamFormatterFactory::TStreamFormatterFactory()
{
}



TStreamFormatterFactory::~TStreamFormatterFactory()
{
}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TStreamFormatterFactory *TStreamFormatterFactory::instance()
{
	if( fFactory == 0 )
	{
		fFactory = new TStreamFormatterFactory();
	}

	return fFactory;
}


TAStreamFormatter *TStreamFormatterFactory::getFormatter(TADataSet* ds, TAStreamFormatter::ETextFormat format, string seperator) 
{	// Function to obtain a pointer instance of the requested text stream formatter
	int nameWidth = max(ds->getPointNameWidth(), 10);
	int coordWidth = max(ds->getCoordPrecision()+7, 10);
	int coordWidthRes= max(coordWidth-6, 7);

	int obsPrecision = ds->getLengthPrecision();
	if(obsPrecision < (ds->getAnglePrecision()))
	{
		obsPrecision = ds->getAnglePrecision();
	}
	int obsWidth = max(obsPrecision + 6, 7);
	int obsWidthRes= max(obsWidth-5, 7);

	TPointFormat pointFormat (nameWidth, coordWidth, ds->getCoordPrecision(),coordWidthRes,ds->getCoordPrecision(),0, TSpatialStatus::kCala);
	TObservationFormat obsFormat (nameWidth, obsWidth, ds->getLengthPrecision(), ds->getAnglePrecision(), obsWidthRes, ds->getLengthPrecision(), ds->getAnglePrecision(), 0);

	TAStreamFormatter *formatter = 0;

	switch( format )
	{
	case TAStreamFormatter::kFreeFormat:
		//formatter = new TFreeFormatTStream();
		break;

	case TAStreamFormatter::kSeparatorFormat:
			formatter = new TSeparatedFormatTStream(TAStreamFormatter::kWrite, *ds, pointFormat, obsFormat);
			break;

	case TAStreamFormatter::kColumnFormat:
			formatter = new TFixedColFormatTStream(TAStreamFormatter::kWrite, *ds, pointFormat, obsFormat);
			break;

	default:
		// undefined angle filter requested
		assert( formatter != 0 );
		break;
	}
	formatter->setSeparator(seperator);
	
	if (!ds->isSpaceBetweenData())
		formatter->setNoGapBetweenData();

	return formatter;
}


TAStreamFormatter *TStreamFormatterFactory::getFormatter( TAStreamFormatter::ETextFormat  format, const string& str) 
{	// Function to obtain a pointer instance of the requested text stream formatter
	ignoring(str);
	TAStreamFormatter *formatter = 0;

	switch( format )
	{
	case TAStreamFormatter::kFreeFormat:
		//formatter = new TFreeFormatTStream(str);
		break;

	default:
		// undefined angle filter requested
		assert( formatter != 0 );
		break;
	}

	return formatter;
}

/*////////////////////////////////////
TAStreamFormatter *TStreamFormatterFactory::getFormatter( TAStreamFormatter::ETextFormat  format, TAStreamFormatter::EIOType io, const char* filename) 
{	// Function to obtain a pointer instance of the requested text stream formatter
	
	TAStreamFormatter *formatter = 0;

	switch( format )
	{
	case TAStreamFormatter::kFreeFormat:
		formatter = new TFreeFormatTStream(io, filename);
		break;

	default:
		// undefined angle filter requested
		assert( formatter != 0 );
		break;
	}

	return formatter;
}
//////////////////////////


TAStreamFormatter *TStreamFormatterFactory::getFormatter( TAStreamFormatter::ETextFormat  format, QTextStream*  stream) 
{	// Function to obtain a pointer instance of the requested text stream formatter
	
	TAStreamFormatter *formatter = 0;

	switch( format )
	{
	case TAStreamFormatter::kFreeFormat:
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

string  TStreamFormatterFactory::descriptionOf( enum TAStreamFormatter::ETextFormat code )  const
{	// Get a description of a given text formatter
	
	string  description;

	switch( code )
	{
	case TAStreamFormatter::kFreeFormat:
		description = "Free Format";
		break;

	default:
		break;
	}

	return  description;
}


TAStreamFormatter::ETextFormat  TStreamFormatterFactory::whichDataSet( const string& description )  const
{	// Get a text formatter enumeration from a given description
	
	TAStreamFormatter::ETextFormat  code;

	if( description == "Free Format" )
	{
		code = TAStreamFormatter::kFreeFormat;
	}
	else
	{
		code = TAStreamFormatter::kNull;
	}

	return code;
}





//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

