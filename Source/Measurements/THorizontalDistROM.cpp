// 
// THorizontalDistROM.cpp : implementation File
//
// Class for a round of horizontal distance measurements
#include "TDistMeasStation.h"

#include "THorizontalDistROM.h"

/*
//////////////////////////
// no argument constructor
//////////////////////////
THorizontalDistROM::THorizontalDistROM(TDistMeasStation& parent):
TAUnreferencedROM(&parent){

	fParentStation = &parent;
	THorizontalDistROM* hdrom = fParentStation->addHorDistROM(this);

}
*/

THorizontalDistROM::THorizontalDistROM() :
TAUnreferencedROM(){
}

///////////////////
// copy constructor
///////////////////
THorizontalDistROM::THorizontalDistROM(const THorizontalDistROM &source)
: TAUnreferencedROM(source), fMeasuredDists(source.fMeasuredDists)
/*fParentStation(source.fParentStation)*/{

			
}
	

/////////////
// destructor
/////////////
THorizontalDistROM::~THorizontalDistROM(){

}

////////////////////////////
// copy assignement operator
////////////////////////////
THorizontalDistROM& THorizontalDistROM::operator=(const THorizontalDistROM& source){

	fMeasuredDists = source.fMeasuredDists;
	fSeries = source.fSeries;

	return (*this);
}


//////////////////////////////////////////////////////
// checks if the dist meas is already in the container
//////////////////////////////////////////////////////
bool THorizontalDistROM::notInContainer(THorizontalDistMeas* dist) const{

	HorDistMeasConstIter iter = getDistMeasBeginIterator();
	HorDistMeasConstIter iterEnd = getDistMeasEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*dist) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}



//////////////////////////////////////////////////////////////////
// returns an iterator pointing to the first dist of the container
//////////////////////////////////////////////////////////////////
HorDistMeasIterator THorizontalDistROM::getDistMeasBeginIterator() {

	return fMeasuredDists.begin();
}

///////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the first dist of the container
///////////////////////////////////////////////////////////////////////
HorDistMeasConstIter THorizontalDistROM::getDistMeasBeginIterator() const{

	return fMeasuredDists.begin();
}

////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to one element past the end of the container
////////////////////////////////////////////////////////////////////////////
HorDistMeasIterator THorizontalDistROM::getDistMeasEndIterator() {

	return fMeasuredDists.end();
}

/////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to one element past the end of the container
/////////////////////////////////////////////////////////////////////////////////
HorDistMeasConstIter THorizontalDistROM::getDistMeasEndIterator() const{

	return fMeasuredDists.end();
}

/*
///////////////////////////////////////////////////////////
// returns the number of distances measured within this ROM
///////////////////////////////////////////////////////////
int THorizontalDistROM::getDistCount() const{

	return fMeasuredDists.size();
}

*/
//////////////////////////////
// adds an EDM dist to the set
//////////////////////////////
THorizontalDistMeas* THorizontalDistROM::addHorDist(THorizontalDistMeas* dist){

	//if (notInContainer(dist)){
		fMeasuredDists.push_back(*dist);
//		fParentStation->updateTotalEqCount(dist->equationsCount());  
		return &(*(--fMeasuredDists.end()));
	//}
	//else{
		//cerr << "HorDistMeas not inserted : already stored" << endl;
	//	return 0;
	//}
}

////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the dist meas made at the specified point
////////////////////////////////////////////////////////////////////////////
HorDistMeasIterator THorizontalDistROM::getDistMeas(/*TWorkingPoints::PointIterator*/TSpatialPointName iterTg) {

	HorDistMeasIterator iter = fMeasuredDists.begin();
	HorDistMeasIterator iterEnd = fMeasuredDists.end();

	bool notFound = true;
	while (notFound && (iter != iterEnd)){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}

/////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the dist meas made at the specified point
/////////////////////////////////////////////////////////////////////////////////
HorDistMeasConstIter THorizontalDistROM::getDistMeas(/*TWorkingPoints::PointIterator*/TSpatialPointName iterTg) const{

	HorDistMeasConstIter iter = fMeasuredDists.begin();
	HorDistMeasConstIter iterEnd = fMeasuredDists.end();

	bool notFound = true;
	while (notFound && (iter != iterEnd)){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}


/*
/////////////////////////////////////////
// activates the readyToActivate children
/////////////////////////////////////////
void THorizontalDistROM::activateReadyChildren() {

	HorDistMeasIterator iter = fMeasuredDists.begin();
	HorDistMeasIterator iterEnd = fMeasuredDists.end();

	while (iter != iterEnd){
		if (iter->readyToActivate())
			iter->activate();
		iter++;
	}
}

////////////////////////////////////////////////////////////////////////////
// responds to a sufficient total number of equations for the parent station
////////////////////////////////////////////////////////////////////////////
void THorizontalDistROM::parentEnoughTotalEqns(){

	//message is just relayed as the ROm has no reference point
	sendEnoughTotalEqns();
}

////////////////////////////////////////////////////////
// tells the parent station that the ROM is disactivated
////////////////////////////////////////////////////////
void THorizontalDistROM::sendDisactivatedToParent() {

	fParentStation->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

///////////////////////////////////////////////////////
// tells the parent station that the ROM is Activated
///////////////////////////////////////////////////////
void THorizontalDistROM::sendActivatedToParent() {

	fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

////////////////////////////////////////////////////////////////////////
// tells the children the rom has a sufficient total number of equations
////////////////////////////////////////////////////////////////////////
void THorizontalDistROM::sendEnoughTotalEqns() {

	HorDistMeasIterator iter = fMeasuredDists.begin();
	HorDistMeasIterator iterEnd = fMeasuredDists.end();

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

///////////////////////////////////////////////////////////////
// tells the children measurements that the ROM is disactivated
///////////////////////////////////////////////////////////////
void THorizontalDistROM::sendDisactivatedToChildren() {

	HorDistMeasIterator iter = fMeasuredDists.begin();
	HorDistMeasIterator iterEnd = fMeasuredDists.end();

	while (iter != iterEnd){
		iter->parentDisactivated();
		iter++;
	}
}

//////////////////////////////////////////////////////////////
// tells the children measurements that the ROM is Activated
//////////////////////////////////////////////////////////////
void THorizontalDistROM::sendActivatedToChildren() {

	HorDistMeasIterator iter = fMeasuredDists.begin();
	HorDistMeasIterator iterEnd = fMeasuredDists.end();

	while (iter != iterEnd){
		iter->parentActivated();
		iter++;
	}
}


*/
