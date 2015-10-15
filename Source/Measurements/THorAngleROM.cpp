// 
// THorAngleROM.cpp : implementation File
//
// Class for a horizontal angle round of measurements
//#include "TTheodoliteStation.h"
#include "TSpatialPosition.h"

#include "THorAngleROM.h"

//////////////
// constructor
//////////////
THorAngleROM::THorAngleROM():
TAUnreferencedROM() {

	// TODO: removed fRefMeasAngle(0) from pre-constructor calls
	fV0.setRadiansValue(LITERAL(0.0));
	fV0Status = TAMeasurement::kVariable;

	//fParentStation = &parent;
	//fV0.setStatus(TANumericValue::kVariable);

	//THorAngleROM* harom = fParentStation->addHorAngleROM(this);

}


///////////////////
// copy constructor
///////////////////
THorAngleROM::THorAngleROM(const THorAngleROM &source)
:TAUnreferencedROM(source), fMeasuredAngles(source.fMeasuredAngles)
//, fParentStation(source.fParentStation)
{

	fRefMeasAngle = source.fRefMeasAngle;
	fV0 = source.fV0;
	fV0Status = source.fV0Status;
}
	

/////////////
// destructor
/////////////
THorAngleROM::~THorAngleROM(){
	// TODO: commented this out
	/*if (fRefMeasAngle != 0){
		fRefMeasAngle = 0;
	}*/


}

////////////////////////////
// copy assignement operator
////////////////////////////
THorAngleROM& THorAngleROM::operator=(const THorAngleROM& source){

//	fParentStation = source.fParentStation;
//	fV0 = source.fV0;
	fMeasuredAngles = source.fMeasuredAngles;
	fRefMeasAngle = source.fRefMeasAngle;
	fSeries = source.fSeries;

	return (*this);
}

/////////////////////////////
// 

/////////////////////////////////////////////
// returns a non-const pointer to this object
/////////////////////////////////////////////
THorAngleROM* THorAngleROM::getPointer() const{

	return const_cast<THorAngleROM*>(this);
}

/////////////////////////////////////////
// activates the readyToActivate children
/////////////////////////////////////////
/*void THorAngleROM::activateReadyChildren() {

	HorAngMeasIterator iter = fMeasuredAngles.begin();
	HorAngMeasIterator iterEnd = fMeasuredAngles.end();

	while (iter != iterEnd){
		if (iter->readyToActivate())
			iter->activate();
		iter++;
	}
}
*/

///////////////////////////////////////////////////
// sets the unknown/variable/fixed status of the V0
///////////////////////////////////////////////////
void THorAngleROM::setV0Status(TAMeasurement::ECalcStatus s){

	fV0Status = s;
}

///////////////////////////////////////
// sets the provisional value of the V0
///////////////////////////////////////
void THorAngleROM::setV0ProvValue(TAngle provValue){

	fV0.setRadiansValue(provValue.getRadiansValue());
}

/////////////////////////////////
// checks if the V0 isn't unknown
/////////////////////////////////
/*bool THorAngleROM::knownV0() const{

	if (fV0.isNull())
		return false;
	else
		return true;
//	return fV0.known();
}*/

/////////////////
// returns the V0
/////////////////
TAngle THorAngleROM::getV0() const{

	return fV0;
}


////////////////////////
// returns the V0 status
////////////////////////
TAMeasurement::ECalcStatus THorAngleROM::getV0Status() const{

	return fV0Status;
}



/////////////////////////////////////
// adds a horizontal angle to the set
/////////////////////////////////////
THorAngleMeasurement* THorAngleROM::addHorAngle(THorAngleMeasurement* horang){

	// case of the first angle to be inserted
	if (fMeasuredAngles.empty()){
		setRefAngle(horang);
//		fParentStation->updateTotalEqCount(horang->equationsCount());
		return &(*(fRefMeasAngle));
	}
	else{
		//if (notInContainer(horang)){
			fMeasuredAngles.push_back(*horang);
//			fParentStation->updateTotalEqCount(horang->equationsCount());
			return &(*(--fMeasuredAngles.end()));
		//}
		//else{
		//	cerr << "HorAngleMeasurement not inserted : already stored\n";
		//	return 0;
		//}
	}
}

/////////////////////////////////////////////////////////////
// checks if the horizontal angle is already in the container
/////////////////////////////////////////////////////////////
bool THorAngleROM::notInContainer(THorAngleMeasurement* ha) const{

	HorAngMeasConstIter iter = getHorAngMeasBeginIterator();
	HorAngMeasConstIter iterEnd = getHorAngMeasEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*ha) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}

//////////////////////////////////////////
// sets the measure at the reference point
//////////////////////////////////////////
void THorAngleROM::setRefAngle(THorAngleMeasurement* refAngle){

	fMeasuredAngles.push_back(*refAngle);

	fRefMeasAngle = --(fMeasuredAngles.end());
//	fReferencePoint = fRefMeasAngle->getTargetPoint();

	//computation of the rom's V0 provisional value
/*	bool xStk = (fParentStation->getStationedPoint()->getPosition()).knownXCoord();
	bool yStk = (fParentStation->getStationedPoint()->getPosition()).knownYCoord();
	bool xRefk = (fReferencePoint->getPosition()).knownXCoord();
	bool yRefk = (fReferencePoint->getPosition()).knownYCoord();

	if (xStk && yStk && yRefk && xRefk){
		quad xSt = ((fParentStation->getStationedPoint()->getPosition()).getXCoord()).getMetresValue();
		quad ySt = ((fParentStation->getStationedPoint()->getPosition()).getYCoord()).getMetresValue();
		quad xRef = (fReferencePoint->getPosition()).getXCoord().getMetresValue();
		quad yRef = (fReferencePoint->getPosition()).getYCoord().getMetresValue();

		TAngle gisCalc = (TAngle::aTan((xRef-xSt)/(yRef-ySt)));
		TAngle v0 = (TAngle::aTan((xRef-xSt)/(yRef-ySt))) - fRefMeasAngle->getAngleValue();
		setV0ProvValue(v0);
	}
	else {
		cerr << fParentStation->getSetup() << " : Warning : the V0 of " << getSeries() << " is not computable due to missing coordinates : DISACTIVATED\n";
		disactivate();
	}*/
}


/////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the angle meas made at the specified point
/////////////////////////////////////////////////////////////////////////////
HorAngMeasIterator THorAngleROM::getHorAngMeas( TSpatialPointName iterTg) {

	HorAngMeasIterator iter = fMeasuredAngles.begin();
	HorAngMeasIterator iterEnd = fMeasuredAngles.end();

	bool notFound = true;
	while ((iter != iterEnd) && notFound){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}

//////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the angle meas made at the specified point
//////////////////////////////////////////////////////////////////////////////////
HorAngMeasConstIter THorAngleROM::getHorAngMeas(TSpatialPointName iterTg) const{

	HorAngMeasConstIter iter = fMeasuredAngles.begin();
	HorAngMeasConstIter iterEnd = fMeasuredAngles.end();

	bool notFound = true;
	while ((iter != iterEnd) && notFound){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}

/////////////////////////////////////////////////////////////
// returns an iterator pointing to the first angle of the set
/////////////////////////////////////////////////////////////
HorAngMeasIterator THorAngleROM::getHorAngMeasBeginIterator() {

	return fMeasuredAngles.begin();
}


/////////////////////////////////////////////////////////////
// returns a const iterator pointing to the first angle of the set
/////////////////////////////////////////////////////////////
HorAngMeasConstIter THorAngleROM::getHorAngMeasBeginIterator() const{

	return fMeasuredAngles.begin();
}


//////////////////////////////////////////////////////////////////
// returns an iterator pointing one past the last angle of the set
//////////////////////////////////////////////////////////////////
HorAngMeasIterator THorAngleROM::getHorAngMeasEndIterator() {

	return fMeasuredAngles.end();
}

///////////////////////////////////////////////////////////////////////
// returns a const iterator pointing one past the last angle of the set
///////////////////////////////////////////////////////////////////////
HorAngMeasConstIter THorAngleROM::getHorAngMeasEndIterator() const{

	return fMeasuredAngles.end();
}




///////////////////////////////////////////////////////////////////////
// returnsthe size of the list
///////////////////////////////////////////////////////////////////////
int THorAngleROM::size() const{

	return fMeasuredAngles.size();
}





//////////////////////////////////////////////////////
// gets the value of the measure made at the ref point
//////////////////////////////////////////////////////
TAngle THorAngleROM::getRefMeasValue() const{

	return fRefMeasAngle->getAngleValue();
}


//////////////////////////////////////////////////////
// returns an iterator to the measure to the reference
//////////////////////////////////////////////////////
HorAngMeasIterator THorAngleROM::getRefMeas() {

	return fRefMeasAngle;
}

///////////////////////////////////////////////////////////
// returns a const iterator to the measure to the reference
///////////////////////////////////////////////////////////
HorAngMeasConstIter THorAngleROM::getRefMeas() const{

	return fRefMeasAngle;
}

////////////////////////////////////////////////////////////////////////
// tells the children the rom has a sufficient total number of equations
////////////////////////////////////////////////////////////////////////
/*void THorAngleROM::sendEnoughTotalEqns() {

	HorAngMeasIterator iter = fMeasuredAngles.begin();
	HorAngMeasIterator iterEnd = fMeasuredAngles.end();

	while (iter != iterEnd){
		iter->parentEnoughTotalEqns();
		iter++;
	}

	TAUnreferencedROM::ROMListnrIterator iterL = fROMListeners.begin();
	TAUnreferencedROM::ROMListnrIterator iterLEnd = fROMListeners.end();

	while (iterL != iterLEnd){
		(*iterL)->roundOfMeasUpdated(TVRoundOfMeasListener::EUpdateMessage::kEnoughTotalEqns);
		iterL++;
	}
}
//////////////////////////////////////////////////
// tells the children meas the rom is disactivated
//////////////////////////////////////////////////
void THorAngleROM::sendDisactivatedToChildren() {

	HorAngMeasIterator iter = fMeasuredAngles.begin();
	HorAngMeasIterator iterEnd = fMeasuredAngles.end();

	while (iter != iterEnd){
		iter->parentDisactivated();
		iter++;
	}
}

/////////////////////////////////////////////////
// tells the children meas the rom is Activated
/////////////////////////////////////////////////
void THorAngleROM::sendActivatedToChildren() {

	HorAngMeasIterator iter = fMeasuredAngles.begin();
	HorAngMeasIterator iterEnd = fMeasuredAngles.end();

	while (iter != iterEnd){
		iter->parentActivated();
		iter++;
	}
}

////////////////////////////////////////////////////////////////////////////
// responds to a sufficient total number of equations for the parent station
////////////////////////////////////////////////////////////////////////////
void THorAngleROM::parentEnoughTotalEqns(){

	//message is just relayed as the ROm has no reference point
	sendEnoughTotalEqns();
}

///////////////////////////////////////////////////
// tells the parent station the rom is disactivated
///////////////////////////////////////////////////
void THorAngleROM::sendDisactivatedToParent() {

	fParentStation->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

//////////////////////////////////////////////////
// tells the parent station the rom is Activated
//////////////////////////////////////////////////
void THorAngleROM::sendActivatedToParent() {

	fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}
*/