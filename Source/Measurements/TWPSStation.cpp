// 
// TWPSStation.cpp : implementation File
//
// Class for a WPS instrument station

//#include "TWorkingStations.h"

#include "TWPSStation.h"

//////////////
// constructor
//////////////

TWPSStation::TWPSStation(TSpatialPointName setupPt) : TAInstrumentStation(setupPt),
fOmega(LITERAL(0.0)), fPhi(LITERAL(0.0)), fKappa(LITERAL(0.0)) {
}
/*
TWPSStation::TWPSStation(TWorkingStations* parent, TWorkingPoints::PointIterator iterSetupPt, TAngle omega, TAngle phi, TAngle kappa):
TAInstrumentStation(iterSetupPt)
{

	fOmega = omega;
	fPhi = phi;
	fKappa = kappa;

	TWPSStation* wpss = parent->addWPSStation(this);
	
	if (wpss != 0)
		iterSetupPt->addListener(static_cast<TVSpatialPtListener*>(wpss));
}
*/
/////////////
// destructor
/////////////
TWPSStation::~TWPSStation(){

}

///////////////////
// copy constructor
///////////////////
TWPSStation::TWPSStation(const TWPSStation& source):
TAInstrumentStation(source), fWPSMeas(source.fWPSMeas),
fOmega(source.getOmega()), fPhi(source.getPhi()), fKappa(source.getKappa()){

}

////////////////////////////
// copy assignement operator
////////////////////////////
TWPSStation& TWPSStation::operator=(const TWPSStation& source){

	fOmega = source.fOmega;
	fPhi = source.fPhi;
	fKappa = source.fKappa;
	fInstrumentNumber = source.fInstrumentNumber;
	fSetup = source.fSetup;
	fStationedPoint = source.fStationedPoint;
	fWPSMeas = source.fWPSMeas;

	return (*this);
}

///////////////////////////////
// overloaded equality operator
///////////////////////////////
bool TWPSStation::operator==(const TWPSStation& right) const{

		if ((fInstrumentNumber==right.fInstrumentNumber) &&
		(fStationedPoint==right.fStationedPoint) &&
		(fSetup==right.fSetup))
		return true;
	else return false;
}

///////////////////////////////////////////////
// returns a pointer to the associated WPS meas
///////////////////////////////////////////////
WPSMeasIterator TWPSStation::getWPSMeas() {

	return &fWPSMeas;
}

/////////////////////////////////////////////////////
// returns a const pointer to the associated WPS meas
/////////////////////////////////////////////////////
WPSMeasConstIter TWPSStation::getWPSMeas() const{

	return const_cast<TWPSMeasurement*>(&fWPSMeas);
}

///////////////////////////
// returns the omega angle
///////////////////////////
TAngle TWPSStation::getOmega() const{

	return fOmega;
}
/*
/////////////////////////
// returns omega's status
/////////////////////////
TUnknownAngle::EUnknownStatus TWPSStation::getOmegaStatus() const{

	return fOmegaStatus;
}
*/
///////////////////////////
// returns the Phi angle
///////////////////////////
TAngle TWPSStation::getPhi() const{

	return fPhi;
}
/*
/////////////////////////
// returns Phi's status
/////////////////////////
TUnknownAngle::EUnknownStatus TWPSStation::getPhiStatus() const{

	return fPhiStatus;
}
*/
///////////////////////////
// returns the Kappa angle
///////////////////////////
TAngle TWPSStation::getKappa() const{

	return fKappa;
}
/*
/////////////////////////
// returns Kappa's status
/////////////////////////
TUnknownAngle::EUnknownStatus TWPSStation::getKappaStatus() const{

	return fKappaStatus;
}

//////////////////////
// sets omega's status
//////////////////////
void TWPSStation::setOmegaStatus(TUnknownAngle::EUnknownStatus status){

	fOmegaStatus = status;
}

//////////////////////
// sets Phi's status
//////////////////////
void TWPSStation::setPhiStatus(TUnknownAngle::EUnknownStatus status){

	fPhiStatus = status;
}

//////////////////////
// sets Kappa's status
//////////////////////
void TWPSStation::setKappaStatus(TUnknownAngle::EUnknownStatus status){

	fKappaStatus = status;
}
*/




/////////////////////////////////
// sets the station's measurement
/////////////////////////////////
TWPSMeasurement* TWPSStation::setWPSMeas(TWPSMeasurement* wpsMeas){

	fWPSMeas = *wpsMeas;
	return &fWPSMeas;
}
/*
/////////////////////////////////////////////////////////
// tells the measurement that the station is disactivated
/////////////////////////////////////////////////////////
void TWPSStation::sendDisactivatedToChildren() {

	const TAObjectMeasurement* caom = &fWPSMeas;
	TAObjectMeasurement* aom = const_cast< TAObjectMeasurement* >(caom);
	aom->parentDisactivated();
}

/////////////////////////////////////////////////////////
// tells the ROM children that the station is Activated
/////////////////////////////////////////////////////////
void TWPSStation::sendActivatedToChildren() {

	const TAObjectMeasurement* caom = &fWPSMeas;
	TAObjectMeasurement* aom = const_cast< TAObjectMeasurement* >(caom);
	aom->parentActivated();
}


// temporary
void TWPSStation::sendEnoughTotalEqns() {

}

void TWPSStation::childReadyToActivate(int a, int b, int c){

}

void TWPSStation::activateReadyChildren() {

}

*/
