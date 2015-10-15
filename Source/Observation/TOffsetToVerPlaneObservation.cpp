// 
// TOffsetToVerPlaneObservation.cpp : implementation file
//
// Class for a facade for an ECHO measurement
//

#include "TRefSystemFactory.h"

#include "TOffsetToVerPlaneObservation.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TOffsetToVerPlaneObservation::TOffsetToVerPlaneObservation()
{//default constructor
	fIsActive = false;
}


TOffsetToVerPlaneObservation::TOffsetToVerPlaneObservation(DistStConstIter station, OffsetToVerPlaneMeasConstIter meas,
														 PointConstIter stationedPt,
														 PointConstIter firstTargetPt, PointConstIter secondTargetPt, DistConstantConstIter cte, int id)
{//constructor
	fStation= station;
	fMeasurement = meas;
	fStationedPoint = stationedPt;
	fFirstTargetPoint = firstTargetPt;
	fSecondTargetPoint = secondTargetPt;
	fMeasConst = cte;
	fIsActive = true; // en attendant.
	observationID = id;
}


TOffsetToVerPlaneObservation::TOffsetToVerPlaneObservation(const TOffsetToVerPlaneObservation &source)
{//Copy Constructor
	fStation= source.fStation;
	fMeasurement = source.fMeasurement;
	fStationedPoint = source.fStationedPoint;
	fFirstTargetPoint = source.fFirstTargetPoint;
	fSecondTargetPoint = source.fSecondTargetPoint;
	fMeasConst = source.fMeasConst;
	fIsActive = source.fIsActive;
	observationID = source.observationID;
}


TOffsetToVerPlaneObservation::~TOffsetToVerPlaneObservation()
{// Destructor
}


////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////
bool TOffsetToVerPlaneObservation::operator==(const TOffsetToVerPlaneObservation& right) const
{//Overloaded equality operator
	
	bool isEqualTo = false;

	if ((fStationedPoint == right.fStationedPoint) &&
		(fFirstTargetPoint == right.fFirstTargetPoint) && (fSecondTargetPoint == right.fSecondTargetPoint))
		isEqualTo = true;

	return isEqualTo;
}

	
TLength  TOffsetToVerPlaneObservation::getOffsetToVerPlaneValue() const
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


TLength  TOffsetToVerPlaneObservation::getSigmaValue() const
{//Returns the angle's sigma from the measurement

	return getMeasurement()->getSigma();
}


TPositionVector		TOffsetToVerPlaneObservation::getStationedPosition() const
{//Returns the position vector of the stationed point 

	return fStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToVerPlaneObservation::getStationedPosStatus() const
{//Returns the spatial status of the stationed point 

	return fStationedPoint->getPosition().getObjectStatus();
}


string	TOffsetToVerPlaneObservation::getStationedPosName() const
{//Returns the concatenated name of the stationed point 
	return fStationedPoint->getName().getName();
}


TPositionVector		TOffsetToVerPlaneObservation::getFirstTargetPosition() const
{//Returns the position vector of the point which define the target line 

	return fFirstTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToVerPlaneObservation::getFirstTargetPosStatus() const
{//Returns the spatial status of the target point 
	return fFirstTargetPoint->getPosition().getObjectStatus();
}


string				TOffsetToVerPlaneObservation::getFirstTargetPosName() const
{//Returns the concatenated name of the target point

	return fFirstTargetPoint->getName().getName();
}



TPositionVector		TOffsetToVerPlaneObservation::getSecondTargetPosition() const
{//Returns the position vector of the point which define the target line 

	return fSecondTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToVerPlaneObservation::getSecondTargetPosStatus() const
{//Returns the spatial status of the target point 
	return fSecondTargetPoint->getPosition().getObjectStatus();
}


string		TOffsetToVerPlaneObservation::getSecondTargetPosName() const
{//Returns the concatenated name of the target point

	return fSecondTargetPoint->getName().getName();
}


		
string		TOffsetToVerPlaneObservation::getTargetPlaneName() const
{/*! Returns the concatenated name of the target Plane*/
	return this->getMeasurement()->getTargetPlaneName().getName();
}


TLength		TOffsetToVerPlaneObservation::getConstantValue() const
{// Returns the constant distance 
	return fMeasConst->getValue();
}


TAMeasurement::ECalcStatus		TOffsetToVerPlaneObservation::getConstantStatus() const
{// Returns the constant distance'status 
	return fMeasConst->getStatus();
}


string		TOffsetToVerPlaneObservation::getConstantName() const
{// Returns the constant distance's identifier 
	return fMeasConst->getConstantName();
}



// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TOffsetToVerPlaneObservation::get3DStationPosVec() const
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


TPositionVector	TOffsetToVerPlaneObservation::get3DFirstTargetPosVec() const
{
	TPositionVector posVec(this->getFirstTargetPosition());
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(this->getFirstTargetPoint()->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}


TPositionVector	TOffsetToVerPlaneObservation::get3DSecondTargetPosVec() const
{
	TPositionVector posVec(this->getSecondTargetPosition());
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(this->getSecondTargetPoint()->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}




////////////////////////////////////
//END
///////////////////////////////////
