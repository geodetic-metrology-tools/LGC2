// TVerticalDistMeasurement.cpp
//
/** Class for a vertical distance measurement */
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
//#include "TWorkingStations.h"
//#include "TVerticalDistROM.h"
#include "TVerticalDistMeasurement.h"
////////////////////////////////////////////////////////////////


//ClassImp(TVerticalDistMeasurement)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////
// default constructor
//////////////////////
TVerticalDistMeasurement::TVerticalDistMeasurement():
TAPointMeasurement() ,fMeasuredDist(LITERAL(0.0)),fSigmaAPriori(0), fMeasConstant(){

}


//////////////////////////////////////////////////////////////////////////////////////////
// constructor setting the target and reference names, the measured dist and its precision
//////////////////////////////////////////////////////////////////////////////////////////
TVerticalDistMeasurement::TVerticalDistMeasurement(TSpatialPointName tgName, TLength obsDist,
												   TLength sigma):
TAPointMeasurement(tgName),fMeasuredDist(obsDist), fSigmaAPriori(sigma), fMeasConstant(){

}

/////////////
// Destructor
/////////////
TVerticalDistMeasurement::~TVerticalDistMeasurement()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


// **This needs to be implemented**
////////////////////////////
// Copy assignement operator
////////////////////////////
TVerticalDistMeasurement&  TVerticalDistMeasurement::operator=(const TVerticalDistMeasurement& right)
{	

	if (this != &right)
	{
		fMeasuredDist = right.getVDistValue();
		fSigmaAPriori = right.getSigma();
		fTargetPtName = right.fTargetPtName;
		fMeasConstant = right.fMeasConstant;

		fIdentifier = right.fIdentifier;
		fComment = right.fComment;
		fHeaderComment = right.fHeaderComment;
	}
	return *this;
}


//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// returns the string representing the measurement kind
///////////////////////////////////////////////////////
string TVerticalDistMeasurement::getMeasKind() const{

	string s("VERTDIST");
	return s;
}


//////////////////////////////////////////
// returns a copy of the measured distance
//////////////////////////////////////////
TLength TVerticalDistMeasurement::getVDistValue() const{

	return fMeasuredDist;
}

///////////////////////////////////////
// returns a copy of the a priori sigma
///////////////////////////////////////
TLength TVerticalDistMeasurement::getSigma() const{

	return fSigmaAPriori;
}


/////////////////////////////////////
// sets the sigma if it is still zero
/////////////////////////////////////
void TVerticalDistMeasurement::ifNotDoneSetSigma(TLength sigma){

	if (fSigmaAPriori.getMetresValue() == LITERAL(0.0))
		fSigmaAPriori = sigma;
}

////////////////////////////////////////////////////
// sets the measurement constant if it is still zero
////////////////////////////////////////////////////
void TVerticalDistMeasurement::ifNotDoneSetMeasConst(const TDistConstants& cte){

	if (fMeasConstant.getValue().getMetresValue() == LITERAL(0.0))
		fMeasConstant = cte;
}













/*
//////////////
// Constructor
//////////////
TVerticalDistMeasurement::TVerticalDistMeasurement(TVerticalDistROM& parentROM, TWorkingPoints::PointIterator iterTg, TLength obsDist, TLength sigma):
TAMeasurement(&parentROM, iterTg), fMeasuredDist(obsDist), fSigmaAPriori(sigma), fParentROM(&parentROM)
{	
	fActiveMeasCount = 1;
	fActiveEqCount = 1;
	fActiveUnkCount = 0;


	TVerticalDistMeasurement* vd = fParentROM->addVertDist(this);
	fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(vd));

	if (ha != 0)
		ha->sendInitialMessages();
}
*/
/*
//////////////////////////////////////////////////////////////
// uses fRefPtName to add this measurement to the relevant ROM
//////////////////////////////////////////////////////////////
bool TVerticalDistMeasurement::addToROM(TWorkingPoints* wp, TWorkingStations* ws){

	TWorkingPoints::PointIterator noSuchPoint = wp->getPointsEndIterator();
	TWorkingPoints::PointIterator refPoint = wp->getPoint(fRefPtName);

	//check if the reference point already exists
	if (refPoint == noSuchPoint)
		return false;
	else{

		//check if a station has a rom referenced from the ref point
		TWorkingStations::LevelStIterator levelSt = ws->getLevelStBeginIterator();
		TWorkingStations::LevelStIterator levelStEnd = ws->getLevelStEndIterator();

		bool romFound = false;
		TLevelStation::VertDistROMIterator vdROM;

		while ((levelSt != levelStEnd) && (!romFound)){

			vdROM = levelSt->getVertDistROM(refPoint);
			if (vdROM != levelSt->getVertDistROMEndIterator())
				romFound = true;
			else
				levelSt++;
		}

		//such a ROM already exists
		if (romFound){

			fParentROM = &(*vdROM);

			fActiveMeasCount = 1;
			fActiveEqCount = 1;
			fActiveUnkCount = 0;

			TVerticalDistMeasurement* vd = fParentROM->addVertDist(this);
			fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(vd));

			return true;
		}
		//if not the ROM and possibly the first station are created
		else{
			//if no station exists, one is created
			if (ws->numberOfLevelStations() == 0){

				//station creation
				TLevelStation* lSt = new TLevelStation(ws);
				delete lSt;

				levelSt = ws->getLevelStBeginIterator();

				//VertDistROM creation
				TVerticalDistROM* vdr = new TVerticalDistROM(*levelSt, refPoint);
				delete vdr;

				vdROM = levelSt->getVertDistROMBeginIterator();

				//measurement creation
				fParentROM = &(*vdROM);

				fActiveMeasCount = 1;
				fActiveEqCount = 1;
				fActiveUnkCount = 0;
	
				TVerticalDistMeasurement* vd = fParentROM->addVertDist(this);
				fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(vd));
	
				return true;
			}
			//if a station already exists, the rom is created and added to the first station
			else{

				levelSt = ws->getLevelStBeginIterator();

				//VertDistROM creation
				TVerticalDistROM* vdr = new TVerticalDistROM(*levelSt, refPoint);
				delete vdr;

				vdROM = --(levelSt->getVertDistROMEndIterator());

				//measurement creation
				fParentROM = &(*vdROM);

				fActiveMeasCount = 1;
				fActiveEqCount = 1;
				fActiveUnkCount = 0;
	
				TVerticalDistMeasurement* vd = fParentROM->addVertDist(this);
				fTargetPoint->addListener(static_cast<TVSpatialPtListener*>(vd));
	
				return true;
			}
		}
	}
}



///////////////////////////////////////////////////
// tells the parent ROM the measure is disactivated
///////////////////////////////////////////////////
void TVerticalDistMeasurement::sendDisactivatedToParent() {

	fParentROM->childDisactivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

//////////////////////////////////////////////////
// tells the parent ROM the measure is Activated
//////////////////////////////////////////////////
void TVerticalDistMeasurement::sendActivatedToParent() {

	fParentROM->childActivated(fActiveMeasCount, fActiveEqCount, fActiveUnkCount);

}

*/