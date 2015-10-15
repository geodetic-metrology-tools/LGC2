//
//  TZenithDistMeasurement.cpp : implementation file
//
//



#include  "TZenithDistROM.h"
#include  "TZenithDistMeasurement.h"


//////////////////////
// default constructor
//////////////////////

TZenithDistMeasurement::TZenithDistMeasurement() : TAPointMeasurement(),
fMeasuredAngle(LITERAL(0.0)), fSigmaAPriori(LITERAL(0.0)), fPrismeHeight (LITERAL(0.0))
{
	fHeightStatus = kVariable;

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// constructor setting the station point name, the target point name, the observed angle and it's precision
///////////////////////////////////////////////////////////////////////////////////////////////////////////
TZenithDistMeasurement::TZenithDistMeasurement(TSpatialPointName tgName, TAngle obsAng, TAngle sigma):
TAPointMeasurement(tgName), fMeasuredAngle(obsAng), fSigmaAPriori(sigma)
{
	// if prisme heigth is not given, it is supposed to be variable
	fPrismeHeight.setMetresValue(LITERAL(0.0));
	fHeightStatus = kFixed;

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// constructor setting the station point name, the target point name, the observed angle and it's precision, and the prisme height
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TZenithDistMeasurement::TZenithDistMeasurement(TSpatialPointName tgName, TAngle obsAng, TAngle sigma, TLength hp):
TAPointMeasurement(tgName), fMeasuredAngle(obsAng), fSigmaAPriori(sigma)
{
	// if prisme heigth given, supposed to be fixed
	fPrismeHeight = hp;
	fHeightStatus = kFixed;

}


///////////////////
// copy constructor
///////////////////
TZenithDistMeasurement::TZenithDistMeasurement(const TZenithDistMeasurement& source)
{
	*this = source;	

}

/////////////
// destructor
/////////////
TZenithDistMeasurement::~TZenithDistMeasurement()
{

}



////////////////////////////
// copy assignement operator
////////////////////////////
TZenithDistMeasurement&  TZenithDistMeasurement::operator=(const TZenithDistMeasurement& source)
{
	if (this != &source)
	{
		fTargetPtName = source.fTargetPtName;
		fMeasuredAngle = source.getAngleValue();
		fSigmaAPriori = source.getSigma();
		fPrismeHeight = source.fPrismeHeight;
		fHeightStatus = source.fHeightStatus;
		fIdentifier = source.fIdentifier;
		fComment = source.fComment;
		fHeaderComment = source.fHeaderComment;
	}
	
	return (*this);
	
}


/////////////////////////////////////////////
// returns a non-const pointer to this object
/////////////////////////////////////////////
TZenithDistMeasurement* TZenithDistMeasurement::getPointer() const{

	return const_cast<TZenithDistMeasurement*>(this);
}



//////////////////////////////////////////////////////
// returns the string representing the measurement kind
//////////////////////////////////////////////////////
string TZenithDistMeasurement::getMeasKind() const{

	string s("ZENDIST");
	return s;
}


///////////////////////////////////////
// returns a copy of the measured angle
///////////////////////////////////////
TAngle TZenithDistMeasurement::getAngleValue() const{

	return fMeasuredAngle;
}


//////////////////////////////////////
// returns a copy of the a priori sigma
//////////////////////////////////////
TAngle TZenithDistMeasurement::getSigma() const{

	return fSigmaAPriori;
}


////////////////////////////
// returns the prisme height
////////////////////////////
TLength TZenithDistMeasurement::getPrismeHeight() const{

	return fPrismeHeight;
}


///////////////////////////////////
// returns the prisme height status
///////////////////////////////////
TAMeasurement::ECalcStatus TZenithDistMeasurement::getPrismeHeightStatus() const{

	return fHeightStatus;
}


/////////////////////////
// sets the prisme height
/////////////////////////
void TZenithDistMeasurement::setPrismeHeight(TLength hp){

	fPrismeHeight = hp;
	return;
}


///////////////////////////////////
// returns the prisme height status
///////////////////////////////////
void TZenithDistMeasurement::setPrismeHeightStatus(ECalcStatus status){

	fHeightStatus = status;
	return;
}

/////////////////////////////////////
// sets the sigma if it is still zero
/////////////////////////////////////
void TZenithDistMeasurement::ifNotDoneSetSigma(TAngle sigma){

	if (fSigmaAPriori.getRadiansValue() == LITERAL(0.0))
		fSigmaAPriori = sigma;
}





















//#include  "TWorkingStations.h"

/*
///////////////////////////////////////////////////////////////////////
//constructor setting the measured point and the measured angle's value
///////////////////////////////////////////////////////////////////////
TZenithDistMeasurement::TZenithDistMeasurement(TZenithDistROM& parent, TWorkingPoints::PointIterator iterTg, TAngle obsAngle, TAngle sigma):
fMeasuredAngle(obsAngle), fSigmaAPriori(sigma), TAMeasurement(&parent, iterTg), fParentROM(&parent){
	
	fActiveMeasCount = 1;
	fActiveEqCount = 1;
	fActiveUnkCount =0;

	TZenithDistMeasurement* zd = fParentROM->addZenithDist(this);
	fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(zd));

	if (zd != 0)
		zd->sendInitialMessages();
}
*/




/*
//////////////////////////////////////////////////////////////////////
// uses fStationPtName to add this measurement to the relevant station
//////////////////////////////////////////////////////////////////////
bool TZenithDistMeasurement::addToStation(TWorkingPoints* wp, TWorkingStations* ws)
{
	TWorkingPoints::PointIterator noSuchPoint = wp->getPointsEndIterator();
	TWorkingPoints::PointIterator setupPoint = wp->getPoint(fStPtName);

	if (setupPoint == noSuchPoint)
		return false;
	else{
		TWorkingStations::TheodStIterator noSuchStation = ws->getTheodStEndIterator();
		TWorkingStations::TheodStIterator searchedStation = ws->getTheodStation(setupPoint);

		if (searchedStation == noSuchStation){

			TTheodoliteStation* newTheodSt = new TTheodoliteStation(ws, setupPoint);
			delete newTheodSt;
			searchedStation = ws->getTheodStation(setupPoint);
	
			TZenithDistROM* newZDROM = new TZenithDistROM(*searchedStation);
			delete newZDROM;
			TTheodoliteStation::ZenithDistROMIterator iterZDRom = searchedStation->getZenithDistROMBeginIterator();

			fParentROM = &(*iterZDRom);
	
			fActiveMeasCount = 1;
			fActiveEqCount = 1;
			fActiveUnkCount = 0;

			TAMeasurement::fParent = static_cast< TVParentMeasTreeNode* >(fParentROM);

			TZenithDistMeasurement* zd = fParentROM->addZenithDist(this);
			fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(zd));

			return true;
		}

		else{

			TTheodoliteStation::ZenithDistROMIterator iterZDRom = searchedStation->getZenithDistROMBeginIterator();

			if (iterZDRom == searchedStation->getZenithDistROMEndIterator())
			{
				TZenithDistROM* newZDROM = new TZenithDistROM(*searchedStation);
				delete newZDROM;
				iterZDRom = searchedStation->getZenithDistROMBeginIterator();
			}


			fParentROM = &(*iterZDRom);

			fActiveMeasCount = 1;
			fActiveEqCount = 1;
			fActiveUnkCount = 0;

			TAMeasurement::fParent = static_cast< TVParentMeasTreeNode* >(fParentROM);
	
			TZenithDistMeasurement* zd = fParentROM->addZenithDist(this);
			fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(zd));

			return true;
		}
	}
}


///////////////////////////////////////////////////
// tells the parent ROM the measure is disactivated
///////////////////////////////////////////////////
void TZenithDistMeasurement::sendDisactivatedToParent() {

	fParentROM->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}


////////////////////////////////////////////////
// tells the parent ROM the measure is activated
////////////////////////////////////////////////
void TZenithDistMeasurement::sendActivatedToParent() {

	fParentROM->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

*/

