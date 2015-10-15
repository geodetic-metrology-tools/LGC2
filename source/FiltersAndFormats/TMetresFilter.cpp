// TMetresFilter.cpp
//
// Concrete Class of the TLengthFilter PABC.
// Handles the IO of an TLength object lengths in a text stream with 
// metre units. 
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
#include "TPointFormat.h"
#include "TObservationFormat.h"
#include  "TMetresFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TMetresFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TMetresFilter *TMetresFilter::fFilter = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMetresFilter::TMetresFilter()
{}

TMetresFilter::~TMetresFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TMetresFilter *TMetresFilter::instance()
{	// get the instance of a TMetresFilter
	if( fFilter == 0 )
	{
		fFilter = new TMetresFilter;
	}

	return fFilter;
}


void	TMetresFilter::input(TAStreamFormatter&	iStream, TLength& length)const
{	// function to get a metre length from a QTextStream object
	// and set the value in a TLength object
	TReal	lenVal;
	
	iStream.skipWhiteSpace();
	if(iStream.peek()=='\n')
	{//check if there's length to read
		length.setStatus(TVNumericValue::kNull);
		iStream.setError("No length value to read");
	}

	else
	{
		iStream >> lenVal;
		if(!(iStream.fail()))
		{
			length.setMetresValue(lenVal);
		}
		else
		{
			length.setStatus(TVNumericValue::kNull);
			iStream.clear();
			//read the wrong meters value
			while(iStream.peek()!=' ' && iStream.peek()!='\n')
			{
				iStream.readChar();
			}
			iStream.setError("Invalid length value");
		}
	}	

	return;
}

////////////////////////////////////////
// outputs a formatted TLength in metres
////////////////////////////////////////
void	TMetresFilter::output(TAStreamFormatter& oStream, const TLength& length)const
{	// function to put a metre length to a QTextStream object.
	// Gets the value from a TLength object

	oStream.setf(ios::fixed,ios::floatfield);
	oStream.width(oStream.getWidthFormat());
	oStream.precision(oStream.getPrecisionFormat());
	oStream<<right;
	oStream <<length.getMetresValue();

	return;
}

//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

