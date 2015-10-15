// TDMSFilter.cpp
//
// Concrete Class of the TAngleFilter PABC.
// Handles the IO of an TAngle object angles in a text stream with 
// sexagesimal units (DMS: Degrees Minutes Seconds). 
// 
// Patterns:
// This class is a Singleton.
// Works as a Strategy of a TTStreamFormatter object. 
//
// Copyright 2002, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations


#include "TAngle.h"
#include "TObservationFormat.h"
#include "TPointFormat.h"
#include "TDMSFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TDMSFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TDMSFilter *TDMSFilter::fFilter = 0;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TDMSFilter::TDMSFilter()
{}

TDMSFilter::~TDMSFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TDMSFilter *TDMSFilter::instance()
{
	if( fFilter == 0 )
	{
		fFilter = new TDMSFilter();
	}

	return fFilter;
}


void	TDMSFilter::input(TAStreamFormatter& iStream, TAngle& angle)const
{	// function to get a sexagesimal angle from a QTextStream object
	// and set the value in a TAngle object
	int		degs, mins;
	TReal	secs;

	iStream.skipWhiteSpace();
	if(iStream.peek()=='\n')
	{//check if there's value to read
		angle.setStatus(TVNumericValue::kNull);
		iStream.setError("No angle value to read");
	}

	else
	{
		iStream >> degs;
		if(iStream.fail())
		{/*check if there's coordinate to read
			and if degs is valid*/
			angle.setStatus(TVNumericValue::kNull);
			//read the "wrong" degs
			iStream.clear();
			while(iStream.peek()!=' ' && iStream.peek()!='\n')
			{
				iStream.readChar();
			}
			iStream.setError("Invalid degree value");
			
		}
		else
		{
			iStream.skipWhiteSpace();
			if(iStream.peek()=='\n')
			{//check if there's value to read
				angle.setStatus(TVNumericValue::kNull);
				iStream.setError("No minute value to read");
			}
			else
			{
				iStream>> mins;
				if(iStream.fail())
				{/*check if there's coordinate to read
				and if mins is valid*/
					angle.setStatus(TVNumericValue::kNull);
					//read the "wrong" mins
					iStream.clear();
					while(iStream.peek()!=' ' && iStream.peek()!='\n')
					{
						iStream.readChar();
					}
					iStream.setError("Invalid minute value");
				}
			
				else
				{
					iStream.skipWhiteSpace();
					if(iStream.peek()=='\n')
					{//check if there's value to read
						angle.setStatus(TVNumericValue::kNull);
						iStream.setError("No second value to read");
					}
					else
					{
						iStream>> secs;
						if(iStream.fail())
						{/*check if there's coordinate to read
						and if secs is valid*/
							angle.setStatus(TVNumericValue::kNull);
							//read the "wrong" mins
							iStream.clear();
							while(iStream.peek()!=' ' && iStream.peek()!='\n')
							{
								iStream.readChar();
							}
							iStream.setError("Invalid second value");
						}
						else
						{
							angle.setDMSValue(degs, mins, secs);
						}
					}
				}				
			}
		}
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////
// outputs an angle formatted in degrees, minuts, seconds units to the stream
/////////////////////////////////////////////////////////////////////////////
void	TDMSFilter::output(TAStreamFormatter& oStream, const TAngle& angle)const
{	// function to put a sexagesimal angle to a QTextStream object.
	// Gets the value from a TAngle object

	string sep =oStream.getSeparator();

	
	int dWidth, mWidth, sWidth, precision;
	precision = oStream.getPrecisionFormat() - 4; 
	if (precision <=1)
		precision = 0;
	sWidth = precision + 5;
	mWidth = 4;
	dWidth = max(oStream.getWidthFormat()-(sWidth+mWidth), 5);
		
	oStream.width(dWidth);
	oStream.precision(0);
	oStream<<right;
	oStream<<(angle.getDegreesValue());
	
	oStream<<sep;

	oStream.width(mWidth);
	oStream.precision(0);
	oStream<<right;
	oStream<< angle.getMinutesValue();

	oStream<<sep;

	oStream.setf(ios::fixed,ios::floatfield);
	oStream.width(sWidth);
	oStream.precision(precision);
	oStream<<right;
	oStream<< angle.getSecondsValue();
	
	return;

}



//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

