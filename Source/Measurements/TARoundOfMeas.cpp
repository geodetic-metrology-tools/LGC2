//
// TARoundOfMeas.cpp : implementation file
// abstract base class holding the behaviour common to all
// referenced rounds of measurements, mainly the active/inactive management


#include "TARoundOfMeas.h"

//////////////////////
// default constructor
//////////////////////
TARoundOfMeas::TARoundOfMeas(): fRefPtName(){

	fSeries = "";
}


////////////////////
// Constructor
////////////////////
TARoundOfMeas::TARoundOfMeas(TSpatialPointName refPt): fRefPtName(refPt),
fSeries("") {
}

///////////////////
// copy constructor
///////////////////
TARoundOfMeas::TARoundOfMeas(const TARoundOfMeas& source) :
/*fUsedInCalc(source.fUsedInCalc), fParentUsedInCalc(source.fParentUsedInCalc),
fActiveMeasCount(source.fActiveMeasCount), fActiveEqCount(source.fActiveEqCount),
fActiveUnkCount(source.fActiveUnkCount),fParentStation(source.fParentStation), 
fReferencePoint(source.fReferencePoint)*/ 
fSeries(source.fSeries), fRefPtName(source.fRefPtName)
{

}

/////////////
// destructor
/////////////
TARoundOfMeas::~TARoundOfMeas(){

}
/*
///////////////////////////////////////
// returns the rom's equation count
///////////////////////////////////////
int TARoundOfMeas::equationsCount() const{

	return fActiveEqCount;
}

//////////////////////////////////////////
// returns the rom's measurement count
//////////////////////////////////////////
int TARoundOfMeas::measurementsCount() const{

	return fActiveMeasCount;
}

///////////////////////////////////////
// returns the rom's unknowns count
///////////////////////////////////////
int TARoundOfMeas::unknownsCount() const{

	return fActiveUnkCount;
}

/////////////////////////////////////////////
// returns an iterator to the reference point
/////////////////////////////////////////////
TWorkingPoints::PointIterator TARoundOfMeas::getRefPoint() const{

	return fReferencePoint;
}
*/
//////////////////////////////
// returns the rom's series
//////////////////////////////
string TARoundOfMeas::getSeries() const{

	return fSeries;
}

//////////////////////////////////////
// returns the rom's ref.point's name
//////////////////////////////////////
TSpatialPointName TARoundOfMeas::getRefPtName() const {

	return fRefPtName;
}

///////////////////////////
// sets the rom's series
///////////////////////////
void TARoundOfMeas::setSeries(string series){

	fSeries = series;
}

/////////////
// = operator
/////////////
// not implemented.

/////////////
// < operator
/////////////
bool TARoundOfMeas::operator<(const TARoundOfMeas& right) const{

	if (fSeries<right.fSeries)
		return true;
	else
		return false;
}

//////////////
// == operator
//////////////
bool TARoundOfMeas::operator==(const TARoundOfMeas& right) const{

	if (fSeries == right.fSeries)
		return true;
	else
		return false;
}


/*
///////////////////////////////////
// adds a listener to the container
///////////////////////////////////
void TARoundOfMeas::addListener(TVRoundOfMeasListener* romListnr){

	fROMListeners.push_back(romListnr);
}


///////////////////////
// disactivates the ROM
///////////////////////
void TARoundOfMeas::disactivate(){

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
		fReferencePoint->updateEqCount(-fActiveEqCount);

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
void TARoundOfMeas::activate(){

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
		if (fReferencePoint != 0)
			fReferencePoint->updateEqCount(fActiveEqCount);
		
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
bool TARoundOfMeas::isActive() const{

	return fUsedInCalc;
}

//////////////////////////////////////////////
// responds to the disactivation of the parent
//////////////////////////////////////////////
void TARoundOfMeas::parentDisactivated(){

	//no reaction if !fParentUsedInCalc

	bool fParentWasUsedInCalc = fParentUsedInCalc;
	bool fWasUsedInCalc = fUsedInCalc;

	//!fParentUsedInCalc shouldn't occur, anyway no reaction

	if ((!fWasUsedInCalc) && (fParentWasUsedInCalc))
		fParentUsedInCalc = false;

	if ((fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fParentUsedInCalc = false;
		fReferencePoint->updateEqCount(-fActiveEqCount);
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
void TARoundOfMeas::parentActivated(){

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
			fReferencePoint->updateEqCount(fActiveEqCount);

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
			fReferencePoint->updateEqCount(fActiveEqCount);

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
void TARoundOfMeas::childDisactivated(int measCount, int eqCount, int unkCount){

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
		fReferencePoint->updateEqCount(-eqCount);
	}
}


//////////////////////////////////////////
// responds to the reactivation of a child
//////////////////////////////////////////
void TARoundOfMeas::childActivated(int measCount, int eqCount, int unkCount){

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
			fReferencePoint->updateEqCount(fActiveEqCount);
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
		fReferencePoint->updateEqCount(eqCount);
	}
}

///////////////////////////////////////////////////////
// responds to the disactivation of the reference point
///////////////////////////////////////////////////////
void TARoundOfMeas::pointDisactivated(){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;

	//!fUsedInCalc shouldn't occur because the reference
	//doesn't reply to the ROM when the latter is disactivated
	
	//No message sent to the measurements because they've already
	//received an ParentDisactivated message when Station was Disactivated
	if ((fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		fUsedInCalc = false;
		fParentStation->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasDisactivated();
			iter++;
		}

	}

	//
	if ((fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fUsedInCalc = false;
		fParentStation->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
		sendDisactivatedToChildren();

		ROMListnrIterator iter = fROMListeners.begin();
		ROMListnrIterator iterEnd = fROMListeners.end();
		while (iter != iterEnd){
			(*iter)->roundOfMeasDisactivated();
			iter++;
		}
	}
}

//////////////////////////////////////////////////////
// responds to the reactivation of the reference point
//////////////////////////////////////////////////////
void TARoundOfMeas::pointActivated(){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;

	//no reaction if fUIC

	if ((!fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		//no reaction if fAMC==0
		if (fActiveMeasCount != 0){
			fUsedInCalc = true;
			fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasActivated();
				iter++;
			}
		}
	}

	if ((!fWasUsedInCalc) && (fParentWasUsedInCalc)){ 
		//no reaction if fAMC==0
		if (fActiveMeasCount != 0){
			fUsedInCalc = true;
			fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
			sendActivatedToChildren();
			fReferencePoint->updateEqCount(fActiveEqCount);

			ROMListnrIterator iter = fROMListeners.begin();
			ROMListnrIterator iterEnd = fROMListeners.end();
			while (iter != iterEnd){
				(*iter)->roundOfMeasActivated();
				iter++;
			}
		}
	}
}


///
//pipeau
void TARoundOfMeas::childReadyToActivate(int a, int b, int c){

}

void TARoundOfMeas::activateReadyChildren() {

}

void TARoundOfMeas::parentEnoughTotalEqns(){

}

bool TARoundOfMeas::enoughTotalEqns() const{

	return true;
}
*/