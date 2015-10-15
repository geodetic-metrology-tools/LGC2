// T2DOffsetMeasurement.cpp : implementation file
// Class for a 2D WPS measurement
// 

//#include "TWorkingStations.h"
#include "T2DOffsetROM.h"

#include "T2DOffsetMeasurement.h"

//////////////////////
// default constructor
//////////////////////
T2DOffsetMeasurement::T2DOffsetMeasurement()//:
/*TAObjectMeasurement(0),fParentROM(0), fTargetLine(0), 
fHUsedInCalc(false), fVUsedInCalc(false),fLineName(""), fSetupPtName("")*/ {

}

////////////////////////////////////////////////////////////////////////////////
// constructor setting the point names and the measurements and their precisions
////////////////////////////////////////////////////////////////////////////////
T2DOffsetMeasurement::T2DOffsetMeasurement(/*TSpatialPointName setupName,*/ string lineName, 
								 TLength obsHOffset, TLength hSigma, TLength obsVOffset, TLength vSigma):
fLineName(lineName),/* fSetupPtName(setupName), */
fHorOffset(obsHOffset), fHorSigma(hSigma), 
fVertOffset(obsVOffset), fVertSigma(vSigma){

}

/*
//////////////
// constructor
//////////////
T2DOffsetMeasurement::T2DOffsetMeasurement(T2DOffsetROM& parent, TWorkingTargetObjects::SpLineIterator iterSpl, TLength obsHOffset, TLength hSigma, TLength obsVOffset, TLength vSigma):
TAObjectMeasurement(&parent), fParentROM(&parent), fTargetLine(iterSpl),
fHorOffset(obsHOffset), fHorSigma(hSigma), 
fVertOffset(obsVOffset), fVertSigma(vSigma),
fHUsedInCalc(true), fVUsedInCalc(true)
{

	T2DOffsetMeasurement* om = fParentROM->add2DOffset(this);
	fTargetLine->addListener(static_cast<TVMeasurableEntityLstnr*>(om));

}
*/
/////////////
// destructor
/////////////
T2DOffsetMeasurement::~T2DOffsetMeasurement() //: 
/*fSetupPtName(""), fLineName(""), fVertOffset(LITERAL(0.0)), 
fVertSigma(LITERAL(0.0)), fHorOffset(LITERAL(0.0)), fHorSigma(LITERAL(0.0)), 
fActiveMeasCount(0), fActiveEqCount(0), fActiveUnkCount(0)*/ {

}

///////////////////
// copy constructor
///////////////////
T2DOffsetMeasurement::T2DOffsetMeasurement(const T2DOffsetMeasurement& source):
TAObjectMeasurement(source),
/*fTargetLine(source.getTargetLine()), fParentROM(source.fParentROM),*/
fVertOffset(source.getVertOffset()), fVertSigma(source.getVertSigma()),
fHorOffset(source.getHorOffset()), fHorSigma(source.getHorSigma()), 
/*fHUsedInCalc(source.hActive()), fVUsedInCalc(source.vActive()),*/ 
fLineName(source.fLineName)/*, fSetupPtName(source.fSetupPtName)*/
{

}

////////////////////////////
// copy assignement operator
////////////////////////////
T2DOffsetMeasurement& T2DOffsetMeasurement::operator=(const T2DOffsetMeasurement& source){

//	fTargetLine = source.getTargetLine();
//	fParentROM =  source.fParentROM;
	fVertOffset = source.getVertOffset();
	fVertSigma = source.getVertSigma();
	fHorOffset = source.getHorOffset();
	fHorSigma = source.getHorSigma();

	fLineName = source.fLineName;
//	fSetupPtName = source.fSetupPtName;

//	fHUsedInCalc = source.hActive();
//	fVUsedInCalc = source.vActive();

//	fActiveMeasCount = source.fActiveMeasCount;
//	fActiveEqCount = source.fActiveEqCount;
//	fActiveUnkCount = source.fActiveUnkCount;


	return (*this);
}


/////////////
// < operator
/////////////
bool T2DOffsetMeasurement::operator<(const T2DOffsetMeasurement& right) const{
/*
	if (*fTargetLine < (*right.fTargetLine))
		return true;
	else
		return false; */
	if (fLineName < right.fLineName)
		return true;
	else
		return false;
}

//////////////
// == operator
//////////////
bool T2DOffsetMeasurement::operator==(const T2DOffsetMeasurement& right) const{

/*	if (*fTargetLine == (*right.fTargetLine))
		return true;
	else
		return false; */
	if (fLineName == right.fLineName)
		return true;
	else
		return false;
}

/*
//////////////////////////////////////////
// returns an interator to the target wire
//////////////////////////////////////////
TWorkingTargetObjects::SpLineIterator T2DOffsetMeasurement::getTargetLine() const{

	return fTargetLine;
}
*/

///////////////////////////////////////
// returns the measured vertical offset
///////////////////////////////////////
TLength T2DOffsetMeasurement::getVertOffset() const{

	return fVertOffset;
}

///////////////////////////////////////////////////
// returns the measured vertical offset's precision
///////////////////////////////////////////////////
TLength T2DOffsetMeasurement::getVertSigma() const{

	return fVertSigma;
}

///////////////////////////////////////////
// returns the line's name
////////////////////////////
string T2DOffsetMeasurement::getLineName() const {

	return fLineName;
}

//////////////////////////////////
// sets the line's name
///////////////////////////
void T2DOffsetMeasurement::setLineName(string ln) {

	fLineName = ln;
}

////////////////////////////////////////////////
// sets the vertical sigma if it is still zero
////////////////////////////////////////////////
void T2DOffsetMeasurement::ifNotDoneSetVSigma(TLength sigma){

	if (fVertSigma.getMetresValue() == LITERAL(0.0))
		fVertSigma = sigma;
}


/////////////////////////////////////////
// returns the measured transverse offset
/////////////////////////////////////////
TLength T2DOffsetMeasurement::getHorOffset() const{

	return fHorOffset;
}

/////////////////////////////////////////////////////
// returns the measured transverse offset's precision
/////////////////////////////////////////////////////
TLength T2DOffsetMeasurement::getHorSigma() const{

	return fHorSigma;
}

////////////////////////////////////////////////
// sets the horizontal sigma if it is still zero
////////////////////////////////////////////////
void T2DOffsetMeasurement::ifNotDoneSetHSigma(TLength sigma){

	if (fHorSigma.getMetresValue() == LITERAL(0.0))
		fHorSigma = sigma;
}


///////////////////////////////////////////////////////
// returns the string representing the measurement kind
///////////////////////////////////////////////////////
string T2DOffsetMeasurement::getMeasKind() const{

	string s("2DOffsetMeas");
	return s;
}

/*
////////////////////////////////////////////////
// tells if the transverse offset meas is active
////////////////////////////////////////////////
bool T2DOffsetMeasurement::hActive() const{

	return fHUsedInCalc;
}

//////////////////////////////////////////////
// tells if the vertical offset meas is active
//////////////////////////////////////////////
bool T2DOffsetMeasurement::vActive() const{

	return fVUsedInCalc;
}


//////////////////////////////////
// activates the horizontal offset
//////////////////////////////////
void T2DOffsetMeasurement::activateHOffset(){

	fHUsedInCalc = true;
}

/////////////////////////////////////
// disactivates the transverse offset
/////////////////////////////////////
void T2DOffsetMeasurement::disactivateHOffset(){

	fHUsedInCalc = false;
}

//////////////////////////////////
// activates the vertical offset
//////////////////////////////////
void T2DOffsetMeasurement::activateVOffset(){

	fVUsedInCalc = true;
}

/////////////////////////////////////
// disactivates the vertical offset
/////////////////////////////////////
void T2DOffsetMeasurement::disactivateVOffset(){

	fVUsedInCalc = false;
}




///////////////////////////////////////////////////
// tells the parent ROM the measure is disactivated
///////////////////////////////////////////////////
void T2DOffsetMeasurement::sendDisactivatedToParent() {

	fParentROM->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

//////////////////////////////////////////////////
// tells the parent ROM the measure is Activated
//////////////////////////////////////////////////
void T2DOffsetMeasurement::sendActivatedToParent() {

	fParentROM->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}


///////////////////////////////////////////
// sets the target line using the line name
///////////////////////////////////////////
bool T2DOffsetMeasurement::setTargetLine(TWorkingTargetObjects* wto){

	///get the spatial line
	TWorkingTargetObjects::SpLineIterator iterSpl = wto->getSpLine(fLineName);
	TWorkingTargetObjects::SpLineIterator noSuchSpLine = wto->getSpLineEndIterator();

	//case where the wire already exists
	if (iterSpl != noSuchSpLine){
		fTargetLine = iterSpl;
		return true;
	}
	//case where the wire doesn't exist
	else
		return false;

}

//////////////////////////////////////////////////////////////////////////
// Adds this measurement to the relevant Dist Meas  station, using the latter's setup point's name
//////////////////////////////////////////////////////////////////////////
bool T2DOffsetMeasurement::addToStation(TWorkingPoints* wp, TWorkingStations* ws){

	//check if the station's setup point exists
	TWorkingPoints::PointIterator noSuchPoint = wp->getPointsEndIterator();
	TWorkingPoints::PointIterator iterStPt = wp->getPoint(fSetupPtName);

	if (iterStPt == noSuchPoint)
		return false;

	else{
		TWorkingStations::DistStIterator noSuchStation = ws->getDistStEndIterator();
		TWorkingStations::DistStIterator searchedStation = ws->getDistStation(iterStPt);
		//station doesn't already exist
		if (searchedStation == noSuchStation){

			TDistMeasStation* newDSt = new TDistMeasStation(ws, iterStPt);
			delete newDSt;
			searchedStation = ws->getDistStation(iterStPt);

			T2DOffsetROM* newOROM = new T2DOffsetROM(*searchedStation);
			delete newOROM;
			TDistMeasStation::Off2DROMIterator iterODrom
				= searchedStation->get2DOffsetROMBeginIterator();

			fParentROM = &(*iterODrom);

			fActiveMeasCount = 2;
			fActiveEqCount = 2;
			fActiveUnkCount = 0;

			
			T2DOffsetMeasurement* om = fParentROM->add2DOffset(this);
			fTargetLine->addListener(static_cast<TVMeasurableEntityLstnr*>(om));

			return true;
		}
		else{

			TDistMeasStation::Off2DROMIterator iterODrom
				= searchedStation->get2DOffsetROMBeginIterator();

			//case where the station has no rom yet
			if (iterODrom == searchedStation->get2DOffsetROMEndIterator()){
				T2DOffsetROM* newODROM = new T2DOffsetROM(*searchedStation);
				delete newODROM;
				iterODrom = searchedStation->get2DOffsetROMBeginIterator();
			}


			fParentROM = &(*iterODrom);

			fActiveMeasCount = 2;
			fActiveEqCount = 2;
			fActiveUnkCount = 0;

			
			T2DOffsetMeasurement* om = fParentROM->add2DOffset(this);
			fTargetLine->addListener(static_cast<TVMeasurableEntityLstnr*>(om));

			return true;
		}
	}
}

*/
