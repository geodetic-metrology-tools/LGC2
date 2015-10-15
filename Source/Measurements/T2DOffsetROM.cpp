// 
// T2DOffsetROM.cpp : implementation File
//
// Class for a round of horizontal distance measurements
#include "TDistMeasStation.h"

#include "T2DOffsetROM.h"


T2DOffsetROM::T2DOffsetROM() :
TAUnreferencedROM() {
}

///////////////////
// copy constructor
///////////////////
T2DOffsetROM::T2DOffsetROM(const T2DOffsetROM &source)
: TAUnreferencedROM(source), fMeasuredOffsets(source.fMeasuredOffsets)
/*fParentStation(source.fParentStation)*/{

			
}
	

/////////////
// destructor
/////////////
T2DOffsetROM::~T2DOffsetROM(){

}

////////////////////////////
// copy assignement operator
////////////////////////////
T2DOffsetROM& T2DOffsetROM::operator=(const T2DOffsetROM& source){

	fMeasuredOffsets = source.fMeasuredOffsets;
	fSeries = source.fSeries;

	return (*this);
}


//////////////////////////////////////////
// adds a 2D offset measurement to the set
//////////////////////////////////////////
T2DOffsetMeasurement* T2DOffsetROM::add2DOffset(T2DOffsetMeasurement* om){

	if (notInContainer(om)){
		fMeasuredOffsets.push_back(*om);
		//fParentStation->updateTotalEqCount(dist->equationsCount());  
		return &(*(--fMeasuredOffsets.end()));
	}
	else{
		cerr << "2DOffsetMeas not inserted : already stored" << endl;
		return 0;
	}
}

//////////////////////////////////////////////////////
// checks if the offset is already in the container
//////////////////////////////////////////////////////
bool T2DOffsetROM::notInContainer(T2DOffsetMeasurement* om) const{

	Off2DMeasConstIter iter = get2DOffsetMeasBeginIterator();
	Off2DMeasConstIter iterEnd = get2DOffsetMeasEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*om) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}


////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the offset meas made at the specified line
////////////////////////////////////////////////////////////////////////////
Off2DMeasIterator T2DOffsetROM::get2DOffsetMeas(/*TWorkingTargetObjects::SpLineIterator*/ string iterSpl) {

	Off2DMeasIterator iter = fMeasuredOffsets.begin();
	Off2DMeasIterator iterEnd = fMeasuredOffsets.end();

	bool notFound = true;
	while (notFound && (iter != iterEnd)){
		if /*((iter->getTargetLine()) == iterSpl)*/ ((iter->getLineName()) == iterSpl)
			notFound = false;
		else
			iter++;
	}
	return iter;
}

//////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the offset meas made at the specified line
//////////////////////////////////////////////////////////////////////////////////
Off2DMeasConstIter T2DOffsetROM::get2DOffsetMeas(/*TWorkingTargetObjects::SpLineIterator*/ string iterSpl) const{

	Off2DMeasConstIter iter = fMeasuredOffsets.begin();
	Off2DMeasConstIter iterEnd = fMeasuredOffsets.end();

	bool notFound = true;
	while (notFound && (iter != iterEnd)){
		if /*((iter->getTargetLine()) == iterSpl)*/ ((iter->getLineName()) == iterSpl)
			notFound = false;
		else
			iter++;
	}
	return iter;
}


//////////////////////////////////////////////////////////////////
// returns an iterator pointing to the first offset of the container
//////////////////////////////////////////////////////////////////
Off2DMeasIterator T2DOffsetROM::get2DOffsetMeasBeginIterator() {

	return fMeasuredOffsets.begin();
}

/////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the first offset of the container
/////////////////////////////////////////////////////////////////////////
Off2DMeasConstIter T2DOffsetROM::get2DOffsetMeasBeginIterator() const{

	return fMeasuredOffsets.begin();
}

////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to one element past the end of the container
////////////////////////////////////////////////////////////////////////////
Off2DMeasIterator T2DOffsetROM::get2DOffsetMeasEndIterator() {

	return fMeasuredOffsets.end();
}

/////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to one element past the end of the container
/////////////////////////////////////////////////////////////////////////////////
Off2DMeasConstIter T2DOffsetROM::get2DOffsetMeasEndIterator() const{

	return fMeasuredOffsets.end();
}
/*
//////////////////////////////////////////////////////////
// returns the number of offsets measured within this ROM
///////////////////////////////////////////////////////////
int T2DOffsetROM::getOffsetCount() const{

	return fMeasuredOffsets.size();
}
*/

/*
//////////////////////////
// no argument constructor
//////////////////////////
T2DOffsetROM::T2DOffsetROM(TDistMeasStation& parent):
TAUnreferencedROM(&parent){

	fParentStation = &parent;
	T2DOffsetROM* orom = fParentStation->add2DOffsetROM(this);

}

////////////////////////////////////////////////////////
// tells the parent station that the ROM is disactivated
////////////////////////////////////////////////////////
void T2DOffsetROM::sendDisactivatedToParent() {

	fParentStation->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

///////////////////////////////////////////////////////
// tells the parent station that the ROM is Activated
///////////////////////////////////////////////////////
void T2DOffsetROM::sendActivatedToParent() {

	fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}


///////////////////////////////////////////////////////////////
// tells the children measurements that the ROM is disactivated
///////////////////////////////////////////////////////////////
void T2DOffsetROM::sendDisactivatedToChildren() {

	Off2DMeasIterator iter = fMeasuredOffsets.begin();
	Off2DMeasIterator iterEnd = fMeasuredOffsets.end();

	while (iter != iterEnd){
		iter->parentDisactivated();
		iter++;
	}
}

//////////////////////////////////////////////////////////////
// tells the children measurements that the ROM is Activated
//////////////////////////////////////////////////////////////
void T2DOffsetROM::sendActivatedToChildren() {

	Off2DMeasIterator iter = fMeasuredOffsets.begin();
	Off2DMeasIterator iterEnd = fMeasuredOffsets.end();

	while (iter != iterEnd){
		iter->parentActivated();
		iter++;
	}
}


void T2DOffsetROM::parentEnoughTotalEqns(){} 
void T2DOffsetROM::sendEnoughTotalEqns() {}
void T2DOffsetROM::activateReadyChildren() {}
*/