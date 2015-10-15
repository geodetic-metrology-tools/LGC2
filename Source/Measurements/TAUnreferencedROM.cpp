//
// TAUnreferencedROM.cpp : implementation file
// abstract base class holding the behaviour common to all
// unreferenced rounds of measurements, mainly the active/inactive management

#include "TAUnreferencedROM.h"

//////////////////////
// default constructor
//////////////////////
TAUnreferencedROM::TAUnreferencedROM(){

	//fUsedInCalc = false;
	//fUsedInCalc = true;
	//fReadyToActivate = false;
	//fParentStation = parent;
	//fParentUsedInCalc = fParentStation->isActive();
	//fActiveMeasCount = 0;
	//fActiveEqCount = 0;
	//fActiveUnkCount = 0;
	//fTotalEqnNumber = 0;

	fSeries = "";
}

///////////////////
// copy constructor
///////////////////
TAUnreferencedROM::TAUnreferencedROM(const TAUnreferencedROM& source):
/*fUsedInCalc(source.fUsedInCalc), fParentUsedInCalc(source.fParentUsedInCalc),
fReadyToActivate(source.fReadyToActivate), fTotalEqnNumber(source.fTotalEqnNumber),
fActiveMeasCount(source.fActiveMeasCount), fActiveEqCount(source.fActiveEqCount),
fActiveUnkCount(source.fActiveUnkCount), */
fSeries(source.fSeries) 
//fParentStation(source.fParentStation)
{

}
/////////////
// destructor
/////////////
TAUnreferencedROM::~TAUnreferencedROM(){

}

///////////////////////////////////////
// returns the rom's equation count
///////////////////////////////////////
/*int TAUnreferencedROM::equationsCount() const{

	return fActiveEqCount;
}

//////////////////////////////////////////
// returns the rom's measurement count
//////////////////////////////////////////
int TAUnreferencedROM::measurementsCount() const{

	return fActiveMeasCount;
}

///////////////////////////////////////
// returns the rom's unknowns count
///////////////////////////////////////
int TAUnreferencedROM::unknownsCount() const{

	return fActiveUnkCount;
}

/////////////////////////////////////////////////////////////
// tells if the rom has enough (active or inactive) equations
/////////////////////////////////////////////////////////////
bool TAUnreferencedROM::enoughTotalEqns() const{

	//the rom has no reference point, only the station has to be considered
	return fParentStation->enoughTotalEqns();
}
*/
//////////////////////////////
// returns the rom's series
//////////////////////////////
string TAUnreferencedROM::getSeries() const{

	return fSeries;
}

///////////////////////////
// sets the rom's series
///////////////////////////
void TAUnreferencedROM::setSeries(string series){

	fSeries = series;
}

/////////////
// < operator
/////////////
bool TAUnreferencedROM::operator<(const TAUnreferencedROM& right) const{

	if (fSeries<right.fSeries)
		return true;
	else
		return false;
}

//////////////
// == operator
//////////////
bool TAUnreferencedROM::operator==(const TAUnreferencedROM& right) const{

	if (fSeries == right.fSeries)
		return true;
	else
		return false;
}


///////////////////////////////////
// adds a listener to the container
///////////////////////////////////
/*void TAUnreferencedROM::addListener(TVRoundOfMeasListener* romListnr){

	fROMListeners.push_back(romListnr);
}


///////////////////////
// disactivates the ROM
///////////////////////
void TAUnreferencedROM::disactivate(){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;

	//No reaction if !fUsedInCalc

	//Only message sent to station. Ref and Meas already consider
	//that the ROM is inactive
	if ((fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		fUsedInCalc = false;
		sendDisactivatedToParent();

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasDisactivated();
			iter++;
		}
	}

	//Repercussions on everybody
	if ((fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fUsedInCalc = false;
		sendDisactivatedToParent();
		sendDisactivatedToChildren();

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasDisactivated();
			iter++;
		}
	}
}

//////////////////////
// reactivates the ROM
//////////////////////
void TAUnreferencedROM::activate(){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;

	//no reaction if fUIC

	if ((!fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		//no reaction if fAMC==0
		if (fActiveMeasCount != 0){
			fUsedInCalc = true;
			sendActivatedToParent();

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasActivated();
				iter++;
			}
		}
	}

	if ((!fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fUsedInCalc = true;
		sendActivatedToParent();
		sendActivatedToChildren();
		
		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasActivated();
			iter++;
		}

	}
}

/////////////////////////////////////
// returns the active/inactive status
/////////////////////////////////////
bool TAUnreferencedROM::isActive() const{

	return fUsedInCalc;
}

////////////////////////////////////////
// tells if the rom is ready to activate
////////////////////////////////////////
bool TAUnreferencedROM::readyToActivate() const{

	return fReadyToActivate;
}

//////////////////////////////////////////////
// responds to the disactivation of the parent
//////////////////////////////////////////////
void TAUnreferencedROM::parentDisactivated(){

	//no reaction if !fParentUsedInCalc

	bool fParentWasUsedInCalc = fParentUsedInCalc;
	bool fWasUsedInCalc = fUsedInCalc;

	//!fParentUsedInCalc shouldn't occur, anyway no reaction

	if ((!fWasUsedInCalc) && (fParentWasUsedInCalc))
		fParentUsedInCalc = false;

	if ((fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fParentUsedInCalc = false;
		sendDisactivatedToChildren();

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasDisactivated();
			iter++;
		}
	}
}

/////////////////////////////////////////////
// responds to the reactivation of the parent
/////////////////////////////////////////////
void TAUnreferencedROM::parentActivated(){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;

	//no reaction if fParentUsedInCalc

	if ((!fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		if (fActiveMeasCount == 0)
			fParentUsedInCalc = true;
		else {
			fParentUsedInCalc = true;
			fUsedInCalc = true;
			sendActivatedToChildren();

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasActivated();
				iter++;
			}
		}
	}

	if ((fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		
		//fAMC == 0 shouldn't happen, but who knows...
		if (fActiveMeasCount == 0)
			fParentUsedInCalc = true;
		else {
			fParentUsedInCalc = true;
			sendActivatedToChildren();

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasActivated();
				iter++;
			}
		}
	}
}

///////////////////////////////////////////
// responds to the disactivation of a child
///////////////////////////////////////////
void TAUnreferencedROM::childDisactivated(int measCount, int eqCount, int unkCount){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;
	
	//!fUIC implies that the Parent's ActiveMeasCount has already been updated
	if ((!fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		fActiveMeasCount -= measCount;
		fActiveEqCount -= eqCount;
		fActiveUnkCount -= unkCount;

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
			iter++;
		}
	}

	
	//!fUIC implies that the Parent's ActiveMeasCount has already been updated
	if ((!fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fActiveMeasCount -= measCount;
		fActiveEqCount -= eqCount;
		fActiveUnkCount -= unkCount;

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
			iter++;
		}
	}

	//The station, though it is Inactive, has to decrement its MeasCount
	if ((fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		fActiveMeasCount -= measCount;
		fActiveEqCount -= eqCount;
		fActiveUnkCount -= unkCount;

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
			iter++;
		}

		if (fActiveMeasCount == 0){
			fUsedInCalc = false;
			sendDisactivatedToChildren();

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasDisactivated();
				iter++;
			}
		}
		fParentStation->childDisactivated(measCount, eqCount, unkCount);
	}

	//Repercuted on everybody
	if ((fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fActiveMeasCount -= measCount;
		fActiveEqCount -= eqCount;
		fActiveUnkCount -= unkCount;

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
			iter++;
		}

		if (fActiveMeasCount == 0){
			//fUsedInCalc = false;
			sendDisactivatedToChildren();

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasDisactivated();
				iter++;
			}
		}
		fParentStation->childDisactivated(measCount, eqCount, unkCount);
	}
}

//////////////////////////////////////////////////
// responds to an additional readyToActivate Child
//////////////////////////////////////////////////
void TAUnreferencedROM::childReadyToActivate(int measCount, int eqCount, int unkCount){

	fReadyToActivate = true;
	fParentStation->childReadyToActivate(measCount, eqCount, unkCount);
}

//////////////////////////////////////////
// responds to the reactivation of a child
//////////////////////////////////////////
void TAUnreferencedROM::childActivated(int measCount, int eqCount, int unkCount){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;

	if ((!fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		if (fActiveMeasCount != 0){
			fActiveMeasCount += measCount;
			fActiveEqCount += eqCount;
			fActiveUnkCount += unkCount;

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
				iter++;
			}
		}


		if (fActiveMeasCount == 0){
			fActiveMeasCount += measCount;
			fActiveEqCount += eqCount;
			fActiveUnkCount += unkCount;

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
				(*iter)->roundOfMeasActivated();
				iter++;
			}

			fUsedInCalc = true;
			fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
		}
	}


	if ((!fWasUsedInCalc) && (fParentWasUsedInCalc)){
		if (fActiveMeasCount != 0){
			fActiveMeasCount += measCount;
			fActiveEqCount += eqCount;
			fActiveUnkCount += unkCount;

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
				iter++;
			}
		}

		
		if (fActiveMeasCount == 0){
			fActiveMeasCount += measCount;
			fActiveEqCount += eqCount;
			fActiveUnkCount += unkCount;

			fUsedInCalc = true;

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
				(*iter)->roundOfMeasActivated();
				iter++;
			}

			
			fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
			sendActivatedToChildren();
		}
	}


	if ((fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		fActiveMeasCount += measCount;
		fActiveEqCount += eqCount;
		fActiveUnkCount += unkCount;

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
			iter++;
		}

		fParentStation->childActivated(measCount, eqCount, unkCount);
	}


	if ((fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fActiveMeasCount += measCount;
		fActiveEqCount += eqCount;
		fActiveUnkCount += unkCount;

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kCountsChange);
			iter++;
		}

		fParentStation->childActivated(measCount, eqCount, unkCount);
	}
}
*/
