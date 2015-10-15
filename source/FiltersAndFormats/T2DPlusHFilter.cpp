// T2DPlusHFilter.cpp
//
// Concrete class of a TSpatialPositionFilter.
// Handles the IO of an TSpatialPosition object for a
// 2D+H coordinate system. 
//
// 
// Patterns:
// This is a Singleton.
// Works as a Strategy of a TAStreamFormatter object. 
//
// Copyright 2002, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations
#include  "TLengthFilter.h"
#include  "TLength.h"
#include  "TSpatialPosition.h"
#include  "T2DPlusHFilter.h"
#include  "TAStreamFormatter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(T2DPlusHCoorSys)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
T2DPlusHFilter *T2DPlusHFilter::fCoorSys = 0;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

T2DPlusHFilter::T2DPlusHFilter()
{}

T2DPlusHFilter::~T2DPlusHFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
T2DPlusHFilter  *T2DPlusHFilter::instance()
{/*!return the unique class component*/
	if( fCoorSys == 0 )
	{
		fCoorSys = new T2DPlusHFilter;
	}
	return fCoorSys;
}


/////////////////////////////////////////////////////////////////////
//TSpatialPosition Functions
/////////////////////////////////////////////////////////////////////
void	T2DPlusHFilter::input(TAStreamFormatter& iStream, TSpatialPosition& position) const
{/*!Get coordinates from a TAStreamFormatter object
	and set the values in a TSpatialPosition object
*/
	TLength	x, y, h;
	TPositionVector pv (TCoordSysFactory::k2DPlusH);

	iStream.skipWhiteSpace();
	if(iStream.peek()=='\n' || iStream.atEndCom() == true)
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
				TAStreamFormatter::getLengthFilter(iStream.getLengthUnits())->input(iStream, h);
			//	if(h.getStatus()==TVNumericValue::kNull || (iStream.peek()!='\n' && iStream.peek()!='&' && iStream.peek()!=EOF))
				if(h.getStatus()==TVNumericValue::kNull)
				{/*check if coordinate which has been read is valid and
					check if there's not too much information, except comments*/
					pv.setStatus(TVNumericValue::kNull);
					if(h.getStatus()==TVNumericValue::kNull)
					{
						iStream.setError("The <h> coordinate is not valid");
					}
					/*if(iStream.peek()!='\n' && iStream.peek()!='&' && iStream.peek()!=EOF)
					{
						pv.setStatus(TVNumericValue::kNull);
						iStream.setError("Too much information to read");
					}*/
				}
				else
				{
					pv.setX(x.getMetresValue());
					pv.setY(y.getMetresValue());
					pv.setH(h.getMetresValue());
					pv.setStatus(TVNumericValue::kKnown);

				}
			}
		}
	}

	position.setCoordinates(pv);
	return;
}


void	T2DPlusHFilter::output(TAStreamFormatter& oStream, const TSpatialPosition& position) const
{/*!Get coordinates from a TSpatialPosition object
	and output the values in a TAStreamFormatter object
*/
	
	//outputs the coordinates
	TPositionVector pv (TCoordSysFactory::k2DPlusH);
	pv=position.getCoordinates(TCoordSysFactory::k2DPlusH);
	
	this->output( oStream,  pv);

	return;
}


/////////////////////////////////////////////////////////////////////
//TPositionVector Functions
/////////////////////////////////////////////////////////////////////
void	T2DPlusHFilter::input(TAStreamFormatter& iStream, TPositionVector& pv) const
{/*!Get coordinates from a TAStreamFormatter object
	and set the values in a TPositionVector object
*/
	TLength	x, y, h;
	pv.setCoordSys(TCoordSysFactory::k2DPlusH);

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
				TAStreamFormatter::getLengthFilter(iStream.getLengthUnits())->input(iStream, h);
				if(h.getStatus()==TVNumericValue::kNull)
				{/*check if coordinate which has been read is valid and
					check if there's not too much information, except comments*/
					pv.setStatus(TVNumericValue::kNull);
					if(h.getStatus()==TVNumericValue::kNull)
					{
						iStream.setError("The <h> coordinate is not valid");
					}
				}
				else
				{
					pv.setX(x.getMetresValue());
					pv.setY(y.getMetresValue());
					pv.setH(h.getMetresValue());
					pv.setStatus(TVNumericValue::kKnown);

				}
			}
		}
	}
	return;
}


void	T2DPlusHFilter::output(TAStreamFormatter& oStream, const TPositionVector& pv) const
{/*!Get coordinates from a TPositionVector object
	and put the values in a TAStreamFormatter object
*/

	string sep =oStream.getSeparator();

	//outputs the coordinates
	if (pv.getStatus()!= TANumericValue::kNull)
	{	
		oStream<<pv.getX()<<sep<<pv.getY()<<sep<<pv.getH() << sep;
	
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

