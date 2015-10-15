//
// TWire.cpp : implementation file
// class modelising a WPS wire

#include "TWire.h"

//////////////
// constructor
//////////////
TWire::TWire(PointIterator firstEnd, PointIterator secondEnd, string name):
fFirstEnd(firstEnd), fSecondEnd(secondEnd), fName(name),
fEqCount(0){

	fUsedInCalc = true;
//	fNeededEquations = fFirstEnd->getVariableDimension() +  fSecondEnd->getVariableDimension();
	fNeededEquations = 0;
//	fEndsActive = (fFirstEnd->isActive()) && (fSecondEnd->isActive());
	fEndsActive = true;

}

///////////////////
// copy constructor
///////////////////
TWire::TWire(const TWire& source):
fFirstEnd(source.getFirstEnd()), fSecondEnd(source.getSecondEnd()),
fName(source.fName),
fEqCount(source.fEqCount), fNeededEquations(source.fNeededEquations), 
fUsedInCalc(source.fUsedInCalc), fEndsActive(source.fEndsActive),
fWireListeners(source.fWireListeners){

}

////////////
// detructor
////////////
TWire::~TWire(){

}

////////////////////////
// < operator definition
////////////////////////
bool TWire::operator<(const TWire& right) const{

	if (fName < right.getName())
		return true;
	else{
		if (fName == right.getName()){
			if ((*fFirstEnd) < *(right.getFirstEnd()))
				return true;
			else{
				if ((*fSecondEnd) < *(right.getSecondEnd()))
					return true;
				else
					return false;
			}
		}
		else
			return false;
	}
}

/////////////////////////
// == operator definition
/////////////////////////
bool TWire::operator==(const TWire& right) const{

	if (fName == right.getName())
		return true;
	else
		return false;
}




/////////////////////////////////////
// returns the active/inactive status
/////////////////////////////////////
bool TWire::isActive() const{

	return fUsedInCalc;
}

/////////////////////
// activates the Wire
/////////////////////
void TWire::activate(){

	//no reaction if fUsedInCalc

	if (!fUsedInCalc)
		sendActivatedToAll();
}

////////////////////////
// disactivates the wire
////////////////////////
void TWire::disactivate(){

	//no reaction if !fUsedInCalc
	if (fUsedInCalc){
		fUsedInCalc = false;
		fEqCount = 0;
		sendDisactivatedToAll();
	}
}

/////////////////////////////////////////////////////////
// tells everyone concerned that the wire is disactivated
/////////////////////////////////////////////////////////
void TWire::sendDisactivatedToAll() {

	WireListnrIterator iter = fWireListeners.begin();
	WireListnrIterator iterEnd = fWireListeners.end();

	while (iter != iterEnd){
		(*iter)->measEntityDisactivated();
		iter++;
	}
}

//////////////////////////////////////////////////////////
// tells every concerned object that the wire is activated
//////////////////////////////////////////////////////////
void TWire::sendActivatedToAll() {

	WireListnrIterator iter = fWireListeners.begin();
	WireListnrIterator iterEnd = fWireListeners.end();

	while (iter != iterEnd){
		(*iter)->measEntityActivated();
		iter++;
	}
}

/////////////////////////////////////////////////////////
// returns an iterator to the first end point of the wire
/////////////////////////////////////////////////////////
PointConstIter TWire::getFirstEnd() const{

	return fFirstEnd;
}

//////////////////////////////////////////////////////////
// returns an iterator to the second end point of the wire
//////////////////////////////////////////////////////////
PointConstIter TWire::getSecondEnd() const{

	return fSecondEnd;
}

///////////////////////
// sets the wire's name
///////////////////////
void TWire::setName(string name){

	fName = name;
}

///////////////////////
// gets the wire's name
///////////////////////
string TWire::getName() const{

	return fName;
}

////////////////////////////////////
// update the needed equations count
////////////////////////////////////
void TWire::updateNeededEquations(){

	int oldNdEq = fNeededEquations;
	fNeededEquations = fFirstEnd->getVariableDimension() +  fSecondEnd->getVariableDimension();

	WireListnrIterator iter = fWireListeners.begin();
	WireListnrIterator iterEnd = fWireListeners.end();
	while (iter != iterEnd){
		(*iter)->measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage::kNeededEqChange);
		iter++;
	}

	//maybe the wire has to be activated or disactivated
	int ndEqUpdt = oldNdEq - fNeededEquations;

	if ((!fUsedInCalc) && (ndEqUpdt>0)){
		if ((fEqCount>fNeededEquations) && (fEqCount<oldNdEq))
			activate();
	}

	if ((fUsedInCalc) && (ndEqUpdt<0)){
		if (fEqCount<fNeededEquations)
			disactivate();
	}
}

/////////////////////////////
// updates the equation count
/////////////////////////////
void TWire::updateEqCount(int eqCountUpdate){

	if (eqCountUpdate >0){

		if (fUsedInCalc){
			fEqCount += eqCountUpdate;
			WireListnrIterator iter = fWireListeners.begin();
			WireListnrIterator iterEnd = fWireListeners.end();
			while (iter != iterEnd){
				(*iter)->measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage::kEqCountChange);
				iter++;
			}
		}


		if (!fUsedInCalc){
			if ((fEqCount + eqCountUpdate) >= fNeededEquations){
				fEqCount += eqCountUpdate;
				fUsedInCalc = true;

				WireListnrIterator iter = fWireListeners.begin();
				WireListnrIterator iterEnd = fWireListeners.end();
				while (iter != iterEnd){
					(*iter)->measEntityActivated();
					(*iter)->measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage::kEqCountChange);
					iter++;
				}
			}
			else{
				WireListnrIterator iter = fWireListeners.begin();
				WireListnrIterator iterEnd = fWireListeners.end();
				while (iter != iterEnd){
					(*iter)->measEntityDisactivated();
					//the only object it will affect is the sender of the updateEqCount message
					iter++;
				}
			}
		}
	}


	if (eqCountUpdate <0){
		//no reaction if !fUsedInCalc, shouldn't occur

		if (fUsedInCalc){

			fEqCount += eqCountUpdate;
			WireListnrIterator iter = fWireListeners.begin();
			WireListnrIterator iterEnd = fWireListeners.end();
			while (iter != iterEnd){
				(*iter)->measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage::kEqCountChange);
				iter++;
			}

			if (fEqCount < fNeededEquations){
				fUsedInCalc = false;
				iter = fWireListeners.begin();
				while (iter != iterEnd){
					(*iter)->measEntityDisactivated();
					iter++;
				}
			}
		}
	}
}

///////////////////////////////////
// adds a listener to the container
///////////////////////////////////
void TWire::addListener(TVMeasurableEntityLstnr* mel){

	fWireListeners.push_back(mel);
}

//////////////////////////////
//empty point listener methods
//////////////////////////////
void TWire::pointUpdated(TVSpatialPtListener::EUpdateMessage um){

}

void TWire::pointActivated(){

}

void TWire::pointDisactivated(){

}
	

