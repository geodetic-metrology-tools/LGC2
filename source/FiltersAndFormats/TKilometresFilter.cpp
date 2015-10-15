// TKilometresFilter.cpp
//
// Concrete Class of the TLengthFilter PABC.
// Handles the IO of an TLength object lengths in a text stream with 
// kilometre units. 
// 
// Patterns:
// This class is a Singleton.
// Works as a Strategy of a TTStreamFormatter object. 
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations
#include  "TLength.h"
#include "TObservationFormat.h"
#include "TPointFormat.h"
#include  "TKilometresFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TKilometresFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TKilometresFilter *TKilometresFilter::fFilter = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TKilometresFilter::TKilometresFilter()
{}

TKilometresFilter::~TKilometresFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TKilometresFilter *TKilometresFilter::instance()
{	// get the instance of a TMetresFilter
	if( fFilter == 0 )
	{
		fFilter = new TKilometresFilter;
	}

	return fFilter;
}


void	TKilometresFilter::input(TAStreamFormatter&	iStream, TLength& length)const
{	// function to get a kilometre length from a QTextStream object
	// and set the value in a TLength object
	TReal	lenVal;

	iStream >> lenVal;
	if(!(iStream.fail()))
	{
		length.setKMetresValue(lenVal);
	}
	else
	{
		length.setStatus(TVNumericValue::kNull);
		iStream.clear();
			
		while(iStream.peek()!=' ' && iStream.peek()!='\n')
		{
			iStream.readChar();
		}
	}
	

	return;
}

////////////////////////////////////////////
// outputs a formatted TLength in kilometres
////////////////////////////////////////////
void	TKilometresFilter::output(TAStreamFormatter&	oStream,const TLength&	length)const
{	// function to put a metre length to a QTextStream object.
	// Gets the value from a TLength object

	oStream.setf(ios::fixed,ios::floatfield);
	oStream.width(oStream.getWidthFormat()+3);
	oStream.precision(oStream.getPrecisionFormat()+3);
	oStream<<right;
	oStream <<length.getKMetresValue();
	
	return;

}

//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

