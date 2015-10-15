// TGeodeticFilter.cpp
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
#include	"TAngleFilter.h"
#include	"TLengthFilter.h"
#include	"TAngle.h"
#include	"TLength.h"
#include	"TSpatialPosition.h"
#include	"TGeodeticFilter.h"
#include	"TAStreamFormatter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TGeodeticCoorSys)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TGeodeticFilter *TGeodeticFilter::fCoorSys = 0;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TGeodeticFilter::TGeodeticFilter()
{}

TGeodeticFilter::~TGeodeticFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TGeodeticFilter  *TGeodeticFilter::instance()
{
	if( fCoorSys == 0 )
	{
		fCoorSys = new TGeodeticFilter;
	}

	return fCoorSys;
}


//////////////////////////////////////////////////////////////////////
//TSpatialPosition Functions
//////////////////////////////////////////////////////////////////////
void	TGeodeticFilter::input(TAStreamFormatter& iStream, TSpatialPosition& position) const
{	// function to get x-, y-, and z-coordinates from a Stream object
	// and set the values in a TSpatialPosition object

	TAngle phi, lambda;
	TLength H;
	TPositionVector pv (TCoordSysFactory::kGeodetic);

	iStream.skipWhiteSpace();
	if(iStream.peek()=='\n')
	{//check if there's coordinate to read
		pv.setStatus(TVNumericValue::kNull);
		iStream.setError("No coordinates to read");
	}
	else
	{
		(TAStreamFormatter::getAngleFilter(iStream.getAngleUnits()))->input(iStream , phi);
		if(iStream.peek()=='\n' || phi.getStatus()==TVNumericValue::kNull)
		{/*check if coordinate which has been read is valid and
			if there's always coordinate to read*/
			pv.setStatus(TVNumericValue::kNull);
			if(iStream.peek()=='\n')
			{
				iStream.setError("There's only one coordinate to read");
			}
			if(phi.getStatus()==TVNumericValue::kNull)
			{
				iStream.setError("The <Phi> coordinate is not valid");
			}
		}
		else
		{
			(TAStreamFormatter::getAngleFilter(iStream.getAngleUnits()))->input(iStream , lambda);
			iStream.skipWhiteSpace();
			if(iStream.peek()=='\n' || lambda.getStatus()==TVNumericValue::kNull)
			{/*check if coordinate which has been read is valid and
			if there's always coordinate to read*/
				pv.setStatus(TVNumericValue::kNull);
				if(iStream.peek()=='\n')
				{
					iStream.setError("There's only two coordinates to read");
				}
				if(phi.getStatus()==TVNumericValue::kNull)
				{
					iStream.setError("The <Lambda> coordinate is not valid");
				}
			}
			else
			{
				(TAStreamFormatter::getLengthFilter(iStream.getLengthUnits()))->input(iStream , H);
				iStream.skipWhiteSpace();
				//if(H.getStatus()==TVNumericValue::kNull || (iStream.peek()!='\n' && iStream.peek()!='&' && iStream.peek()!=EOF))
				if(H.getStatus()==TVNumericValue::kNull)
				{/*check if coordinate which has been read is valid and
					check if there's not too much information, except comments*/
					pv.setStatus(TVNumericValue::kNull);
					if(H.getStatus()==TVNumericValue::kNull)
					{
						iStream.setError("The <H> coordinate is not valid");
					}
				/*	if(iStream.peek()!='\n' && iStream.peek()!='&' && iStream.peek()!=EOF)
					{
						pv.setStatus(TVNumericValue::kNull);
						iStream.setError("Too much information to read");
					}*/
				}
				else
				{
					pv.setPhiEllipsoid(phi);
					pv.setLambdaEllipsoid(lambda);
					pv.setH(H.getMetresValue());
					pv.setStatus(TVNumericValue::kKnown);

				}
			}
		}
	}

	position.setCoordinates(pv);

	return;
}


void	TGeodeticFilter::output(TAStreamFormatter& oStream, const TSpatialPosition& position) const
{	// function to put x-, y-, and z-coordinates to a Stream object.
	// Gets the coordinate values from a TSpatialPosition object

	// sets the stream's width and precision and outputs the coordinates
	TPositionVector pv (TCoordSysFactory::kGeodetic);
	pv=position.getCoordinates(TCoordSysFactory::kGeodetic);
	
	this->output( oStream,  pv);

	return;
}



//////////////////////////////////////////////////////////////////////
//TPositionVector Functions
//////////////////////////////////////////////////////////////////////
void	TGeodeticFilter::input(TAStreamFormatter& iStream, TPositionVector& pv) const
{	// function to get x-, y-, and z-coordinates from a Stream object
	// and set the values in a TPositionVector object

	TAngle phi, lambda;
	TLength H;
	pv.setCoordSys(TCoordSysFactory::kGeodetic);

	iStream.skipWhiteSpace();
	if(iStream.peek()=='\n')
	{//check if there's coordinate to read
		pv.setStatus(TVNumericValue::kNull);
		iStream.setError("No coordinates to read");
	}
	else
	{
		(TAStreamFormatter::getAngleFilter(iStream.getAngleUnits()))->input(iStream , phi);
		if(iStream.peek()=='\n' || phi.getStatus()==TVNumericValue::kNull)
		{/*check if coordinate which has been read is valid and
			if there's always coordinate to read*/
			pv.setStatus(TVNumericValue::kNull);
			if(iStream.peek()=='\n')
			{
				iStream.setError("There's only one coordinate to read");
			}
			if(phi.getStatus()==TVNumericValue::kNull)
			{
				iStream.setError("The <Phi> coordinate is not valid");
			}
		}
		else
		{
			(TAStreamFormatter::getAngleFilter(iStream.getAngleUnits()))->input(iStream , lambda);
			iStream.skipWhiteSpace();
			if(iStream.peek()=='\n' || lambda.getStatus()==TVNumericValue::kNull)
			{/*check if coordinate which has been read is valid and
			if there's always coordinate to read*/
				pv.setStatus(TVNumericValue::kNull);
				if(iStream.peek()=='\n')
				{
					iStream.setError("There's only two coordinates to read");
				}
				if(phi.getStatus()==TVNumericValue::kNull)
				{
					iStream.setError("The <Lambda> coordinate is not valid");
				}
			}
			else
			{
				(TAStreamFormatter::getLengthFilter(iStream.getLengthUnits()))->input(iStream , H);
				iStream.skipWhiteSpace();
				//if(H.getStatus()==TVNumericValue::kNull || (iStream.peek()!='\n' && iStream.peek()!='&' && iStream.peek()!=EOF))
				if(H.getStatus()==TVNumericValue::kNull)
				{/*check if coordinate which has been read is valid and
					check if there's not too much information, except comments*/
					pv.setStatus(TVNumericValue::kNull);
					if(H.getStatus()==TVNumericValue::kNull)
					{
						iStream.setError("The <H> coordinate is not valid");
					}
				/*	if(iStream.peek()!='\n' && iStream.peek()!='&' && iStream.peek()!=EOF)
					{
						pv.setStatus(TVNumericValue::kNull);
						iStream.setError("Too much information to read");
					}*/
				}
				else
				{
					pv.setPhiEllipsoid(phi);
					pv.setLambdaEllipsoid(lambda);
					pv.setH(H.getMetresValue());
					pv.setStatus(TVNumericValue::kKnown);

				}
			}
		}
	}
	return;
}


void	TGeodeticFilter::output(TAStreamFormatter& oStream, const TPositionVector& pv) const
{	// function to put x-, y-, and z-coordinates to a Stream object.
	// Gets the coordinate values from a TPositionVector object

	string sep =oStream.getSeparator();

	// sets the stream's width and precision and outputs the coordinates
	if (pv.getStatus()!= TANumericValue::kNull)
	{
		int width = oStream.getWidthFormat();
		int precision = oStream.getPrecisionFormat();

		if(	oStream.getAngleUnits() == TAngle::kDMS && 
			oStream.getCoordSys() == TCoordSysFactory::kGeodetic)
		{
			oStream.setWidthFormat(width + 3);
			oStream.setPrecisionFormat(precision + 6);
		}
		else if (	oStream.getAngleUnits() == TAngle::kGons && 
					oStream.getCoordSys() == TCoordSysFactory::kGeodetic)
		{
			oStream.setWidthFormat(width + 1);
			oStream.setPrecisionFormat(precision + 5);
		}

		oStream<<pv.getPhiEllipsoid()<<sep<<pv.getLambdaEllipsoid() << sep;
		
		oStream.setWidthFormat(width);
		oStream.setPrecisionFormat(precision);
		oStream<<pv.getH() << sep;
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
