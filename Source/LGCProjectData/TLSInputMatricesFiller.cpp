////////////////////////////////////////////////////////////////////
// TinputMatricesFiller.cpp : implementation file
// Class responsible for reading the least squares calculationmeasurements and constraints 
// and filling the least squares matrices
// Specific to a least squares calculation
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////


#include "TLSInputMatricesFiller.h"

#include "TLSMatricesAdapter.h"
#include "LSCalcDataSet.h"

#include "TLGCObsLSContributionGenerator.h"
#include "TLSFreeCnstrCG.h"

#include "TModifiedLocalAstronomicalRF.h"
#include "TARefFrameTransformation.h"
#include "TGC2MLATransformation.h"
#include "TMLA2GCTransformation.h"

#include "TLGCDataSet.h"
#include "TRefSystemFactory.h"

int pairCompare(const void* p1, const void* p2)
{
	if ((*((pair<int, TReal>*) p1)).first < (*((pair<int, TReal>*) p2)).first)
		return -1;
	if ((*((pair<int, TReal>*) p1)).first == (*((pair<int, TReal>*) p2)).first)
		return 0;
	return 1;
}


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
void TLSInputMatricesFiller::initMatriceDimension(const LSCalcDataSet& data, TLSInputMatricesAdapter& matrices)
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

	if(data.hasConstraints())
	{
		int cnstrNumber = data.getFreeConstraints().getNumberOfConstraint();
		if (cnstrNumber != 0 || data.numOffSpaLineCnstr() != 0 || data.numOffVerPlaneCnstr() != 0)
		{
			// Setting of the input matrices size with the observations & parameters indices
			matrices.setDimensions(ueoi.UIndex, ueoi.EIndex, ueoi.OIndex, cnstrObs,
				cnstrNumber, data.numOffSpaLineCnstr() * 2 + data.numOffVerPlaneCnstr() * 2);
			matrices.setConstraintNewColumn();
			matrices.setConstraintTransposedNewColumn();

			if (cnstrNumber != 0)
			{
				processFreeCnstr(data, matrices, data.getFreeConstraints());
			}

			if(data.numOffSpaLineCnstr() != 0)
				addOffSpaLineCnstrContributions(data, matrices);

			if(data.numOffVerPlaneCnstr() != 0)
				addOffVerPlaneCnstrContributions(data, matrices);
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



bool TLSInputMatricesFiller::fillMatrices(LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{

	bool filled = false;

	initMatriceDimension(data,matrices);
	matrices.setNewColumn();

	if ( fError == "" )
	{
		filled = true;
		
		if(data.numPolarObs() != 0)
			addPolarContributions(data, matrices);

		if(data.numHorAngObs() != 0)
			addHorAngContributions(data, matrices);

		if(data.numZenDistObs() != 0)
			addZenDistContributions(data, matrices);

		if(data.numSpaDistObs() != 0)
			addSpaDistContributions<TheodoliteTarget>(data.beginLSSpaDist(), data.endLSSpaDist(), matrices);

		if(data.numEDMSpaDistObs() != 0)
			addSpaDistContributions<EDMTarget>(data.beginLSEDMSpaDist(), data.endLSEDMSpaDist(), matrices);

		if(data.numHorDistObs() != 0)
			addHorDistContributions<TheodoliteTarget>(data.beginLSHorDist(), data.endLSHorDist(), matrices);
		
		if(data.numLevelHorDistObs() != 0)
			addHorDistContributions<Staff>(data.beginLSLevelHorDist(), data.endLSLevelHorDist(), matrices);

		if(data.numVertDistObs() != 0)
			addVertDistContributions(data, matrices);

		if (data.numLevelObs() != 0)
			addLevelObsContributions(data, matrices);

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
	}
	else
	{
		cout << fError << endl;
		filled = false;
	}

	matrices.finishedFillingMatrices();

	return filled;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
//OBSERVATION PROCESSING
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	TLSInputMatricesFiller::processFreeCnstr(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices, const TLSConstraintIdentifier& cnstr)
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

void TLSInputMatricesFiller::addPolarContributions(LSCalcDataSet& data, TLSInputMatricesAdapter& matrices)
{
	LSPolarIter	obsIt1 = data.beginLSPolar();
	LSPolarConstIter	endObsIt = data.endLSPolar();
	
    pair<int, TReal>* cs = new pair<int, TReal>[9];
    int count;
	while ( obsIt1 != endObsIt)
	{
		const TLSCalcHorAngleObservation* obsIt = obsIt1->getHorizontalAngleObservation();
		//gets the observation's indices
		MatrixIndex angOInd = obsIt1->getObsIndex();

		// get/set the contributions for the observation
		PolarContrib contributions = fGenerator.getPolarDsgnMxContributions(obsIt1);
		TFreeVector*  cgVec = contributions.fCoordContrib;//target position coefficients (negative values give station coefficients)

		for (int i = 0; i < 3; i++)
		{
			count = 0;
			bool isProcessOK = true;

			//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
			if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && cgVec[i].getX().getMetresValue() != 0)
			{//xSt coefficient
				cs[count].first = obsIt->getStXIndex();
				cs[count++].second = -cgVec[i].getX().getMetresValue();
			}

			
			if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && cgVec[i].getY().getMetresValue() != 0)
			{//ySt coefficient
				cs[count].first = obsIt->getStYIndex();
				cs[count++].second = -cgVec[i].getY().getMetresValue();
			}

			
			if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && cgVec[i].getZ().getMetresValue() != 0)
			{//zSt coefficient
				cs[count].first = obsIt->getStZIndex();
				cs[count++].second = -cgVec[i].getZ().getMetresValue();
			}

			
			if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && cgVec[i].getX().getMetresValue() != 0)
			{//xTg coefficient
				cs[count].first = obsIt->getTgXIndex();
				cs[count++].second = cgVec[i].getX().getMetresValue();
			}

			
			if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && cgVec[i].getY().getMetresValue() != 0)
			{//yTg coefficient
				cs[count].first = obsIt->getTgYIndex();
				cs[count++].second = cgVec[i].getY().getMetresValue();
			}

			
			if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && cgVec[i].getZ().getMetresValue() != 0)
			{//zTg coefficient
				cs[count].first = obsIt->getTgZIndex();
				cs[count++].second = cgVec[i].getZ().getMetresValue();
			}

			
			if (obsIt->getV0()->getKappaStatus() == TALSCalcParameter::kVariable && contributions.rotAnglesContrib[i].kappa.getRadiansValue() != 0)
			{//kappa
				cs[count].first = obsIt->getV0()->getKappaIndex();
				cs[count++].second = contributions.rotAnglesContrib[i].kappa.getRadiansValue();
			}

			
			if (obsIt->getV0()->getOmegaStatus() == TALSCalcParameter::kVariable && contributions.rotAnglesContrib[i].omega.getRadiansValue() != 0)
			{//omega
				cs[count].first = obsIt->getV0()->getOmegaIndex();
				cs[count++].second = contributions.rotAnglesContrib[i].omega.getRadiansValue();
			}

			
			if (obsIt->getV0()->getPhiStatus() == TALSCalcParameter::kVariable && contributions.rotAnglesContrib[i].phi.getRadiansValue() != 0)
			{//phi
				cs[count].first = obsIt->getV0()->getPhiIndex();
				cs[count++].second = contributions.rotAnglesContrib[i].phi.getRadiansValue();
			}

			qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

			for (int j = 0; j < count; j++)
			{
				matrices.setFirstDgnMtrxTransElement(cs[j].first, cs[j].second);
			}

			//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
			isProcessOK = isProcessOK && matrices.setSecondDgnMtrxTransElement(angOInd, contributions.haContrib[i]);
			isProcessOK = isProcessOK && matrices.setSecondDgnMtrxTransElement(angOInd + 1, contributions.zdContrib[i]);
			isProcessOK = isProcessOK && matrices.setSecondDgnMtrxTransElement(angOInd + 2, contributions.sdContrib[i]);

			//SETTING MISCLOSURE VECTOR ELEMENT
			isProcessOK = isProcessOK && matrices.setMisclosureVectorElement(angOInd + i, contributions.miscVecElements[i]);

			//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
			if (i == 0)
			{
				obsIt1->getHorizontalAngleObservation()->calculateSigma();
				isProcessOK = isProcessOK && matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt1->getHorizontalAngleObservation()->getSigmaAPriori().getRadiansValue(), 2));
			}
			else if (i == 1)
			{
				obsIt1->getZenithDistanceObservation()->calculateSigma();
				isProcessOK = isProcessOK && matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt1->getZenithDistanceObservation()->getSigmaAPriori().getRadiansValue(), 2));
			}
			else
			{
				obsIt1->getSpatialDistanceObservation()->calculateSigma();
				isProcessOK = isProcessOK && matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt1->getSpatialDistanceObservation()->getSigmaAPriori().getMetresValue(), 2));
			}
			
			if (!isProcessOK)
			{
				fError += "Polar observation contribution generators wrongly processed\n";
				break;
			}

			matrices.setNewColumn();
		}

		obsIt1++;
	}
    delete[] cs;
}

/* Add the design matrices contributions for the hor. angle observations */
void TLSInputMatricesFiller::addHorAngContributions(LSCalcDataSet& data, TLSInputMatricesAdapter& matrices)
{//process of horizontal angle observations

	bool isProcessOK = true;
	LSHorAngIter	obsIt = data.beginLSHorAng();
	LSHorAngConstIter	endObsIt = data.endLSHorAng();

    pair<int, TReal>* cs = new pair<int, TReal>[7];
    int count;
	while ( obsIt != endObsIt)
	{
        count = 0;

		//gets the observation's indices
		MatrixIndex angOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		TheoStnContrib contributions = fGenerator.getHorAngDsgnMxContributions(obsIt);
		TAngle &calcHAng = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
		TReal e = contributions.fV0Contrib;//v0 coefficient
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
		{//xSt coefficient
			cs[count].first = obsIt->getStXIndex();
			cs[count++].second = cgVec.getX().getMetresValue();
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
		{//ySt coefficient
			cs[count].first = obsIt->getStYIndex();
			cs[count++].second = cgVec.getY().getMetresValue();
		}

		
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
		{//ySt coefficient
			cs[count].first = obsIt->getStZIndex();
			cs[count++].second = cgVec.getZ().getMetresValue();
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTgXIndex();
			cs[count++].second = -cgVec.getX().getMetresValue();
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgYIndex();
			cs[count++].second = -cgVec.getY().getMetresValue();
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgZIndex();
			cs[count++].second = -cgVec.getZ().getMetresValue();
		}

		
		if (obsIt->getV0Status() == TALSCalcParameter::kVariable)
		{//v0 coefficient
			cs[count].first = obsIt->getV0Index();
			cs[count++].second = e;
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}

		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = isProcessOK && matrices.setSecondDgnMtrxTransElement(angOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -LITERAL(1.0) * (obsIt->getObsAngle()  - calcHAng + obsIt->getEstimatedV0()).getRadiansValue();
		isProcessOK = isProcessOK && matrices.setMisclosureVectorElement(angOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		obsIt->calculateSigma();
		isProcessOK = isProcessOK && matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getRadiansValue(), 2));

		if (!isProcessOK)
		{
			fError += "Horizontal angle contribution generators wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}

    delete[] cs;
	return;
}


/* Add the design matrices contributions for the zenithal angle observations */
void TLSInputMatricesFiller::addZenDistContributions(LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{//process of zenithal angle observations

	bool isProcessOK = true;

	LSZenDistIter	obsIt = data.beginLSZenDist();
	LSZenDistConstIter	endObsIt = data.endLSZenDist();

    pair<int, TReal>* cs = new pair<int, TReal>[7];
    int count;
	while ( obsIt != endObsIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex angOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation		
		TheoStnContrib contributions = fGenerator.getZenDistDsgnMxContributions(obsIt);
		TAngle &calcZDist = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
		TReal g = contributions.fHIContrib;// instrument height contribution
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
		{//xSt coefficient
			cs[count].first = obsIt->getStXIndex();
			cs[count++].second = cgVec.getX().getMetresValue();
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
		{//ySt coefficient
			cs[count].first = obsIt->getStYIndex();
			cs[count++].second = cgVec.getY().getMetresValue();
		}

		
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
		{//zSt coefficient
			cs[count].first = obsIt->getStZIndex();
			cs[count++].second = cgVec.getZ().getMetresValue();
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTgXIndex();
			cs[count++].second = -cgVec.getX().getMetresValue();
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgYIndex();
			cs[count++].second = -cgVec.getY().getMetresValue();
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
		{//zTg coefficient
			cs[count].first = obsIt->getTgZIndex();
			cs[count++].second = -cgVec.getZ().getMetresValue();
		}

		//hi coefficient
		if (obsIt->getHInstStatus() == TALSCalcParameter::kVariable)
		{
			cs[count].first = obsIt->getHInstIndex();
			cs[count++].second = g;
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}

		//**Adding the contribution to the second design matrix
		isProcessOK = matrices.setSecondDgnMtrxTransElement(angOInd, -1);

		//** setting the misclosure vector element
		k = -LITERAL(1.0) * (obsIt->getObsAngle()- calcZDist).getRadiansValue();
		isProcessOK = matrices.setMisclosureVectorElement(angOInd, k);

		//** setting the weight matrix element
		obsIt->calculateSigma();
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getRadiansValue(), 2));


		if (!isProcessOK)
		{
			fError += "Zenithal distance contribution generators wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}

template <typename T>
void TLSInputMatricesFiller::addSpaDistContributions(typename list<TLSCalcSpatialDistObservation<T> >::iterator obsIt, typename list<TLSCalcSpatialDistObservation<T> >::const_iterator endObsIt,TLSInputMatricesAdapter& matrices)
{//process of spatial distance observations
	
	bool isProcessOK = true;

    pair<int, TReal>* cs = new pair<int, TReal>[8];
    int count;
	while ( obsIt != endObsIt )
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		DistStnContrib contributions = fGenerator.getSpaDistDsgnMxContributions<T>(obsIt);
		TLength &calcDist = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
		TReal g = contributions.fHIContrib;//Instrument Height coefficient
		TReal h = contributions.fConstContrib;//Unknown measurement constant contribution
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
		{//xSt coefficient
			cs[count].first = obsIt->getStXIndex();
			cs[count++].second = cgVec.getX().getMetresValue();
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
		{//ySt coefficient
			cs[count].first = obsIt->getStYIndex();
			cs[count++].second = cgVec.getY().getMetresValue();
		}

		
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
		{//zSt coefficient
			cs[count].first = obsIt->getStZIndex();
			cs[count++].second = cgVec.getZ().getMetresValue();
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTgXIndex();
			cs[count++].second = -cgVec.getX().getMetresValue();
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgYIndex();
			cs[count++].second = -cgVec.getY().getMetresValue();
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
		{//zTg coefficient
			cs[count].first = obsIt->getTgZIndex();
			cs[count++].second = -cgVec.getZ().getMetresValue();
		}

		//hi coefficient
		if (obsIt->getHInstStatus() == TALSCalcParameter::kVariable)
		{
			cs[count].first = obsIt->getHInstIndex();
			cs[count++].second = g;
		}

		//c coefficient
		if (obsIt->getDistConstStatus() == TALSCalcParameter::kVariable){
			cs[count].first = obsIt->getDistConstIndex();
			cs[count++].second = h;
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}
		
		//**Adding the contribution to the second design matrix
		isProcessOK = matrices.setSecondDgnMtrxTransElement(distOInd, -1);

		//** setting the misclosure vector element
		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//** setting the weight matrix element
		obsIt->calculateSigma();
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(), 2));

		if (!isProcessOK)
		{
			fError += "Spatial distance contribution generator wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}


/* Add the design matrices contributions for the hor. dist. observations */
template <typename T>
void	TLSInputMatricesFiller::addHorDistContributions(typename list<TLSCalcHorDistObservation<T> >::iterator obsIt, typename list<TLSCalcHorDistObservation<T> >::const_iterator endObsIt,TLSInputMatricesAdapter& matrices)
{// process of horizontal distance observations

	bool isProcessOK = true;

    pair<int, TReal>* cs = new pair<int, TReal>[4];
    int count;
	while ( obsIt != endObsIt )
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		pair<TLength, TFreeVector> contributions = fGenerator.getHorDistDsgnMxContributions<T>(obsIt);
		TLength &calcDist = contributions.first; // calculated value for the measurement
		TReal a = contributions.second.getX().getMetresValue();//station position x coefficient (negative values give target coefficient)
		TReal b = contributions.second.getY().getMetresValue();//station position y coefficient (negative values give target coefficient)
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && a != 0)
		{//xSt coefficient
			cs[count].first = obsIt->getStXIndex();
			cs[count++].second = a;
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && b != 0)
		{//ySt coefficient
			cs[count].first = obsIt->getStYIndex();
			cs[count++].second = b;
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && a != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTgXIndex();
			cs[count++].second = -a;
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && b != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgYIndex();
			cs[count++].second = -b;
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}
		
		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxTransElement(distOInd, -1);


		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(), 2));


		if (!isProcessOK)
		{
			fError += "Horizontal distance contribution generator wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}

/* Add the design matrices contributions for the vertical dist. observations */
void	TLSInputMatricesFiller::addVertDistContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{//process of vertical distance observations

	bool isProcessOK = true;

	LSVertDistConstIter	obsIt = data.beginLSVertDist();
	LSVertDistConstIter	endObsIt = data.endLSVertDist();

    pair<int, TReal>* cs = new pair<int, TReal>[6];
    int count;
	while ( obsIt != endObsIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		LevelStnContrib contributions = fGenerator.getVertDistDsgnMxContributions(obsIt);
		TLength &calcDist = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &cgRefVec = contributions.fRefCoordContrib;//reference position coefficients (negative values give target coefficients)
		TFreeVector  &cgTgtVec = contributions.fTgtCoordContrib;//target position coefficients (negative values give target coefficients)
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getTg1XStatus() == TALSCalcParameter::kVariable && cgRefVec.getX().getMetresValue() != 0)
		{//xSt coefficient
			cs[count].first = obsIt->getTg1XIndex();
			cs[count++].second = cgRefVec.getX().getMetresValue();
		}

		
		if (obsIt->getTg1YStatus() == TALSCalcParameter::kVariable && cgRefVec.getY().getMetresValue() != 0)
		{//ySt coefficient
			cs[count].first = obsIt->getTg1YIndex();
			cs[count++].second = cgRefVec.getY().getMetresValue();
		}

		
		if (obsIt->getTg1ZStatus() == TALSCalcParameter::kVariable && cgRefVec.getZ().getMetresValue() != 0)
		{//zSt coefficient
			cs[count].first = obsIt->getTg1ZIndex();
			cs[count++].second = cgRefVec.getZ().getMetresValue();
		}

		
		if (obsIt->getTg2XStatus() == TALSCalcParameter::kVariable && cgTgtVec.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTg2XIndex();
			cs[count++].second = cgTgtVec.getX().getMetresValue();
		}

		
		if (obsIt->getTg2YStatus() == TALSCalcParameter::kVariable && cgTgtVec.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTg2YIndex();
			cs[count++].second = cgTgtVec.getY().getMetresValue();
		}

		
		if (obsIt->getTg2ZStatus() == TALSCalcParameter::kVariable && cgTgtVec.getZ().getMetresValue() != 0)
		{//zTg coefficient
			cs[count].first = obsIt->getTg2ZIndex();
			cs[count++].second = cgTgtVec.getZ().getMetresValue();
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}
		
		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxTransElement(distOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		// TODO: not all sigmas for vertical distance and leveling measurements are used!
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(),2));

		if (!isProcessOK)
		{
			fError += "Vertical distance contribution generator wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}

void	TLSInputMatricesFiller::addLevelObsContributions(const LSCalcDataSet& data, TLSInputMatricesAdapter& matrices)
{
	bool isProcessOK = true;

	LSLevelConstIter	obsIt = data.beginLSLevelObs();
	LSLevelConstIter	endObsIt = data.endLSLevelObs();

    pair<int, TReal>* cs = new pair<int, TReal>[6];
    int count;
	while ( obsIt != endObsIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		LevelStnContrib contributions = fGenerator.getLevelObsDsgnMxContributions(obsIt);
		TLength &calcDist = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &cgRefVec = contributions.fRefCoordContrib;//reference position coefficients (negative values give target coefficients)
		TFreeVector  &cgTgtVec = contributions.fTgtCoordContrib;//target position coefficients (negative values give target coefficients)
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && cgRefVec.getX().getMetresValue() != 0)
		{//xSt coefficient
			cs[count].first = obsIt->getStXIndex();
			cs[count++].second = cgRefVec.getX().getMetresValue();
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && cgRefVec.getY().getMetresValue() != 0)
		{//ySt coefficient
			cs[count].first = obsIt->getStYIndex();
			cs[count++].second = cgRefVec.getY().getMetresValue();
		}

		
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && cgRefVec.getZ().getMetresValue() != 0)
		{//zSt coefficient
			cs[count].first = obsIt->getStZIndex();
			cs[count++].second = cgRefVec.getZ().getMetresValue();
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && cgTgtVec.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTgXIndex();
			cs[count++].second = cgTgtVec.getX().getMetresValue();
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && cgTgtVec.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgYIndex();
			cs[count++].second = cgTgtVec.getY().getMetresValue();
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && cgTgtVec.getZ().getMetresValue() != 0)
		{//zTg coefficient
			cs[count].first = obsIt->getTgZIndex();
			cs[count++].second = cgTgtVec.getZ().getMetresValue();
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}
		
		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxTransElement(distOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue() - calcDist.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(), 2));

		if (!isProcessOK)
		{
			fError += "Vertical distance contribution generator wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}

    delete[] cs;
}


/* Add the design matrices contributions for the offset to ver. line observations */
void	TLSInputMatricesFiller::addOffsetToVerLineContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{//process of offset ECVE observations
	
	bool isProcessOK = true;

	LSOffsetToVerLineConstIter	obsIt = data.beginLSOffsetToVerLine();
	LSOffsetToVerLineConstIter	endObsIt = data.endLSOffsetToVerLine();

    pair<int, TReal>* cs = new pair<int, TReal>[6];
    int count;
	while ( obsIt != endObsIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		OffsetToVerLineContrib contributions = fGenerator.getOffsetToVerLineDsgnMxContributions(obsIt);
		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &vecCG1 = contributions.fCoordContrib;
		TFreeVector  &ptLine = contributions.ptOnLineContrib;
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTgXIndex();
			cs[count++].second = vecCG1.getX().getMetresValue();
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgYIndex();
			cs[count++].second = vecCG1.getY().getMetresValue();
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
		{//zTg coefficient
			cs[count].first = obsIt->getTgZIndex();
			cs[count++].second = vecCG1.getZ().getMetresValue();
		}
		
		if (obsIt->getPointOnLine() != obsIt->getTg() && obsIt->getPointOnLine()->getXStatus() == TALSCalcParameter::kVariable && ptLine.getX().getMetresValue() != 0)
		{//xOr coefficient
			cs[count].first = obsIt->getPointOnLine()->getXIndex();
			cs[count++].second = ptLine.getX().getMetresValue();
		}
		
		if (obsIt->getPointOnLine() != obsIt->getTg() && obsIt->getPointOnLine()->getYStatus() == TALSCalcParameter::kVariable && ptLine.getY().getMetresValue() != 0)
		{//yOr coefficient
			cs[count].first = obsIt->getPointOnLine()->getYIndex();
			cs[count++].second = ptLine.getY().getMetresValue();
		}
		
		if (obsIt->getPointOnLine() != obsIt->getTg() && obsIt->getPointOnLine()->getZStatus() == TALSCalcParameter::kVariable && ptLine.getZ().getMetresValue() != 0)
		{//zOr coefficient
			cs[count].first = obsIt->getPointOnLine()->getZIndex();
			cs[count++].second = ptLine.getZ().getMetresValue();
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}


		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxTransElement(distOInd, contributions.secondDesignMatrixContrib);

		//SETTING MISCLOSURE VECTOR ELEMENT
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, contributions.miscVecContrib);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(), 2));

		if (!isProcessOK)
		{
			fError += "Offset to vertical line contribution generator wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}


/* Add the design matrices contributions for the offset to spa. line observations */
void	TLSInputMatricesFiller::addOffsetToSpaLineContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{//process of offset ESCP observations
	
	bool isProcessOK = true;

	LSOffsetToSpaLineConstIter	obsIt = data.beginLSOffsetToSpaLine();
	LSOffsetToSpaLineConstIter	endObsIt = data.endLSOffsetToSpaLine();

    pair<int, TReal>* cs = new pair<int, TReal>[9];
    int count;
	while ( obsIt != endObsIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		OffsetStnContrib contributions = fGenerator.getOffsetToSpaLineDsgnMxContributions(obsIt);
		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &stC = contributions.fTgCoordContrib;//station position coefficients (negative values give target coefficients)
		TFreeVector  &plC = contributions.fOriginCoordContrib;//1st target position coefficients (negative values give target coefficients)
		TFreeVector  &unV = contributions.fUnitVectorCoordContrib;//2nd target position coefficients (negative values give target coefficients)
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && stC.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTgXIndex();
			cs[count++].second = stC.getX().getMetresValue();
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && stC.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgYIndex();
			cs[count++].second = stC.getY().getMetresValue();
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && stC.getZ().getMetresValue() != 0)
		{//zTg coefficient
			cs[count].first = obsIt->getTgZIndex();
			cs[count++].second = stC.getZ().getMetresValue();
		}

		if (obsIt->getPointOnLine()->getXStatus() == TALSCalcParameter::kVariable && plC.getX().getMetresValue() != 0)
		{
			cs[count].first = obsIt->getPointOnLine()->getXIndex();
			cs[count++].second = plC.getX().getMetresValue();
		}

		if (obsIt->getPointOnLine()->getYStatus() == TALSCalcParameter::kVariable && plC.getY().getMetresValue() != 0)
		{
			cs[count].first = obsIt->getPointOnLine()->getYIndex();
			cs[count++].second = plC.getY().getMetresValue();
		}

		if (obsIt->getPointOnLine()->getZStatus() == TALSCalcParameter::kVariable && plC.getZ().getMetresValue() != 0)
		{
			cs[count].first = obsIt->getPointOnLine()->getZIndex();
			cs[count++].second = plC.getZ().getMetresValue();
		}

		if (obsIt->getUnitVector()->getName() == "two points")
		{
			if (obsIt->getSecondPointOnLine()->getXStatus() == TALSCalcParameter::kVariable && unV.getX().getMetresValue() != 0)
			{
				cs[count].first = obsIt->getSecondPointOnLine()->getXIndex();
				cs[count++].second = unV.getX().getMetresValue();
			}

			if (obsIt->getSecondPointOnLine()->getYStatus() == TALSCalcParameter::kVariable && unV.getY().getMetresValue() != 0)
			{
				cs[count].first = obsIt->getSecondPointOnLine()->getYIndex();
				cs[count++].second = unV.getY().getMetresValue();
			}

			if (obsIt->getSecondPointOnLine()->getZStatus() == TALSCalcParameter::kVariable && unV.getZ().getMetresValue() != 0)
			{
				cs[count].first = obsIt->getSecondPointOnLine()->getZIndex();
				cs[count++].second = unV.getZ().getMetresValue();
			}
		}
		else
		{
			if (obsIt->getUnitVector()->getXStatus() == TALSCalcParameter::kVariable && unV.getX().getMetresValue() != 0)
			{
				cs[count].first = obsIt->getUnitVector()->getXIndex();
				cs[count++].second = unV.getX().getMetresValue();
			}

			if (obsIt->getUnitVector()->getYStatus() == TALSCalcParameter::kVariable && unV.getY().getMetresValue() != 0)
			{
				cs[count].first = obsIt->getUnitVector()->getYIndex();
				cs[count++].second = unV.getY().getMetresValue();
			}

			if (obsIt->getUnitVector()->getZStatus() == TALSCalcParameter::kVariable && unV.getZ().getMetresValue() != 0)
			{
				cs[count].first = obsIt->getUnitVector()->getZIndex();
				cs[count++].second = unV.getZ().getMetresValue();
			}
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}

		//** Adding the contribution to the second design matrix
		isProcessOK = matrices.setSecondDgnMtrxTransElement(distOInd, contributions.fDistanceContrib.getMetresValue());

		//** setting the misclosure vector element
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, contributions.miscVecContrib);

		//** adding the contributions to the weight matrix
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(), 2));


		if (!isProcessOK)
		{
			fError += "Offset to spatial line contribution generator wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}


/* Add the design matrices contributions for the offset to ver. plane observations */
void	TLSInputMatricesFiller::addOffsetToVerPlaneContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{//process of offset ECHO observations
	
	bool isProcessOK = true;

	LSOffsetToVerPlaneConstIter	obsIt = data.beginLSOffsetToVerPlane();
	LSOffsetToVerPlaneConstIter	endObsIt = data.endLSOffsetToVerPlane();

    pair<int, TReal>* cs = new pair<int, TReal>[7];
    int count;
	while ( obsIt != endObsIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		OffsetStnContrib contributions = fGenerator.getOffsetToVerPlaneDsgnMxContributions(obsIt);
		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &vecCG1 = contributions.fTgCoordContrib;
		TFreeVector  &vecCG2 = contributions.fUnitVectorCoordContrib;
		TLength &dist = contributions.fDistanceContrib;
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getMeasuredPointXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getMeasuredPointXIndex();
			cs[count++].second = vecCG1.getX().getMetresValue();
		}
		
		if (obsIt->getMeasuredPointYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getMeasuredPointYIndex();
			cs[count++].second = vecCG1.getY().getMetresValue();
		}

		if (obsIt->getMeasuredPointZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
		{//zTg coefficient
			cs[count].first = obsIt->getMeasuredPointZIndex();
			cs[count++].second = vecCG1.getZ().getMetresValue();
		}

		cs[count].first = obsIt->getPlaneNormalVector()->getXIndex();
		cs[count++].second = vecCG2.getX().getMetresValue();

		cs[count].first = obsIt->getPlaneNormalVector()->getYIndex();
		cs[count++].second = vecCG2.getY().getMetresValue();

		cs[count].first = obsIt->getPlaneNormalVector()->getZIndex();
		cs[count++].second = vecCG2.getZ().getMetresValue();

		cs[count].first = obsIt->getOriginPointDistanceFromPlaneUnknown()->getIndex();
		cs[count++].second = dist.getMetresValue();

		// no sorting needed here
		// qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}

		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxTransElement(distOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -LITERAL(1.0) * (obsIt->getObsDist().getMetresValue()- calcOffset.getMetresValue());
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(), 2));

		if (!isProcessOK)
		{
			fError += "Offset to vertical plane contribution generator wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}


/* Add the design matrices contributions for the offset to theo. plane observations */
void	TLSInputMatricesFiller::addOffsetToTheoPlaneContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{//process of offset ECTH observations
	
	bool isProcessOK = true;

	LSOffsetToTheoPlaneConstIter	obsIt = data.beginLSOffsetToTheoPlane();
	LSOffsetToTheoPlaneConstIter	endObsIt = data.endLSOffsetToTheoPlane();

    pair<int, TReal>* cs = new pair<int, TReal>[7];
    int count;
	while ( obsIt != endObsIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex distOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		TheoOffsetStnContrib contributions = fGenerator.getOffsetToTheoPlaneDsgnMxContributions(obsIt);
		TLength &calcOffset = contributions.fCalcMeas; // calculated value for the measurement
		TFreeVector  &vecCG1 = contributions.fCoordContrib;//station position coefficients (negative values give target coefficients)
		TReal v = contributions.fV0Contrib;//v0 coefficient
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
		{//xSt coefficient
			cs[count].first = obsIt->getStXIndex();
			cs[count++].second = vecCG1.getX().getMetresValue();
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
		{//ySt coefficient
			cs[count].first = obsIt->getStYIndex();
			cs[count++].second = vecCG1.getY().getMetresValue();
		}

		
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
		{//zSt coefficient
			cs[count].first = obsIt->getStZIndex();
			cs[count++].second = vecCG1.getZ().getMetresValue();
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTgXIndex();
			cs[count++].second = -vecCG1.getX().getMetresValue();
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgYIndex();
			cs[count++].second = -vecCG1.getY().getMetresValue();
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
		{//zTg coefficient
			cs[count].first = obsIt->getTgZIndex();
			cs[count++].second = -vecCG1.getZ().getMetresValue();
		}

		
		if (obsIt->getV0Status() == TALSCalcParameter::kVariable)
		{//v0 coefficient
			cs[count].first = obsIt->getV0Index();
			cs[count++].second = v;
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}

		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxTransElement(distOInd, -1);
		
		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -LITERAL(1.0) * (obsIt->getObsDist() - calcOffset).getMetresValue();
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getMetresValue(), 2));

		if (!isProcessOK)
		{
			fError += "Offset to theodolite plane  contribution generator wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}


/* Add the design matrices contributions for the gyro. orientation observations */
void	TLSInputMatricesFiller::addGyroOrieContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{//process of gyro. orientation observations
	
	bool isProcessOK = true;

	LSGyroOrieConstIter	obsIt = data.beginLSGyroOrie();
	LSGyroOrieConstIter	endObsIt = data.endLSGyroOrie();

    pair<int, TReal>* cs = new pair<int, TReal>[6];
    int count;
	while ( obsIt != endObsIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex angOInd = obsIt->getObsIndex();

		// get/set the contributions for the observation
		pair<TAngle, TFreeVector> contributions = fGenerator.getGyroOrieDsgnMxContributions(obsIt);
		TAngle &calcHAng = contributions.first; // calculated value for the measurement
		TFreeVector  &vecCG1 = contributions.second;//station position coefficients (negative values give target coefficients)
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (obsIt->getStXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
		{//xSt coefficient
			cs[count].first = obsIt->getStXIndex();
			cs[count++].second = vecCG1.getX().getMetresValue();
		}

		
		if (obsIt->getStYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
		{//ySt coefficient
			cs[count].first = obsIt->getStYIndex();
			cs[count++].second = vecCG1.getY().getMetresValue();
		}

		
		if (obsIt->getStZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
		{//zSt coefficient
			cs[count].first = obsIt->getStZIndex();
			cs[count++].second = vecCG1.getZ().getMetresValue();
		}

		
		if (obsIt->getTgXStatus() == TALSCalcParameter::kVariable && vecCG1.getX().getMetresValue() != 0)
		{//xTg coefficient
			cs[count].first = obsIt->getTgXIndex();
			cs[count++].second = -vecCG1.getX().getMetresValue();
		}

		
		if (obsIt->getTgYStatus() == TALSCalcParameter::kVariable && vecCG1.getY().getMetresValue() != 0)
		{//yTg coefficient
			cs[count].first = obsIt->getTgYIndex();
			cs[count++].second = -vecCG1.getY().getMetresValue();
		}

		
		if (obsIt->getTgZStatus() == TALSCalcParameter::kVariable && vecCG1.getZ().getMetresValue() != 0)
		{//zTg coefficient
			cs[count].first = obsIt->getTgZIndex();
			cs[count++].second = -vecCG1.getZ().getMetresValue();
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}

		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxTransElement(angOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = -LITERAL(1.0) * (obsIt->getObsAngle() - calcHAng).getRadiansValue();
		isProcessOK = matrices.setMisclosureVectorElement(angOInd, k);

		//ADDING CONTRIBUTIONS TO THE WEIGHT MATRIX
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(obsIt->getSigmaAPriori().getRadiansValue(), 2));

		if (!isProcessOK)
		{
			fError += "Offset to gyro. orientation  contribution generator wrongly processed\n";
			break;
		}
		obsIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}


/* Add the design matrices contributions for the radial offset constraints */
void	TLSInputMatricesFiller::addRadOffCnstrContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{//process of offset radial constraints
	
	bool isProcessOK = true;

	LSRadOffCnstrConstIter	cnstrIt = data.beginLSRadOffCnstr();
	LSRadOffCnstrConstIter	endCnstrIt = data.endLSRadOffCnstr();

    pair<int, TReal>* cs = new pair<int, TReal>[3];
    int count;
	while ( cnstrIt != endCnstrIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex distOInd = cnstrIt->getObsIndex();

		// get/set the contributions for the observation
		pair<TLength, TFreeVector> contributions = fGenerator.getRadOffCnstrDsgnMxContributions(cnstrIt);
		TLength &calcOffset = contributions.first; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.second;//station position coefficients 
		TReal k; //miclosure vector element

		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX
		if (cnstrIt->getPtXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
		{//xSt coefficient
			cs[count].first = cnstrIt->getPtXIndex();
			cs[count++].second = cgVec.getX().getMetresValue();
		}

		
		if (cnstrIt->getPtYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
		{//ySt coefficient
			cs[count].first = cnstrIt->getPtYIndex();
			cs[count++].second = cgVec.getY().getMetresValue();
		}

		
		if (cnstrIt->getPtZStatus() == TALSCalcParameter::kVariable && cgVec.getZ().getMetresValue() != 0)
		{//zSt coefficient
			cs[count].first = cnstrIt->getPtZIndex();
			cs[count++].second = cgVec.getZ().getMetresValue();
		}

		qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			matrices.setFirstDgnMtrxTransElement(cs[i].first, cs[i].second);
		}

		//** Adding the contribution to the second design matrix
		isProcessOK = matrices.setSecondDgnMtrxTransElement(distOInd, -1);

		//** setting the misclosure vector element
		k = -LITERAL(1.0) * calcOffset.getMetresValue();
		isProcessOK = matrices.setMisclosureVectorElement(distOInd, k);

		//** adding the contributions to the weight matrix
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(cnstrIt->getSigmaAPriori().getMetresValue(),2));

		if (!isProcessOK)
		{
			fError += "Radial offset constraints  contribution generator wrongly processed\n";
			break;
		}
		cnstrIt++;

		matrices.setNewColumn();
	}
    delete[] cs;
	return;
}

/* Add the design matrices contributions for the orientation constraints */
void	TLSInputMatricesFiller::addOrieCnstrContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{//process of offset orientation constraints 
	
	bool isProcessOK = true;

	LSOrieCnstrConstIter	cnstrIt = data.beginLSOrieCnstr();
	LSOrieCnstrConstIter	endCnstrIt = data.endLSOrieCnstr();

	while ( cnstrIt != endCnstrIt )
	{
		//gets the observation's indices
		MatrixIndex angOInd = cnstrIt->getObsIndex();

		// get/set the contributions for the observation
		pair<TAngle, TFreeVector> contributions = fGenerator.getOrientationCnstrDsgnMxContributions(cnstrIt);
		TAngle &calcBear = contributions.first; // calculated value for the measurement
		TFreeVector  &cgVec = contributions.second;//station position coefficients 
		TReal k; //miclosure vector element


		//ADDING CONTRIBUTIONS TO THE FIRST DESIGN MATRIX

		if (cnstrIt->getRefXStatus() == TALSCalcParameter::kVariable && cgVec.getX().getMetresValue() != 0)
		{//xRef coefficient
			isProcessOK = matrices.setFirstDgnMtrxTransElement(cnstrIt->getRefXIndex(), -LITERAL(1.0) * cgVec.getX().getMetresValue());
		}
		
		if (cnstrIt->getRefYStatus() == TALSCalcParameter::kVariable && cgVec.getY().getMetresValue() != 0)
		{//yRef coefficient
			isProcessOK = matrices.setFirstDgnMtrxTransElement(cnstrIt->getRefYIndex(), -LITERAL(1.0) * cgVec.getY().getMetresValue());
		}


		//ADDING CONTRIBUTIONS TO THE SECOND DESIGN MATRIX
		isProcessOK = matrices.setSecondDgnMtrxTransElement(angOInd, -1);

		//SETTING MISCLOSURE VECTOR ELEMENT
		k = 0;
		if(fIsPdorBearingDefined)
		{
			k = -LITERAL(1.0) * (cnstrIt->getBearing() - calcBear).getRadiansValue();
		}
		isProcessOK = matrices.setMisclosureVectorElement(angOInd, k);

		//** setting the weight matrix element
		isProcessOK = matrices.setWeightMtrxElement(LITERAL(1.0)/powq(cnstrIt->getSigmaAPriori().getRadiansValue(),2));

		if (!isProcessOK)
		{
			fError += "Orientation contribution generator wrongly processed\n";
			break;
		}
		cnstrIt++;

		matrices.setNewColumn();
	}
	return;
}


void	TLSInputMatricesFiller::addOffSpaLineCnstrContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{
	bool isProcessOK = true;


	
	// TODO: Analyze why this is called seperately from the non-transposed version
	throw std::logic_error("FIXME: addOffSpaLineCnstrContributions uses strange setter for constraint matrix.");


	LSOffsetToSpaLineCstrConstIter	cnstrIt = data.beginLSOffSpaLineCnstr();
	LSOffsetToSpaLineCstrConstIter	endCnstrIt = data.endLSOffSpaLineCnstr();

    pair<int, TReal>* cs = new pair<int, TReal>[6];
    int count;
	while ( cnstrIt != endCnstrIt)
	{
        count = 0;
		//gets the observation's indices
		MatrixIndex angOInd = cnstrIt->getObsIndex() + data.getFreeConstraints().getNumberOfConstraint();

		// get/set the contributions for the observation
		OffsetSpatialLineConstraint contributions = fGenerator.getOffSpaLineCnstrDsgnMxContributions(cnstrIt);
		TFreeVector &pointOnLineContrib = contributions.firstCstrPtOnLineContrib;
		TFreeVector  &unitContrib = contributions.firstCstrUnitContrib;

		//ADDING CONTRIBUTIONS TO THE FIRST CONSTRAINT DESIGN MATRIX
		// for the first constraint
		cs[count].first = cnstrIt->getPointOnLine()->getXIndex();
		cs[count++].second = pointOnLineContrib.getX().getMetresValue();
		cs[count].first = cnstrIt->getPointOnLine()->getYIndex();
		cs[count++].second = pointOnLineContrib.getY().getMetresValue();
		cs[count].first = cnstrIt->getPointOnLine()->getZIndex();
		cs[count++].second = pointOnLineContrib.getZ().getMetresValue();
		
		cs[count].first = cnstrIt->getUnitVector()->getXIndex();
		cs[count++].second = unitContrib.getX().getMetresValue();
		cs[count].first = cnstrIt->getUnitVector()->getYIndex();
		cs[count++].second = unitContrib.getY().getMetresValue();
		cs[count].first = cnstrIt->getUnitVector()->getZIndex();
		cs[count++].second = unitContrib.getZ().getMetresValue();

		// No sorting is needed here, as the indices are in ascending order anyway.
		// qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			//matrices.setCnstrFirstDgnMtrxTransposedElement(cs[i].first, cs[i].second);
		}

		//SETTING MISCLOSURE VECTOR ELEMENT
		isProcessOK = matrices.setCnstrMisclosureVectorElement(angOInd, contributions.firstCstrMiscVec);
		
		// new row for the second constraint
		matrices.setConstraintTransposedNewColumn();
		count = 0;
		
		unitContrib = contributions.secondCstrUnitContrib;

		//ADDING CONTRIBUTIONS TO THE FIRST CONSTRAINT DESIGN MATRIX
		// for the second constraint

		cs[count].first = cnstrIt->getUnitVector()->getXIndex();
		cs[count++].second = unitContrib.getX().getMetresValue();
		cs[count].first = cnstrIt->getUnitVector()->getYIndex();
		cs[count++].second = unitContrib.getY().getMetresValue();
		cs[count].first = cnstrIt->getUnitVector()->getZIndex();
		cs[count++].second = unitContrib.getZ().getMetresValue();

		// No sorting is needed here, as the indices are in ascending order anyway.
		// qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < count; i++)
		{
			//matrices.setCnstrFirstDgnMtrxTransposedElement(cs[i].first, cs[i].second);
		}

		//SETTING MISCLOSURE VECTOR ELEMENT
		isProcessOK = matrices.setCnstrMisclosureVectorElement(angOInd + 1, contributions.secondCstrMiscVec);

		if (!isProcessOK)
		{
			fError += "Offset to spa line constraint contribution generator wrongly processed\n";
			break;
		}
		cnstrIt++;

		matrices.setConstraintTransposedNewColumn();
	}
    delete[] cs;
	return;
}


void	TLSInputMatricesFiller::addOffVerPlaneCnstrContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices)
{
	bool isProcessOK = true;

	
	// TODO: Analyze why this is called seperately from the non-transposed version
	throw std::logic_error("FIXME: addOffVerPlaneCnstrContributions uses strange setter for constraint matrix.");



	LSOffsetToVerPlaneCstrConstIter	cnstrIt = data.beginLSOffVerPlaneCnstr();
	LSOffsetToVerPlaneCstrConstIter	endCnstrIt = data.endLSOffVerPlaneCnstr();

    pair<int, TReal>* cs = new pair<int, TReal>[3];
    int count;
	while ( cnstrIt != endCnstrIt)
	{
		//gets the observation's indices
		MatrixIndex angOInd = cnstrIt->getObsIndex() + data.getFreeConstraints().getNumberOfConstraint();

		// get/set the contributions for the observation
		pair<pair<TReal, TFreeVector>, pair<TReal, TFreeVector> > contributions = fGenerator.getOffVerPlaneCnstrDsgnMxContributions(cnstrIt);
		TFreeVector& unitContrib = contributions.first.second;

		//ADDING CONTRIBUTIONS TO THE FIRST CONSTRAINT DESIGN MATRIX
		// for the first constraint
		
		cs[0].first = cnstrIt->getUnitVector()->getXIndex();
		cs[0].second = unitContrib.getX().getMetresValue();
		cs[1].first = cnstrIt->getUnitVector()->getYIndex();
		cs[1].second = unitContrib.getY().getMetresValue();
		cs[2].first = cnstrIt->getUnitVector()->getZIndex();
		cs[2].second = unitContrib.getZ().getMetresValue();

		// No sorting is needed here, as the indices are in ascending order anyway.
		// qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < 3; i++)
		{
			//matrices.setCnstrFirstDgnMtrxTransposedElement(cs[i].first, cs[i].second);
		}

		//SETTING MISCLOSURE VECTOR ELEMENT
		isProcessOK = matrices.setCnstrMisclosureVectorElement(angOInd, contributions.first.first);

		// new row for the second constraint
		matrices.setConstraintTransposedNewColumn();

		unitContrib = contributions.second.second;

		//ADDING CONTRIBUTIONS TO THE FIRST CONSTRAINT DESIGN MATRIX
		// for the second constraint
		
		cs[0].first = cnstrIt->getUnitVector()->getXIndex();
		cs[0].second = unitContrib.getX().getMetresValue();
		cs[1].first = cnstrIt->getUnitVector()->getYIndex();
		cs[1].second = unitContrib.getY().getMetresValue();
		cs[2].first = cnstrIt->getUnitVector()->getZIndex();
		cs[2].second = unitContrib.getZ().getMetresValue();

		// No sorting is needed here, as the indices are in ascending order anyway.
		// qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);

		for (int i = 0; i < 3; i++)
		{
			//matrices.setCnstrFirstDgnMtrxTransposedElement(cs[i].first, cs[i].second);
		}

		//SETTING MISCLOSURE VECTOR ELEMENT
		isProcessOK = matrices.setCnstrMisclosureVectorElement(angOInd + 1, contributions.second.first);

		if (!isProcessOK)
		{
			fError += "Offset to ver plane constraint contribution generator wrongly processed\n";
			break;
		}
		cnstrIt++;

		matrices.setConstraintTransposedNewColumn();
	}
    delete[] cs;
	return;
}

/* Add the design matrices contributions for the constraints of a free network calculation */
/*void	TLSInputMatricesFiller::addFreeCnstrContributions(const LSCalcDataSet& data,TLSInputMatricesAdapter& matrices, TLSConstraintIdentifier& cnstr)
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





