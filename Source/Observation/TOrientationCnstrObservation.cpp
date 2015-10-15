// 
// TOrientationCnstrObservation.cpp : implementation file
//
// Class for a facade for an PDOR measurement
//

#include "TRefSystemFactory.h"
#include "TOrientationCnstrObservation.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TOrientationCnstrObservation::TOrientationCnstrObservation()
{//default constructor
	fIsActive = false;
}


TOrientationCnstrObservation::TOrientationCnstrObservation(PointConstIter fixedPoint, PointConstIter refPoint, OrieCnstrConstIter cnstr, int id)
{//constructor
	fMeasurement = cnstr;
	fFixedPoint = fixedPoint;
	fRefPoint = refPoint;
	fIsActive = true; // en attendant.
	observationID = id;
}


TOrientationCnstrObservation::TOrientationCnstrObservation(const TOrientationCnstrObservation &source)
{//Copy Constructor
	fMeasurement = source.fMeasurement;
	fFixedPoint = source.fFixedPoint;
	fRefPoint = source.fRefPoint;
	fIsActive = source.fIsActive;
	observationID = source.observationID;
}


TOrientationCnstrObservation::~TOrientationCnstrObservation()
{// Destructor
}


////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////
bool TOrientationCnstrObservation::operator==(const TOrientationCnstrObservation& right) const
{//Overloaded equality operator
	
	bool isEqualTo = false;

	if ((fFixedPoint == right.fFixedPoint) &&
		(fRefPoint == right.fRefPoint))
		isEqualTo = true;

	return isEqualTo;
}

	
TAngle  TOrientationCnstrObservation::getOrieCnstrBearing() const
{//Returns the PDOR offset value from the measurement
	return fMeasurement->getCnstrBearing();
}


TAngle  TOrientationCnstrObservation::getSigmaValue() const
{//return the sigma (constant for PDOR)
	return getMeasurement()->getSigma();
}

TPositionVector		TOrientationCnstrObservation::getFixedPosition() const
{//Returns the position vector of the point which has a constraint
	return fFixedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOrientationCnstrObservation::getFixedPosStatus() const
{//Returns the spatial status of the point which has a constraint
	return fFixedPoint->getPosition().getObjectStatus();
}


string		TOrientationCnstrObservation::getFixedPosName() const
{//Returns the concatenated name of the point which has a constraint
	return fFixedPoint->getName().getName();
}



TPositionVector		TOrientationCnstrObservation::getRefPosition() const
{//Returns the position vector of the point which has a constraint
	return fRefPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOrientationCnstrObservation::getRefPosStatus() const
{//Returns the spatial status of the point which has a constraint
	return fRefPoint->getPosition().getObjectStatus();
}


string		TOrientationCnstrObservation::getRefPosName() const
{//Returns the concatenated name of the point which has a constraint
	return fRefPoint->getName().getName();
}


// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TOrientationCnstrObservation::get3DRefPosVec() const
{
	TPositionVector posVec(this->getRefPosition());
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(this->getRefPoint()->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}


TPositionVector	TOrientationCnstrObservation::get3DFixedPosVec() const
{
	TPositionVector posVec(this->getFixedPosition());
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(this->getFixedPoint()->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}





////////////////////////////////////
//END
///////////////////////////////////
