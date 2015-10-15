// 
// TSpatialDistMeas.cpp : implementation file
//
// Class for a EDM dist measurement as defined for survey purposes
//
//#include "TWorkingStations.h"
#include "TSpatialDistROM.h"
#include "TSpatialDistMeasurement.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////
// default constructor
//////////////////////
TSpatialDistMeasurement::TSpatialDistMeasurement():
TAPointMeasurement(),
fObservedDist(LITERAL(0.0)), fSigmaAPriori(0), fSigmaPpm(0),
fPrismHeight(0), fPrismNumber(""), fMeasConstant(){

	fHeightStatus = kFixed;

}


/////////////////////////////////////////////////////////////////////////////////////////
// constructor storing the target point's name, the observed distance value and its sigma
/////////////////////////////////////////////////////////////////////////////////////////
TSpatialDistMeasurement::TSpatialDistMeasurement(TSpatialPointName targetName, TLength obsDist, TLength sigma/*, PrismNb pn*/):
TAPointMeasurement(targetName),fObservedDist(obsDist), fSigmaAPriori(sigma), fSigmaPpm(0)/*, fPrismNumber(pn)*/, fMeasConstant(){

	fPrismHeight.setMetresValue(LITERAL(0.0));
	fHeightStatus = kFixed;

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// constructor storing the target point's name, the observed distance value and its sigma, and the prism height
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSpatialDistMeasurement::TSpatialDistMeasurement(TSpatialPointName targetName, TLength obsDist, TLength sigma, TLength height, PrismNb pn):
TAPointMeasurement(targetName),fObservedDist(obsDist), fSigmaAPriori(sigma), fSigmaPpm(0), fPrismHeight(height), fPrismNumber(pn),
fMeasConstant(){

	fHeightStatus = kFixed;

}

//////////////////////
// Destructor
/////////////////////

TSpatialDistMeasurement::~TSpatialDistMeasurement()
{	

}


///////////////////
// copy constructor
///////////////////
TSpatialDistMeasurement::TSpatialDistMeasurement(const TSpatialDistMeasurement& source)
{
	*this = source;
}

////////////////////////////
// copy assignement operator
////////////////////////////
TSpatialDistMeasurement& TSpatialDistMeasurement::operator=(const TSpatialDistMeasurement& source){

	if (this != &source)
	{
		fObservedDist = source.getDistValue();
		fSigmaAPriori = source.getSigma();
		fSigmaPpm = source.fSigmaPpm;

		fPrismHeight = source.fPrismHeight;
		fPrismNumber = source.fPrismNumber;
		fHeightStatus = source.fHeightStatus;

		fTargetPtName = source.fTargetPtName;
		fIdentifier = source.fIdentifier;
		fComment = source.fComment;
		fHeaderComment = source.fHeaderComment;

		fMeasConstant = source.fMeasConstant;
	}

	return (*this);
}

///////////////////////////////////////////////////////
// returns the string representing the measurement kind
///////////////////////////////////////////////////////
string TSpatialDistMeasurement::getMeasKind() const{

	string s("SpatialDist");
	return s;
}


////////////////////////////
// returns the observed dist
////////////////////////////
TLength TSpatialDistMeasurement::getDistValue() const{

	return fObservedDist;
}

/////////////////////////////
// returns the sigma a priori
/////////////////////////////
TLength TSpatialDistMeasurement::getSigma() const{

	return fSigmaAPriori;
}

////////////////////////
// returns the sigma ppm
////////////////////////
TLength TSpatialDistMeasurement::getSigmaPpm() const{

	return fSigmaPpm;
}



///////////////////////////
// returns the prism number
///////////////////////////
PrismNb TSpatialDistMeasurement::getPrismNumber() const{
	
	return fPrismNumber;
}

///////////////////////////
// returns the prism height
///////////////////////////
TLength TSpatialDistMeasurement::getPrismHeight() const{
	
	return fPrismHeight;
}

//////////////////////////////////
// returns the prism height status
//////////////////////////////////
TAMeasurement::ECalcStatus TSpatialDistMeasurement::getPrismHeightStatus() const{
	
	return fHeightStatus;
}


/////////////////////////////////////
// sets the sigma if it is still zero
/////////////////////////////////////
void TSpatialDistMeasurement::ifNotDoneSetSigma(TLength sigma){

	if (fSigmaAPriori.getMetresValue() == LITERAL(0.0))
		fSigmaAPriori = sigma;
}


/////////////////////////////////////////
// sets the sigma ppm if it is still zero
/////////////////////////////////////////
void TSpatialDistMeasurement::ifNotDoneSetSigmaPpm(TLength sigmaPpm){

	if (fSigmaPpm.getMetresValue() == LITERAL(0.0))
		fSigmaPpm = sigmaPpm;
}

////////////////////////////////////////////////////
// sets the measurement constant if it is still zero
////////////////////////////////////////////////////
void TSpatialDistMeasurement::ifNotDoneSetMeasConst(const TDistConstants& cte){

	if (fMeasConstant.getValue().getMetresValue() == LITERAL(0.0))
		fMeasConstant = cte;
}

//////////////////////////
// sets a the prism height
//////////////////////////
void TSpatialDistMeasurement::setPrismHeight(TLength hp){

	fPrismHeight = hp;
}

//////////////////////////
// sets a the prism height
//////////////////////////
void TSpatialDistMeasurement::setPrismHeightStatus(ECalcStatus status){

	fHeightStatus = status;
}

/*
//////////////////////////////////////////////////////////////////////////
// constructor setting the links to the parent rom and to the target point
//////////////////////////////////////////////////////////////////////////
TSpatialDistMeasurement::TSpatialDistMeasurement(TSpatialDistROM& parent, TWorkingPoints::PointIterator iterPt, TLength obsDist, TLength sigma, TLength height, PrismNb pn)
:fObservedDist(obsDist), fSigmaAPriori(sigma), TAMeasurement(&parent, iterPt* targetName), fPrismHeight(height), fPrismNumber(pn){

	fActiveMeasCount = 1;
	fActiveEqCount = 1;
	fActiveUnkCount = 0;
	
	fParentROM = &parent;

	TSpatialDistMeasurement* sd = fParentROM->addSpatialDist(this);
	fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(sd));

	fTargetPtName = fTargetPoint->getName();

	//WARNING
	// The station's name is not set (for the moment) in this constructor
	
	if (sd != 0)
		sd->sendInitialMessages(); 
}




///////////////////////////////////////////////////////
// tells the parent ROM the measurement is disactivated
///////////////////////////////////////////////////////
void TSpatialDistMeasurement::sendDisactivatedToParent() {

	fParentROM->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

//////////////////////////////////////////////////////
// tells the parent ROM the measurement is Activated
//////////////////////////////////////////////////////
void TSpatialDistMeasurement::sendActivatedToParent() {

	fParentROM->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}



///////////////////////////////////////////////////////////////
// uses fStationPtName to add this meas to the relevant station
///////////////////////////////////////////////////////////////
bool TSpatialDistMeasurement::addToStation(TWorkingPoints* wp, TWorkingStations* ws){

	//check if the station point exists
	TWorkingPoints::PointIterator noSuchPoint = wp->getPointsEndIterator();
	TWorkingPoints::PointIterator setupPoint = wp->getPoint(fStationPtName);

	if ( setupPoint == noSuchPoint)
		return false;
	else{
		TWorkingStations::DistStIterator noSuchStation = ws->getDistStEndIterator();
		TWorkingStations::DistStIterator searchedStation = ws->getDistStation(setupPoint);
		//station doesn't already exist
		if (searchedStation == noSuchStation){

			TDistMeasStation* newDSt = new TDistMeasStation(ws, setupPoint);
			delete newDSt;
			searchedStation = ws->getDistStation(setupPoint);

			TSpatialDistROM* newDROM = new TSpatialDistROM(*searchedStation);
			delete newDROM;
			TDistMeasStation::SpatialDistROMIterator iterSDrom
				= searchedStation->getSpatialDistROMBeginIterator();

			fParentROM = &(*iterSDrom);

			fActiveMeasCount = 1;
			fActiveEqCount = 1;
			fActiveUnkCount = 0;

			TAMeasurement::fParent = static_cast< TVParentMeasTreeNode* >(fParentROM);

			TSpatialDistMeasurement* sd = fParentROM->addSpatialDist(this);
			fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(sd));

			return true;
		}
		else{

			TDistMeasStation::SpatialDistROMIterator iterSDrom
				= searchedStation->getSpatialDistROMBeginIterator();

			//case where the station has no rom yet
			if (iterSDrom == searchedStation->getSpatialDistROMEndIterator()){
				TSpatialDistROM* newSDROM = new TSpatialDistROM(*searchedStation);
				delete newSDROM;
				iterSDrom = searchedStation->getSpatialDistROMBeginIterator();
			}


			fParentROM = &(*iterSDrom);

			fActiveMeasCount = 1;
			fActiveEqCount = 1;
			fActiveUnkCount = 0;

			TAMeasurement::fParent = static_cast< TVParentMeasTreeNode* >(fParentROM);

			TSpatialDistMeasurement* sd = fParentROM->addSpatialDist(this);
			fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(sd));

			return true;
		}
	}
}

*/




