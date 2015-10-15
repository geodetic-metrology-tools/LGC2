// TLSLocalSys2DOffMeasCG.cpp
//
/** Concrete Class for a LS contrib generator processing 2D offset measurements
 defined in a local system */
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
#include "TLSInputMatrices.h"
#include  "TLSLocalSys2DOffMeasCG.h"
////////////////////////////////////////////////////////////////


//ClassImp(TLSLocalSys2DOffMeasCG)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TLSLocalSys2DOffMeasCG::TLSLocalSys2DOffMeasCG()
{	// default constructor

}

/////////////////////////////////////////
// constructor setting the input matrices
/////////////////////////////////////////
TLSLocalSys2DOffMeasCG::TLSLocalSys2DOffMeasCG(TLSInputMatrices* lsim):
TALS2DOffMeasContribGenerator(lsim){

}

// **This needs to be implemented**
TLSLocalSys2DOffMeasCG::TLSLocalSys2DOffMeasCG( const  TLSLocalSys2DOffMeasCG& original ):
TALS2DOffMeasContribGenerator(original)
{	// copy constructor

}


TLSLocalSys2DOffMeasCG::~TLSLocalSys2DOffMeasCG()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
// **This needs to be implemented**
TLSLocalSys2DOffMeasCG&  TLSLocalSys2DOffMeasCG::operator=(const TLSLocalSys2DOffMeasCG& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		fInputMatrices = right.fInputMatrices;
		fS0APrioriScaleFactor = right.fS0APrioriScaleFactor;
		fCurrentStation = right.fCurrentStation;
		fCurrentROM = right.fCurrentROM;
	}
	return *this;
}


//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// calculates and adds the offset measurement's contributions to the matrices
//////////////////////////////////////////////////////////////////////////
void TLSLocalSys2DOffMeasCG::process2DOffsetMeas(TLSCalc2DOffsetROM::Calc2DOffsetIterator om){

	// gets the station's (provisional) coordinates, status and indices
	double xSt, ySt, zSt;
	MatrixIndex xStInd, yStInd, zStInd;
	TSpatialPosition::ECoordStatus stsxSt, stsySt, stszSt;

	bool xStk = ((fCurrentStation->getStationedPoint())->getProvPosition())->knownXCoord();
	bool yStk = ((fCurrentStation->getStationedPoint())->getProvPosition())->knownYCoord();
	bool zStk = ((fCurrentStation->getStationedPoint())->getProvPosition())->knownZCoord();

	if (xStk){
		xStInd = (fCurrentStation->getStationedPoint())->getXIndex();
		xSt = (((fCurrentStation->getStationedPoint())->getProvPosition())->getXCoord()).getMetresValue();
		stsxSt = ((fCurrentStation->getStationedPoint())->getProvPosition())->getXCoordStatus();
	}
	if (yStk){
		yStInd = (fCurrentStation->getStationedPoint())->getYIndex();
		ySt = (((fCurrentStation->getStationedPoint())->getProvPosition())->getYCoord()).getMetresValue();
		stsySt = ((fCurrentStation->getStationedPoint())->getProvPosition())->getYCoordStatus();
	}
	if (zStk){
		zStInd = (fCurrentStation->getStationedPoint())->getZIndex();
		zSt = (((fCurrentStation->getStationedPoint())->getProvPosition())->getZCoord()).getMetresValue();
		stszSt = ((fCurrentStation->getStationedPoint())->getProvPosition())->getZCoordStatus();
	}

	// gets the spatial line's first end's coordinates, status and indices
	double xE1, yE1, zE1;
	MatrixIndex xE1Ind, yE1Ind, zE1Ind;
	TSpatialPosition::ECoordStatus stsxE1, stsyE1, stszE1;

	TLSCalcWorkingPoints::CalcPtConstIter iE1 = om->getSpLine()->getFirstEnd();

	bool xE1k = iE1->getProvPosition()->knownXCoord();
	bool yE1k = iE1->getProvPosition()->knownYCoord();
	bool zE1k = iE1->getProvPosition()->knownZCoord();

	if (xE1k){
		xE1Ind = iE1->getXIndex();
		xE1 = (iE1->getProvPosition()->getXCoord()).getMetresValue();
		stsxE1 = iE1->getProvPosition()->getXCoordStatus();
	}
	if (yE1k){
		yE1Ind = iE1->getYIndex();
		yE1 = (iE1->getProvPosition()->getYCoord()).getMetresValue();
		stsyE1 = iE1->getProvPosition()->getYCoordStatus();
	}
	if (zE1k){
		zE1Ind = iE1->getZIndex();
		zE1 = (iE1->getProvPosition()->getZCoord()).getMetresValue();
		stszE1 = iE1->getProvPosition()->getZCoordStatus();
	}


	// gets the wire's second end's coordinates, status and indices
	double xE2, yE2, zE2;
	MatrixIndex xE2Ind, yE2Ind, zE2Ind;
	TSpatialPosition::ECoordStatus stsxE2, stsyE2, stszE2;

	TLSCalcWorkingPoints::CalcPtConstIter iE2 = om->getSpLine()->getSecondEnd();

	bool xE2k = iE2->getProvPosition()->knownXCoord();
	bool yE2k = iE2->getProvPosition()->knownYCoord();
	bool zE2k = iE2->getProvPosition()->knownZCoord();

	if (xE2k){
		xE2Ind = iE2->getXIndex();
		xE2 = (iE2->getProvPosition()->getXCoord()).getMetresValue();
		stsxE2 = iE2->getProvPosition()->getXCoordStatus();
	}
	if (yE2k){
		yE2Ind = iE2->getYIndex();
		yE2 = (iE2->getProvPosition()->getYCoord()).getMetresValue();
		stsyE2 = iE2->getProvPosition()->getYCoordStatus();
	}
	if (zE2k){
		zE2Ind = iE2->getZIndex();
		zE2 = (iE2->getProvPosition()->getZCoord()).getMetresValue();
		stszE2 = iE2->getProvPosition()->getZCoordStatus();
	}



	// gets the observations' indices
	//Horizontal Offset
	MatrixIndex hOffEInd = om->getHOEqnIndex();
	MatrixIndex hOffOInd = om->getHOObsIndex();

	//Vertical Offset
	MatrixIndex vOffEInd = om->getVOEqnIndex();
	MatrixIndex vOffOInd = om->getVOObsIndex();

	
	//Checks if the contributions of the horizontal offset can be calculated
	// if yes, calculates them

	//***
	// Transverse Offset Measurement
	//***
	if (xStk && yStk && xE1k && yE1k && xE2k && yE2k && (hOffOInd!=0)){

		//calculated offset
		double numerator = (xE2 - xE1)*(ySt - yE1) - (yE2 - yE1)*(xSt - xE1);
		double d12h = dist(TLength(xE1), TLength(yE1), TLength(xE2), TLength(yE2));
		double calcHOff = numerator/d12h;


		//**gets the observation's provisinal value
		//checks if the observed value has to be simulated
		double sigmaHOff = (om->getHOffsetSigmaAPriori()).getMetresValue();
		double obsHOff;
		if (fSimulate && (om->firstIteration())){
			obsHOff = getSimulatedValue(calcHOff, sigmaHOff);
			TLength temp(obsHOff);
			om->setSimulatedObsHOffset(temp);
		}
		else
			obsHOff = (om->getObsHorOffset()).getMetresValue();
		

		
		//** Adding the contributions to the first desing matrix
		// xSt coefficient
		if (stsxSt == TANumericValue::kVariable){
			double a = -(yE2 - yE1)/d12h;
			fInputMatrices->setFirstDgnMtrxElement(hOffOInd, xStInd, a);
		}

		//ySt coefficient
		if (stsySt == TANumericValue::kVariable){
			double b = (xE2 - xE1)/d12h;
			fInputMatrices->setFirstDgnMtrxElement(hOffOInd, yStInd,b);
		}

		//xE1 coefficient
		if (stsxE1 == TANumericValue::kVariable){
			double c = (yE2 - ySt)/d12h	+ (xE2 - xE1)*numerator/pow(d12h,3);
			fInputMatrices->setFirstDgnMtrxElement(hOffOInd, xE1Ind,c);
		}

		//yE1 coefficient
		if (stsyE1 == TANumericValue::kVariable){
			double d = (xSt - xE2)/d12h	+ (yE2 - yE1)*numerator/pow(d12h,3);
			fInputMatrices->setFirstDgnMtrxElement(hOffOInd, yE1Ind,d);
		}

		//xE2 coefficient
		if (stsxE2 == TANumericValue::kVariable){
			double e = (ySt - yE1)/d12h	- (xE2 - xE1)*numerator/pow(d12h,3);
			fInputMatrices->setFirstDgnMtrxElement(hOffOInd, xE2Ind,e);
		}

		//yE2 coefficient
		if (stsyE2 == TANumericValue::kVariable){
			double f = (xE1 - xSt)/d12h	- (yE2 - yE1)*numerator/pow(d12h,3);
			fInputMatrices->setFirstDgnMtrxElement(hOffOInd, yE2Ind,f);
		}


		//** Adding the contribution to the second design matrix
		fInputMatrices->setSecondDgnMtrxElement(hOffEInd, hOffOInd, -1);


		//** setting the misclosure vector element
		double k = calcHOff - obsHOff;
		fInputMatrices->setMisclosureVectorElement(hOffOInd, k);

		//** adding the contributions to the weight matrix
		fInputMatrices->setWeightMtrxElement(hOffOInd, hOffOInd, pow(fS0APrioriScaleFactor,2)/pow(sigmaHOff,2));
	}
	else
		cerr << "2DOffsetMeas : horizontal offset contributions can't be calculated : not enough known coordinates\n";



	//Checks if the contributions of the vertical offset can be calculated
	// if yes, calculates them

	//***
	// Vertical Offset Measurement
	//***
	if (xStk && zStk && xE1k && zE1k && xE2k && zE2k && (vOffOInd!=0)){

		//calculated offset
		double numerator = (xE2 - xE1)*(zSt - zE1) - (zE2 - zE1)*(xSt - xE1);
		double d12v = dist(TLength(xE1), TLength(zE1), TLength(xE2), TLength(zE2));
		double calcVOff = numerator/d12v;


		//**gets the observation's provisinal value
		//checks if the observed value has to be simulated
		double sigmaVOff = (om->getVOffsetSigmaAPriori()).getMetresValue();
		double obsVOff;
		if (fSimulate && (om->firstIteration())){
			obsVOff = getSimulatedValue(calcVOff, sigmaVOff);
			TLength temp(obsVOff);
			om->setSimulatedObsVOffset(temp);
		}
		else
			obsVOff = (om->getObsVertOffset()).getMetresValue();
		

		//** Adding the contributions to the first desing matrix
		// xSt coefficient
		if (stsxSt == TANumericValue::kVariable){
			double a = -(zE2 - zE1)/d12v;
			fInputMatrices->setFirstDgnMtrxElement(vOffOInd, xStInd, a);
		}

		//zSt coefficient
		if (stszSt == TANumericValue::kVariable){
			double b = (xE2 - xE1)/d12v;
			fInputMatrices->setFirstDgnMtrxElement(vOffOInd, zStInd,b);
		}

		//xE1 coefficient
		if (stsxE1 == TANumericValue::kVariable){
			double c = (zE2 - zSt)/d12v	+ (xE2 - xE1)*numerator/pow(d12v,3);
			fInputMatrices->setFirstDgnMtrxElement(vOffOInd, xE1Ind,c);
		}

		//zE1 coefficient
		if (stszE1 == TANumericValue::kVariable){
			double d = (xSt - xE2)/d12v	+ (zE2 - zE1)*numerator/pow(d12v,3);
			fInputMatrices->setFirstDgnMtrxElement(vOffOInd, zE1Ind,d);
		}

		//xE2 coefficient
		if (stsxE2 == TANumericValue::kVariable){
			double e = (zSt - zE1)/d12v	- (xE2 - xE1)*numerator/pow(d12v,3);
			fInputMatrices->setFirstDgnMtrxElement(vOffOInd, xE2Ind,e);
		}

		//zE2 coefficient
		if (stszE2 == TANumericValue::kVariable){
			double f = (xE1 - xSt)/d12v	- (zE2 - zE1)*numerator/pow(d12v,3);
			fInputMatrices->setFirstDgnMtrxElement(vOffOInd, zE2Ind,f);
		}


		//** Adding the contribution to the second design matrix
		fInputMatrices->setSecondDgnMtrxElement(vOffEInd, vOffOInd, -1);


		//** setting the misclosure vector element
		double k = calcVOff - obsVOff;
		fInputMatrices->setMisclosureVectorElement(vOffOInd, k);

		//** adding the contributions to the weight matrix
		fInputMatrices->setWeightMtrxElement(vOffOInd, vOffOInd, pow(fS0APrioriScaleFactor,2)/pow(sigmaVOff,2));
	}
//	else
//		cerr << "2DOffsetMeas : vertical offset contributions can't be calculated : not enough known coordinates\n";

}