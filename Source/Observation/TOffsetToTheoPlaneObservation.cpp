// 
// TOffsetToTheoPlaneObservation.cpp : implementation file
//
// Class for a facade for an ECHO measurement
//

#include "TRefSystemFactory.h"
#include "TOffsetToTheoPlaneObservation.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TOffsetToTheoPlaneObservation::TOffsetToTheoPlaneObservation()
{//default constructor
	fIsActive = false;
}


TOffsetToTheoPlaneObservation::TOffsetToTheoPlaneObservation(DistStConstIter station, OffsetToTheoPlaneMeasConstIter meas,
														 PointConstIter stationedPt,
														 PointConstIter targetPt, TheodStConstIter targetSt, TAngle angle, DistConstantConstIter cte, int id)
{//constructor
	fStation= station;
	fMeasurement = meas;
	fStationedPoint = stationedPt;
	fTargetStationedPoint = targetPt;
	fTargetStation = targetSt;
	fAngle = angle;
	fMeasConst = cte;
	fIsActive = true; // en attendant.
	observationID = id;
}


TOffsetToTheoPlaneObservation::TOffsetToTheoPlaneObservation(const TOffsetToTheoPlaneObservation &source)
{//Copy Constructor
	fStation= source.fStation;
	fMeasurement = source.fMeasurement;
	fStationedPoint = source.fStationedPoint;
	fTargetStationedPoint = source.fTargetStationedPoint;
	fTargetStation = source.fTargetStation;
	fAngle = source.fAngle;
	fMeasConst = source.fMeasConst;
	fIsActive = source.fIsActive;
	observationID = source.observationID;
}


TOffsetToTheoPlaneObservation::~TOffsetToTheoPlaneObservation()
{// Destructor
}


////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////
bool TOffsetToTheoPlaneObservation::operator==(const TOffsetToTheoPlaneObservation& right) const
{//Overloaded equality operator
	
	bool isEqualTo = false;

	if ((fStationedPoint == right.fStationedPoint) &&
		(fTargetStationedPoint == right.fTargetStationedPoint) && (fTargetStation == right.fTargetStation))
		isEqualTo = true;

	return isEqualTo;
}

	
TLength  TOffsetToTheoPlaneObservation::getOffsetToTheoPlaneValue() const
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


TLength  TOffsetToTheoPlaneObservation::getSigmaValue() const
{//Returns the angle's sigma from the measurement

	return getMeasurement()->getSigma();
}


TPositionVector		TOffsetToTheoPlaneObservation::getStationedPosition() const
{//Returns the position vector of the stationed point 

	return fStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToTheoPlaneObservation::getStationedPosStatus() const
{//Returns the spatial status of the stationed point 

	return fStationedPoint->getPosition().getObjectStatus();
}


string	TOffsetToTheoPlaneObservation::getStationedPosName() const
{//Returns the concatenated name of the stationed point 
	return fStationedPoint->getName().getName();
}


TPositionVector		TOffsetToTheoPlaneObservation::getTargetTheoPosition() const
{//Returns the position vector of the point which define the target line 

	return fTargetStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToTheoPlaneObservation::getTargetTheoPosStatus() const
{//Returns the spatial status of the target point 
	return fTargetStationedPoint->getPosition().getObjectStatus();
}


string		TOffsetToTheoPlaneObservation::getTargetTheoPosName() const
{//Returns the concatenated name of the target point

	return fTargetStationedPoint->getName().getName();
}


TAngle		TOffsetToTheoPlaneObservation::getTargetAngle() const
{
	return fAngle;
}


string		TOffsetToTheoPlaneObservation::getTargetPlaneName() const
{/*! Returns the concatenated name of the target Plane*/
	return this->getMeasurement()->getTargetPlaneName().getName();
}


struct Angles	TOffsetToTheoPlaneObservation::getTheoV0() const
{// Returns the V0 from the rom
	struct Angles V0;
	V0.omega = TAngle(LITERAL(0.0));
	V0.phi = TAngle(LITERAL(0.0));
	V0.kappa = fTargetStation->getHorAngROMBeginIterator()->getV0();//Actuellement, il n y a toujours qu'un ROM par station
	return V0;
}


string	TOffsetToTheoPlaneObservation::getTheoV0Name() const
{// Return V0's identifier
	string name("Setup#");
	name += fTargetStation->getSetup();
	name += "_Serie#";
	name += fTargetStation->getHorAngROMBeginIterator()->getSeries();//Actuellement, il n y a toujours qu'un ROM par station
	return name;
}


TAMeasurement::ECalcStatus	TOffsetToTheoPlaneObservation::getTheoV0Status() const
{// Returns V0's status
	return fTargetStation->getHorAngROMBeginIterator()->getV0Status();//Actuellement, il n y a toujours qu'un ROM par station
}


TLength		TOffsetToTheoPlaneObservation::getConstantValue() const
{// Returns the constant distance 
	return fMeasConst->getValue();
}


TAMeasurement::ECalcStatus		TOffsetToTheoPlaneObservation::getConstantStatus() const
{// Returns the constant distance'status 
	return fMeasConst->getStatus();
}


string		TOffsetToTheoPlaneObservation::getConstantName() const
{// Returns the constant distance's identifier 
	return fMeasConst->getConstantName();
}


// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TOffsetToTheoPlaneObservation::get3DStationPosVec() const
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


TPositionVector	TOffsetToTheoPlaneObservation::get3DTargetStPosVec() const
{
	TPositionVector posVec(this->getTargetTheoPosition());
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(this->getTargetStationedPoint()->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}




////////////////////////////////////
//END
///////////////////////////////////
