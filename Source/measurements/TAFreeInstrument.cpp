//
// TAFreeInstrument.cpp : implementation file
// abstract base class holding what the behaviour common to all
// instrument stations without setup point

#include "TAFreeInstrument.h"

TAFreeInstrument::TAFreeInstrument(const string& id, const TLength* ics)
{
	fSetup = "";
	instrumentID = id;
	instrumentCenteringSigma = ics;
}

const string& TAFreeInstrument::getSetup() const
{
	return fSetup;
}

void TAFreeInstrument::setSetup(string setup)
{
	fSetup = setup;
}

const string& TAFreeInstrument::getInstrumentID() const
{
	return instrumentID;
}

bool TAFreeInstrument::operator ==(const TAFreeInstrument& right) const
{
	return instrumentID == right.instrumentID;
}

/*
///////////////////////////////////////
// returns the station's equation count
///////////////////////////////////////
int TAFreeInstrument::equationsCount() const{

	return fActiveEqCount;
}

//////////////////////////////////////////
// returns the station's measurement count
//////////////////////////////////////////
int TAFreeInstrument::measurementsCount() const{

	return fActiveMeasCount;
}

///////////////////////////////////////
// returns the station's unknowns count
///////////////////////////////////////
int TAFreeInstrument::unknownsCount() const{

	return fActiveUnkCount;
}

/////////////////////////////////////////////////////////////////////////////////
// tells if the station has enough  (active or inactive) equations for its setup point to be determined
/////////////////////////////////////////////////////////////////////////////////
bool TAFreeInstrument::enoughTotalEqns() const{

//	if (fTotalEqnNumber >= fStationedPoint->getVariableDimension())
		return true;
//	else
//		return false;
}

///////////////////////////////////////////////////////////////////////////
// updates the stations counts (used when a meas is inserted in a child ROM
///////////////////////////////////////////////////////////////////////////
void TAFreeInstrument::updateCounts(int measUpd, int eqUpd, int unkUpd){

	fActiveMeasCount += measUpd;
	fActiveEqCount += eqUpd;
	fActiveUnkCount += unkUpd;
}

/////////////////////////////////////////////////////////////
// updates the total number of (active or inactive) equations
/////////////////////////////////////////////////////////////
void TAFreeInstrument::updateTotalEqCount(int totECUpdt){

	fTotalEqnNumber += totECUpdt;

	if ((fTotalEqnNumber >= fStationedPoint->getVariableDimension()) &&
		((fTotalEqnNumber-totECUpdt) < fStationedPoint->getVariableDimension()))
		sendEnoughTotalEqns();

}


/////////////////////////////////////
// returns the active/inactive status
/////////////////////////////////////
bool TAFreeInstrument::isActive() const{

	return fUsedInCalc;
}

///////////////////////////////////
// adds a listener to the container
///////////////////////////////////
void TAFreeInstrument::addListener(TVStationListener* stListnr){

	fStListeners.push_back(stListnr);
}

///////////////////////////
// disactivates the station
///////////////////////////
void TAFreeInstrument::disactivate(){

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
void TAFreeInstrument::activate(){

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
void TAFreeInstrument::childDisactivated(int measCount, int eqCount, int unkCount){


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
void TAFreeInstrument::childActivated(int measCount, int eqCount, int unkCount){

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
void TAFreeInstrument::sendEnoughTotalEqns() {

}

void TAFreeInstrument::childReadyToActivate(int a, int b, int c){

}

void TAFreeInstrument::activateReadyChildren() {

}

*/

