//  TZenithDistROM.cpp
//




#include	"TTheodoliteStation.h"
//#include    "TSpatialPosition.h"

#include  "TZenithDistROM.h"
////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
TZenithDistROM::TZenithDistROM(TTheodoliteStation& parent) : TAUnreferencedROM(&parent)
{	
	fParentStation = &parent;
	TZenithDistROM* zdrom = fParentStation->addZenithDistROM(this);

}
*/

///////////////////////
// default constructor
///////////////////////
TZenithDistROM::TZenithDistROM() {
}

///////////////////
// copy constructor
///////////////////
TZenithDistROM::TZenithDistROM( const  TZenithDistROM& source ) :
fMeasuredAngles(source.fMeasuredAngles), TAUnreferencedROM(source)/*, fParentStation(source.fParentStation)*/
{
}


/////////////
// destructor
/////////////
TZenithDistROM::~TZenithDistROM()
{
}

///////////////////
// Member Functions
///////////////////

///////////////////////////
// Copy Assignment operator
///////////////////////////
TZenithDistROM&  TZenithDistROM::operator=(const TZenithDistROM& source)
{
//	fParentStation = source.fParentStation;
	fMeasuredAngles = source.fMeasuredAngles;
	fSeries = source.fSeries;

	return *this;
}

/////////////////////////////////////////////
// returns a non-const pointer to this object
/////////////////////////////////////////////
TZenithDistROM* TZenithDistROM::getPointer() const{

	return const_cast<TZenithDistROM*>(this);
}


//////////////////////////////////////
// adds a zenithal distance to the set
//////////////////////////////////////
TZenithDistMeasurement* TZenithDistROM::addZenithDist(TZenithDistMeasurement* zendist){

	
	//	if (notInContainer(zendist)){
			fMeasuredAngles.push_back(*zendist);
//			fParentStation->updateTotalEqCount(zendist->equationsCount());
			return &(*(--fMeasuredAngles.end()));
	//	}
	//	else{
	//		cerr << "ZenithDistMeasurement not inserted : already stored\n";
	//		return 0;
	//	}
}	

//////////////////////////////////////////////////////////////
// checks if the zenithal distance is already in the container
//////////////////////////////////////////////////////////////
bool TZenithDistROM::notInContainer(TZenithDistMeasurement* ha) const {

	ZenithDistMeasConstIter iter = getZenithDistMeasBeginIterator();
	ZenithDistMeasConstIter iterEnd = getZenithDistMeasEndIterator();

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




/////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the angle meas made at the specified point
/////////////////////////////////////////////////////////////////////////////
ZenithDistMeasIterator TZenithDistROM::getZenithDistMeas(/*TWorkingPoints::PointIterator*/TSpatialPointName iterTg) {

	ZenithDistMeasIterator iter = fMeasuredAngles.begin();
	ZenithDistMeasIterator iterEnd = fMeasuredAngles.end();

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
ZenithDistMeasConstIter TZenithDistROM::getZenithDistMeas(/*TWorkingPoints::PointIterator*/ TSpatialPointName iterTg) const{

	ZenithDistMeasConstIter iter = fMeasuredAngles.begin();
	ZenithDistMeasConstIter iterEnd = fMeasuredAngles.end();

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
ZenithDistMeasIterator TZenithDistROM::getZenithDistMeasBeginIterator() {

	return fMeasuredAngles.begin();
}

//////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the first angle of the set
//////////////////////////////////////////////////////////////////
ZenithDistMeasConstIter TZenithDistROM::getZenithDistMeasBeginIterator() const{

	return fMeasuredAngles.begin();
}


//////////////////////////////////////////////////////////////////
// returns an iterator pointing one past the last angle of the set
//////////////////////////////////////////////////////////////////
ZenithDistMeasIterator TZenithDistROM::getZenithDistMeasEndIterator() {

	return fMeasuredAngles.end();
}

///////////////////////////////////////////////////////////////////////
// returns a const iterator pointing one past the last angle of the set
///////////////////////////////////////////////////////////////////////
ZenithDistMeasConstIter TZenithDistROM::getZenithDistMeasEndIterator() const{

	return fMeasuredAngles.end();
}

/*
//////////////////////////////////////////////////
// tells the children meas the rom is disactivated
//////////////////////////////////////////////////
void TZenithDistROM::sendDisactivatedToChildren() {

	ZenithDistMeasIterator iter = fMeasuredAngles.begin();
	ZenithDistMeasIterator iterEnd = fMeasuredAngles.end();

	while (iter != iterEnd){
		iter->parentDisactivated();
		iter++;
	}
}

///////////////////////////////////////////////
// tells the children meas the rom is Activated
///////////////////////////////////////////////
void TZenithDistROM::sendActivatedToChildren() {

	ZenithDistMeasIterator iter = fMeasuredAngles.begin();
	ZenithDistMeasIterator iterEnd = fMeasuredAngles.end();

	while (iter != iterEnd){
		iter->parentActivated();
		iter++;
	}
}


///////////////////////////////////////////////////
// tells the parent station the rom is disactivated
///////////////////////////////////////////////////
void TZenithDistROM::sendDisactivatedToParent() {

	fParentStation->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

////////////////////////////////////////////////
// tells the parent station the rom is Activated
////////////////////////////////////////////////
void TZenithDistROM::sendActivatedToParent() {

	fParentStation->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}









void TZenithDistROM::activateReadyChildren() {

}


void TZenithDistROM::parentEnoughTotalEqns(){

}


void TZenithDistROM::sendEnoughTotalEqns() {

}
*/