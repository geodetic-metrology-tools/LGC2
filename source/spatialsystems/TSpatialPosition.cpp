// TSpatialPosition.h: 
//
// The Spatial Position of a point in a given reference frame.
// 
// Provides an interface with restricted access to the coordinates
// a TSpatialPointObject.
// 
// Patterns:
// This class is a Proxy.
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// other forward declarations
#include  "TAReferenceFrame.h"
#include  "TReferenceEllipsoid.h"
#include  "TSpatialPosition.h"
#include  "TLength.h"
#include  "TAngle.h"
#include  <math.h>
////////////////////////////////////////////////////////////////


//ClassImp(TSpatialPosition)


////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////
TSpatialPosition::TSpatialPosition()
{//no argument constructor
 //private constructor
 //deliberatly not implemented
	fX = new TPositionVector(TCoordSysFactory::k3DCartesian);//pour insure

}


TSpatialPosition::TSpatialPosition( TAReferenceFrame* trf )
{//constructor
	fX = new TPositionVector(TCoordSysFactory::k3DCartesian);
	setObjectStatus(TSpatialStatus::kPosNull);
	setRefFrame(trf);
}


TSpatialPosition::TSpatialPosition(TAReferenceFrame* rf, const TPositionVector& pv) :
	fX(new TPositionVector(pv)) {	
	if(fX == 0)
	{
		// object not created properly
		// reagi
		setObjectStatus(TSpatialStatus::kPosNull);
	}
	else
	{
		setObjectStatus(TSpatialStatus::kCala);
	}
	setRefFrame(rf);
}

TSpatialPosition::TSpatialPosition( TAReferenceFrame* trf,
								   const TReal x, const TReal y, const TReal z,
								   TCoordSysFactory::ECoordSys cs )
{//constructor
	fX = new TPositionVector(x,y,z,cs);
	if(fX == 0)
	{
		// object not created properly
		// reagi
		setObjectStatus(TSpatialStatus::kPosNull);
	}
	else
	{
		setObjectStatus(TSpatialStatus::kCala);
	}
	setRefFrame(trf);
}


TSpatialPosition::TSpatialPosition( TAReferenceFrame* trf, counted_ptr< TSpatialStatus> cpps )
/*fX(0)*/
{//constructor
	fX = new TPositionVector(TCoordSysFactory::k3DCartesian);//default
	if(fX == 0)
	{
		// object not created properly
		// reagi
		setObjectStatus(TSpatialStatus::kPosNull);
	}
	else
	{
		setObjectStatus(cpps);
	}
	setRefFrame(trf);
}


TSpatialPosition::TSpatialPosition(const TSpatialPosition& tsp )
{//copy constructor
	fX=0;
	*this = tsp;
}


TSpatialPosition::~TSpatialPosition()
{//destructor
	if (fX != 0)
	{
		delete fX;
		fX = 0;
	}
}


///////////////////////////////////////////////////////////////////////
// Copy assignement operator
///////////////////////////////////////////////////////////////////////
TSpatialPosition& TSpatialPosition::operator=(const TSpatialPosition& tsp)
{
	if (this != &tsp)
	{
		if(fX != 0)
		{
			delete fX;
			fX = 0;
		}

		fX = new TPositionVector(*(tsp.fX));
		if(fX == 0)
		{
			// object not created properly
			// reagi
			setObjectStatus(TSpatialStatus::kPosNull);
		}
		else
		{
			setObjectStatus(tsp.getObjectStatus());
		}
		setRefFrame(tsp.getRefFrame());
	}
	return *this;
}



///////////////////////////////////////////////////////////////////////
//MEMBER FUNCZIONS
///////////////////////////////////////////////////////////////////////
TPositionVector TSpatialPosition::getCoordinates(TCoordSysFactory::ECoordSys coordsys) const
{//!Return a position vector of a spatial position through TVReferenceFrame
	return getRefFrame()->getCoordinates( this, coordsys );
}


bool TSpatialPosition::setCoordinates(const TPositionVector& pv)
{//! Set a position vector of a spatial position through TVReferenceFrame
	if (pv.getStatus() != TANumericValue::kNull)
	{
		return getRefFrame()->setCoordinates(this, pv);
	}

	else
	{
		setObjectStatus(TSpatialStatus::kPosNull);
		return false;
	}
}


bool TSpatialPosition::transform(TAReferenceFrame* to)
{//transformation between 2 RF
	if ( this->getRefFrame() == to )
	{ return true; }

	else
	{ return getRefFrame()->transform( this, to ); }
}



void TSpatialPosition::changeRefFrameTo(TAReferenceFrame* rf)
{// change the reference frame
	setRefFrame(rf);
}



/////////////////////////////////////////////////////////////////////////////////////////
//PRIVATE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////
TPositionVector TSpatialPosition::getPositionVector() const
{// Get a copy of the position vector pointed by the spatial position
	TPositionVector vector(*fX);
	return vector;
}


bool TSpatialPosition::setPositionVector(const TPositionVector& pv)
{// Set a position vector of a spatial position
	if (fX != 0)
	{
		delete fX;
		fX = 0;
	}
	fX= new TPositionVector(pv);
	if(fX == 0)
	{
		// object not created properly
		// reagi
		setObjectStatus(TSpatialStatus::kPosNull);
	}
	else
	{
		if (pv.getStatus() == TVNumericValue::kNull)
			setObjectStatus(TSpatialStatus::kPosNull);
		else if (this->getObjectStatus() != TSpatialStatus::kPosNull)
			setObjectStatus(this->getObjectStatus());
		else
			setObjectStatus(TSpatialStatus::kCala);
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////////////////////////////////
