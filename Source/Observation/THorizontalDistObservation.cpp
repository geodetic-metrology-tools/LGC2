//THorizontalDistObservation.cpp

#include "THorizontalDistObservation.h"

#include "TDistMeasStation.h"
#include "THorizontalDistROM.h"
#include "THorizontalDistMeas.h"
#include "TSpatialPoint.h"
#include "TRefSystemFactory.h"

//////////////////////////////////////////////////////////////
// Constructors and Destructors
//////////////////////////////////////////////////////////////

////////////////////////
// Default constructor
////////////////////////
THorizontalDistObservation::THorizontalDistObservation()
{
	fIsActive = false;
}

///////////////////////////////////////////////////////
// Constructor taking the distance measurement constant
///////////////////////////////////////////////////////
THorizontalDistObservation::THorizontalDistObservation(DistStConstIter station, 
											   HorDistROMConstIter rom, HorDistMeasConstIter meas,
											   PointConstIter st, PointConstIter tg, DistConstantConstIter cte,
											   int id) :
fStation(station), fROM(rom), fMeasurement(meas), fStationedPoint(st), 
fTargetPoint(tg), fMeasConst(cte) {
	// à adapter en fonction du code dans spatial measurement ou ailleurs
/*	if ((fStation->isActive()) && (fROM->isActive()) 
		&& (fMeasurement->isActive()))
		fIsActive = true;
	else 
		fIsActive = false;
*/
	fIsActive = true; // en attendant...
	observationID = id;
}

//////////////////////////////////////////////////
// Copy constructor
//////////////////////////////////////////////////
THorizontalDistObservation::THorizontalDistObservation(const THorizontalDistObservation& source) {

	fStation = source.fStation;
	fROM = source.fROM;
	fMeasurement = source.fMeasurement;
	fStationedPoint = source.fStationedPoint;
	fTargetPoint = source.fTargetPoint;
	fMeasConst = source.fMeasConst;

	fIsActive = source.fIsActive;
	observationID = source.observationID;
}

//////////////////////////
// Destructor
/////////////////////////
THorizontalDistObservation::~THorizontalDistObservation()
{
}

/*
////////////////////////////////////////////////
// Copy assignement operator: not implemented 
////////////////////////////////////////////////
THorizontalDistObservation& THorizontalDistObservation::operator =(THorizontalDistObservation& right) {
}
*/


bool THorizontalDistObservation::operator ==(const THorizontalDistObservation& right) const
{// Overloaded equality operator

	bool isEqualTo = false;
	if ((fStation == right.fStation) && (fTargetPoint == right.fTargetPoint))
		isEqualTo = true;

	return isEqualTo;
}

// Returns the horizontal distance value as a TLength from the measurement
TLength		THorizontalDistObservation::getHorDistValue() const {

	return fMeasurement->getDistValue();

	/*if (fMeasConst->getStatus() == TAMeasurement::kFixed)
	{
		return ((fMeasurement->getDistValue()) + (fMeasConst->getValue()));
	}
	else
	{
		TLength null;
		cout << "Error : Distance Constant for Horizontal Distance not Fixed!";
		return null;
	}*/
}

 
TLength		THorizontalDistObservation::getSigmaValue() const
{// Returns the horizontal distance sigma as a TLength from the measurement
	return getMeasurement()->getSigma();
}


TLength		THorizontalDistObservation::getSigmaPpmValue() const
{// Returns the horizontal distance sigma PPM as a TLength from the measurement
	return getMeasurement()->getSigmaPpm();
}


//////////////////////////////////////////////////////
// Returns the position vector of the stationed point 
///////////////////////////////////////////////////////
TPositionVector	THorizontalDistObservation::getStationedPosition() const {

	return fStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}

///////////////////////////////////////////////////////////
// Returns the spatial status of the stationed point 
//////////////////////////////////////////////////////
TSpatialStatus::ESpatialStatus		THorizontalDistObservation::getStationedPosStatus() const {

	return fStationedPoint->getPosition().getObjectStatus();
}

//////////////////////////////////////////////////////////
// Returns the concatenated name of the stationed point 
////////////////////////////////////////////////////////////
string	THorizontalDistObservation::getStationedPosName() const {

	return fStationedPoint->getName().getName();
}

/////////////////////////////////////////////////////
// Returns the position vector of the target point 
/////////////////////////////////////////////////////
TPositionVector		THorizontalDistObservation::getTargetPosition() const {

	return fTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}

///////////////////////////////////////////////////
// Returns the spatial status of the target point 
///////////////////////////////////////////////////
TSpatialStatus::ESpatialStatus		THorizontalDistObservation::getTargetPosStatus() const {

	return fTargetPoint->getPosition().getObjectStatus();
}

//////////////////////////////////////////////////////
// Returns the concatenated name of the target point 
//////////////////////////////////////////////////////
string		THorizontalDistObservation::getTargetPosName() const {

	return fTargetPoint->getName().getName();
}




// Returns the constant distance 
TLength		THorizontalDistObservation::getConstantValue() const {
	return fMeasConst->getValue();
}
// Returns the constant distance'status 
TAMeasurement::ECalcStatus		THorizontalDistObservation::getConstantStatus() const {
	return fMeasConst->getStatus();
}
// Returns the constant distance's identifier 
string		THorizontalDistObservation::getConstantName() const {
	return fMeasConst->getConstantName();
}



// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	THorizontalDistObservation::get3DStationPosVec() const
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


TPositionVector	THorizontalDistObservation::get3DTargetPosVec() const
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

