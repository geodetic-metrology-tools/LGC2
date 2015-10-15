// T100MicroGonsFilter.cpp
//
// Concrete Class of the TAngleFilter PABC.
// Handles the IO of an TAngle object angles in a text stream with 
// gons units. 
// 
// Patterns:
// This class is a Singleton.
// Works as a Strategy of a TTStreamFormatter object. 
//
// Copyright 2003, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Other forward declarations
#include "TAngle.h"
#include "TObservationFormat.h"
#include "TPointFormat.h"
#include "T100MicroGonsFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(T100MicroGonsFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
T100MicroGonsFilter *T100MicroGonsFilter::fFilter = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

T100MicroGonsFilter::T100MicroGonsFilter()
{}

T100MicroGonsFilter::~T100MicroGonsFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

T100MicroGonsFilter *T100MicroGonsFilter::instance()
{
	if( fFilter == 0 )
	{
		fFilter = new T100MicroGonsFilter;
	}

	return fFilter;
}


void	T100MicroGonsFilter::input(TAStreamFormatter& iStream, TAngle& angle)const
{	// function to get a gons angle from a QTextStream object
	// and set the value in a TAngle object
	TReal	gons;

	iStream.skipWhiteSpace();
	if(iStream.peek()=='\n')
	{//check if there's coordinate to read
		angle.setStatus(TVNumericValue::kNull);
		iStream.setError("No angle value to read");
	}

	else
	{
		iStream >> gons;
		if(!(iStream.fail()))
		{
			angle.setStatus(TVNumericValue::kKnown);
			angle.setGonsValue(gons/1000);
		}
		else
		{
			angle.setStatus(TVNumericValue::kNull);
			//make iStream's flags to "goodbit"
			iStream.clear();
			//read the wrong gons value
			/*while(iStream.peek()!=' ' && iStream.peek()!='\n')
			{
				iStream.readChar();
			}*/
			iStream.readLine();
			iStream.setError("Invalid angle value");
		}
	}

	
	
	return;
}

/////////////////////////////////////////////////////////
// outputs an angle formatted in Gons units to the stream
/////////////////////////////////////////////////////////
void	T100MicroGonsFilter::output(TAStreamFormatter& oStream,const TAngle& angle)const
{	// function to put a gons angle to a QTextStream object.
	// Gets the value from a TAngle object

	int precision;
	precision = oStream.getPrecisionFormat();

	if (precision <=4)
		{
			precision = 0;
		}
	else
		{
			precision -=4;
		}


	oStream.setf(ios::fixed,ios::floatfield);
	oStream.width(oStream.getWidthFormat());
	oStream.precision((precision));
	/*Modification du 08/05/2003 (suite a la modif de TAngle)
	pour afficher les residus entre en -200	et +200 grades,
	la valeur est demander en radian avant d'etre convertie en CC*/
	//oStream<< ((angle.getGonsValue())*10000);
	oStream<< ((angle.getRadiansValue()*(200/TAngle::pi().getRadiansValue()))*10000);
	

	return;
}




//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

