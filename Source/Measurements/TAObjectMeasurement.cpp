// TAObjectMeasurement.cpp
//
/* Abstract base class for all measurements made to something other than a point  */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
//#include "TVMeasurementListener.h"
//#include "TVParentMeasTreeNode.h"


#include  "TAObjectMeasurement.h"
////////////////////////////////////////////////////////////////


//ClassImp(TAObjectMeasurement)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////
// default constructor
//////////////////////
TAObjectMeasurement::TAObjectMeasurement()/*:
fTargetPtName("")*/ {

/*	fUsedInCalc = true;
	fParent = 0;
	fActiveMeasCount = 1;
	fActiveEqCount = 1;
	fActiveUnkCount = 0; */
}


/*
TAObjectMeasurement::TAObjectMeasurement(TVParentMeasTreeNode* parent)
{	
	//fUsedInCalc = false;
	fUsedInCalc = true;
	fParent = parent;
	if (fParent != 0)
		fParentUsedInCalc = fParent->isActive();
	fActiveMeasCount = 1;
	fActiveEqCount = 1;
	fActiveUnkCount = 0; 
}
*/
// **This needs to be implemented**
TAObjectMeasurement::TAObjectMeasurement( const  TAObjectMeasurement& )//:
/*fUsedInCalc(source.fUsedInCalc), fParentUsedInCalc(source.fParentUsedInCalc),
fActiveMeasCount(source.fActiveMeasCount), fActiveEqCount(source.fActiveEqCount),
fActiveUnkCount(source.fActiveUnkCount), fParent(source.fParent) 
fTargetPtName(source.fTargetPtName)*/
{	// copy constructor

}


TAObjectMeasurement::~TAObjectMeasurement()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

// **This needs to be implemented**
TAObjectMeasurement&  TAObjectMeasurement::operator=(const TAObjectMeasurement& right)
{	// Copy Assignment operator

	if (this != &right)
	{
/*		fParent = right.fParent;
		fUsedInCalc = right.fUsedInCalc;
		fParentUsedInCalc = right.fParentUsedInCalc;
		fActiveMeasCount = right.fActiveMeasCount;
		fActiveEqCount = right.fActiveEqCount;
		fActiveUnkCount = right.fActiveUnkCount;
		*/
//		fTargetPtName = right.fTargetPtName;
	}
	return *this;
}


//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////
/*
//////////////////////////////////////
// returns the target point's name
/////////////////////////////////////
TSpatialPointName TAObjectMeasurement::getTargetPoint() const {
	
	return fTargetPtName;
	
}

/////////////////////////////////////
// set the target point's name
////////////////////////////////////
bool TAObjectMeasurement::setTargetPoint(TSpatialPointName spn) {

	fTargetPtName = spn;
	return true;

}
*/
/*
///////////////////////////////////////
// returns the measurement's equation count
///////////////////////////////////////
int TAObjectMeasurement::equationsCount() const{

	return fActiveEqCount;
}

//////////////////////////////////////////
// returns the measurement's measurement count
//////////////////////////////////////////
int TAObjectMeasurement::measurementsCount() const{

	return fActiveMeasCount;
}

///////////////////////////////////////
// returns the measurement's unknowns count
///////////////////////////////////////
int TAObjectMeasurement::unknownsCount() const{

	return fActiveUnkCount;
}

/////////////////////////////////////////
// tells about the active/inactive status
/////////////////////////////////////////
bool TAObjectMeasurement::isActive() const{

	return fUsedInCalc;
}

///////////////////////////////////////////////
// adds a measurement listener to the container
///////////////////////////////////////////////
void TAObjectMeasurement::addListener(TVMeasurementListener* measListnr){

	fMeasListeners.push_back(measListnr);
}

///////////////////////////////
// disactivates the measurement
///////////////////////////////
void TAObjectMeasurement::disactivate(){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;

	if (fUsedInCalc)
		fUsedInCalc = false;

	//Active Parent
	if ((fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fParent->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
	//	fTargetPoint->updateEqCount(-fActiveEqCount);

		MeasListnrIterator iter = fMeasListeners.begin();
		MeasListnrIterator iterEnd = fMeasListeners.end();
		while (iter != iterEnd){
			(*iter)->measurementDisactivated();
			iter++;
		}
	}

	//Inactive Parent
	//No message sent to the Target  (would be redundant with the one 
	//sent when the Parent was set Inactive
	if ((fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		fParent->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

		MeasListnrIterator iter = fMeasListeners.begin();
		MeasListnrIterator iterEnd = fMeasListeners.end();
		while (iter != iterEnd){
			(*iter)->measurementDisactivated();
			iter++;
		}
	}
}

//////////////////////////////
// reactivates the measurement
//////////////////////////////
void TAObjectMeasurement::activate(){

	//no reaction if fUsedInCalc

	//if !fUIC && !fOUIC message sent only to the Parent
	if ((!fUsedInCalc) && (!fParentUsedInCalc)){
		fUsedInCalc = true;
		fParent->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

		MeasListnrIterator iter = fMeasListeners.begin();
		MeasListnrIterator iterEnd = fMeasListeners.end();
		while (iter != iterEnd){
			(*iter)->measurementActivated();
			iter++;
		}

	}

	//if !fUIC && fOUIC message sent to all concerned objects
	if ((!fUsedInCalc) && (fParentUsedInCalc)){
		fUsedInCalc = true;
		fParent->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
	//	fTargetPoint->updateEqCount(fActiveEqCount);

		MeasListnrIterator iter = fMeasListeners.begin();
		MeasListnrIterator iterEnd = fMeasListeners.end();
		while (iter != iterEnd){
			(*iter)->measurementActivated();
			iter++;
		}
	}
}

////////////////////////////////////////////////////////////////////////
// responds to a sufficient total number of equations for the parent rom
////////////////////////////////////////////////////////////////////////
void TAObjectMeasurement::parentEnoughTotalEqns(){


}

/////////////////////////////////////////////////////////////
// responds to the disactivation of the parent ROM or station
/////////////////////////////////////////////////////////////
void TAObjectMeasurement::parentDisactivated(){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;

	//!fParentUsedInCalc shouldn't occur

	if ((!fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fParentUsedInCalc = false;
	}

	if ((fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fParentUsedInCalc = false;
	//	fTargetPoint->updateEqCount(-fActiveEqCount);

		MeasListnrIterator iter = fMeasListeners.begin();
		MeasListnrIterator iterEnd = fMeasListeners.end();
		while (iter != iterEnd){
			(*iter)->measurementDisactivated();
			iter++;
		}
	}
}

////////////////////////////////////////////////////////////
// responds to the reactivation of the parent ROM or station
////////////////////////////////////////////////////////////
void TAObjectMeasurement::parentActivated(){

	bool fParentWasUsedInCalc = fParentUsedInCalc;
	bool fWasUsedInCalc = fUsedInCalc;

	//no reaction if fParentUsedInCalc

	if ((!fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		fParentUsedInCalc = true;
	}

	if ((fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		fParentUsedInCalc = true;
//		fTargetPoint->updateEqCount(fActiveEqCount);

		MeasListnrIterator iter = fMeasListeners.begin();
		MeasListnrIterator iterEnd = fMeasListeners.end();
		while (iter != iterEnd){
			(*iter)->measurementActivated();
			iter++;
		}
	}
}

///////////////////////////////////////////////////
// tells the parent the measurement is disactivated
///////////////////////////////////////////////////
void TAObjectMeasurement::sendDisactivatedToParent() const{

	fParent->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
}

////////////////////////////////////////////////
// tells the parent the measurement is activated
////////////////////////////////////////////////
void TAObjectMeasurement::sendActivatedToParent() const{

	fParent->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
}


////////////////////////////////////////////////////
// responds to the disactivation of the target entity
////////////////////////////////////////////////////
void TAObjectMeasurement::measEntityDisactivated(){

	bool fWasUsedInCalc = fUsedInCalc;
	bool fParentWasUsedInCalc = fParentUsedInCalc;

	//No response necessary if !fUsedInCalc (shouldn't occur)

	//
	if ((fWasUsedInCalc) && (!fParentWasUsedInCalc)){
		fUsedInCalc = false;
		fParent->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

		MeasListnrIterator iter = fMeasListeners.begin();
		MeasListnrIterator iterEnd = fMeasListeners.end();
		while (iter != iterEnd){
			(*iter)->measurementDisactivated();
			iter++;
		}
	}

	//Same reaction as preceding case. Meant to ease debugging
	if ((fWasUsedInCalc) && (fParentWasUsedInCalc)){
		fUsedInCalc = false;
		fParent->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

		MeasListnrIterator iter = fMeasListeners.begin();
		MeasListnrIterator iterEnd = fMeasListeners.end();
		while (iter != iterEnd){
			(*iter)->measurementDisactivated();
			iter++;
		}
	}
}


///////////////////////////////////////////////////
// responds to the reactivation of the target entity
///////////////////////////////////////////////////
void TAObjectMeasurement::measEntityActivated(){

	//no reaction if fUsedInCalc

	//if !fUIC, the reaction is the same for fPUIC as for !fPUIC
	if (!fUsedInCalc){
		fUsedInCalc = true;
		fParent->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
	//	fTargetPoint->updateEqCount(fActiveEqCount);

		MeasListnrIterator iter = fMeasListeners.begin();
		MeasListnrIterator iterEnd = fMeasListeners.end();
		while (iter != iterEnd){
			(*iter)->measurementActivated();
			iter++;
		}

	}
}


/////////////////////////////////////////////////
// responds to other messages of the target entity
/////////////////////////////////////////////////
void TAObjectMeasurement::measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage msg){

	typedef TVMeasurableEntityLstnr::EUpdateMessage msgEnum;

	switch(msg){

	case msgEnum::kReadyToActivate:
		fReadyToActivate = true;
		fParent->childReadyToActivate(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);
		break;

	default:
		break;
	}
}
*/