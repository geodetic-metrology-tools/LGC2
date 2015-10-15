// TCCsFilter.cpp
//
// Concrete Class of the TAngleFilter PABC.
// Handles the IO of an TAngle object angles in a text stream with 
// cc units. 
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
#include "TAngle.h"
#include "TObservationFormat.h"
#include "TPointFormat.h"
#include "TCCsFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TCCsFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TCCsFilter *TCCsFilter::fFilter = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCCsFilter::TCCsFilter()
{}

TCCsFilter::~TCCsFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TCCsFilter *TCCsFilter::instance()
{
	if( fFilter == 0 )
	{
		fFilter = new TCCsFilter;
	}

	return fFilter;
}


void	TCCsFilter::input(TAStreamFormatter& iStream, TAngle& angle)const
{	// function to get a gons angle from a QTextStream object
	// and set the value in a TAngle object
	TReal	ccs;

	iStream.skipWhiteSpace();
	if(iStream.peek()=='\n')
	{//check if there's coordinate to read
		angle.setStatus(TVNumericValue::kNull);
		iStream.setError("No angle value to read");
	}

	else
	{
		iStream >> ccs;
		ccs *= LITERAL(0.0001);
		if(!(iStream.fail()))
		{
			angle.setStatus(TVNumericValue::kKnown);
			angle.setGonsValue(ccs);
		}
		else
		{
			angle.setStatus(TVNumericValue::kNull);
			//make iStream's flags to "goodbit"
			iStream.clear();
			//read the wrong gons value
			while(iStream.peek()!=' ' && iStream.peek()!='\n')
			{
				iStream.readChar();
			}
			iStream.setError("Invalid angle value");
		}
	}

	
	
	return;
}

/////////////////////////////////////////////////////////
// outputs an angle formatted in Gons units to the stream
/////////////////////////////////////////////////////////
void	TCCsFilter::output(TAStreamFormatter& oStream,const TAngle& angle)const
{	// function to put a gons angle to a QTextStream object.
	// Gets the value from a TAngle object

	oStream.setf(ios::fixed,ios::floatfield);
	oStream.width(oStream.getWidthFormat());
	oStream.precision(oStream.getPrecisionFormat());

	// provisoire
	TAngle a(angle);
	if (a.getRadiansValue()<LITERAL(0.0)) { a += (2*TAngle::pi()); }
	oStream<< a.getGonsValue();
	

	return;
}




//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

