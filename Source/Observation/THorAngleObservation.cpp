// 
// THorAngleObservation.cpp : implementation file
//
// Class for a facade for an horizontal angle measurement
//

#include "TTheodoliteStation.h"
#include "THorAngleROM.h"
#include "TRefSystemFactory.h"

#include "THorAngleObservation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////
// default constructor
//////////////////////
THorAngleObservation::THorAngleObservation()
{
	fIsActive = false;
}


////////////////////////////////////////////////////////////////////////////////////////
// constructor taking 6 param (station, ROM, measurement, stationed and targeted points and angle constant)
////////////////////////////////////////////////////////////////////////////////////////
THorAngleObservation::THorAngleObservation(TheodStConstIter station, HorAngROMConstIter ROM,
										   HorAngMeasConstIter meas, PointConstIter stationedPt,
										   PointConstIter targetPt, AngleConstantConstIter cte, int id)
										   : fMeasConst(cte)
{
	// à adapter en fonction du code dans spatial measurement ou ailleurs
/*	if ((fStation->isActive()) && (fROM->isActive()) 
		&& (fMeasurement->isActive()))
		fIsActive = true;
	else 
		fIsActive = false;
*/
	fStation = station;
	fROM = ROM;
	fMeasurement = meas;
	fStationedPoint = stationedPt;
	fTargetPoint = targetPt;
	fIsActive = true; // en attendant...
	observationID = id;
}




/////////////
// Destructor
/////////////
THorAngleObservation::~THorAngleObservation()
{
}

///////////////////
// Copy Constructor
///////////////////
THorAngleObservation::THorAngleObservation(const THorAngleObservation &source)
{
	fStation = source.fStation; 
	fROM = source.fROM; 
	fMeasurement = source.fMeasurement; 
	fStationedPoint = source.fStationedPoint; 
	fTargetPoint = source.fTargetPoint;
	fMeasConst = source.fMeasConst;
	fIsActive = source.fIsActive;
	observationID = source.observationID;
}

////////////////////////////////////
// Overloaded equality operator
////////////////////////////////////
bool THorAngleObservation::operator==(const THorAngleObservation& right) const
{
	bool isEqualTo = false;
	if ((fStation == right.fStation) && (fTargetPoint == right.fTargetPoint))
		isEqualTo = true;

	return isEqualTo;
}

////////////////////////////
// Copy Assignement Operator
////////////////////////////
/*THorAngleObservation& THorAngleObservation::operator=(const THorAngleObservation &right)
{
	//not implemented
}*/
	
///////////////////////////////////////////////////////////
// Returns the horizontal angle value from the measurement
///////////////////////////////////////////////////////////
TAngle  THorAngleObservation::getHorAngleValue() const {

	return fMeasurement->getAngleValue();

	/*if (fMeasConst->getStatus() == TAMeasurement::kFixed)
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

//////////////////////////////////////////////////
// Returns the angle's sigma from the measurement
//////////////////////////////////////////////////
TAngle  THorAngleObservation::getSigmaValue() const {

	return getMeasurement()->getSigma();
}

////////////////////////////////
// Returns the V0 from the rom
////////////////////////////////
struct Angles	THorAngleObservation::getRomV0() const {

	struct Angles V0;
	V0.omega = TAngle(LITERAL(0.0));
	V0.phi = TAngle(LITERAL(0.0));
	V0.kappa = getRoundOfMeas()->getV0();

	return V0;
}

////////////////////////////////
// Return V0's identifier
////////////////////////////////
string	THorAngleObservation::getRomV0Name() const {

	string name("Setup#");
	name += fStation->getSetup();
	name += "_Serie#";
	name += fROM->getSeries();
	return name;
}

////////////////////////////////
// Returns V0's status
////////////////////////////////
TAMeasurement::ECalcStatus	THorAngleObservation::getRomV0Status() const {

	return getRoundOfMeas()->getV0Status();
}

//////////////////////////////////////////////////////
// Returns the position vector of the stationed point 
///////////////////////////////////////////////////////
TPositionVector		THorAngleObservation::getStationedPosition() const {

	return fStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}

///////////////////////////////////////////////////////////
// Returns the spatial status of the stationed point 
//////////////////////////////////////////////////////
TSpatialStatus::ESpatialStatus		THorAngleObservation::getStationedPosStatus() const {

	return fStationedPoint->getPosition().getObjectStatus();
}

//////////////////////////////////////////////////////////
// Returns the concatenated name of the stationed point 
////////////////////////////////////////////////////////////
string	THorAngleObservation::getStationedPosName() const {

	return fStationedPoint->getName().getName();
}

/////////////////////////////////////////////////////
// Returns the position vector of the target point 
/////////////////////////////////////////////////////
TPositionVector		THorAngleObservation::getTargetPosition() const {

	return fTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}

///////////////////////////////////////////////////
// Returns the spatial status of the target point 
///////////////////////////////////////////////////
TSpatialStatus::ESpatialStatus		THorAngleObservation::getTargetPosStatus() const {

	return fTargetPoint->getPosition().getObjectStatus();
}

//////////////////////////////////////////////////////
// Returns the concatenated name of the target point 
//////////////////////////////////////////////////////
string				THorAngleObservation::getTargetPosName() const {

	return fTargetPoint->getName().getName();
}


// Returns the constant angle 
TAngle		THorAngleObservation::getConstantValue() const {
	return fMeasConst->getValue();
}
// Returns the constant angle'status 
TAMeasurement::ECalcStatus		THorAngleObservation::getConstantStatus() const {
	return fMeasConst->getStatus();
}
// Returns the constant angle's identifier 
string		THorAngleObservation::getConstantName() const {
	return fMeasConst->getConstantName();
}



// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	THorAngleObservation::get3DStationPosVec() const
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


TPositionVector	THorAngleObservation::get3DTargetPosVec() const
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