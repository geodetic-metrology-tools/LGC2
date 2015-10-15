// TGyroOrientationObservation.cpp : implementation file
//
// Class for a facade for an orientation measurement
//

#include "TGyroOrientationObservation.h"
#include "TTheodoliteStation.h"
#include "TGyroOrientationROM.h"
#include "TRefSystemFactory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TGyroOrientationObservation::TGyroOrientationObservation()
{// default constructor
	fIsActive = false;
}


TGyroOrientationObservation::TGyroOrientationObservation(TheodStConstIter station, GyroOrieROMConstIter ROM,
										   HorAngMeasConstIter meas, PointConstIter stationedPt,
										   PointConstIter targetPt, AngleConstantConstIter cte, int id)
										   : fMeasConst(cte)
{// constructor taking 6 param (station, ROM, measurement, stationed and targeted points and angle constant)
	fStation = station;
	fROM = ROM;
	fMeasurement = meas;
	fStationedPoint = stationedPt;
	fTargetPoint = targetPt;
	fIsActive = true; // en attendant...
	observationID = id;
}


TGyroOrientationObservation::TGyroOrientationObservation(const TGyroOrientationObservation &source)
{//Copy Constructor
	fStation = source.fStation; 
	fROM = source.fROM; 
	fMeasurement = source.fMeasurement; 
	fStationedPoint = source.fStationedPoint; 
	fTargetPoint = source.fTargetPoint;
	fMeasConst = source.fMeasConst;
	fIsActive = source.fIsActive;
	observationID = source.observationID;
}


TGyroOrientationObservation::~TGyroOrientationObservation()
{//Destructor
}

///////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
///////////////////////////////////////////////////////////////////////
bool TGyroOrientationObservation::operator==(const TGyroOrientationObservation& right) const
{//Overloaded equality operator
	
	bool isEqualTo = false;

	if ((fStation == right.fStation) && (fTargetPoint == right.fTargetPoint))
		isEqualTo = true;

	return isEqualTo;
}


TAngle  TGyroOrientationObservation::getGyroOrieValue() const
{//Returns the horizontal angle value from the measurement

	return fMeasurement->getAngleValue();
	
/*	if (fMeasConst->getStatus() == TAMeasurement::kFixed)
	{
		return ((fMeasurement->getAngleValue()) + (fMeasConst->getValue()));
	}
	else
	{
		TAngle null;
		cout << "Error : Angle Constant for Horizontal Angle not Fixed!";
		return null;
	}*/
}


TAngle  TGyroOrientationObservation::getSigmaValue() const
{//Returns the angle's sigma from the measurement
	return getMeasurement()->getSigma();
}



TPositionVector		TGyroOrientationObservation::getStationedPosition() const
{//Returns the position vector of the stationed point 
	return fStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TGyroOrientationObservation::getStationedPosStatus() const
{//Returns the spatial status of the stationed point 
	return fStationedPoint->getPosition().getObjectStatus();
}


string	TGyroOrientationObservation::getStationedPosName() const
{//Returns the concatenated name of the stationed point 

	return fStationedPoint->getName().getName();
}


TPositionVector		TGyroOrientationObservation::getTargetPosition() const
{//Returns the position vector of the target point
	return fTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TGyroOrientationObservation::getTargetPosStatus() const
{//Returns the spatial status of the target point
	return fTargetPoint->getPosition().getObjectStatus();
}


string				TGyroOrientationObservation::getTargetPosName() const
{//Returns the concatenated name of the target point
	return fTargetPoint->getName().getName();
}


TAngle		TGyroOrientationObservation::getConstantValue() const
{// Returns the constant angle 
	return fMeasConst->getValue();
}


TAMeasurement::ECalcStatus		TGyroOrientationObservation::getConstantStatus() const
{// Returns the constant angle'status 
	return fMeasConst->getStatus();
}

 
string		TGyroOrientationObservation::getConstantName() const
{// Returns the constant angle's identifier
	return fMeasConst->getConstantName();
}

// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TGyroOrientationObservation::get3DStationPosVec() const
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


TPositionVector	TGyroOrientationObservation::get3DTargetPosVec() const
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


///////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////