// 
// TOffsetToSpaLineObservation.cpp : implementation file
//
// Class for a facade for an ECSP measurement
//

#include "TRefSystemFactory.h"
#include "TOffsetToSpaLineObservation.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TOffsetToSpaLineObservation::TOffsetToSpaLineObservation()
{//default constructor
	fIsActive = false;
}


TOffsetToSpaLineObservation::TOffsetToSpaLineObservation(DistStConstIter station, OffsetToSpaLineMeasConstIter meas,
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


TOffsetToSpaLineObservation::TOffsetToSpaLineObservation(const TOffsetToSpaLineObservation &source)
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


TOffsetToSpaLineObservation::~TOffsetToSpaLineObservation()
{// Destructor
}


////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////
bool TOffsetToSpaLineObservation::operator==(const TOffsetToSpaLineObservation& right) const
{//Overloaded equality operator
	
	bool isEqualTo = false;

	if ((fStationedPoint == right.fStationedPoint) &&
		(fFirstTargetPoint == right.fFirstTargetPoint) && (fSecondTargetPoint == right.fSecondTargetPoint))
		isEqualTo = true;

	return isEqualTo;
}

	
TLength  TOffsetToSpaLineObservation::getOffsetToSpaLineValue() const
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


TLength  TOffsetToSpaLineObservation::getSigmaValue() const
{//Returns the angle's sigma from the measurement

	return getMeasurement()->getSigma();
}


TPositionVector		TOffsetToSpaLineObservation::getStationedPosition() const
{//Returns the position vector of the stationed point 

	return fStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToSpaLineObservation::getStationedPosStatus() const
{//Returns the spatial status of the stationed point 

	return fStationedPoint->getPosition().getObjectStatus();
}


string	TOffsetToSpaLineObservation::getStationedPosName() const
{//Returns the concatenated name of the stationed point 
	return fStationedPoint->getName().getName();
}


TPositionVector		TOffsetToSpaLineObservation::getFirstTargetPosition() const
{//Returns the position vector of the point which define the target line 

	return fFirstTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToSpaLineObservation::getFirstTargetPosStatus() const
{//Returns the spatial status of the target point 
	return fFirstTargetPoint->getPosition().getObjectStatus();
}


string				TOffsetToSpaLineObservation::getFirstTargetPosName() const
{//Returns the concatenated name of the target point

	return fFirstTargetPoint->getName().getName();
}



TPositionVector		TOffsetToSpaLineObservation::getSecondTargetPosition() const
{//Returns the position vector of the point which define the target line 

	return fSecondTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TOffsetToSpaLineObservation::getSecondTargetPosStatus() const
{//Returns the spatial status of the target point 
	return fSecondTargetPoint->getPosition().getObjectStatus();
}


string		TOffsetToSpaLineObservation::getSecondTargetPosName() const
{//Returns the concatenated name of the target point

	return fSecondTargetPoint->getName().getName();
}


string		TOffsetToSpaLineObservation::getTargetLineName() const
{/*! Returns the concatenated name of the target Plane*/
	return this->getMeasurement()->getTargetLineName().getName();
}


TLength		TOffsetToSpaLineObservation::getConstantValue() const
{// Returns the constant distance 
	return fMeasConst->getValue();
}


TAMeasurement::ECalcStatus		TOffsetToSpaLineObservation::getConstantStatus() const
{// Returns the constant distance'status 
	return fMeasConst->getStatus();
}


string		TOffsetToSpaLineObservation::getConstantName() const
{// Returns the constant distance's identifier 
	return fMeasConst->getConstantName();
}


// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TOffsetToSpaLineObservation::get3DStationPosVec() const
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


TPositionVector	TOffsetToSpaLineObservation::get3DFirstTargetPosVec() const
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


TPositionVector	TOffsetToSpaLineObservation::get3DSecondTargetPosVec() const
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