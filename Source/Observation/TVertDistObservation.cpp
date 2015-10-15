//TVertDistObservation.cpp

#include "TVertDistObservation.h"

#include "TLevelStation.h"
#include "TVerticalDistROM.h"
#include "TRefSystemFactory.h"

//////////////////////////////////////////////////////////////
// Constructors and Destructors
//////////////////////////////////////////////////////////////

////////////////////////
// Default constructor
////////////////////////
TVertDistObservation::TVertDistObservation()
{
	fIsActive = false;
}

///////////////////////////
// Constructor
///////////////////////////
TVertDistObservation::TVertDistObservation(LevelStConstIter station, VertDistROMConstIter rom,
	VertDistMeasConstIter meas,PointConstIter ref, PointConstIter tf, DistConstantConstIter cte, int id) :
fStation(station), fROM(rom), fMeasurement(meas), fRefPoint(ref), fTargetPoint(tf), fMeasConst(cte)
{

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
TVertDistObservation::TVertDistObservation(const TVertDistObservation& source) {

	fStation = source.fStation;
	fROM = source.fROM;
	fMeasurement = source.fMeasurement;
	fRefPoint = source.fRefPoint;
	fTargetPoint = source.fTargetPoint;
	fMeasConst = source.fMeasConst;

	fIsActive = source.fIsActive;
	observationID = source.observationID;
}

//////////////////////////
// Destructor
/////////////////////////
TVertDistObservation::~TVertDistObservation()
{
}

/*
////////////////////////////////////////////////
// Copy assignement operator: not implemented 
////////////////////////////////////////////////
TVertDistObservation& TVertDistObservation::operator =(TVertDistObservation& right) {
}
*/


bool TVertDistObservation::operator==(const TVertDistObservation& right) const
{// Overloaded equality operator

	bool isEqualTo = false;
	if ((fStation == right.fStation) && (fTargetPoint == right.fTargetPoint))
		isEqualTo = true;

	return isEqualTo;
}

// Returns the horizontal distance value as a TLength from the measurement
TLength		TVertDistObservation::getVertDistValue() const {

	return fMeasurement->getVDistValue();

	/*if (fMeasConst->getStatus() == TAMeasurement::kFixed)
	{
		return ((fMeasurement->getVDistValue()) + (fMeasConst->getValue()));
	}
	else
	{
		TLength null;
		cout << "Error : Distance Constant for Vertical Distance not Fixed!";
		return null;
	}*/
}

// Returns the horizontal distance sigma as a TLength from the measurement 
TLength		TVertDistObservation::getSigmaValue() const {

	return getMeasurement()->getSigma();
}

//////////////////////////////////////////////////////
// Returns the position vector of the stationed point 
///////////////////////////////////////////////////////
TPositionVector	TVertDistObservation::getRefPosition() const {

	return fRefPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}

///////////////////////////////////////////////////////////
// Returns the spatial status of the stationed point 
//////////////////////////////////////////////////////
TSpatialStatus::ESpatialStatus		TVertDistObservation::getRefPosStatus() const {

	return fRefPoint->getPosition().getObjectStatus();
}

//////////////////////////////////////////////////////////
// Returns the concatenated name of the stationed point 
////////////////////////////////////////////////////////////
string	TVertDistObservation::getRefPosName() const {

	return fRefPoint->getName().getName();
}



TPositionVector		TVertDistObservation::getTargetPosition() const
{// Returns the position vector of the target point 

	return fTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}


TSpatialStatus::ESpatialStatus		TVertDistObservation::getTargetPosStatus() const
{// Returns the spatial status of the target point 

	return fTargetPoint->getPosition().getObjectStatus();
}


string		TVertDistObservation::getTargetPosName() const
{// Returns the concatenated name of the target point 

	return fTargetPoint->getName().getName();
}


// Returns the constant distance 
TLength		TVertDistObservation::getConstantValue() const {
	return fMeasConst->getValue();
}
// Returns the constant distance'status 
TAMeasurement::ECalcStatus		TVertDistObservation::getConstantStatus() const {
	return fMeasConst->getStatus();
}
// Returns the constant distance's identifier 
string		TVertDistObservation::getConstantName() const {
	return fMeasConst->getConstantName();
}


// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TVertDistObservation::get3DRefPosVec() const
{
	TPositionVector posVec(this->getRefPosition());
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(this->getReferencePoint()->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}


TPositionVector	TVertDistObservation::get3DTargetPosVec() const
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





