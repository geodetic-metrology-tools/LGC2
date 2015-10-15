// 
// TOffsetToVerLineObservation.cpp : implementation file
//
// Class for a facade for an ECVE measurement
//


#include "TRefSystemFactory.h"
#include "TOffsetToVerLineObservation.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TOffsetToVerLineObservation::TOffsetToVerLineObservation()
{//default constructor
	fIsActive = false;
}


TOffsetToVerLineObservation::TOffsetToVerLineObservation(DistStConstIter station, OffsetToVerLineMeasConstIter meas, PointConstIter stationedPt, PointConstIter targetPt, DistConstantConstIter cte, int id)
{//constructor
	fStation= station;
	fMeasurement = meas;
	fStationedPoint = stationedPt;
	fTargetPoint = targetPt;
	fMeasConst = cte;
	fIsActive = true; // en attendant.
	observationID = id;
}


TOffsetToVerLineObservation::TOffsetToVerLineObservation(const TOffsetToVerLineObservation &source)
{//Copy Constructor
	fStation= source.fStation;
	fMeasurement = source.fMeasurement;
	fStationedPoint = source.fStationedPoint;
	fTargetPoint = source.fTargetPoint;
	fMeasConst = source.fMeasConst;
	fIsActive = source.fIsActive;
	observationID = source.observationID;
}


TOffsetToVerLineObservation::~TOffsetToVerLineObservation()
{// Destructor
}


////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////
bool TOffsetToVerLineObservation::operator==(const TOffsetToVerLineObservation& right) const
{//Overloaded equality operator
	
	bool isEqualTo = false;

	if ((fStationedPoint == right.fStationedPoint) && (fTargetPoint == right.fTargetPoint))
		isEqualTo = true;

	return isEqualTo;
}

	
TLength  TOffsetToVerLineObservation::getOffsetToVerLineValue() const
{//Returns the horizontal angle value from the measurement
	return fMeasurement->getLengthValue();
/*	if (fMeasConst->getStatus() == TAMeasurement::kFixed)
	{
		return ((fMeasurement->getLengthValue()) + (fMeasConst->getValue()));
	}
	else
	{
		TLength null;
		cout << "Error : distance Constant for Offset not Fixed!";
		return null;
	}*/
}


TLength  TOffsetToVerLineObservation::getSigmaValue() const
{//Returns the angle's sigma from the measurement

	return getMeasurement()->getSigma();
}


TPositionVector		TOffsetToVerLineObservation::getStationedPosition() const
{//Returns the position vector of the stationed point 

	return fStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToVerLineObservation::getStationedPosStatus() const
{//Returns the spatial status of the stationed point 

	return fStationedPoint->getPosition().getObjectStatus();
}


string	TOffsetToVerLineObservation::getStationedPosName() const
{//Returns the concatenated name of the stationed point 
	return fStationedPoint->getName().getName();
}


TPositionVector		TOffsetToVerLineObservation::getTargetPosition() const
{//Returns the position vector of the point which define the target line 

	return fTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToVerLineObservation::getTargetPosStatus() const
{//Returns the spatial status of the target point 
	return fTargetPoint->getPosition().getObjectStatus();
}


string				TOffsetToVerLineObservation::getTargetPosName() const
{//Returns the concatenated name of the target point

	return fTargetPoint->getName().getName();
}


string		TOffsetToVerLineObservation::getTargetLineName() const
{/*! Returns the concatenated name of the target Plane*/
	return this->getMeasurement()->getTargetLineName().getName();
}


TLength		TOffsetToVerLineObservation::getConstantValue() const
{// Returns the constant distance 
	return fMeasConst->getValue();
}


TAMeasurement::ECalcStatus		TOffsetToVerLineObservation::getConstantStatus() const
{// Returns the constant distance'status 
	return fMeasConst->getStatus();
}


string		TOffsetToVerLineObservation::getConstantName() const
{// Returns the constant distance's identifier 
	return fMeasConst->getConstantName();
}


// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TOffsetToVerLineObservation::get3DStationPosVec() const
{
	TPositionVector posVec(this->getStationedPosition());
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(this->getStationedPoint()->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}


TPositionVector	TOffsetToVerLineObservation::get3DTargetPosVec() const
{
	TPositionVector posVec(this->getTargetPosition());
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(this->getTargetPoint()->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}





////////////////////////////////////
//END
///////////////////////////////////