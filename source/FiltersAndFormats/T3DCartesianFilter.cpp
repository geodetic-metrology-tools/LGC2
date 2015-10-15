// T3DCartesianFilter.cpp
//
// Concrete class of a TSpatialPositionFilter.
// Handles the IO of an TSpatialPosition object for a
// 3D Cartesian coordinate system. 
//
// 
// Patterns:
// This is a Singleton.
// Works as a Strategy of a TAStreamFormatter object. 
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations
#include  "TLengthFilter.h"
#include  "TLength.h"
#include  "TSpatialPosition.h"
#include  "T3DCartesianFilter.h"
#include  "TAStreamFormatter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(T3DCartesianCoorSys)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
T3DCartesianFilter *T3DCartesianFilter::fCoorSys = 0;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

T3DCartesianFilter::T3DCartesianFilter()
{}

T3DCartesianFilter::~T3DCartesianFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

T3DCartesianFilter  *T3DCartesianFilter::instance()
{
	if( fCoorSys == 0 )
	{
		fCoorSys = new T3DCartesianFilter;
	}

	return fCoorSys;
}

//////////////////////////////////////////////////////////////////////////////
//TSpatialPosition Functions
//////////////////////////////////////////////////////////////////////////////
void	T3DCartesianFilter::input(TAStreamFormatter& iStream, TSpatialPosition& position) const
{	// function to get x-, y-, and z-coordinates from a QTextStream object
	// and set the values in a TSpatialPosition object
	TLength	x, y, z;
	TPositionVector pv (TCoordSysFactory::k3DCartesian);

	iStream.skipWhiteSpace();
	if(iStream.peek()=='\n' || iStream.atEndCom() == true)//at endCom added by Eve 20/1/05 9:18
	{//check if there's coordinate to read
		pv.setStatus(TVNumericValue::kNull);
		iStream.setError("No coordinates to read");
	}
	else
	{
		TAStreamFormatter::getLengthFilter(iStream.getLengthUnits())->input(iStream, x);
		iStream.skipWhiteSpace();
		if(iStream.peek()=='\n' || x.getStatus()==TVNumericValue::kNull || iStream.atEndCom() == true)
		{/*check if coordinate which has been read is valid and
			if there's always coordinate to read*/
			pv.setStatus(TVNumericValue::kNull);
			if(iStream.peek()=='\n' || iStream.atEndCom() == true)
			{
				iStream.setError("There's only one coordinate to read");
			}
			if(x.getStatus()==TVNumericValue::kNull)
			{
				iStream.setError("The <x> coordinate is not valid");
			}
		}	
		else
		{
			TAStreamFormatter::getLengthFilter(iStream.getLengthUnits())->input(iStream, y);
			iStream.skipWhiteSpace();
			if(iStream.peek()=='\n' || y.getStatus()==TVNumericValue::kNull || iStream.atEndCom() == true)
			{/*check if coordinate which has been read is valid and
			if there's always coordinate to read*/
				pv.setStatus(TVNumericValue::kNull);
				if(iStream.peek()=='\n' || iStream.atEndCom() == true)
				{
					iStream.setError("There's only two coordinates to read");
				}
				if(y.getStatus()==TVNumericValue::kNull)
				{
					iStream.setError("The <y> coordinate is not valid");
				}
			}
			else
			{
				TAStreamFormatter::getLengthFilter(iStream.getLengthUnits())->input(iStream, z);
		
				if(z.getStatus()==TVNumericValue::kNull)
				{/*check if coordinate which has been read is valid and
					check if there's not too much information, except comments*/
					pv.setStatus(TVNumericValue::kNull);
					if(z.getStatus()==TVNumericValue::kNull)
					{
						iStream.setError("The <z> coordinate is not valid");
					}
				
				}
				else
				{
					pv.setX(x.getMetresValue());
					pv.setY(y.getMetresValue());
					pv.setZ(z.getMetresValue());
					pv.setStatus(TVNumericValue::kKnown);

				}
			}
		}
	}

	position.setCoordinates(pv);
	
	return;
}


void	T3DCartesianFilter::output(TAStreamFormatter& oStream, const TSpatialPosition& position) const
{	// function to put x-, y-, and z-coordinates to a stream object.
	// Gets the coordinate values from a TSpatialPosition object
	
	//outputs the coordinates
	TPositionVector pv (TCoordSysFactory::k3DCartesian);
	pv=position.getCoordinates(TCoordSysFactory::k3DCartesian);
	
	this->output( oStream,  pv);

	return;
}


//////////////////////////////////////////////////////////////////////////
//TPositionVector Functions
//////////////////////////////////////////////////////////////////////////
void	T3DCartesianFilter::input(TAStreamFormatter& iStream, TPositionVector& pv) const
{	// function to get x-, y-, and z-coordinates from a QTextStream object
	// and set the values in a TPositionVector object
	TLength	x, y, z;
	pv.setCoordSys(TCoordSysFactory::k3DCartesian);

	iStream.skipWhiteSpace();
	if(iStream.peek()=='\n')
	{//check if there's coordinate to read
		pv.setStatus(TVNumericValue::kNull);
		iStream.setError("No coordinates to read");
	}
	else
	{
		TAStreamFormatter::getLengthFilter(iStream.getLengthUnits())->input(iStream, x);
		iStream.skipWhiteSpace();
		if(iStream.peek()=='\n' || x.getStatus()==TVNumericValue::kNull)
		{/*check if coordinate which has been read is valid and
			if there's always coordinate to read*/
			pv.setStatus(TVNumericValue::kNull);
			if(iStream.peek()=='\n')
			{
				iStream.setError("There's only one coordinate to read");
			}
			if(x.getStatus()==TVNumericValue::kNull)
			{
				iStream.setError("The <x> coordinate is not valid");
			}
		}	
		else
		{
			TAStreamFormatter::getLengthFilter(iStream.getLengthUnits())->input(iStream, y);
			iStream.skipWhiteSpace();
			if(iStream.peek()=='\n' || y.getStatus()==TVNumericValue::kNull)
			{/*check if coordinate which has been read is valid and
			if there's always coordinate to read*/
				pv.setStatus(TVNumericValue::kNull);
				if(iStream.peek()=='\n')
				{
					iStream.setError("There's only two coordinates to read");
				}
				if(y.getStatus()==TVNumericValue::kNull)
				{
					iStream.setError("The <y> coordinate is not valid");
				}
			}
			else
			{
				TAStreamFormatter::getLengthFilter(iStream.getLengthUnits())->input(iStream, z);
				if(z.getStatus()==TVNumericValue::kNull)
				{/*check if coordinate which has been read is valid and
					check if there's not too much information, except comments*/
					pv.setStatus(TVNumericValue::kNull);
					if(z.getStatus()==TVNumericValue::kNull)
					{
						iStream.setError("The <z> coordinate is not valid");
					}
				}
				else
				{
					pv.setX(x.getMetresValue());
					pv.setY(y.getMetresValue());
					pv.setZ(z.getMetresValue());
					pv.setStatus(TVNumericValue::kKnown);
				}
			}
		}
	}

	return;
}


void	T3DCartesianFilter::output(TAStreamFormatter& oStream, const TPositionVector& pv) const
{	// function to put x-, y-, and z-coordinates to a stream object.
	// Gets the coordinate values from a TPositionVector object
	
	string sep =oStream.getSeparator();

	//outputs the coordinates
	if (pv.getStatus()!= TANumericValue::kNull)
	{
		oStream<<pv.getX()<<sep<<pv.getY()<<sep<<pv.getZ()<<sep;
	}
	else
	{
		// get the width from the text stream's TPointFormat
		int width= oStream.getPointFormat()->getCoordWidth();

		for (int i=0; i<3; i++)
		{
			oStream.width(width);
			oStream << " " << sep;
		}
	}

	return;
}



//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

