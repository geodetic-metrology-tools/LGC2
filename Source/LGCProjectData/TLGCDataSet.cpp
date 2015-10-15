////////////////////////////////////////////////////////////////////
// TLGCCalcParams.cpp : implementation file
// Container for the LGC project input data set
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////
//TLGCDataSet.cpp : implementation file
//Class for a LGC document model holding all the data



#include "TLGCDataSet.h"

#include "TLSInputMatrices.h"
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

	fNumCala = 0;
	fNumVx = 0;
	fNumVy = 0;
	fNumVz = 0;
	fNumVxy = 0;
	fNumVxz = 0;
	fNumVyz = 0;
	fNumVxyz = 0;

	fNumAngl = 0;
	fNumZeni = 0;
	fNumZenh = 0;
	fNumDhor = 0;
	fNumDmes = 0;
	fNumDthe = 0;
	fNumDver = 0;
	fNumDlev = 0;
	fNumECVE = 0;
	fNumECSP = 0;
	fNumECHO = 0;
	fNumECTH = 0;
	fNumOrie = 0;
	fNumRADI = 0;
	fNumPDOR = 0;

	fDftSigAngl = false;
	fDftSigZeni = false;
	fDftSigZenh = false;
	fDftSigDhor = false;
	fDftSigDmes = false;
	fDftSigDthe = false;
	fDftSigDver = false;
	fDftSigDlev = false;
	fDftSigECVE = false;
	fDftSigECSP = false;
	fDftSigECHO = false;
	fDftSigECTH = false;
	fDftSigOrie = false;
	fDftSigRADI = false;
	ferrorellipses = false;

	noDuplicates = false;
	
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

}


//////////////////////////////////////////////////////////////////////
// Static Member Functions
//////////////////////////////////////////////////////////////////////
const bool TLGCDataSet::usedOrieCnstr()
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
void TLGCDataSet::addToAnglNum()
{
	fNumAngl += 1;
	return;
}

////////////////////////////////////////////////////////////////
// increment the number of read zenithal distances' measurements
////////////////////////////////////////////////////////////////
void TLGCDataSet::addToZenDistNum(TTheodoliteStation* thSt)
{
	if (thSt->getInstHeightStatus() == TAMeasurement::kVariable)
	{
		fNumZeni += 1;
	}
	else
	{
		fNumZenh += 1;
	}
	return;
}

//////////////////////////////////////////////////////////////////
// increment the number of read horizontal distances' measurements
//////////////////////////////////////////////////////////////////
void TLGCDataSet::addToHorDistNum()
{
	fNumDhor += 1;
	return;
}

///////////////////////////////////////////////////////////////
// increment the number of read spatial distances' measurements
///////////////////////////////////////////////////////////////
void TLGCDataSet::addToSpaDistNum(TDistMeasStation* distSt)
{
	if (distSt->getInstHeightStatus() == TAMeasurement::kVariable)
	{
		fNumDthe += 1;
	}
	else
	{
		fNumDmes += 1;
	}
	return;
}

////////////////////////////////////////////////////////////////
// increment the number of read vertical distances' measurements
////////////////////////////////////////////////////////////////
void TLGCDataSet::addToVertDistNum(bool isDLEV)
{
	if (isDLEV)
		fNumDlev++;

	else
		fNumDver++;

	return;
}


void TLGCDataSet::addToOffsetToVerLineNum()
{//increment the number of read Offset to vertical line measurements
	fNumECVE += 1;
	return;
}


void TLGCDataSet::addToOffsetToSpaLineNum()
{//increment the number of read Offset to vertical line measurements
	fNumECSP += 1;
	return;
}


void TLGCDataSet::addToOffsetToVerPlaneNum()
{//increment the number of read Offset to vertical line measurements
	fNumECHO += 1;
	return;
}


void TLGCDataSet::addToOffsetToTheoPlaneNum()
{//increment the number of read Offset to vertical line measurements
	fNumECTH += 1;
	return;
}


void TLGCDataSet::addToGyroOrieNum()
{//increment the number of read Offset to vertical line measurements
	fNumOrie += 1;
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
	// Zero points by default
	int i(0);

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
int TLGCDataSet::getZDDimension(TAMeasurement::ECalcStatus status) const
{
	// Non-dimeansional by default
	int i(0);
	switch (status){
	case TAMeasurement::kFixed:
		i = fNumZenh;
		break;
	case TAMeasurement::kVariable:
		i = fNumZeni;
		break;
	}
	return i;
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
int TLGCDataSet::getSDDimension(TAMeasurement::ECalcStatus status) const
{
	// Non-dimeansional by default
	int i(0);
	switch (status){
	case TAMeasurement::kFixed:
		i = fNumDmes;
		break;
	case TAMeasurement::kVariable:
		i = fNumDthe;
		break;
	}
	return i;
}



////////////////////////////////////////////////////////////
//return the number of read vertical distances' measurements
////////////////////////////////////////////////////////////
int TLGCDataSet::getVDDimension(bool isDLEV) const
{
	return isDLEV ? fNumDlev : fNumDver;
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

///////////////////////////////////////////////////////////////////////////////////
//DEFAULT SIGMA
///////////////////////////////////////////////////////////////////////////////////
void TLGCDataSet::setDefaultAnglSig(const bool b)
{
	fDftSigAngl = b;
	return;
}


void TLGCDataSet::setDefaultZenDistSig(const bool b, TAMeasurement::ECalcStatus status)
{
	if (status == TAMeasurement::kVariable)
	{
		fDftSigZeni = b;
	}
	else
	{
		fDftSigZenh = b;
	}
	return;
}


void TLGCDataSet::setDefaultHorDistSig(const bool b)
{
	fDftSigDhor = b;
	return;
}


void TLGCDataSet::setDefaultSpaDistSig(const bool b, TAMeasurement::ECalcStatus status)
{
	if (status == TAMeasurement::kVariable)
	{
		fDftSigDthe = b;
	}
	else
	{
		fDftSigDmes = b;
	}
	return;
}


void TLGCDataSet::setDefaultVertDistSig(const bool isDLEV, const bool b)
{
	if (isDLEV)
		fDftSigDlev = b;
	else
		fDftSigDver = b;
	return;
}


void TLGCDataSet::setDefaultOffsetToVerLineSig(const bool b)
{
	fDftSigECVE = b;
	return;
}


void TLGCDataSet::setDefaultOffsetToSpaLineSig(const bool b)
{
	fDftSigECSP = b;
	return;
}


void TLGCDataSet::setDefaultOffsetToVerPlaneSig(const bool b)
{
	fDftSigECHO = b;
	return;
}


void TLGCDataSet::setDefaultOffsetToTheoPlaneSig(const bool b)
{
	fDftSigECTH = b;
	return;
}


void TLGCDataSet::setDefaultGyroOrieSig(const bool b)
{
	fDftSigOrie = b;
	return;
}


void TLGCDataSet::setDefaultRadOffCnstrSig(const bool b)
{
	fDftSigRADI = b;
	return;
}



bool TLGCDataSet::useDefaultSigHA() const
{
	return fDftSigAngl;
}


bool TLGCDataSet::useDefaultSigZD(TAMeasurement::ECalcStatus status) const
{
	// No default sigma as default
	bool b(false);
	switch (status){
	case TAMeasurement::kFixed:
		b = fDftSigZenh;
		break;
	case TAMeasurement::kVariable:
		b = fDftSigZeni;
		break;
	}
	return b;
}


bool TLGCDataSet::useDefaultSigHD() const
{
	return fDftSigDhor;
}


bool TLGCDataSet::useDefaultSigSD(TAMeasurement::ECalcStatus status) const
{
	// No default sigma as default
	bool b(false);
	switch (status){
	case TAMeasurement::kFixed:
		b = fDftSigDmes;
		break;
	case TAMeasurement::kVariable:
		b = fDftSigDthe;
		break;
	}
	return b;
}


bool TLGCDataSet::useDefaultSigVD(bool isDLEV) const
{
	return isDLEV ? fDftSigDlev : fDftSigDver;
}


bool TLGCDataSet::useDefaultSigECVE() const
{
	return fDftSigECVE;
}


bool TLGCDataSet::useDefaultSigECSP() const
{
	return fDftSigECSP;
}


bool TLGCDataSet::useDefaultSigECHO() const
{
	return fDftSigECHO;
}


bool TLGCDataSet::useDefaultSigECTH() const
{
	return fDftSigECTH;
}


bool TLGCDataSet::useDefaultSigORIE() const
{
	return fDftSigOrie;
}


bool TLGCDataSet::useDefaultSigRADI() const
{
	return fDftSigRADI;
}

////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////