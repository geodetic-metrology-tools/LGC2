////////////////////////////////////////////////////////////////////
// TinputMatricesFiller.cpp : implementation file
// Class responsible for reading the least squares calculationmeasurements and constraints 
// and filling the least squares matrices
// Specific to a least squares calculation
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////


#include <list>

#include "TLSInputMatricesFiller.h"

#include "TLSInputMatrices.h"
#include "LSCalcDataSet.h"

#include "TLGCObsLSContributionGenerator.h"
#include "TLSFreeCnstrCG.h"

#include "TModifiedLocalAstronomicalRF.h"
#include "TARefFrameTransformation.h"
#include "TGC2MLATransformation.h"
#include "TMLA2GCTransformation.h"

#include "TLGCDataSet.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////////////////
TLSInputMatricesFiller::TLSInputMatricesFiller(TRefSystemFactory::ERefFrame refSurface, bool pdor)
: fGenerator(refSurface)
{//constructor setting the pointer to the contrib. generators factory

	fError = "";
	fIsPdorBearingDefined = pdor;
}


TLSInputMatricesFiller::~TLSInputMatricesFiller()
{//Destructor
//	delete fCGFactory;	
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////
void TLSInputMatricesFiller::initMatriceDimension(const LSCalcDataSet& data, TLSInputMatrices& matrices)
{
	// Update and extraction of the observations & parameters indices
	UEOIndices ueoi = data.getDimensions();
	if ((ueoi.EIndex == 0))
	{
		fError += "Equation index is null\n";
	}
	if ((ueoi.OIndex == 0))
	{
		fError += "Observation index is null\n";
	}
/*	if ((ueoi.UIndex == 0))
	{
		fError += "Unknown index is null\n";
	}*/

	int cnstrObs;
	if (TLGCDataSet::usedOrieCnstr())
	{
		cnstrObs = 1;
	}
	else
	{
		cnstrObs = 0;
	}

	if(data.isLibrOptionUsed())
	{
		int cnstrNumber = data.getFreeConstraints().getNumberOfConstraint();
		if (cnstrNumber != 0)
		{
			// Setting of the input matrices size with the observations & parameters indices
			matrices.setDimensions(ueoi.UIndex, ueoi.EIndex, ueoi.OIndex, cnstrObs, cnstrNumber);

			processFreeCnstr(data, matrices, data.getFreeConstraints());
			
		}
		else
		{
			fError += "Constraint index is null, and LIBR option is used\n";
		}
	}
	else
	{
		// Setting of the input matrices size with the observations & parameters indices
		matrices.setDimensions(ueoi.UIndex, ueoi.EIndex, ueoi.OIndex, cnstrObs);
	}
	return;
}



bool TLSInputMatricesFiller::fillMatrices(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{

	bool filled = false;

	initMatriceDimension(data,matrices);

	if ( fError != "" )
	{
		cout << fError << endl;
		filled = false;
	}
	else
	{
		try
		{
			if(data.numHorAngObs() != 0)
				addHorAngContributions(data, matrices);

			if(data.numZenDistObs() != 0)
				addZenDistContributions(data, matrices);

			if(data.numSpaDistObs() != 0)
				addSpaDistContributions(data, matrices);

			if(data.numHorDistObs() != 0)
				addHorDistContributions(data, matrices);
			
			if(data.numVertDistObs(0) != 0)
				addVertDistContributions(data, matrices, 0);
			if(data.numVertDistObs(1) != 0)
				addVertDistContributions(data, matrices, 1);

			if(data.numOffsetToVerLineObs() != 0)
				addOffsetToVerLineContributions(data, matrices);

			if(data.numOffsetToSpaLineObs() != 0)
				addOffsetToSpaLineContributions(data, matrices);

			if(data.numOffsetToVerPlaneObs() != 0)
				addOffsetToVerPlaneContributions(data, matrices);
		
			if(data.numOffsetToTheoPlaneObs() != 0)
				addOffsetToTheoPlaneContributions(data, matrices);

			if(data.numGyroOrieObs() != 0)
				addGyroOrieContributions(data, matrices);

			if(data.numRadOffCnstrObs() != 0)
				addRadOffCnstrContributions(data, matrices);

			if(data.numOrieCnstrObs() != 0)
				addOrieCnstrContributions(data, matrices);

			filled = true;
		}
		catch (const std::logic_error & e)
		{
			fError += e.what();
			std::cout << fError << std::endl;
			filled = false;
		}
	}

	return filled;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
//OBSERVATION PROCESSING
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	TLSInputMatricesFiller::processFreeCnstr(const LSCalcDataSet& data,TLSInputMatrices& matrices, const TLSConstraintIdentifier& cnstr)
{//process of offset constraints for a free calculation
	
	TLSFreeCnstrCG contribution;
	bool processed = true;

	processed = contribution.processFreeCnstr(data, matrices, cnstr);
	if (!processed)
	{
		fError += "Constraints for free Calculation contribution generator wrongly processed\n";
		return;
	}

}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods to add the design matrix contributions for each type of observation and constraint
///////////////////////////////////////////////////////////////////////////////////////////////////////
/* Add the design matrices contributions for the hor. angle observations */
void TLSInputMatricesFiller::addHorAngContributions(const LSCalcDataSet& data, TLSInputMatrices& matrices)
{//process of horizontal angle observations

	bool isProcessOK = true;
	LSHorAngConstIter	obsIt = data.beginLSHorAng();
	LSHorAngConstIter	endObsIt = data.endLSHorAng();

	while ( obsIt != endObsIt)
	{
		//gets the observation's indices
		MatrixIndex angEInd = obsIt->getEqnIndex();
		MatrixIndex angOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		TheoStnContrib contributions = fGenerator.getHorAngDsgnMxContributions(obsIt);
		TAngle &calcHAng = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
		double e = contributions.fV0Contrib;//v0 coefficient
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable)
		{//xSt coefficient
			isProcessOK = isProcessOK && matrices.setFirstDgnMtrxElement(	angOInd,
																			obsIt->getStXIndex(),
																			cgVec.getX().getMetresValue());
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable)
		{//ySt coefficient
			isProcessOK = isProcessOK && matrices.setFirstDgnMtrxElement(	angOInd, 
															obsIt->getStYIndex(), 
															cgVec.getY().getMetresValue());
		}

		
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable)
		{//zSt coefficient
			isProcessOK = isProcessOK && matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getStZIndex(), 
															cgVec.getZ().getMetresValue());
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable)
		{//xTg coefficient
			isProcessOK = isProcessOK && matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getTgXIndex(),
															-1.0 * cgVec.getX().getMetresValue());
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable)
		{//yTg coefficient
			isProcessOK = isProcessOK && matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getTgYIndex(),
															-1.0 * cgVec.getY().getMetresValue());
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable)
		{//zTg coefficient
			isProcessOK = isProcessOK && matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getTgZIndex(),
															-1.0 * cgVec.getZ().getMetresValue());
		}

		
		if (obsIt->getV0Status() == TALSCalcParameter::kVariable)
		{//v0 coefficient
			isProcessOK = isProcessOK && matrices.setFirstDgnMtrxElement(angOInd, obsIt->getV0Index(), e);
		}

		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = isProcessOK && matrices.setSecondDgnMtrxElement(angEInd, angOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -1.0 * (obsIt->getObsAngle()  - calcHAng + obsIt->getEstimatedV0()).getRadiansValue();
		isProcessOK = isProcessOK && matrices.setMisclosureVectorElement(angOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = isProcessOK && matrices.setWeightMtrxElement(angOInd, angOInd, (scaleK*scaleK)/pow(obsIt->getSigmaAPriori().getRadiansValue(),2));

		if (!isProcessOK)
		{
			fError += "Horizontal angle contribution generators wrongly processed\n";
			return;
		}
		obsIt++;
	}

	return;
}


/* Add the design matrices contributions for the zenithal angle observations */
void TLSInputMatricesFiller::addZenDistContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{//process of zenithal angle observations

	bool isProcessOK = true;

	LSZenDistConstIter	obsIt = data.beginLSZenDist();
	LSZenDistConstIter	endObsIt = data.endLSZenDist();

	while ( obsIt != endObsIt )
	{
		//gets the observation's indices
		MatrixIndex angEInd = obsIt->getEqnIndex();
		MatrixIndex angOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation		
		TheoStnContrib contributions = fGenerator.getZenDistDsgnMxContributions(obsIt);
		TAngle &calcZDist = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
		double g = contributions.fHIContrib;// instrument height contribution
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable)
		{//xSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getStXIndex(),
															cgVec.getX().getMetresValue());
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable)
		{//ySt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getStYIndex(),
															cgVec.getY().getMetresValue());
		}

		
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable)
		{//zSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getStZIndex(),
															cgVec.getZ().getMetresValue());
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable)
		{//xTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getTgXIndex(),
															-1.0 * cgVec.getX().getMetresValue());
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable)
		{//yTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getTgYIndex(),
															-1.0 * cgVec.getY().getMetresValue());
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable)
		{//zTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	angOInd,
															obsIt->getTgZIndex(),
															-1.0 * cgVec.getZ().getMetresValue());
		}

		//hi coefficient
		if (obsIt->getHInstStatus() == TALSCalcParameter::kVariable)
		{
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, obsIt->getHInstIndex(), g);
		}

		//**Adding the contribution to the second design matrix
		isProcessOK = matrices.setSecondDgnMtrxElement(angEInd, angOInd, -1);

		//** setting the misclosure vector element
		k = -1.0 * (obsIt->getObsAngle()- calcZDist).getRadiansValue();
		isProcessOK = matrices.setMisclosureVectorElement(angOInd, k);

		//** setting the weight matrix element
		isProcessOK = matrices.setWeightMtrxElement(angOInd, angOInd, (scaleK*scaleK)/pow(obsIt->getSigmaAPriori().getRadiansValue(),2));


		if (!isProcessOK)
		{
			fError += "Zenithal distance contribution generators wrongly processed\n";
			return;
		}
		obsIt++;
	}
	return;
}


/* Add the design matrices contributions for the spatial dist. observations */
void	TLSInputMatricesFiller::addSpaDistContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{//process of spatial distance observations
	
	bool isProcessOK = true;

	LSSpaDistConstIter	obsIt = data.beginLSSpaDist();
	LSSpaDistConstIter	endObsIt = data.endLSSpaDist();

	while ( obsIt != endObsIt )
	{
		//gets the observation's indices
		MatrixIndex distEInd = obsIt->getEqnIndex();
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		DistStnContrib contributions = fGenerator.getSpaDistDsgnMxContributions(obsIt);
		TLength &calcDist = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
		double g = contributions.fHIContrib;//Instrument Height coefficient
		double h = contributions.fConstContrib;//Unknown measurement constant contribution
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		//xSt coefficient
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable){
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getStXIndex(),
				cgVec.getX().getMetresValue());
		}

		//ySt coefficient
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable){
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getStYIndex(), 
				cgVec.getY().getMetresValue());
		}

		//zSt coefficient
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable){
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getStZIndex(), 
				cgVec.getZ().getMetresValue());
		}

		//xTg coefficient
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable){
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getTgXIndex(),
				-1.0 * cgVec.getX().getMetresValue());
		}

		//yTg coefficient
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable){
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getTgYIndex(),
				-1.0 * cgVec.getY().getMetresValue());
		}

		//zTg coefficient
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable){
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getTgZIndex(),
				-1.0 * cgVec.getZ().getMetresValue());
		}

		//hi coefficient
		if (obsIt->getHInstStatus() == TALSCalcParameter::kVariable){
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getHInstIndex(), g);
		}

		//c coefficient
		if (obsIt->getDistConstStatus() == TALSCalcParameter::kVariable){
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getDistConstIndex(), h);
		}

		
		//**Adding the contribution to the second design matrix
		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);

		//** setting the misclosure vector element
		k = -1.0 * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//** setting the weight matrix element
		//isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, 1.0/pow(obsIt->getSigmaAPriori().getMetresValue(),2));
		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, (scaleK*scaleK)/pow(obsIt->getSigmaAPriori().getMetresValue(),2));

		if (!isProcessOK)
		{
			fError += "Spatial distance contribution generator wrongly processed\n";
			return;
		}
		obsIt++;
	}
	return;
}


/* Add the design matrices contributions for the hor. dist. observations */
void	TLSInputMatricesFiller::addHorDistContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{// process of horizontal distance observations

	bool isProcessOK = true;

	LSHorDistConstIter	obsIt = data.beginLSHorDist();
	LSHorDistConstIter	endObsIt = data.endLSHorDist();

	while ( obsIt != endObsIt )
	{
		//gets the observation's indices
		MatrixIndex distEInd = obsIt->getEqnIndex();
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		pair<TLength, TFreeVector> contributions = fGenerator.getHorDistDsgnMxContributions(obsIt);
		TLength &calcDist = contributions.first; // calculated value for the measurement
		double a = contributions.second.getX().getMetresValue();//station position x coefficient (negative values give target coefficient)
		double b = contributions.second.getY().getMetresValue();//station position y coefficient (negative values give target coefficient)
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable)
		{// xSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getStXIndex(), a);
		}
		
		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable)
		{//ySt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getStYIndex(), b);
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable)
		{//xTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getTgXIndex(), -1.0 * a);
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable)
		{//yTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getTgYIndex(), -1.0 * b);
		}

		
		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);


		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -1.0 * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, (scaleK*scaleK)/pow(obsIt->getSigmaAPriori().getMetresValue(),2));


		if (!isProcessOK)
		{
			fError += "Horizontal distance contribution generator wrongly processed\n";
			return;
		}
		obsIt++;
	}
	return;
}


/* Add the design matrices contributions for the vertical dist. observations */
void	TLSInputMatricesFiller::addVertDistContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices, bool isDLEV)
{//process of vertical distance observations

	bool isProcessOK = true;

	LSVertDistConstIter	obsIt = data.beginLSVertDist(isDLEV);
	LSVertDistConstIter	endObsIt = data.endLSVertDist(isDLEV);

	while (obsIt != endObsIt )
	{
		//gets the observation's indices
		MatrixIndex distEInd = obsIt->getEqnIndex();
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		LevelStnContrib contributions = isDLEV ? fGenerator.getDLEVDsgnMxContributions(obsIt) : fGenerator.getVertDistDsgnMxContributions(obsIt);
		TLength &calcDist = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &cgRefVec = contributions.fRefCoordContrib;//reference position coefficients (negative values give target coefficients)
		TFreeVector  &cgTgtVec = contributions.fTgtCoordContrib;//target position coefficients (negative values give target coefficients)
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getRefXStatus() == TALSCalcParameter::kVariable)
		{//xRef coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getRefXIndex(),cgRefVec.getX().getMetresValue());
		}

		if (obsIt->getRefYStatus() == TALSCalcParameter::kVariable)
		{//yRef coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getRefYIndex(),cgRefVec.getY().getMetresValue());
		}

		if (obsIt->getRefZStatus() == TALSCalcParameter::kVariable)
		{//zRef coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getRefZIndex(),cgRefVec.getZ().getMetresValue());
		}

		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable)
		{//xTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getTgXIndex(),cgTgtVec.getX().getMetresValue());
		}

		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable)
		{//yTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getTgYIndex(),cgTgtVec.getY().getMetresValue());
		}

		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable)
		{//zTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getTgZIndex(),cgTgtVec.getZ().getMetresValue());
		}
		
		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -1.0 * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, (scaleK*scaleK)/pow(obsIt->getSigmaAPriori().getMetresValue(),2));

		if (!isProcessOK)
		{
			fError += "Vertical distance contribution generator wrongly processed\n";
			return;
		}
		obsIt++;
	}
	return;
}


/* Add the design matrices contributions for the offset to ver. line observations */
void	TLSInputMatricesFiller::addOffsetToVerLineContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{//process of offset ECVE observations
	
	bool isProcessOK = true;

	LSOffsetToVerLineConstIter	obsIt = data.beginLSOffsetToVerLine();
	LSOffsetToVerLineConstIter	endObsIt = data.endLSOffsetToVerLine();

	while ( obsIt != endObsIt )
	{
		//gets the observation's indices
		MatrixIndex distEInd = obsIt->getEqnIndex();
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		pair<TLength, TFreeVector> contributions = fGenerator.getOffsetToVerLineDsgnMxContributions(obsIt);
		TLength &calcOffset = contributions.first; // calculated value for the measurement
		TFreeVector  &vecCG1 = contributions.second;//station position coefficients (negative values give target coefficients)
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable)
		{// xSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getStXIndex(),
															vecCG1.getX().getMetresValue());
		}

		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable)
		{//ySt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getStYIndex(),
															vecCG1.getY().getMetresValue());
		}

		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable)
		{//zSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getStZIndex(),
															vecCG1.getZ().getMetresValue());
		}

		
		if (obsIt->getFirstTgXStatus() == TALSCalcParameter::kVariable)
		{//xTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getFirstTgXIndex(),
															-1.0*vecCG1.getX().getMetresValue());
		}

		if (obsIt->getFirstTgYStatus() == TALSCalcParameter::kVariable)
		{//yTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getFirstTgYIndex(),
															-1.0*vecCG1.getY().getMetresValue());
		}

		if (obsIt->getFirstTgZStatus() == TALSCalcParameter::kVariable)
		{//zTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getFirstTgZIndex(),
															-1.0*vecCG1.getZ().getMetresValue());
		}


		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -1.0 * (obsIt->getObsDist().getMetresValue() - calcOffset.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, (scaleK*scaleK)/pow((obsIt->getSigmaAPriori()).getMetresValue(),2));

		if (!isProcessOK)
		{
			fError += "Offset to vertical line contribution generator wrongly processed\n";
			return;
		}
		obsIt++;
	}
	return;
}


/* Add the design matrices contributions for the offset to spa. line observations */
void	TLSInputMatricesFiller::addOffsetToSpaLineContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{//process of offset ESCP observations
	
	bool isProcessOK = true;

	LSOffsetToSpaLineConstIter	obsIt = data.beginLSOffsetToSpaLine();
	LSOffsetToSpaLineConstIter	endObsIt = data.endLSOffsetToSpaLine();

	while ( obsIt != endObsIt )
	{
		//gets the observation's indices
		MatrixIndex distEInd = obsIt->getEqnIndex();
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		OffsetStnContrib contributions = fGenerator.getOffsetToSpaLineDsgnMxContributions(obsIt);
		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &stC = contributions.fStnCoordContrib;//station position coefficients (negative values give target coefficients)
		TFreeVector  &fTgC = contributions.fTgt1CoordContrib;//1st target position coefficients (negative values give target coefficients)
		TFreeVector  &sTgC = contributions.fTgt2CoordContrib;//2nd target position coefficients (negative values give target coefficients)
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable)
		{// xSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd,
				obsIt->getStXIndex(), stC.getX().getMetresValue());	}
			
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable)
		{//ySt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd,
				obsIt->getStYIndex(), stC.getY().getMetresValue());	}

		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable)
		{//zSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getStZIndex(), stC.getZ().getMetresValue());	}

		if (obsIt->getFirstTgXStatus() == TALSCalcParameter::kVariable)
		{//xFTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd,
				obsIt->getFirstTgXIndex(), fTgC.getX().getMetresValue());	}

		if (obsIt->getFirstTgYStatus() == TALSCalcParameter::kVariable)
		{//yFTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getFirstTgYIndex(), fTgC.getY().getMetresValue());	}

		if (obsIt->getFirstTgZStatus() == TALSCalcParameter::kVariable)
		{//zFTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getFirstTgZIndex(), fTgC.getZ().getMetresValue());	}
			
		if (obsIt->getSecondTgXStatus() == TALSCalcParameter::kVariable)
		{//xSTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getSecondTgXIndex(), sTgC.getX().getMetresValue());	}

		if (obsIt->getSecondTgYStatus() == TALSCalcParameter::kVariable)
		{//ySTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getSecondTgYIndex(), sTgC.getY().getMetresValue());	}

		if (obsIt->getSecondTgZStatus() == TALSCalcParameter::kVariable)
		{//zSTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getSecondTgZIndex(), sTgC.getZ().getMetresValue());	}


		//** Adding the contribution to the second design matrix
		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);

		//** setting the misclosure vector element
		k = -1.0 * (obsIt->getObsDist().getMetresValue() - calcOffset.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//** adding the contributions to the weight matrix
		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, (scaleK*scaleK)/pow((obsIt->getSigmaAPriori()).getMetresValue(),2));


		if (!isProcessOK)
		{
			fError += "Offset to spatial line contribution generator wrongly processed\n";
			return;
		}
		obsIt++;
	}
	return;
}


/* Add the design matrices contributions for the offset to ver. plane observations */
void	TLSInputMatricesFiller::addOffsetToVerPlaneContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{//process of offset ECHO observations
	
	bool isProcessOK = true;

	LSOffsetToVerPlaneConstIter	obsIt = data.beginLSOffsetToVerPlane();
	LSOffsetToVerPlaneConstIter	endObsIt = data.endLSOffsetToVerPlane();

	while ( obsIt != endObsIt )
	{
		//gets the observation's indices
		MatrixIndex distEInd = obsIt->getEqnIndex();
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		OffsetStnContrib contributions = fGenerator.getOffsetToVerPlaneDsgnMxContributions(obsIt);
		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &vecCG1 = contributions.fStnCoordContrib;//station position coefficients (negative values give target coefficients)
		TFreeVector  &vecCG2 = contributions.fTgt1CoordContrib;//1st target position coefficients (negative values give target coefficients)
		TFreeVector  &vecCG3 = contributions.fTgt2CoordContrib;//2nd target position coefficients (negative values give target coefficients)
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable)
		{// xSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	distOInd,
															obsIt->getStXIndex(),
															vecCG1.getX().getMetresValue());
		}

		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable)
		{//ySt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	distOInd,
															obsIt->getStYIndex(),
															vecCG1.getY().getMetresValue());
		}

		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable)
		{//zSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	distOInd,
															obsIt->getStZIndex(),
															vecCG1.getZ().getMetresValue());
		}
		
		if (obsIt->getFirstTgXStatus() == TALSCalcParameter::kVariable)
		{//xFTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	distOInd,
															obsIt->getFirstTgXIndex(),
															vecCG2.getX().getMetresValue());
		}

		if (obsIt->getFirstTgYStatus() == TALSCalcParameter::kVariable)
		{//yFTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	distOInd,
															obsIt->getFirstTgYIndex(),
															vecCG2.getY().getMetresValue());
		}

		if (obsIt->getFirstTgZStatus() == TALSCalcParameter::kVariable)
		{//zFTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	distOInd,
															obsIt->getFirstTgZIndex(),
															vecCG2.getZ().getMetresValue());
		}
		

		if (obsIt->getSecondTgXStatus() == TALSCalcParameter::kVariable)
		{//xSTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	distOInd,
															obsIt->getSecondTgXIndex(),
															vecCG3.getX().getMetresValue());
		}

		if (obsIt->getSecondTgYStatus() == TALSCalcParameter::kVariable)
		{//ySTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	distOInd,
															obsIt->getSecondTgYIndex(),
															vecCG3.getY().getMetresValue());
		}

		if (obsIt->getSecondTgZStatus() == TALSCalcParameter::kVariable)
		{//zSTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(	distOInd,
															obsIt->getSecondTgZIndex(),
															vecCG3.getZ().getMetresValue());
		}

		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -1.0 * (obsIt->getObsDist().getMetresValue()- calcOffset.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, (scaleK*scaleK)/pow(obsIt->getSigmaAPriori().getMetresValue(),2));

		if (!isProcessOK)
		{
			fError += "Offset to vertical plane contribution generator wrongly processed\n";
			return;
		}
		obsIt++;
	}
	return;
}


/* Add the design matrices contributions for the offset to theo. plane observations */
void	TLSInputMatricesFiller::addOffsetToTheoPlaneContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{//process of offset ECTH observations
	
	bool isProcessOK = true;

	LSOffsetToTheoPlaneConstIter	obsIt = data.beginLSOffsetToTheoPlane();
	LSOffsetToTheoPlaneConstIter	endObsIt = data.endLSOffsetToTheoPlane();

	while ( obsIt != endObsIt )
	{
		//gets the observation's indices
		MatrixIndex distEInd = obsIt->getEqnIndex();
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		TheoOffsetStnContrib contributions = fGenerator.getOffsetToTheoPlaneDsgnMxContributions(obsIt);
		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &vecCG1 = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
		double v = contributions.fV0Contrib;//v0 coefficient
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable)
		{// xSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getStXIndex(), vecCG1.getX().getMetresValue());
		}

		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable)
		{//ySt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getStYIndex(), vecCG1.getY().getMetresValue());
		}

		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable)
		{//zSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getStZIndex(), vecCG1.getZ().getMetresValue());
		}
		
		if (obsIt->getFirstTgXStatus() == TALSCalcParameter::kVariable)
		{//xFTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getFirstTgXIndex(), -1.0 * vecCG1.getX().getMetresValue());
		}

		if (obsIt->getFirstTgYStatus() == TALSCalcParameter::kVariable)
		{//yFTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getFirstTgYIndex(), -1.0 * vecCG1.getY().getMetresValue());
		}

		if (obsIt->getFirstTgZStatus() == TALSCalcParameter::kVariable)
		{//zFTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, 
				obsIt->getFirstTgZIndex(), -1.0 * vecCG1.getZ().getMetresValue());
		}
		
		if (obsIt->getV0Status() == TALSCalcParameter::kVariable)
		{//v0 coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, obsIt->getV0Index(), v);
		}


		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);
		
		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -1.0 * (obsIt->getObsDist() - calcOffset).getMetresValue();
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, (scaleK*scaleK)/pow(obsIt->getSigmaAPriori().getMetresValue(),2));

		if (!isProcessOK)
		{
			fError += "Offset to theodolite plane  contribution generator wrongly processed\n";
			return;
		}
		obsIt++;
	}
	return;
}


/* Add the design matrices contributions for the gyro. orientation observations */
void	TLSInputMatricesFiller::addGyroOrieContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{//process of gyro. orientation observations
	
	bool isProcessOK = true;

	LSGyroOrieConstIter	obsIt = data.beginLSGyroOrie();
	LSGyroOrieConstIter	endObsIt = data.endLSGyroOrie();

	while ( obsIt != endObsIt )
	{
		//gets the observation's indices
		MatrixIndex angEInd = obsIt->getEqnIndex();
		MatrixIndex angOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		pair<TAngle, TFreeVector> contributions = fGenerator.getGyroOrieDsgnMxContributions(obsIt);
		TAngle &calcHAng = contributions.first; // calculated value for the measurement
		TFreeVector  &vecCG1 = contributions.second;//station position coefficients (negative values give target coefficients)
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable)
		{//xSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd,
				obsIt->getStXIndex(), vecCG1.getX().getMetresValue());
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable)
		{//ySt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
				obsIt->getStYIndex(), vecCG1.getY().getMetresValue());
		}

		
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable)
		{//zSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
				obsIt->getStZIndex(), vecCG1.getZ().getMetresValue());
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable)
		{//xTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
				obsIt->getTgXIndex(), -1.0 * vecCG1.getX().getMetresValue());
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable)
		{//yTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
				obsIt->getTgYIndex(), -1.0 * vecCG1.getY().getMetresValue());
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable)
		{//zTg coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
				obsIt->getTgZIndex(), -1.0 * vecCG1.getZ().getMetresValue());
		}


		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxElement(angEInd, angOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -1.0 * (obsIt->getObsAngle() - calcHAng).getRadiansValue();
		isProcessOK = matrices.setMisclosureVectorElement(angOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(angOInd, angOInd, (scaleK*scaleK)/pow(obsIt->getSigmaAPriori().getRadiansValue(),2));

		if (!isProcessOK)
		{
			fError += "Offset to gyro. orientation  contribution generator wrongly processed\n";
			return;
		}
		obsIt++;
	}
	return;
}


/* Add the design matrices contributions for the radial offset constraints */
void	TLSInputMatricesFiller::addRadOffCnstrContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{//process of offset radial constraints
	
	bool isProcessOK = true;

	LSRadOffCnstrConstIter	cnstrIt = data.beginLSRadOffCnstr();
	LSRadOffCnstrConstIter	endCnstrIt = data.endLSRadOffCnstr();

	while ( cnstrIt != endCnstrIt )
	{
		//gets the observation's indices
		MatrixIndex distEInd = cnstrIt->getEqnIndex();
		MatrixIndex distOInd = cnstrIt->getObsIndex();

		// get/set the contributions for the observation
		pair<TLength, TFreeVector> contributions = fGenerator.getRadOffCnstrDsgnMxContributions(cnstrIt);
		TLength &calcOffset = contributions.first; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.second;//station position coefficients 
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (cnstrIt->getPtXStatus() == TALSCalcParameter::kVariable)
		{// xSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, cnstrIt->getPtXIndex(), 
															cgVec.getX().getMetresValue());}

		if (cnstrIt->getPtYStatus() == TALSCalcParameter::kVariable)
		{//ySt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, cnstrIt->getPtYIndex(), 
															cgVec.getY().getMetresValue());}

		if (cnstrIt->getPtZStatus() == TALSCalcParameter::kVariable)
		{//zSt coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(distOInd, cnstrIt->getPtZIndex(), 
															cgVec.getZ().getMetresValue());}


		//** Adding the contribution to the second design matrix
		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);

		//** setting the misclosure vector element
		k = -1.0 * calcOffset.getMetresValue();
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//** adding the contributions to the weight matrix
		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, (scaleK*scaleK)/pow(cnstrIt->getSigmaAPriori().getMetresValue(),2));

		if (!isProcessOK)
		{
			fError += "Radial offset constraints  contribution generator wrongly processed\n";
			return;
		}
		cnstrIt++;
	}
	return;
}


/* Add the design matrices contributions for the orientation constraints */
void	TLSInputMatricesFiller::addOrieCnstrContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
{//process of offset orientation constraints 
	
	bool isProcessOK = true;

	LSOrieCnstrConstIter	cnstrIt = data.beginLSOrieCnstr();
	LSOrieCnstrConstIter	endCnstrIt = data.endLSOrieCnstr();

	while ( cnstrIt != endCnstrIt )
	{
		//gets the observation's indices
		MatrixIndex angEInd = cnstrIt->getEqnIndex();
		MatrixIndex angOInd = cnstrIt->getObsIndex();

		// get/set the contributions for the observation
		pair<TAngle, TFreeVector> contributions = fGenerator.getOrientationCnstrDsgnMxContributions(cnstrIt);
		TAngle &calcBear = contributions.first; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.second;//station position coefficients 
		double k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (cnstrIt->getFixedXStatus() == TALSCalcParameter::kVariable)
		{//xFix coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd,
				cnstrIt->getFixedXIndex(), cgVec.getX().getMetresValue());
		}

		if (cnstrIt->getFixedYStatus() == TALSCalcParameter::kVariable)
		{//yFix coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
				cnstrIt->getFixedYIndex(), cgVec.getY().getMetresValue());
		}

		if (cnstrIt->getRefXStatus() == TALSCalcParameter::kVariable)
		{//xRef coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
				cnstrIt->getRefXIndex(), -1.0 * cgVec.getX().getMetresValue());
		}
		
		if (cnstrIt->getRefYStatus() == TALSCalcParameter::kVariable)
		{//yRef coefficient
			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
				cnstrIt->getRefYIndex(), -1.0 * cgVec.getY().getMetresValue());
		}


		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxElement(angEInd, angOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = 0;
		if(fIsPdorBearingDefined)
		{
			k = -1.0 * (cnstrIt->getBearing() - calcBear).getRadiansValue();
		}
		isProcessOK = matrices.setMisclosureVectorElement(angOInd, k);

		//** setting the weight matrix element
		isProcessOK = matrices.setWeightMtrxElement(angOInd, angOInd, (scaleK*scaleK)/pow(cnstrIt->getSigmaAPriori().getRadiansValue(),2));

		if (!isProcessOK)
		{
			fError += "Orientation contribution generator wrongly processed\n";
			return;
		}
		cnstrIt++;
	}
	return;
}


/* Add the design matrices contributions for the constraints of a free network calculation */
/*void	TLSInputMatricesFiller::addFreeCnstrContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices, TLSConstraintIdentifier& cnstr)
{//process of offset constraints for a free calculation
	
	bool isProcessOK = true;

//	isProcessOK = contribution.processFreeCnstr(data, matrices, cnstr);
	
	if (!isProcessOK)
	{
		fError += "Constraints for free Calculation contribution generator wrongly processed\n";
	}
	return;
}
*/







//////////////////////////////////////////////////////////////////////
//// TinputMatricesFiller.cpp : implementation file
//// Class responsible for reading the least squares calculationmeasurements and constraints 
//// and filling the least squares matrices
//// Specific to a least squares calculation
////
//// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////////////////////
//
//
//#include "TLSInputMatricesFiller.h"
//
//#include "TLSInputMatrices.h"
//#include "LSCalcDataSet.h"
//
//#include "TLGCObsLSContributionGenerator.h"
//#include "TLSFreeCnstrCG.h"
//
//#include "TModifiedLocalAstronomicalRF.h"
//#include "TARefFrameTransformation.h"
//#include "TGC2MLATransformation.h"
//#include "TMLA2GCTransformation.h"
//
//#include "TLGCDataSet.h"
//
//int pairCompare(const void* p1, const void* p2)
//{
//	if ((*((pair<int, TReal>*) p1)).first < (*((pair<int, TReal>*) p2)).first)
//		return -1;
//	if ((*((pair<int, TReal>*) p1)).first == (*((pair<int, TReal>*) p2)).first)
//		return 0;
//	return 1;
//}
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//TLSInputMatricesFiller::TLSInputMatricesFiller(TDataParameters::ERefFrame refSurface, bool pdor)
//: fGenerator(refSurface)
//{//constructor setting the pointer to the contrib. generators factory
//
//	fError = "";
//	fIsPdorBearingDefined = pdor;
//}
//
//
//TLSInputMatricesFiller::~TLSInputMatricesFiller()
//{//Destructor
////	delete fCGFactory;	
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////MEMBER FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//void TLSInputMatricesFiller::initMatriceDimension(const LSCalcDataSet& data, TLSInputMatrices& matrices)
//{
//	// Update and extraction of the observations & parameters indices
//	UEOIndices ueoi = data.getDimensions();
//	if ((ueoi.EIndex == 0))
//	{
//		fError += "Equation index is null\n";
//	}
//	if ((ueoi.OIndex == 0))
//	{
//		fError += "Observation index is null\n";
//	}
///*	if ((ueoi.UIndex == 0))
//	{
//		fError += "Unknown index is null\n";
//	}*/
//
//	int cnstrObs;
//	if (TLGCDataSet::usedOrieCnstr())
//	{
//		cnstrObs = 1;
//	}
//	else
//	{
//		cnstrObs = 0;
//	}
//
//	if(data.isLibrOptionUsed())
//	{
//		int cnstrNumber = data.getFreeConstraints().getNumberOfConstraint();
//		if (cnstrNumber != 0)
//		{
//			// Setting of the input matrices size with the observations & parameters indices
//			matrices.setDimensions(ueoi.UIndex, ueoi.EIndex, ueoi.OIndex, cnstrObs, cnstrNumber);
//			//matrices.setConstraintNewColumn();
//
//			processFreeCnstr(data, matrices, data.getFreeConstraints());
//		}
//		else
//		{
//			fError += "Constraint index is null, and LIBR option is used\n";
//		}
//	}
//	else
//	{
//		// Setting of the input matrices size with the observations & parameters indices
//		matrices.setDimensions(ueoi.UIndex, ueoi.EIndex, ueoi.OIndex, cnstrObs);
//	}
//	return;
//}
//
//
//
//bool TLSInputMatricesFiller::fillMatrices(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{
//
//	bool filled = false;
//
//	initMatriceDimension(data,matrices);
//	//matrices.setNewRow();
//
//	if ( fError == "" )
//	{
//		filled = true;
//		if(data.numHorAngObs() != 0)
//			addHorAngContributions(data, matrices);
//
//		if(data.numZenDistObs() != 0)
//			addZenDistContributions(data, matrices);
//
//		if(data.numSpaDistObs() != 0)
//			addSpaDistContributions(data, matrices);
//
//		if(data.numHorDistObs() != 0)
//			addHorDistContributions(data, matrices);
//
//		if(data.numVertDistObs() != 0)
//			addVertDistContributions(data, matrices);
//
//		if(data.numOffsetToVerLineObs() != 0)
//			addOffsetToVerLineContributions(data, matrices);
//
//		if(data.numOffsetToSpaLineObs() != 0)
//			addOffsetToSpaLineContributions(data, matrices);
//
//		if(data.numOffsetToVerPlaneObs() != 0)
//			addOffsetToVerPlaneContributions(data, matrices);
//		
//		if(data.numOffsetToTheoPlaneObs() != 0)
//			addOffsetToTheoPlaneContributions(data, matrices);
//
//		if(data.numGyroOrieObs() != 0)
//			addGyroOrieContributions(data, matrices);
//
//		if(data.numRadOffCnstrObs() != 0)
//			addRadOffCnstrContributions(data, matrices);
//
//		if(data.numOrieCnstrObs() != 0)
//			addOrieCnstrContributions(data, matrices);
//	}
//	else
//	{
//		cout << fError << endl;
//		filled = false;
//	}
//
//	//matrices.finishedFillingMatrices();
//
//	return filled;
//}
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////OBSERVATION PROCESSING
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//void	TLSInputMatricesFiller::processFreeCnstr(const LSCalcDataSet& data,TLSInputMatrices& matrices, TLSConstraintIdentifier& cnstr)
//{//process of offset constraints for a free calculation
//	
//	TLSFreeCnstrCG contribution;
//	bool processed = true;
//
//	processed = contribution.processFreeCnstr(data, matrices, cnstr);
//	if (!processed)
//	{
//		fError += "Constraints for free Calculation contribution generator wrongly processed\n";
//		return;
//	}
//
//}
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////Methods to add the design matrix contributions for each type of observation and constraint
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///* Add the design matrices contributions for the hor. angle observations */
//void TLSInputMatricesFiller::addHorAngContributions(const LSCalcDataSet& data, TLSInputMatrices& matrices)
//{//process of horizontal angle observations
//
//	bool isProcessOK = true;
//	LSHorAngConstIter	obsIt = data.beginLSHorAng();
//	LSHorAngConstIter	endObsIt = data.endLSHorAng();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[7];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//
//		//gets the observation's indices
//		MatrixIndex angEInd = obsIt->getEqnIndex();
//		MatrixIndex angOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		TheoStnContrib contributions = fGenerator.getHorAngDsgnMxContributions(obsIt);
//		TAngle &calcHAng = contributions.fCalcMeas; // calculated value for the measurement
//		TFreeVector  &cgVec = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
//		TReal e = contributions.fV0Contrib;//v0 coefficient
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getStXIndex();
//			cs[count++].second = cgVec.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStYIndex();
//			cs[count++].second = cgVec.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStZIndex();
//			cs[count++].second = cgVec.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
//		{//xTg coefficient
//			cs[count].first = obsIt->getTgXIndex();
//			cs[count++].second = -cgVec.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
//		{//yTg coefficient
//			cs[count].first = obsIt->getTgYIndex();
//			cs[count++].second = -cgVec.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
//		{//yTg coefficient
//			cs[count].first = obsIt->getTgZIndex();
//			cs[count++].second = -cgVec.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getV0Status() == TALSCalcParameter::kVariable)
//		{//v0 coefficient
//			cs[count].first = obsIt->getV0Index();
//			cs[count++].second = e;
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//
//		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
//		isProcessOK = isProcessOK && matrices.setSecondDgnMtrxElement(angEInd, angOInd, -1);
//
//		//SETTING MISCLOSURE VECTOR ELEMENT
//		k = -LITERAL(1.0) * (obsIt->getObsAngle()  - calcHAng + obsIt->getEstimatedV0()).getRadiansValue();
//		isProcessOK = isProcessOK && matrices.setMisclosureVectorElement(angOInd, k);
//
//		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
//		isProcessOK = isProcessOK && matrices.setWeightMtrxElement(angOInd, angOInd, LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getRadiansValue(),2));
//
//		if (!isProcessOK)
//		{
//			fError += "Horizontal angle contribution generators wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//
//	return;
//}
//
//
///* Add the design matrices contributions for the zenithal angle observations */
//void TLSInputMatricesFiller::addZenDistContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of zenithal angle observations
//
//	bool isProcessOK = true;
//
//	LSZenDistConstIter	obsIt = data.beginLSZenDist();
//	LSZenDistConstIter	endObsIt = data.endLSZenDist();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[7];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex angEInd = obsIt->getEqnIndex();
//		MatrixIndex angOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation		
//		TheoStnContrib contributions = fGenerator.getZenDistDsgnMxContributions(obsIt);
//		TAngle &calcZDist = contributions.fCalcMeas; // calculated value for the measurement
//		TFreeVector  &cgVec = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
//		TReal g = contributions.fHIContrib;// instrument height contribution
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getStXIndex();
//			cs[count++].second = cgVec.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStYIndex();
//			cs[count++].second = cgVec.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getStZIndex();
//			cs[count++].second = cgVec.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
//		{//xTg coefficient
//			cs[count].first = obsIt->getTgXIndex();
//			cs[count++].second = -cgVec.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
//		{//yTg coefficient
//			cs[count].first = obsIt->getTgYIndex();
//			cs[count++].second = -cgVec.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
//		{//zTg coefficient
//			cs[count].first = obsIt->getTgZIndex();
//			cs[count++].second = -cgVec.getZ().getMetresValue();
//		}
//
//		//hi coefficient
//		if (obsIt->getHInstStatus() == TALSCalcParameter::kVariable)
//		{
//			cs[count].first = obsIt->getHInstIndex();
//			cs[count++].second = g;
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//
//		//**Adding the contribution to the second design matrix
//		isProcessOK = matrices.setSecondDgnMtrxElement(angEInd, angOInd, -1);
//
//		//** setting the misclosure vector element
//		k = -LITERAL(1.0) * (obsIt->getObsAngle()- calcZDist).getRadiansValue();
//		isProcessOK = matrices.setMisclosureVectorElement(angOInd, k);
//
//		//** setting the weight matrix element
//		isProcessOK = matrices.setWeightMtrxElement(angOInd, angOInd, LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getRadiansValue(),2));
//
//
//		if (!isProcessOK)
//		{
//			fError += "Zenithal distance contribution generators wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the spatial dist. observations */
//void	TLSInputMatricesFiller::addSpaDistContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of spatial distance observations
//	
//	bool isProcessOK = true;
//
//	LSSpaDistConstIter	obsIt = data.beginLSSpaDist();
//	LSSpaDistConstIter	endObsIt = data.endLSSpaDist();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[8];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex distEInd = obsIt->getEqnIndex();
//		MatrixIndex distOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		DistStnContrib contributions = fGenerator.getSpaDistDsgnMxContributions(obsIt);
//		TLength &calcDist = contributions.fCalcMeas; // calculated value for the measurement
//		TFreeVector  &cgVec = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
//		TReal g = contributions.fHIContrib;//Instrument Height coefficient
//		TReal h = contributions.fConstContrib;//Unknown measurement constant contribution
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getStXIndex();
//			cs[count++].second = cgVec.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStYIndex();
//			cs[count++].second = cgVec.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getStZIndex();
//			cs[count++].second = cgVec.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
//		{//xTg coefficient
//			cs[count].first = obsIt->getTgXIndex();
//			cs[count++].second = -cgVec.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
//		{//yTg coefficient
//			cs[count].first = obsIt->getTgYIndex();
//			cs[count++].second = -cgVec.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
//		{//zTg coefficient
//			cs[count].first = obsIt->getTgZIndex();
//			cs[count++].second = -cgVec.getZ().getMetresValue();
//		}
//
//		//hi coefficient
//		if (obsIt->getHInstStatus() == TALSCalcParameter::kVariable)
//		{
//			cs[count].first = obsIt->getHInstIndex();
//			cs[count++].second = g;
//		}
//
//		//c coefficient
//		if (obsIt->getDistConstStatus() == TALSCalcParameter::kVariable){
//			cs[count].first = obsIt->getDistConstIndex();
//			cs[count++].second = h;
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//		
//		//**Adding the contribution to the second design matrix
//		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);
//
//		//** setting the misclosure vector element
//		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
//		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);
//
//		//** setting the weight matrix element
//		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(),2));
//
//		if (!isProcessOK)
//		{
//			fError += "Spatial distance contribution generator wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the hor. dist. observations */
//void	TLSInputMatricesFiller::addHorDistContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{// process of horizontal distance observations
//
//	bool isProcessOK = true;
//
//	LSHorDistConstIter	obsIt = data.beginLSHorDist();
//	LSHorDistConstIter	endObsIt = data.endLSHorDist();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[4];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex distEInd = obsIt->getEqnIndex();
//		MatrixIndex distOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		pair<TLength, TFreeVector> contributions = fGenerator.getHorDistDsgnMxContributions(obsIt);
//		TLength &calcDist = contributions.first; // calculated value for the measurement
//		TReal a = contributions.second.getX().getMetresValue();//station position x coefficient (negative values give target coefficient)
//		TReal b = contributions.second.getY().getMetresValue();//station position y coefficient (negative values give target coefficient)
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && a != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getStXIndex();
//			cs[count++].second = a;
//		}
//
//		
//		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && b != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStYIndex();
//			cs[count++].second = b;
//		}
//
//		
//		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && a != 0)
//		{//xTg coefficient
//			cs[count].first = obsIt->getTgXIndex();
//			cs[count++].second = -a;
//		}
//
//		
//		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && b != 0)
//		{//yTg coefficient
//			cs[count].first = obsIt->getTgYIndex();
//			cs[count++].second = -b;
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//		
//		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
//		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);
//
//
//		//SETTING MISCLOSURE VECTOR ELEMENT
//		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
//		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);
//
//		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
//		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(),2));
//
//
//		if (!isProcessOK)
//		{
//			fError += "Horizontal distance contribution generator wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the vertical dist. observations */
//void	TLSInputMatricesFiller::addVertDistContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of vertical distance observations
//
//	bool isProcessOK = true;
//
//	LSVertDistConstIter	obsIt = data.beginLSVertDist();
//	LSVertDistConstIter	endObsIt = data.endLSVertDist();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[6];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex distEInd = obsIt->getEqnIndex();
//		MatrixIndex distOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		LevelStnContrib contributions = fGenerator.getVertDistDsgnMxContributions(obsIt);
//		TLength &calcDist = contributions.fCalcMeas; // calculated value for the measurement
//		TFreeVector  &cgRefVec = contributions.fRefCoordContrib;//reference position coefficients (negative values give target coefficients)
//		TFreeVector  &cgTgtVec = contributions.fTgtCoordContrib;//target position coefficients (negative values give target coefficients)
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getRefXStatus() == TALSCalcParameter::kVariable && cgRefVec.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getRefXIndex();
//			cs[count++].second = cgRefVec.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getRefYStatus() == TALSCalcParameter::kVariable && cgRefVec.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getRefYIndex();
//			cs[count++].second = cgRefVec.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getRefZStatus() == TALSCalcParameter::kVariable && cgRefVec.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getRefZIndex();
//			cs[count++].second = cgRefVec.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && cgTgtVec.getX().getMetresValue() != 0)
//		{//xTg coefficient
//			cs[count].first = obsIt->getTgXIndex();
//			cs[count++].second = cgTgtVec.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && cgTgtVec.getY().getMetresValue() != 0)
//		{//yTg coefficient
//			cs[count].first = obsIt->getTgYIndex();
//			cs[count++].second = cgTgtVec.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && cgTgtVec.getZ().getMetresValue() != 0)
//		{//zTg coefficient
//			cs[count].first = obsIt->getTgZIndex();
//			cs[count++].second = cgTgtVec.getZ().getMetresValue();
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//		
//		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
//		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);
//
//		//SETTING MISCLOSURE VECTOR ELEMENT
//		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
//		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);
//
//		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
//		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(),2));
//
//		if (!isProcessOK)
//		{
//			fError += "Vertical distance contribution generator wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the offset to ver. line observations */
//void	TLSInputMatricesFiller::addOffsetToVerLineContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of offset ECVE observations
//	
//	bool isProcessOK = true;
//
//	LSOffsetToVerLineConstIter	obsIt = data.beginLSOffsetToVerLine();
//	LSOffsetToVerLineConstIter	endObsIt = data.endLSOffsetToVerLine();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[6];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex distEInd = obsIt->getEqnIndex();
//		MatrixIndex distOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		pair<TLength, TFreeVector> contributions = fGenerator.getOffsetToVerLineDsgnMxContributions(obsIt);
//		TLength &calcOffset = contributions.first; // calculated value for the measurement
//		TFreeVector  &vecCG1 = contributions.second;//station position coefficients (negative values give target coefficients)
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getStXIndex();
//			cs[count++].second = vecCG1.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStYIndex();
//			cs[count++].second = vecCG1.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getStZIndex();
//			cs[count++].second = vecCG1.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
//		{//xTg coefficient
//			cs[count].first = obsIt->getFirstTgXIndex();
//			cs[count++].second = -vecCG1.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
//		{//yTg coefficient
//			cs[count].first = obsIt->getFirstTgYIndex();
//			cs[count++].second = -vecCG1.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
//		{//zTg coefficient
//			cs[count].first = obsIt->getFirstTgZIndex();
//			cs[count++].second = -vecCG1.getZ().getMetresValue();
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//
//
//		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
//		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);
//
//		//SETTING MISCLOSURE VECTOR ELEMENT
//		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue() - calcOffset.getMetresValue());
//		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);
//
//		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
//		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, LITERAL(1.0)/powq((obsIt->getSigmaAPriori()).getMetresValue(),2));
//
//		if (!isProcessOK)
//		{
//			fError += "Offset to vertical line contribution generator wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the offset to spa. line observations */
//void	TLSInputMatricesFiller::addOffsetToSpaLineContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of offset ESCP observations
//	
//	bool isProcessOK = true;
//
//	LSOffsetToSpaLineConstIter	obsIt = data.beginLSOffsetToSpaLine();
//	LSOffsetToSpaLineConstIter	endObsIt = data.endLSOffsetToSpaLine();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[9];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex distEInd = obsIt->getEqnIndex();
//		MatrixIndex distOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		OffsetStnContrib contributions = fGenerator.getOffsetToSpaLineDsgnMxContributions(obsIt);
//		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
//		TFreeVector  &stC = contributions.fStnCoordContrib;//station position coefficients (negative values give target coefficients)
//		TFreeVector  &fTgC = contributions.fTgt1CoordContrib;//1st target position coefficients (negative values give target coefficients)
//		TFreeVector  &sTgC = contributions.fTgt2CoordContrib;//2nd target position coefficients (negative values give target coefficients)
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && stC.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getStXIndex();
//			cs[count++].second = stC.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && stC.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStYIndex();
//			cs[count++].second = stC.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && stC.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getStZIndex();
//			cs[count++].second = stC.getZ().getMetresValue();
//		}
//
//		if (obsIt->getFirstTgXStatus() == TALSCalcParameter::kVariable && fTgC.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getFirstTgXIndex();
//			cs[count++].second = fTgC.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgYStatus() == TALSCalcParameter::kVariable && fTgC.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getFirstTgYIndex();
//			cs[count++].second = fTgC.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgZStatus() == TALSCalcParameter::kVariable && fTgC.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getFirstTgZIndex();
//			cs[count++].second = fTgC.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getSecondTgXStatus() == TALSCalcParameter::kVariable && sTgC.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getSecondTgXIndex();
//			cs[count++].second = sTgC.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getSecondTgYStatus() == TALSCalcParameter::kVariable && sTgC.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getSecondTgYIndex();
//			cs[count++].second = sTgC.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getSecondTgZStatus() == TALSCalcParameter::kVariable && sTgC.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getSecondTgZIndex();
//			cs[count++].second = sTgC.getZ().getMetresValue();
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//
//		//** Adding the contribution to the second design matrix
//		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);
//
//		//** setting the misclosure vector element
//		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue() - calcOffset.getMetresValue());
//		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);
//
//		//** adding the contributions to the weight matrix
//		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, LITERAL(1.0)/powq((obsIt->getSigmaAPriori()).getMetresValue(),2));
//
//
//		if (!isProcessOK)
//		{
//			fError += "Offset to spatial line contribution generator wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the offset to ver. plane observations */
//void	TLSInputMatricesFiller::addOffsetToVerPlaneContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of offset ECHO observations
//	
//	bool isProcessOK = true;
//
//	LSOffsetToVerPlaneConstIter	obsIt = data.beginLSOffsetToVerPlane();
//	LSOffsetToVerPlaneConstIter	endObsIt = data.endLSOffsetToVerPlane();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[9];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex distEInd = obsIt->getEqnIndex();
//		MatrixIndex distOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		OffsetStnContrib contributions = fGenerator.getOffsetToVerPlaneDsgnMxContributions(obsIt);
//		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
//		TFreeVector  &vecCG1 = contributions.fStnCoordContrib;//station position coefficients (negative values give target coefficients)
//		TFreeVector  &vecCG2 = contributions.fTgt1CoordContrib;//1st target position coefficients (negative values give target coefficients)
//		TFreeVector  &vecCG3 = contributions.fTgt2CoordContrib;//2nd target position coefficients (negative values give target coefficients)
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getStXIndex();
//			cs[count++].second = vecCG1.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStYIndex();
//			cs[count++].second = vecCG1.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getStZIndex();
//			cs[count++].second = vecCG1.getZ().getMetresValue();
//		}
//
//		if (obsIt->getFirstTgXStatus() == TALSCalcParameter::kVariable && vecCG2.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getFirstTgXIndex();
//			cs[count++].second = vecCG2.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgYStatus() == TALSCalcParameter::kVariable && vecCG2.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getFirstTgYIndex();
//			cs[count++].second = vecCG2.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgZStatus() == TALSCalcParameter::kVariable && vecCG2.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getFirstTgZIndex();
//			cs[count++].second = vecCG2.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getSecondTgXStatus() == TALSCalcParameter::kVariable && vecCG3.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getSecondTgXIndex();
//			cs[count++].second = vecCG3.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getSecondTgYStatus() == TALSCalcParameter::kVariable && vecCG3.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getSecondTgYIndex();
//			cs[count++].second = vecCG3.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getSecondTgZStatus() == TALSCalcParameter::kVariable && vecCG3.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getSecondTgZIndex();
//			cs[count++].second = vecCG3.getZ().getMetresValue();
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//
//		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
//		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);
//
//		//SETTING MISCLOSURE VECTOR ELEMENT
//		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue()- calcOffset.getMetresValue());
//		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);
//
//		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
//		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(),2));
//
//		if (!isProcessOK)
//		{
//			fError += "Offset to vertical plane contribution generator wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the offset to theo. plane observations */
//void	TLSInputMatricesFiller::addOffsetToTheoPlaneContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of offset ECTH observations
//	
//	bool isProcessOK = true;
//
//	LSOffsetToTheoPlaneConstIter	obsIt = data.beginLSOffsetToTheoPlane();
//	LSOffsetToTheoPlaneConstIter	endObsIt = data.endLSOffsetToTheoPlane();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[7];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex distEInd = obsIt->getEqnIndex();
//		MatrixIndex distOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		TheoOffsetStnContrib contributions = fGenerator.getOffsetToTheoPlaneDsgnMxContributions(obsIt);
//		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
//		TFreeVector  &vecCG1 = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
//		TReal v = contributions.fV0Contrib;//v0 coefficient
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getStXIndex();
//			cs[count++].second = vecCG1.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStYIndex();
//			cs[count++].second = vecCG1.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getStZIndex();
//			cs[count++].second = vecCG1.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
//		{//xTg coefficient
//			cs[count].first = obsIt->getFirstTgXIndex();
//			cs[count++].second = -vecCG1.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
//		{//yTg coefficient
//			cs[count].first = obsIt->getFirstTgYIndex();
//			cs[count++].second = -vecCG1.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getFirstTgZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
//		{//zTg coefficient
//			cs[count].first = obsIt->getFirstTgZIndex();
//			cs[count++].second = -vecCG1.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getV0Status() == TALSCalcParameter::kVariable)
//		{//v0 coefficient
//			cs[count].first = obsIt->getV0Index();
//			cs[count++].second = v;
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//
//		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
//		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);
//		
//		//SETTING MISCLOSURE VECTOR ELEMENT
//		k = -LITERAL(1.0) * (obsIt->getObsDist() - calcOffset).getMetresValue();
//		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);
//
//		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
//		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(),2));
//
//		if (!isProcessOK)
//		{
//			fError += "Offset to theodolite plane  contribution generator wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the gyro. orientation observations */
//void	TLSInputMatricesFiller::addGyroOrieContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of gyro. orientation observations
//	
//	bool isProcessOK = true;
//
//	LSGyroOrieConstIter	obsIt = data.beginLSGyroOrie();
//	LSGyroOrieConstIter	endObsIt = data.endLSGyroOrie();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[6];
//    int count;
//	while ( obsIt != endObsIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex angEInd = obsIt->getEqnIndex();
//		MatrixIndex angOInd = obsIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		pair<TAngle, TFreeVector> contributions = fGenerator.getGyroOrieDsgnMxContributions(obsIt);
//		TAngle &calcHAng = contributions.first; // calculated value for the measurement
//		TFreeVector  &vecCG1 = contributions.second;//station position coefficients (negative values give target coefficients)
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = obsIt->getStXIndex();
//			cs[count++].second = vecCG1.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = obsIt->getStYIndex();
//			cs[count++].second = vecCG1.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = obsIt->getStZIndex();
//			cs[count++].second = vecCG1.getZ().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
//		{//xTg coefficient
//			cs[count].first = obsIt->getTgXIndex();
//			cs[count++].second = -vecCG1.getX().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
//		{//yTg coefficient
//			cs[count].first = obsIt->getTgYIndex();
//			cs[count++].second = -vecCG1.getY().getMetresValue();
//		}
//
//		
//		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
//		{//zTg coefficient
//			cs[count].first = obsIt->getTgZIndex();
//			cs[count++].second = -vecCG1.getZ().getMetresValue();
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//
//		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
//		isProcessOK = matrices.setSecondDgnMtrxElement(angEInd, angOInd, -1);
//
//		//SETTING MISCLOSURE VECTOR ELEMENT
//		k = -LITERAL(1.0) * (obsIt->getObsAngle() - calcHAng).getRadiansValue();
//		isProcessOK = matrices.setMisclosureVectorElement(angOInd, k);
//
//		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
//		isProcessOK = matrices.setWeightMtrxElement(angOInd, angOInd, LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getRadiansValue(),2));
//
//		if (!isProcessOK)
//		{
//			fError += "Offset to gyro. orientation  contribution generator wrongly processed\n";
//			break;
//		}
//		obsIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the radial offset constraints */
//void	TLSInputMatricesFiller::addRadOffCnstrContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of offset radial constraints
//	
//	bool isProcessOK = true;
//
//	LSRadOffCnstrConstIter	cnstrIt = data.beginLSRadOffCnstr();
//	LSRadOffCnstrConstIter	endCnstrIt = data.endLSRadOffCnstr();
//
//    pair<int, TReal>* cs = new pair<int, TReal>[3];
//    int count;
//	while ( cnstrIt != endCnstrIt)
//	{
//        count = 0;
//		//gets the observation's indices
//		MatrixIndex distEInd = cnstrIt->getEqnIndex();
//		MatrixIndex distOInd = cnstrIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		pair<TLength, TFreeVector> contributions = fGenerator.getRadOffCnstrDsgnMxContributions(cnstrIt);
//		TLength &calcOffset = contributions.first; // calculated value for the measurement
//		TFreeVector  &cgVec = contributions.second;//station position coefficients 
//		TReal k; //miclosure vector element
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//		if (cnstrIt->getPtXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
//		{//xSt coefficient
//			cs[count].first = cnstrIt->getPtXIndex();
//			cs[count++].second = cgVec.getX().getMetresValue();
//		}
//
//		
//		if (cnstrIt->getPtYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
//		{//ySt coefficient
//			cs[count].first = cnstrIt->getPtYIndex();
//			cs[count++].second = cgVec.getY().getMetresValue();
//		}
//
//		
//		if (cnstrIt->getPtZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
//		{//zSt coefficient
//			cs[count].first = cnstrIt->getPtZIndex();
//			cs[count++].second = cgVec.getZ().getMetresValue();
//		}
//
//		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//		for (int i = 0; i < count; i++)
//		{
//			matrices.setFirstDgnMtrxElement(0, cs[i].first, cs[i].second);
//		}
//
//		//** Adding the contribution to the second design matrix
//		isProcessOK = matrices.setSecondDgnMtrxElement(distEInd, distOInd, -1);
//
//		//** setting the misclosure vector element
//		k = -LITERAL(1.0) * calcOffset.getMetresValue();
//		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);
//
//		//** adding the contributions to the weight matrix
//		isProcessOK = matrices.setWeightMtrxElement(distOInd, distOInd, LITERAL(1.0)/powq(cnstrIt->getSigmaAPriori().getMetresValue(),2));
//
//		if (!isProcessOK)
//		{
//			fError += "Radial offset constraints  contribution generator wrongly processed\n";
//			break;
//		}
//		cnstrIt++;
//
////		matrices.setNewRow();
//	}
//
//	delete[] cs;
//	return;
//}
//
//
///* Add the design matrices contributions for the orientation constraints */
//void	TLSInputMatricesFiller::addOrieCnstrContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices)
//{//process of offset orientation constraints 
//	
//	bool isProcessOK = true;
//
//	LSOrieCnstrConstIter	cnstrIt = data.beginLSOrieCnstr();
//	LSOrieCnstrConstIter	endCnstrIt = data.endLSOrieCnstr();
//
//	while ( cnstrIt != endCnstrIt)
//	{
//		//gets the observation's indices
//		MatrixIndex angEInd = cnstrIt->getEqnIndex();
//		MatrixIndex angOInd = cnstrIt->getObsIndex();
//
//		// get/set the contributions for the observation
//		pair<TAngle, TFreeVector> contributions = fGenerator.getOrientationCnstrDsgnMxContributions(cnstrIt);
//		TAngle &calcBear = contributions.first; // calculated value for the measurement
//		TFreeVector  &cgVec = contributions.second;//station position coefficients 
//		TReal k; //miclosure vector element
//
//
//		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
//
//		if (cnstrIt->getRefXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
//		{//xRef coefficient
//			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
//				cnstrIt->getRefXIndex(), -LITERAL(1.0) * cgVec.getX().getMetresValue());
//		}
//		
//		if (cnstrIt->getRefYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
//		{//yRef coefficient
//			isProcessOK = matrices.setFirstDgnMtrxElement(angOInd, 
//				cnstrIt->getRefYIndex(), -LITERAL(1.0) * cgVec.getY().getMetresValue());
//		}
//
//
//		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
//		isProcessOK = matrices.setSecondDgnMtrxElement(angEInd, angOInd, -1);
//
//		//SETTING MISCLOSURE VECTOR ELEMENT
//		k = 0;
//		if(fIsPdorBearingDefined)
//		{
//			k = -LITERAL(1.0) * (cnstrIt->getBearing() - calcBear).getRadiansValue();
//		}
//		isProcessOK = matrices.setMisclosureVectorElement(angOInd, k);
//
//		//** setting the weight matrix element
//		isProcessOK = matrices.setWeightMtrxElement(angOInd, angOInd, LITERAL(1.0)/powq(cnstrIt->getSigmaAPriori().getRadiansValue(),2));
//
//		if (!isProcessOK)
//		{
//			fError += "Orientation contribution generator wrongly processed\n";
//			return;
//		}
//		cnstrIt++;
//
////		matrices.setNewRow();
//	}
//	return;
//}
//
//
///* Add the design matrices contributions for the constraints of a free network calculation */
///*void	TLSInputMatricesFiller::addFreeCnstrContributions(const LSCalcDataSet& data,TLSInputMatrices& matrices, TLSConstraintIdentifier& cnstr)
//{//process of offset constraints for a free calculation
//	
//	bool isProcessOK = true;
//
////	isProcessOK = contribution.processFreeCnstr(data, matrices, cnstr);
//	
//	if (!isProcessOK)
//	{
//		fError += "Constraints for free Calculation contribution generator wrongly processed\n";
//	}
//	return;
//}
//*/
//
//
//
//
//
