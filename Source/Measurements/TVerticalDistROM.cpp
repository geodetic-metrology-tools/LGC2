// TVerticalDistROM.cpp
//
/** Class for a round of vertical distance measurements */
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
#include "TLevelStation.h"
#include  "TVerticalDistROM.h"
////////////////////////////////////////////////////////////////


//ClassImp(TVerticalDistROM)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////
// Default Constructor
//////////////////////
TVerticalDistROM::TVerticalDistROM():
TARoundOfMeas(), fIdentifier(-1)
{	
	
}



/////////////////
// Constructor
/////////////////
TVerticalDistROM::TVerticalDistROM(TSpatialPointName refPt):
TARoundOfMeas(refPt), fIdentifier(-1) {
}

///////////////////
// Copy constructor
///////////////////
TVerticalDistROM::TVerticalDistROM( const  TVerticalDistROM& source):
TARoundOfMeas(source), fIdentifier(source.fIdentifier),
fMeasuredDists(source.fMeasuredDists)
{	

}

/////////////
// Destructor
/////////////
TVerticalDistROM::~TVerticalDistROM()
{

}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

// **This needs to be implemented**
TVerticalDistROM&  TVerticalDistROM::operator=(const TVerticalDistROM& right)
{	// Copy Assignment operator

	if (this != &right)
	{
			fMeasuredDists = right.fMeasuredDists;
			fRefPtName = right.fRefPtName;
			fIdentifier = right.fIdentifier;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////
// adds a vertical dist to the set
//////////////////////////////////
TVerticalDistMeasurement* TVerticalDistROM::addVertDist(TVerticalDistMeasurement* vdist){

	//if (notInContainer(vdist)){
		fMeasuredDists.push_back(*vdist);
		//fParentStation->updateTotalEqCount(vdist->equationsCount());
		return &(*(--fMeasuredDists.end()));
	//}
	//else{
	//	cerr << "Vertical Dist Measurement not inserted : already stored\n";
	//	return 0;
//	}
	
}

/////////////////////////////////////////////////////////////
// checks if a vertical dist is already in the container
/////////////////////////////////////////////////////////////
bool TVerticalDistROM::notInContainer(TVerticalDistMeasurement* vd) const{

	VertDistMeasConstIter iter = getVertDistMeasBeginIterator();
	VertDistMeasConstIter iterEnd = getVertDistMeasEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*vd) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}


/////////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the vert dist meas made at the specified point
/////////////////////////////////////////////////////////////////////////////////
VertDistMeasIterator TVerticalDistROM::getVertDistMeas(/*TWorkingPoints::PointIterator*/TSpatialPointName iterTg) {

	VertDistMeasIterator iter = fMeasuredDists.begin();
	VertDistMeasIterator iterEnd = fMeasuredDists.end();

	bool notFound = true;
	while ((iter != iterEnd) && notFound){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}

//////////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the vert dist meas made at the specified point
//////////////////////////////////////////////////////////////////////////////////////
VertDistMeasConstIter TVerticalDistROM::getVertDistMeas(/*TWorkingPoints::PointIterator*/ TSpatialPointName iterTg) const{

	VertDistMeasConstIter iter = fMeasuredDists.begin();
	VertDistMeasConstIter iterEnd = fMeasuredDists.end();

	bool notFound = true;
	while ((iter != iterEnd) && notFound){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}

////////////////////////////////////////////////////////////
// returns an iterator pointing to the first dist of the set
////////////////////////////////////////////////////////////
VertDistMeasIterator TVerticalDistROM::getVertDistMeasBeginIterator() {

	return fMeasuredDists.begin();
}

/////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the first dist of the set
/////////////////////////////////////////////////////////////////
VertDistMeasConstIter TVerticalDistROM::getVertDistMeasBeginIterator() const{

	return fMeasuredDists.begin();
}


//////////////////////////////////////////////////////////////////
// returns an iterator pointing one past the last angle of the set
//////////////////////////////////////////////////////////////////
VertDistMeasIterator TVerticalDistROM::getVertDistMeasEndIterator() {

	return fMeasuredDists.end();
}

///////////////////////////////////////////////////////////////////////
// returns a const iterator pointing one past the last angle of the set
///////////////////////////////////////////////////////////////////////
VertDistMeasConstIter TVerticalDistROM::getVertDistMeasEndIterator() const{

	return fMeasuredDists.end();
}

/*
////////////////////////////////////////////////////////////////////////
// tells the children the rom has a sufficient total number of equations
////////////////////////////////////////////////////////////////////////
void TVerticalDistROM::sendEnoughTotalEqns() {

	VertDistMeasIterator iter = fMeasuredDists.begin();
	VertDistMeasIterator iterEnd = fMeasuredDists.end();

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
void TVerticalDistROM::sendDisactivatedToChildren() {

	VertDistMeasIterator iter = fMeasuredDists.begin();
	VertDistMeasIterator iterEnd = fMeasuredDists.end();

	while (iter != iterEnd){
		iter->parentDisactivated();
		iter++;
	}
}

/////////////////////////////////////////////////
// tells the children meas the rom is Activated
/////////////////////////////////////////////////
void TVerticalDistROM::sendActivatedToChildren() {

	VertDistMeasIterator iter = fMeasuredDists.begin();
	VertDistMeasIterator iterEnd = fMeasuredDists.end();

	while (iter != iterEnd){
		iter->parentActivated();
		iter++;
	}
}



///////////////////////////////////////////////////
// tells the parent station the rom is disactivated
///////////////////////////////////////////////////
void TVerticalDistROM::sendDisactivatedToParent() {

	fParentStation->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

//////////////////////////////////////////////////
// tells the parent station the rom is Activated
//////////////////////////////////////////////////
void TVerticalDistROM::sendActivatedToParent() {

	fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}
*/