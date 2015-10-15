//
// TAFreeInstrumentStation.cpp : implementation file
// abstract base class holding what the behaviour common to all
// instrument stations without setup point

#include "TAFreeInstrumentStation.h"

//////////////////////
// default constructor
//////////////////////
TAFreeInstrumentStation::TAFreeInstrumentStation(){

	//fUsedInCalc = false;
/*	fUsedInCalc = true;
	fActiveMeasCount = 0;
	fActiveEqCount = 0;
	fActiveUnkCount = 0;

	fTotalEqnNumber = 0;
	fReadyToActivateEqns = 0;
*/
	fSetup = "";
	fInstrumentNumber = "";
}

///////////////////
// copy constructor
///////////////////
TAFreeInstrumentStation::TAFreeInstrumentStation(const TAFreeInstrumentStation& source) :
/*fUsedInCalc(source.fUsedInCalc), fActiveMeasCount(source.fActiveMeasCount),
fActiveEqCount(source.fActiveEqCount), fActiveUnkCount(source.fActiveUnkCount),
fReadyToActivateEqns(source.fReadyToActivateEqns), fTotalEqnNumber(source.fTotalEqnNumber),*/
fSetup(source.fSetup), fInstrumentNumber(source.fInstrumentNumber)
{

}

	

/////////////
// destructor
/////////////
TAFreeInstrumentStation::~TAFreeInstrumentStation(){

}
//////////////////////////////
// returns the station's setup
//////////////////////////////
string TAFreeInstrumentStation::getSetup() const{

	return fSetup;
}

///////////////////////////
// sets the station's setup
///////////////////////////
void TAFreeInstrumentStation::setSetup(string setup){

	fSetup = setup;
}

//////////////////////////////
// returns instrument number
//////////////////////////////
InstNb TAFreeInstrumentStation::getInstrumentNumber() const {

	return fInstrumentNumber;
}

/////////////
// < operator
/////////////
bool TAFreeInstrumentStation::operator<(const TAFreeInstrumentStation& right) const{

	if (fInstrumentNumber < (right.fInstrumentNumber))
		return true;
	else {
		if (fInstrumentNumber==right.fInstrumentNumber){
			if (fSetup<right.fSetup)
				return true;
			else
				return false;
		}
		else
			return false;
	}
}

//////////////
// == operator
//////////////
bool TAFreeInstrumentStation::operator==(const TAFreeInstrumentStation& right) const{

	if ((fInstrumentNumber==right.fInstrumentNumber)/* &&
		(fSetup==right.fSetup)*/)
		return true;
	else return false;
}

/*
///////////////////////////////////////
// returns the station's equation count
///////////////////////////////////////
int TAFreeInstrumentStation::equationsCount() const{

	return fActiveEqCount;
}

//////////////////////////////////////////
// returns the station's measurement count
//////////////////////////////////////////
int TAFreeInstrumentStation::measurementsCount() const{

	return fActiveMeasCount;
}

///////////////////////////////////////
// returns the station's unknowns count
///////////////////////////////////////
int TAFreeInstrumentStation::unknownsCount() const{

	return fActiveUnkCount;
}

/////////////////////////////////////////////////////////////////////////////////
// tells if the station has enough  (active or inactive) equations for its setup point to be determined
/////////////////////////////////////////////////////////////////////////////////
bool TAFreeInstrumentStation::enoughTotalEqns() const{

//	if (fTotalEqnNumber >= fStationedPoint->getVariableDimension())
		return true;
//	else
//		return false;
}

///////////////////////////////////////////////////////////////////////////
// updates the stations counts (used when a meas is inserted in a child ROM
///////////////////////////////////////////////////////////////////////////
void TAFreeInstrumentStation::updateCounts(int measUpd, int eqUpd, int unkUpd){

	fActiveMeasCount += measUpd;
	fActiveEqCount += eqUpd;
	fActiveUnkCount += unkUpd;
}

/////////////////////////////////////////////////////////////
// updates the total number of (active or inactive) equations
/////////////////////////////////////////////////////////////
void TAFreeInstrumentStation::updateTotalEqCount(int totECUpdt){

	fTotalEqnNumber += totECUpdt;

	if ((fTotalEqnNumber >= fStationedPoint->getVariableDimension()) &&
		((fTotalEqnNumber-totECUpdt) < fStationedPoint->getVariableDimension()))
		sendEnoughTotalEqns();

}


/////////////////////////////////////
// returns the active/inactive status
/////////////////////////////////////
bool TAFreeInstrumentStation::isActive() const{

	return fUsedInCalc;
}

///////////////////////////////////
// adds a listener to the container
///////////////////////////////////
void TAFreeInstrumentStation::addListener(TVStationListener* stListnr){

	fStListeners.push_back(stListnr);
}

///////////////////////////
// disactivates the station
///////////////////////////
void TAFreeInstrumentStation::disactivate(){

	//no reaction if !fUsedInCalc

	if (fUsedInCalc){

		fUsedInCalc = false;

		sendDisactivatedToChildren();

		StListnrIterator iter = fStListeners.begin();
		StListnrIterator iterEnd = fStListeners.end();

		while (iter != iterEnd){
			(*iter)->stationDisactivated();
			iter++;
		}
	}
}

//////////////////////////
// reactivates the station
//////////////////////////
void TAFreeInstrumentStation::activate(){

	//no reaction if fUsedInCalc

	if (!fUsedInCalc){
		//no reaction if fActiveMeasCount==0
		if (fActiveMeasCount != 0){
			fUsedInCalc = true;
			sendActivatedToChildren();

			StListnrIterator iter = fStListeners.begin();
			StListnrIterator iterEnd = fStListeners.end();

			while (iter != iterEnd){
				(*iter)->stationActivated();
				iter++;
			}
		}
	}
}

///////////////////////////////////////////
// response to the disactivation of a child
///////////////////////////////////////////
void TAFreeInstrumentStation::childDisactivated(int measCount, int eqCount, int unkCount){


	//No message sent to the setup pt nor to the children
	if (!fUsedInCalc){
		fActiveMeasCount -= measCount;
		fActiveEqCount -= eqCount;
		fActiveUnkCount -= unkCount;

		StListnrIterator iter = fStListeners.begin();
		StListnrIterator iterEnd = fStListeners.end();
		while (iter != iterEnd){
			(*iter)->stationUpdated(TVStationListener::kCountsChange);
			iter++;
		}
	}

	if (fUsedInCalc){
		fActiveMeasCount -= measCount;
		fActiveEqCount -= eqCount;
		fActiveUnkCount -= unkCount;
		StListnrIterator iter = fStListeners.begin();
		StListnrIterator iterEnd = fStListeners.end();
		while (iter != iterEnd){
			(*iter)->stationUpdated(TVStationListener::kCountsChange);
			iter++;
		}

	}

	//What happens when MeasCount goes to zero
	if (fActiveMeasCount == 0){
		//No reaction if !fUIC
		if (fUsedInCalc){
			fUsedInCalc = false;
			//Only to update the OwnerStatus of the acquisitions
			sendDisactivatedToChildren();
			StListnrIterator iter = fStListeners.begin();
			StListnrIterator iterEnd = fStListeners.end();
			while (iter != iterEnd){
				(*iter)->stationDisactivated();
				iter++;
			}
		}
	}
}

//////////////////////////////////////////
// responds to the reactivation of a child
//////////////////////////////////////////
void TAFreeInstrumentStation::childActivated(int measCount, int eqCount, int unkCount){

	bool fWasUsedInCalc = fUsedInCalc;

	if (!fWasUsedInCalc){

		if (fActiveMeasCount != 0){
			fActiveMeasCount += measCount;
			fActiveEqCount += eqCount;
			fActiveUnkCount += unkCount;

			StListnrIterator iter = fStListeners.begin();
			StListnrIterator iterEnd = fStListeners.end();
			while (iter != iterEnd){
				(*iter)->stationUpdated(TVStationListener::kCountsChange);
				iter++;
			}
		}


		if (fActiveMeasCount == 0){
			fUsedInCalc = true;
			fActiveMeasCount += measCount;
			fActiveEqCount += eqCount;
			fActiveUnkCount += unkCount;

			sendActivatedToChildren();

			StListnrIterator iter = fStListeners.begin();
			StListnrIterator iterEnd = fStListeners.end();
			while (iter != iterEnd){
				(*iter)->stationUpdated(TVStationListener::kCountsChange);
				(*iter)->stationActivated();
				iter++;
			}

		}
	}

	if (fWasUsedInCalc){
		//in this case fAMC is never zero
		fActiveMeasCount += measCount;
		fActiveEqCount += eqCount;
		fActiveUnkCount += unkCount;


		StListnrIterator iter = fStListeners.begin();
		StListnrIterator iterEnd = fStListeners.end();
		while (iter != iterEnd){
			(*iter)->stationUpdated(TVStationListener::kCountsChange);
			iter++;
		}

	}
}

/////////
// pipeau
/////////
void TAFreeInstrumentStation::sendEnoughTotalEqns() {

}

void TAFreeInstrumentStation::childReadyToActivate(int a, int b, int c){

}

void TAFreeInstrumentStation::activateReadyChildren() {

}

*/

