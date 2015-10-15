//TSpatialDistObservation.cpp

#include "TSpatialDistObservation.h"
#include "TDistMeasStation.h"
#include "TSpatialDistROM.h"
#include "TRefSystemFactory.h"

//////////////////////////////////////////////////////////////
// Constructors and Destructors
//////////////////////////////////////////////////////////////

////////////////////////
// Default constructor
////////////////////////
TSpatialDistObservation::TSpatialDistObservation()
{
	fIsActive = false;
}


//////////////////////////////////////////////////////////
// Constructor taking an iterator to the distance constant
//////////////////////////////////////////////////////////
TSpatialDistObservation::TSpatialDistObservation(DistStConstIter station, 
											   SpatialDistROMConstIter rom, 
											   SpatialDistMeasConstIter meas,
											   PointConstIter st, 
											   PointConstIter tg,
											   DistConstantConstIter cte, int id) :
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
TSpatialDistObservation::TSpatialDistObservation(const TSpatialDistObservation& source) {

	fStation = source.fStation;
	fROM = source.fROM;
	fMeasurement = source.fMeasurement;
	fStationedPoint = source.fStationedPoint;
	fTargetPoint = source.fTargetPoint;
	fMeasConst = source.fMeasConst;
	fTheodStation = source.fTheodStation;

	fIsActive = source.fIsActive;
	observationID = source.observationID;
}

//////////////////////////
// Destructor
/////////////////////////
TSpatialDistObservation::~TSpatialDistObservation()
{
}

/*
////////////////////////////////////////////////
// Copy assignement operator: not implemented 
////////////////////////////////////////////////
TSpatialDistObservation& TSpatialDistObservation::operator =(TSpatialDistObservation& right) {
}
*/


bool TSpatialDistObservation::operator ==(const TSpatialDistObservation& right) const
{// Overloaded equality operator

	bool isEqualTo = false;
	if ((fStation == right.fStation) && (fTargetPoint == right.fTargetPoint))
		isEqualTo = true;

	return isEqualTo;
}

// Returns the spatial distance value as a TLength from the measurement
TLength		TSpatialDistObservation::getSpatialDistValue() const {

	return getMeasurement()->getDistValue();
}


TLength		TSpatialDistObservation::getSigmaValue() const
{// Returns the spatial distance sigma as a TLength from the measurement 
	return getMeasurement()->getSigma();
}


TLength		TSpatialDistObservation::getSigmaPpmValue() const
{// Returns the spatial distance sigma PPM as a TLength from the measurement 
	return getMeasurement()->getSigmaPpm();
}


// Returns the prism height from the measurement 
TLength	 TSpatialDistObservation::getPrismHeight() const{

	return getMeasurement()->getPrismHeight();
}

// Returns the prism height status from measurement 
TAMeasurement::ECalcStatus	 TSpatialDistObservation::getPrismHeightStatus() const {

	return getMeasurement()->getPrismHeightStatus();
}

//Returns the prism height name
string	TSpatialDistObservation::getPHeightName() const {

	string name("Id#");
	ostringstream oss;

	if (!getMeasurement()->isGeodeIdUsed())
	{name += "M";}
	
	oss << getMeasurement()->getId();
	name += oss.str();

	return name;
}

// Returns the instrument's height from the station 
TLength	TSpatialDistObservation::getInstrumentHeight() const {

	if (fStation->getInstHeightStatus() == TAMeasurement::kFixed)
	{return fStation->getInstrumentHeight();}
	else
	{return fTheodStation->getInstrumentHeight();}
}

// Returns the instrument's status from station 
TAMeasurement::ECalcStatus	TSpatialDistObservation::getInstHeightStatus() const {

	return getStation()->getInstHeightStatus();
}

//Returns the instrument height name
string	TSpatialDistObservation::getIHeightName() const {

	string name("Setup#");
	
	if (fStation->getInstHeightStatus() == TAMeasurement::kFixed)
	{name += fStation->getSetup();}
	else
	{ name += fTheodStation->getSetup();}
	
	return name;
}


// Returns the constant distance 
TLength		TSpatialDistObservation::getConstantValue() const {
	return fMeasConst->getValue();
}
// Returns the constant distance'status 
TAMeasurement::ECalcStatus		TSpatialDistObservation::getConstantStatus() const {
	return fMeasConst->getStatus();
}
// Returns the constant distance's identifier 
string		TSpatialDistObservation::getConstantName() const {
	return fMeasConst->getConstantName();
}


//////////////////////////////////////////////////////
// Returns the position vector of the stationed point 
///////////////////////////////////////////////////////
TPositionVector	TSpatialDistObservation::getStationedPosition() const {

	return fStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}

///////////////////////////////////////////////////////////
// Returns the spatial status of the stationed point 
//////////////////////////////////////////////////////
TSpatialStatus::ESpatialStatus		TSpatialDistObservation::getStationedPosStatus() const {

	return fStationedPoint->getPosition().getObjectStatus();
}

//////////////////////////////////////////////////////////
// Returns the concatenated name of the stationed point 
////////////////////////////////////////////////////////////
string	TSpatialDistObservation::getStationedPosName() const {

	return fStationedPoint->getName().getName();
}

/////////////////////////////////////////////////////
// Returns the position vector of the target point 
/////////////////////////////////////////////////////
TPositionVector		TSpatialDistObservation::getTargetPosition() const {

	return fTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}

///////////////////////////////////////////////////
// Returns the spatial status of the target point 
///////////////////////////////////////////////////
TSpatialStatus::ESpatialStatus		TSpatialDistObservation::getTargetPosStatus() const {

	return fTargetPoint->getPosition().getObjectStatus();
}

//////////////////////////////////////////////////////
// Returns the concatenated name of the target point 
//////////////////////////////////////////////////////
string		TSpatialDistObservation::getTargetPosName() const {

	return fTargetPoint->getName().getName();
}


////////////////////////////////////////////////////
// set the corresponding theodolite station iterator
////////////////////////////////////////////////////
void TSpatialDistObservation::setTheodStation(TheodStConstIter iter)
{
	fTheodStation = iter;
}




// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TSpatialDistObservation::get3DStationPosVec() const
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


TPositionVector	TSpatialDistObservation::get3DTargetPosVec() const
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




