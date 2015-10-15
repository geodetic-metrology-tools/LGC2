// 
// TRadialOffsetCnstrObservation.cpp : implementation file
//
// Class for a facade for an RADI measurement
//


#include "TRefSystemFactory.h"
#include "TRadialOffsetCnstrObservation.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TRadialOffsetCnstrObservation::TRadialOffsetCnstrObservation()
{//default constructor
	fIsActive = false;
}


TRadialOffsetCnstrObservation::TRadialOffsetCnstrObservation(PointConstIter point, RadOffCnstrConstIter meas, int id)
{//constructor
	fMeasurement = meas;
	fPoint = point;
	fIsActive = true; // en attendant.
	observationID = id;
}


TRadialOffsetCnstrObservation::TRadialOffsetCnstrObservation(const TRadialOffsetCnstrObservation &source)
{//Copy Constructor
	fMeasurement = source.fMeasurement;
	fPoint = source.fPoint;
	fIsActive = source.fIsActive;
	observationID = source.observationID;
}


TRadialOffsetCnstrObservation::~TRadialOffsetCnstrObservation()
{// Destructor
}


////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////
bool TRadialOffsetCnstrObservation::operator==(const TRadialOffsetCnstrObservation& right) const
{//Overloaded equality operator
	
	bool isEqualTo = false;

	if ((fPoint == right.fPoint) &&
		(*fMeasurement == *right.fMeasurement) && (fIsActive == right.fIsActive))
		isEqualTo = true;

	return isEqualTo;
}

	
TLength  TRadialOffsetCnstrObservation::getRadialOffsetCnstrSigma() const
{//Returns the RADI offset value from the measurement
	return fMeasurement->getCnstrSigma();
}

	
TAngle  TRadialOffsetCnstrObservation::getRadialOffsetCnstrBearing() const
{//Returns the RADI offset value from the measurement
	return fMeasurement->getCnstrBearing();
}


TPositionVector		TRadialOffsetCnstrObservation::getPosition() const
{//Returns the position vector of the point which has a constraint
	return fPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TRadialOffsetCnstrObservation::getPosStatus() const
{//Returns the spatial status of the point which has a constraint
	return fPoint->getPosition().getObjectStatus();
}


string		TRadialOffsetCnstrObservation::getPosName() const
{//Returns the concatenated name of the point which has a constraint
	return fPoint->getName().getName();
}


// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TRadialOffsetCnstrObservation::get3DPosVec() const
{
	TPositionVector posVec(this->getPosition());
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(this->getPoint()->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}





////////////////////////////////////
//END
///////////////////////////////////
