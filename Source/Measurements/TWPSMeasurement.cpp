// TWPSMeasurement.cpp : implementation file
// Class for a 2D WPS measurement
// 

#include "TWPSStation.h"
//#include "TWorkingStations.h"

#include "TWPSMeasurement.h"

//////////////////////
// default constructor
//////////////////////
TWPSMeasurement::TWPSMeasurement():
fTgWireName(""), fSetupPtName(""), fTransOffset(), fTransSigma(), 
fVertOffset(), fVertSigma()
{

}

////////////////////////////////////////////////////////////////////////////////
// constructor setting the point names and the measurements and their precisions
////////////////////////////////////////////////////////////////////////////////
TWPSMeasurement::TWPSMeasurement(string wireName, string setupName,
								 TLength obsTOffset, TLength tSigma, TLength obsVOffset, 
								 TLength vSigma ):
fSetupPtName(setupName), fTgWireName(wireName), fTransOffset(obsTOffset), fTransSigma(tSigma), 
fVertOffset(obsVOffset), fVertSigma(vSigma){

}

/*
//////////////
// constructor
//////////////
TWPSMeasurement::TWPSMeasurement(TWPSStation& parent, TWorkingTargetObjects::WireIterator iterW, TLength vertMeas, TLength vertSigma, TLength transMeas, TLength transSigma):
TAObjectMeasurement(&parent), fParentStation(&parent), fTargetWire(iterW),
fVertOffset(vertMeas), fVertSigma(vertSigma),
fTransOffset(transMeas), fTransSigma(transSigma),
fTUsedInCalc(true), fVUsedInCalc(true)
{

	TWPSMeasurement* wpsm = fParentStation->setWPSMeas(this);
	fTargetWire->addListener(static_cast<TVMeasurableEntityLstnr*>(wpsm));

}
*/


/////////////
// destructor
/////////////
TWPSMeasurement::~TWPSMeasurement(){

}



////////////////////////////
// copy assignement operator
////////////////////////////
TWPSMeasurement& TWPSMeasurement::operator=(const TWPSMeasurement& source){

	if (this!=&source)
	{
		fTgWireName = source.getTgWireName();
		fSetupPtName = source.fSetupPtName;
		fVertOffset = source.getVertOffset();
		fVertSigma = source.getVertSigma();
		fTransOffset = source.getTransOffset();
		fTransSigma = source.getTransSigma();
	}

	return (*this);
}


/////////////
// < operator
/////////////
bool TWPSMeasurement::operator<(const TWPSMeasurement& right) const{

	if (fTgWireName < right.fTgWireName)
		return true;
	else 
		return false;
}

//////////////
// == operator
//////////////
bool TWPSMeasurement::operator==(const TWPSMeasurement& right) const{

	if (fTgWireName == right.fTgWireName)
		return true;
	else
		return false;
}
/*
////////////////////////////////////////////////
// tells if the transverse offset meas is active
////////////////////////////////////////////////
bool TWPSMeasurement::tActive() const{

	return fTUsedInCalc;
}

//////////////////////////////////////////////
// tells if the vertical offset meas is active
//////////////////////////////////////////////
bool TWPSMeasurement::vActive() const{

	return fVUsedInCalc;
}


//////////////////////////////////
// activates the transverse offset
//////////////////////////////////
void TWPSMeasurement::activateTOffset(){

	fTUsedInCalc = true;
}

/////////////////////////////////////
// disactivates the transverse offset
/////////////////////////////////////
void TWPSMeasurement::disactivateTOffset(){

	fTUsedInCalc = false;
}

//////////////////////////////////
// activates the vertical offset
//////////////////////////////////
void TWPSMeasurement::activateVOffset(){

	fVUsedInCalc = true;
}

/////////////////////////////////////
// disactivates the vertical offset
/////////////////////////////////////
void TWPSMeasurement::disactivateVOffset(){

	fVUsedInCalc = false;
}

*/


/*
//////////////////////////////////////////
// returns an interator to the target wire
//////////////////////////////////////////
TWorkingTargetObjects::WireIterator TWPSMeasurement::getTargetWire() const{

	return fTargetWire;
}
*/

///////////////////////////////////////
// returns the measured vertical offset
///////////////////////////////////////
TLength TWPSMeasurement::getVertOffset() const{

	return fVertOffset;
}

///////////////////////////////////////////////////
// returns the measured vertical offset's precision
///////////////////////////////////////////////////
TLength TWPSMeasurement::getVertSigma() const{

	return fVertSigma;
}

////////////////////////////////////////////////
// sets the vertical sigma if it is still zero
////////////////////////////////////////////////
void TWPSMeasurement::ifNotDoneSetVSigma(TLength sigma){

	if (fVertSigma.getMetresValue() == LITERAL(0.0))
		fVertSigma = sigma;
}


/////////////////////////////////////////
// returns the measured transverse offset
/////////////////////////////////////////
TLength TWPSMeasurement::getTransOffset() const{

	return fTransOffset;
}

/////////////////////////////////////////////////////
// returns the measured transverse offset's precision
/////////////////////////////////////////////////////
TLength TWPSMeasurement::getTransSigma() const{

	return fTransSigma;
}

////////////////////////////////////////////////
// sets the transverse sigma if it is still zero
////////////////////////////////////////////////
void TWPSMeasurement::ifNotDoneSetTSigma(TLength sigma){

	if (fTransSigma.getMetresValue() == LITERAL(0.0))
		fTransSigma = sigma;
}


///////////////////////////////////////////////////////
// returns the string representing the measurement kind
///////////////////////////////////////////////////////
string TWPSMeasurement::getMeasKind() const{

	string s("WPS2D");
	return s;
}

////////////////////////////////////////////////////
// returns the target wire's name
/////////////////////////////////////
string TWPSMeasurement::getTgWireName() const {

	return fTgWireName;
}

//////////////////////////////////////
// sets the taret wire's name
///////////////////////////////
void TWPSMeasurement::setTgWireName(string twn) {

	fTgWireName = twn;
}


/////////////////////////////////
// returns the setup point's name
/////////////////////////////////
TSpatialPointName TWPSMeasurement::getSetupPointName() const {

	return fSetupPtName;
}


/*
///////////////////////////////////////////////////
// tells the parent ROM the measure is disactivated
///////////////////////////////////////////////////
void TWPSMeasurement::sendDisactivatedToParent() {

	fParentStation->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

//////////////////////////////////////////////////
// tells the parent ROM the measure is Activated
//////////////////////////////////////////////////
void TWPSMeasurement::sendActivatedToParent() {

	fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}


/////////////////////////////////////////////////////
// sets the target wire using the two end point names
/////////////////////////////////////////////////////
bool TWPSMeasurement::setTargetWire(TWorkingPoints* wp, TWorkingTargetObjects* wto){

	//check if the two end points exist
	TWorkingPoints::PointIterator noSuchPoint = wp->getPointsEndIterator();

	TWorkingPoints::PointIterator iterPt1 = wp->getPoint(fWireEnd1Name);
	TWorkingPoints::PointIterator iterPt2 = wp->getPoint(fWireEnd2Name);

	if ((iterPt1 == noSuchPoint) || (iterPt1 == noSuchPoint))
		return false;

	///get the wire or create it
	else{

		TWorkingTargetObjects::WireIterator iterW = wto->getWire(iterPt1, iterPt2);
		TWorkingTargetObjects::WireIterator noSuchWire = wto->getWireEndIterator();

		//case where the wire already exists
		if (iterW != noSuchWire){
			fTargetWire = iterW;
			return true;
		}
		//case where the wire has to be created
		else{
			TWire* newWire = new TWire(iterPt1, iterPt2, "");
			wto->addWire(newWire);
			delete newWire;

			fTargetWire = wto->getWire(iterPt1, iterPt2);
			if (fTargetWire != noSuchWire)
				return true;
			else
				return false;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Creates the relevant WPS station, using the latter's setup point's name
//////////////////////////////////////////////////////////////////////////
bool TWPSMeasurement::addToStation(TWorkingPoints* wp, TWorkingStations* ws){

	//check if the station's setup point exists
	TWorkingPoints::PointIterator noSuchPoint = wp->getPointsEndIterator();
	TWorkingPoints::PointIterator iterStPt = wp->getPoint(fSetupPtName);

	if (iterStPt == noSuchPoint)
		return false;
	else{

		//each WPS measurement has its own station, so a station is created each time
		TAngle zero(LITERAL(0.0));
		TWPSStation* newWPSSt = new TWPSStation(ws, iterStPt, zero, zero, zero);
		delete newWPSSt;

		TWorkingStations::WPSStIterator iterWPSSt = --(ws->getWPSStEndIterator());
		TWPSMeasurement* wpsm = iterWPSSt->setWPSMeas(this);
		fTargetWire->addListener(static_cast<TVMeasurableEntityLstnr*>(wpsm));

		return true;
	}
}
*/




