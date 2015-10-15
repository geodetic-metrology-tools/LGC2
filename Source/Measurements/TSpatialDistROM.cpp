// 
// TSpatialDistROM.cpp : implementation File
//
// Class for a round of spatial distance measurements
#include "TDistMeasStation.h"

#include "TSpatialDistROM.h"
/*
//////////////////////////
// no argument constructor
//////////////////////////
TSpatialDistROM::TSpatialDistROM(TDistMeasStation& parent):
TAUnreferencedROM(&parent){

	fParentStation = &parent;
	TSpatialDistROM* hdrom = fParentStation->addSpatialDistROM(this);

}
*/
TSpatialDistROM::TSpatialDistROM() :
TAUnreferencedROM() {
}
///////////////////
// copy constructor
///////////////////
TSpatialDistROM::TSpatialDistROM(const TSpatialDistROM &source)
: TAUnreferencedROM(source), fMeasuredDists(source.fMeasuredDists)
//fParentStation(source.fParentStation)
{

			
}
	

/////////////
// destructor
/////////////
TSpatialDistROM::~TSpatialDistROM(){

}

////////////////////////////
// copy assignement operator
////////////////////////////
TSpatialDistROM& TSpatialDistROM::operator=(const TSpatialDistROM& source){

	fMeasuredDists = source.fMeasuredDists;
	fSeries = source.fSeries;

	return (*this);
}



//////////////////////////////////////////////////////
// checks if the dist meas is already in the container
//////////////////////////////////////////////////////
bool TSpatialDistROM::notInContainer(TSpatialDistMeasurement* dist) const{

	SpatialDistMeasConstIter iter = getDistMeasBeginIterator();
	SpatialDistMeasConstIter iterEnd = getDistMeasEndIterator();

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
SpatialDistMeasIterator TSpatialDistROM::getDistMeasBeginIterator() {

	return fMeasuredDists.begin();
}

///////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the first dist of the container
///////////////////////////////////////////////////////////////////////
SpatialDistMeasConstIter TSpatialDistROM::getDistMeasBeginIterator() const{

	return fMeasuredDists.begin();
}

////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to one element past the end of the container
////////////////////////////////////////////////////////////////////////////
SpatialDistMeasIterator TSpatialDistROM::getDistMeasEndIterator() {

	return fMeasuredDists.end();
}

/////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to one element past the end of the container
/////////////////////////////////////////////////////////////////////////////////
SpatialDistMeasConstIter TSpatialDistROM::getDistMeasEndIterator() const{

	return fMeasuredDists.end();
}
/*
///////////////////////////////////////////////////////////
// returns the number of distances measured within this ROM
///////////////////////////////////////////////////////////
int TSpatialDistROM::getDistCount() const{

	return fMeasuredDists.size();
}

void TSpatialDistROM::sendEnoughTotalEqns() {
}
*/

 //////////////////////////////
// adds an EDM dist to the set
//////////////////////////////
TSpatialDistMeasurement* TSpatialDistROM::addSpatialDist(TSpatialDistMeasurement* dist){

	//if (notInContainer(dist)){
		fMeasuredDists.push_back(*dist);
//		fParentStation->updateTotalEqCount(dist->equationsCount());  
		return &(*(--fMeasuredDists.end()));
	//}
//	else{
	//	cerr << "SpatialDistMeasurement not inserted : already stored" << endl;
	//	return 0;
	//}
}

////////////////////////////////////////////////////////////////////////////
// returns an const_iterator pointing to the dist meas made at the specified point
////////////////////////////////////////////////////////////////////////////
SpatialDistMeasConstIter TSpatialDistROM::getDistMeas(/*TWorkingPoints::PointIterator*/ TSpatialPointName iterTg) const {

	SpatialDistMeasConstIter iter = fMeasuredDists.begin();
	SpatialDistMeasConstIter iterEnd = fMeasuredDists.end();

	bool notFound = true;
	while (notFound && (iter != iterEnd)){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}

////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the dist meas made at the specified point
////////////////////////////////////////////////////////////////////////////
SpatialDistMeasIterator TSpatialDistROM::getDistMeas(/*TWorkingPoints::PointIterator*/ TSpatialPointName iterTg) {

	SpatialDistMeasIterator iter = fMeasuredDists.begin();
	SpatialDistMeasIterator iterEnd = fMeasuredDists.end();

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
/////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the dist meas made at the specified point
/////////////////////////////////////////////////////////////////////////////////
TSpatialDistROM::SpatialDistMeasConstIter TSpatialDistROM::getDistMeas(TWorkingPoints::PointIterator iterTg) const{

	SpatialDistMeasConstIter iter = fMeasuredDists.begin();
	SpatialDistMeasConstIter iterEnd = fMeasuredDists.end();

	bool notFound = true;
	while (notFound && (iter != iterEnd)){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}
*/


/*
/////////////////////////////////////////
// activates the readyToActivate children
/////////////////////////////////////////
void TSpatialDistROM::activateReadyChildren() {

	SpatialDistMeasIterator iter = fMeasuredDists.begin();
	SpatialDistMeasIterator iterEnd = fMeasuredDists.end();

	while (iter != iterEnd){
		if (iter->readyToActivate())
			iter->activate();
		iter++;
	}
}


////////////////////////////////////////////////////////
// tells the parent station that the ROM is disactivated
////////////////////////////////////////////////////////
void TSpatialDistROM::sendDisactivatedToParent() {

	fParentStation->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

///////////////////////////////////////////////////////
// tells the parent station that the ROM is Activated
///////////////////////////////////////////////////////
void TSpatialDistROM::sendActivatedToParent() {

	fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}


///////////////////////////////////////////////////////////////
// tells the children measurements that the ROM is disactivated
///////////////////////////////////////////////////////////////
void TSpatialDistROM::sendDisactivatedToChildren() {

	SpatialDistMeasIterator iter = fMeasuredDists.begin();
	SpatialDistMeasIterator iterEnd = fMeasuredDists.end();

	while (iter != iterEnd){
		iter->parentDisactivated();
		iter++;
	}
}

//////////////////////////////////////////////////////////////
// tells the children measurements that the ROM is Activated
//////////////////////////////////////////////////////////////
void TSpatialDistROM::sendActivatedToChildren() {

	SpatialDistMeasIterator iter = fMeasuredDists.begin();
	SpatialDistMeasIterator iterEnd = fMeasuredDists.end();

	while (iter != iterEnd){
		iter->parentActivated();
		iter++;
	}
}


void TSpatialDistROM::parentEnoughTotalEqns(){
}
*/
