//TZenithDistObservation.cpp

#include "TZenithDistObservation.h"
#include "TTheodoliteStation.h"
#include "TZenithDistROM.h"
#include "TRefSystemFactory.h"

//////////////////////////////////////////////////////////////
// Constructors and Destructors
//////////////////////////////////////////////////////////////

////////////////////////
// Default constructor
////////////////////////
TZenithDistObservation::TZenithDistObservation()
{
	fIsActive = false;
}

///////////////////////////
// Constructor
///////////////////////////
TZenithDistObservation::TZenithDistObservation(TheodStConstIter station, ZenithDistROMConstIter rom,
						ZenithDistMeasConstIter meas, PointConstIter st, PointConstIter tg, int id):
fStation(station), fROM(rom), fMeasurement(meas), fStationedPoint(st), 
fTargetPoint(tg) {
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
TZenithDistObservation::TZenithDistObservation(const TZenithDistObservation& source) {

	fStation  = source.fStation;
	fROM = source.fROM; 
	fMeasurement = source.fMeasurement; 
	fStationedPoint = source.fStationedPoint; 
	fTargetPoint = source.fTargetPoint;
	fIsActive = source.fIsActive;
	observationID = source.observationID;
}

//////////////////////////
// Destructor
/////////////////////////
TZenithDistObservation::~TZenithDistObservation()
{
}

/*
////////////////////////////////////////////////
// Copy assignement operator: not implemented 
////////////////////////////////////////////////
TZenithDistObservation& TZenithDistObservation::operator =(TZenithDistObservation& right) {
}
*/


bool TZenithDistObservation::operator==(const TZenithDistObservation& right) const
{// Overloaded equality operator

	bool isEqualTo = false;

	if ((fStation == right.fStation) && (fTargetPoint == right.fTargetPoint))
		isEqualTo = true;
	
	return isEqualTo;
}

//////////////////////////////////////////////////////
// Access methods
//////////////////////////////////////////////////////

// Returns the angle value from the measurement
TAngle	TZenithDistObservation::getZenAngleValue() const {

	return getMeasurement()->getAngleValue();
}

// Returns the sigma as a TAngle from the measurement 
TAngle	TZenithDistObservation::getSigmaValue() const {

	return getMeasurement()->getSigma();
}

// Returns the prism height from the measurement 
TLength	 TZenithDistObservation::getPrismHeight() const{

	return getMeasurement()->getPrismeHeight();
}

// Returns the prism height status from measurement 
TAMeasurement::ECalcStatus	 TZenithDistObservation::getPrismHeightStatus() const {

	return getMeasurement()->getPrismeHeightStatus();
}

//Returns the prism height name
string	TZenithDistObservation::getPHeightName() const {

	string name("Id#");
	ostringstream oss;

	if (!getMeasurement()->isGeodeIdUsed())
	{name += "M";}
	
	oss << getMeasurement()->getId();
	name += oss.str();

	return name;
}

// Returns the instrument's height from the station 
TLength	TZenithDistObservation::getInstrumentHeight() const {

	return getStation()->getInstrumentHeight();
}

// Returns the instrument's status from station 
TAMeasurement::ECalcStatus	TZenithDistObservation::getInstHeightStatus() const {

	return getStation()->getInstHeightStatus();
}

//Returns the instrument height name
string	TZenithDistObservation::getIHeightName() const {

	string name("Setup#");
	name += fStation->getSetup();
	return name;
}

//////////////////////////////////////////////////////
// Returns the position vector of the stationed point 
///////////////////////////////////////////////////////
TPositionVector	TZenithDistObservation::getStationedPosition() const {

	return fStationedPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}

///////////////////////////////////////////////////////////
// Returns the spatial status of the stationed point 
//////////////////////////////////////////////////////
TSpatialStatus::ESpatialStatus		TZenithDistObservation::getStationedPosStatus() const {

	return fStationedPoint->getPosition().getObjectStatus();
}

//////////////////////////////////////////////////////////
// Returns the concatenated name of the stationed point 
////////////////////////////////////////////////////////////
string	TZenithDistObservation::getStationedPosName() const {

	return fStationedPoint->getName().getName();
}

/////////////////////////////////////////////////////
// Returns the position vector of the target point 
/////////////////////////////////////////////////////
TPositionVector		TZenithDistObservation::getTargetPosition() const {

	return fTargetPoint->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
}

///////////////////////////////////////////////////
// Returns the spatial status of the target point 
///////////////////////////////////////////////////
TSpatialStatus::ESpatialStatus		TZenithDistObservation::getTargetPosStatus() const {

	return fTargetPoint->getPosition().getObjectStatus();
}

//////////////////////////////////////////////////////
// Returns the concatenated name of the target point 
//////////////////////////////////////////////////////
string		TZenithDistObservation::getTargetPosName() const {

	return fTargetPoint->getName().getName();
}



// Converts from 2D+H to 3D cartesian coordinate system, if necessary
TPositionVector	TZenithDistObservation::get3DStationPosVec() const
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


TPositionVector	TZenithDistObservation::get3DTargetPosVec() const
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

