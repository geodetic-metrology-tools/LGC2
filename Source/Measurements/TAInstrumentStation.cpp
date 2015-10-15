//
// TAInstrumentAStation.cpp : implementation file
// abstract base class holding what the behaviour common to all
// instrument stations, mainly the active/inactive management

#include "TAInstrumentStation.h"

//////////////////////
// default constructor
//////////////////////
TAInstrumentStation::TAInstrumentStation()
: fStationedPoint(){

	fSetup = "";
	fInstrumentNumber = "";
}






TAInstrumentStation::TAInstrumentStation(TSpatialPointName setupPt){

	//fUsedInCalc = false;
	//fUsedInCalc = true;
	//fActiveMeasCount = 0;
	//fActiveEqCount = 0;
	//fActiveUnkCount = 0;

	//fTotalEqnNumber = 0;
	//fReadyToActivateEqns = 0;

	fStationedPoint = setupPt;

	fSetup = "";
	fInstrumentNumber = "";
}

TAInstrumentStation::TAInstrumentStation(TSpatialPointName setupPt, InstNb in){

	//fUsedInCalc = false;
	//fUsedInCalc = true;
	//fActiveMeasCount = 0;
	//fActiveEqCount = 0;
	//fActiveUnkCount = 0;

	//fTotalEqnNumber = 0;
	//fReadyToActivateEqns = 0;

	fStationedPoint = setupPt;

	fSetup = "";
	fInstrumentNumber = in;
}

///////////////////
// copy constructor
///////////////////
TAInstrumentStation::TAInstrumentStation(const TAInstrumentStation& source) :
/*fUsedInCalc(source.fUsedInCalc), fActiveMeasCount(source.fActiveMeasCount),
fActiveEqCount(source.fActiveEqCount), fActiveUnkCount(source.fActiveUnkCount),
fReadyToActivateEqns(source.fReadyToActivateEqns), fTotalEqnNumber(source.fTotalEqnNumber),*/
fSetup(source.fSetup), fInstrumentNumber(source.fInstrumentNumber),
fStationedPoint(source.fStationedPoint){

}

	

/////////////
// destructor
/////////////
TAInstrumentStation::~TAInstrumentStation(){

}

///////////////////////////////////////
// returns the station's equation count
///////////////////////////////////////
/*int TAInstrumentStation::equationsCount() const{

	return fActiveEqCount;
}

//////////////////////////////////////////
// returns the station's measurement count
//////////////////////////////////////////
int TAInstrumentStation::measurementsCount() const{

	return fActiveMeasCount;
}

///////////////////////////////////////
// returns the station's unknowns count
///////////////////////////////////////
int TAInstrumentStation::unknownsCount() const{

	return fActiveUnkCount;
}*/

/////////////////////////////////////////////////////////////////////////////////
// tells if the station has enough  (active or inactive) equations for its setup point to be determined
/////////////////////////////////////////////////////////////////////////////////
/*bool TAInstrumentStation::enoughTotalEqns() const{

	if (fTotalEqnNumber >= fStationedPoint->getVariableDimension())
		return true;
	else
		return false;
}*/

///////////////////////////////////////////////////////////////////////////
// updates the stations counts (used when a meas is inserted in a child ROM
///////////////////////////////////////////////////////////////////////////
/*void TAInstrumentStation::updateCounts(int measUpd, int eqUpd, int unkUpd){

	fActiveMeasCount += measUpd;
	fActiveEqCount += eqUpd;
	fActiveUnkCount += unkUpd;
}*/

/////////////////////////////////////////////////////////////
// updates the total number of (active or inactive) equations
/////////////////////////////////////////////////////////////
/*void TAInstrumentStation::updateTotalEqCount(int totECUpdt){

	fTotalEqnNumber += totECUpdt;

	if ((fTotalEqnNumber >= fStationedPoint->getVariableDimension()) &&
		((fTotalEqnNumber-totECUpdt) < fStationedPoint->getVariableDimension()))
		sendEnoughTotalEqns();

}*/

//////////////////////////////
// returns the station's setup
//////////////////////////////
string TAInstrumentStation::getSetup() const{

	return fSetup;
}

///////////////////////////
// sets the station's setup
///////////////////////////
void TAInstrumentStation::setSetup(string setup){

	fSetup = setup;
}


////////////////////////////////
// returns the instrument number
////////////////////////////////
InstNb TAInstrumentStation::getInstrumentNumber() const{

	return fInstrumentNumber;
}

/////////////
// < operator
/////////////
bool TAInstrumentStation::operator<(const TAInstrumentStation& right) const{

	if ((fStationedPoint)<(right.fStationedPoint))
		return true;

	else {
		if ((fStationedPoint)==(right.fStationedPoint)){
			if (fInstrumentNumber<right.fInstrumentNumber)
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
		else
			return false;
	}
}

//////////////
// == operator
//////////////
bool TAInstrumentStation::operator==(const TAInstrumentStation& right) const{

	if (/*(fInstrumentNumber==right.fInstrumentNumber) &&*/
		(fStationedPoint==right.fStationedPoint) /*&&
		(fSetup==right.fSetup)*/)
		return true;
	else return false;
}

///////////////////////////////////////
// returns a pointer to the setup point
///////////////////////////////////////
TSpatialPointName TAInstrumentStation::getStationedPoint() const{

	return fStationedPoint;
}

/////////////////////////////////////
// returns the active/inactive status
/////////////////////////////////////
/*bool TAInstrumentStation::isActive() const{

	return fUsedInCalc;
}

///////////////////////////////////
// adds a listener to the container
///////////////////////////////////
void TAInstrumentStation::addListener(TVStationListener* stListnr){

	fStListeners.push_back(stListnr);
}
*/
///////////////////////////
// disactivates the station
///////////////////////////
/*void TAInstrumentStation::disactivate(){

	//no reaction if !fUsedInCalc

	if (fUsedInCalc){

		fUsedInCalc = false;

		fStationedPoint->updateEqCount(-fActiveMeasCount);
		sendDisactivatedToChildren();

		StListnrIterator iter = fStListeners.begin();
		StListnrIterator iterEnd = fStListeners.end();

		while (iter != iterEnd){
			(*iter)->stationDisactivated();
			iter++;
		}
	}
}
*/
//////////////////////////
// reactivates the station
//////////////////////////
/*void TAInstrumentStation::activate(){

	//no reaction if fUsedInCalc

	if (!fUsedInCalc){
		//no reaction if fActiveMeasCount==0
		if (fActiveMeasCount != 0){
			fUsedInCalc = true;
			fStationedPoint->updateEqCount(fActiveMeasCount);
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
*/
///////////////////////////////////////////
// response to the disactivation of a child
///////////////////////////////////////////
/*void TAInstrumentStation::childDisactivated(int measCount, int eqCount, int unkCount){


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

		fStationedPoint->updateEqCount(-eqCount);
	}
*/
	//What happens when MeasCount goes to zero
/*	if (fActiveMeasCount == 0){
		//No reaction if !fUIC
		if (fUsedInCalc){
			fUsedInCalc = false;
			//In this case the update to the point's EqCount is 0, no message to send to it			//Only to update the OwnerStatus of the acquisitions
			sendDisactivatedToChildren();
			StListnrIterator iter = fStListeners.begin();
			StListnrIterator iterEnd = fStListeners.end();
			while (iter != iterEnd){
				(*iter)->stationDisactivated();
				iter++;
			}
		}
	}*/
//}

//////////////////////////////////////////
// responds to the reactivation of a child
//////////////////////////////////////////
/*void TAInstrumentStation::childActivated(int measCount, int eqCount, int unkCount){

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
			fStationedPoint->updateEqCount(eqCount);

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

		fStationedPoint->updateEqCount(eqCount);

		StListnrIterator iter = fStListeners.begin();
		StListnrIterator iterEnd = fStListeners.end();
		while (iter != iterEnd){
			(*iter)->stationUpdated(TVStationListener::kCountsChange);
			iter++;
		}

	}
}
*/
///////////////////////////////////////////////////
// responds to the disactivation of the setup point
///////////////////////////////////////////////////
/*void TAInstrumentStation::pointDisactivated(){

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
*/
//////////////////////////////////////////////////
// responds to the reactivation of the setup point
//////////////////////////////////////////////////
/*void TAInstrumentStation::pointActivated(){

	//no reaction if fUsedInCalc

	if (!fUsedInCalc){
		//no reaction if fActiveMeasCount ==0
		if (fActiveMeasCount >0){
			fUsedInCalc  =true;
			sendActivatedToChildren();
			fStationedPoint->updateEqCount(fActiveEqCount);

			StListnrIterator iter = fStListeners.begin();
			StListnrIterator iterEnd = fStListeners.end();
			while (iter != iterEnd){
				(*iter)->stationActivated();
				iter++;
			}
		}
	}
}
*/



