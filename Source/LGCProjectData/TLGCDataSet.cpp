////////////////////////////////////////////////////////////////////
// TLGCCalcParams.cpp : implementation file
// Container for the LGC project input data set
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////
//TLGCDataSet.cpp : implementation file
//Class for a LGC document model holding all the data



#include "TLGCDataSet.h"

#include "lsalgo/TLSInputMatrices.h"
#include "T3DLocalRefFrame.h"

////////////////////////////////////
//static attribut
////////////////////////////////////
bool TLGCDataSet::fUsedPDOR = false;



//////////////////////////
// no argument constructor
//////////////////////////
TLGCDataSet::TLGCDataSet(){

	TDataParameters dataParam;
	dataParam.setRefFrame(TRefSystemFactory::kCCS);
	dataParam.setCoordSys(TCoordSysFactory::k3DCartesian);
	dataParam.setLenUnits(TLength::kMetres);
	dataParam.setAngUnits(TAngle::kGons);
	dataParam.setCoordPrecision(TPointFormat::k10Micrometres);
	dataParam.setAnglePrecision(TObservationFormat::k10Microgons);
	dataParam.setLengthPrecision(TObservationFormat::k10Micrometres);
	setDataParams(dataParam);

	fUEOIndices.UIndex = 1;
	fUEOIndices.EIndex = 1;
	fUEOIndices.OIndex = 1;

	fWorkingStations = new TWorkingStations();
	fWorkingPoints = new TWorkingPoints();
	fWorkingConstants = new TWorkingConstants();
	fWorkingConstraints = new TWorkingConstraints();
	fWorkingInstruments = new TWorkingInstruments();
	fWorkingROMs = new TWorkingROMs();

	fNumCala = 0;
	fNumVx = 0;
	fNumVy = 0;
	fNumVz = 0;
	fNumVxy = 0;
	fNumVxz = 0;
	fNumVyz = 0;
	fNumVxyz = 0;

	fNumAngl = 0;
	fNumZend = 0;
	fNumDhor = 0;
	fNumDrdl = 0;
	fNumDspt = 0;
	fNumDver = 0;
	fNumDlev = 0;
	fNumECVE = 0;
	fNumECSP = 0;
	fNumECHO = 0;
	fNumECTH = 0;
	fNumOrie = 0;
	fNumRADI = 0;
	fNumPDOR = 0;


	noDuplicates = false;

	observationsCount = 0;
	unknownsCount = 0;
	
}


/////////////
// Destructor
/////////////
TLGCDataSet::~TLGCDataSet(){

	delete fWorkingPoints;
	delete fWorkingStations;
	delete fWorkingConstants;
/*	delete fWorkingTargetObjects;*/
	delete fWorkingConstraints;
	delete fWorkingInstruments;
	delete fWorkingROMs;

}


//////////////////////////////////////////////////////////////////////
// Static Member Functions
//////////////////////////////////////////////////////////////////////
bool TLGCDataSet::usedOrieCnstr()
{//return true if PDOR is used
	return fUsedPDOR;
}


///////////////////////////////////////////////////////
// gets the number of unknowns, equations, observations
///////////////////////////////////////////////////////
UEOIndices TLGCDataSet::getDimensions() const{

	UEOIndices ueoi = {(fUEOIndices.UIndex-1), (fUEOIndices.EIndex-1), (fUEOIndices.OIndex-1)};
	return ueoi;
}


///////////////////////////////////////////////////////////////////
// increment the number of read points for the corresponding status
///////////////////////////////////////////////////////////////////
void TLGCDataSet::addToPointNum(TSpatialStatus::ESpatialStatus status)
{
	switch (status){
	case TSpatialStatus::kCala:
		fNumCala += 1;
		break;
	case TSpatialStatus::kVx:
		fNumVx += 1;
		break;
	case TSpatialStatus::kVy:
		fNumVy += 1;
		break;
	case TSpatialStatus::kVz:
		fNumVz += 1;
		break;
	case TSpatialStatus::kVxy:
		fNumVxy += 1;
		break;
	case TSpatialStatus::kVxz:
		fNumVxz += 1;
		break;
	case TSpatialStatus::kVyz:
		fNumVyz += 1;
		break;
	case TSpatialStatus::kVxyz:
		fNumVxyz += 1;
		break;
	}

	return;

}


///////////////////////////////////////////////////////////////
// increment the number of read horizontal angles' measurements
///////////////////////////////////////////////////////////////
void TLGCDataSet::addToAnglNum(int count)
{
	fNumAngl += count;
	return;
}

////////////////////////////////////////////////////////////////
// increment the number of read zenithal distances' measurements
////////////////////////////////////////////////////////////////
void TLGCDataSet::addToZenDistNum(int count)
{
	fNumZend += count;
}

//////////////////////////////////////////////////////////////////
// increment the number of read horizontal distances' measurements
//////////////////////////////////////////////////////////////////
void TLGCDataSet::addToHorDistNum(int count)
{
	fNumDhor += count;
	return;
}

void TLGCDataSet::addToPolar(int count)
{
	fNumPolar += count;
}

///////////////////////////////////////////////////////////////
// increment the number of read spatial distances' measurements
///////////////////////////////////////////////////////////////
void TLGCDataSet::addToSpaDistNum(int count)
{
	fNumDrdl += count;
}


void TLGCDataSet::addToEDMSpaDistNum(int count)
{
	fNumDspt += count;
}

////////////////////////////////////////////////////////////////
// increment the number of read vertical distances' measurements
////////////////////////////////////////////////////////////////
void TLGCDataSet::addToVertDistNum(int count)
{
	fNumDver += count;
	return;
}


void TLGCDataSet::addToOffsetToVerLineNum(int count)
{//increment the number of read Offset to vertical line measurements
	fNumECVE += count;
	return;
}


void TLGCDataSet::addToOffsetToSpaLineNum(int count)
{//increment the number of read Offset to vertical line measurements
	fNumECSP += count;
	return;
}


void TLGCDataSet::addToOffsetToVerPlaneNum(int count)
{//increment the number of read Offset to vertical line measurements
	fNumECHO += count;
	return;
}


void TLGCDataSet::addToOffsetToTheoPlaneNum(int count)
{//increment the number of read Offset to vertical line measurements
	fNumECTH += count;
	return;
}


void TLGCDataSet::addToGyroOrieNum(int count)
{//increment the number of read Offset to vertical line measurements
	fNumOrie += count;
	return;
}


void TLGCDataSet::addToRadOffCnstrNum()
{//increment the number of read Offset to vertical line measurements
	fNumRADI += 1;
	return;
}


void TLGCDataSet::addToOrieCnstrNum()
{//increment the number of read Offset to vertical line measurements
	fNumPDOR += 1;
	fUsedPDOR = true;
	return;
}



////////////////////////////////////////////////////////////////
// return the number of read points for the corresponding status
////////////////////////////////////////////////////////////////
int TLGCDataSet::getPointsDimension(TSpatialStatus::ESpatialStatus status) const
{
	int i = 0;
	switch (status){
	case TSpatialStatus::kCala:
		i = fNumCala;
		break;
	case TSpatialStatus::kVx:
		i = fNumVx;
		break;
	case TSpatialStatus::kVy:
		i = fNumVy;
		break;
	case TSpatialStatus::kVz:
		i = fNumVz;
		break;
	case TSpatialStatus::kVxy:
		i = fNumVxy;
		break;
	case TSpatialStatus::kVxz:
		i = fNumVxz;
		break;
	case TSpatialStatus::kVyz:
		i = fNumVyz;
		break;
	case TSpatialStatus::kVxyz:
		i = fNumVxyz;
		break;
	}
	return i;
}

	
///////////////////////////////////////////////////////////
//return the number of read horizontal angles' measurements
///////////////////////////////////////////////////////////
int TLGCDataSet::getHADimension() const
{
	return fNumAngl;
}


////////////////////////////////////////////////////////////
//return the number of read zenithal distances' measurements
////////////////////////////////////////////////////////////
int TLGCDataSet::getZDDimension() const
{
	return fNumZend;
}

	

//////////////////////////////////////////////////////////////
//return the number of read horizontal distances' measurements
//////////////////////////////////////////////////////////////
int TLGCDataSet::getHDDimension() const
{
	return fNumDhor;
}


///////////////////////////////////////////////////////////
//return the number of read spatial distances' measurements
///////////////////////////////////////////////////////////
int TLGCDataSet::getSDDimension() const
{
	return fNumDrdl;
}


int TLGCDataSet::getEDMSDDimension() const
{
	return fNumDspt;
}


////////////////////////////////////////////////////////////
//return the number of read vertical distances' measurements
////////////////////////////////////////////////////////////
int TLGCDataSet::getVDDimension() const
{
	return fNumDver;
}


int TLGCDataSet::getOffsetToVerLineDimension() const
{//return the number of read ECVE measurements
	return fNumECVE;
}


int TLGCDataSet::getOffsetToSpaLineDimension() const
{//return the number of read ECSP measurements
	return fNumECSP;
}


int TLGCDataSet::getOffsetToVerPlaneDimension() const
{//return the number of read ECHO measurements
	return fNumECHO;
}


int TLGCDataSet::getOffsetToTheoPlaneDimension() const
{//return the number of read ECTH measurements
	return fNumECTH;
}


int TLGCDataSet::getGyroOrieDimension() const
{//return the number of read ORIE measurements
	return fNumOrie;
}


int TLGCDataSet::getRadOffCnstrDimension() const
{//return the number of read ORIE measurements
	return fNumRADI;
}


int TLGCDataSet::getOrieCnstrDimension() const
{//return the number of read ORIE measurements
	return fNumPDOR;
}

void TLGCDataSet::setDefaultRadOffCnstrSig(const bool b)
{
	fDftSigRADI = b;
	return;
}



////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////
