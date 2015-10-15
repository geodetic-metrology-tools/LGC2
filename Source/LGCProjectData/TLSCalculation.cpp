////////////////////////////////////////////////////////////////////
// TLSCalculation.cpp : implementation file
// Class managing the least squares calculation of an LGC project
// from input matrices preparation to final results extraction
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
using namespace std;

#include "TLSParametricMtdComputer.h"
#include "TLSResultsMatrices.h"
#include "TLSInputMatrices.h"
#include "TLSResultsMatricesExtractor.h"
#include "TALSComputer.h"
#include "TLSCalcObservationMaker.h"
#include  "TLGCSimResults.h"

#include "TLGCObsLSContributionGenerator.h"
#include "TLSFreeCnstrCG.h"

#include "TLSCalculation.h"
#include "TSparseMatrix.h"

#include "TLSInputMatricesFiller.h"

#include <QuantileFunctions.h>

#include <memory>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
TLSCalculation::TLSCalculation()
: fResultsMatrices(0), fNumberOfMadeIterations(0), fMaxIterations(10), fGeodSys(false)
{// default constructor
	fIsSimulation = false;
}


TLSCalculation::TLSCalculation(const TLGCCalcParams& calcParams) : fCalcParams(calcParams),
fResultsMatrices(0), fNumberOfMadeIterations(0), fMaxIterations(10), fGeodSys(false)
{//Constructor taking the calculation parameters for initialisation 
	fIsSimulation = false;
}


TLSCalculation::~TLSCalculation()
{// destructor
	if(fResultsMatrices != 0)
	{
		delete fResultsMatrices;
	}
}


////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION
////////////////////////////////////////////////////////////////////

bool TLSCalculation::computeLSResults(const ObservationSet& obsSet, LSCalcDataSet& calcDS, TLGCDataSet& data, TLGCCalcParams *params)
{// computes the solution
	bool lastIteration = false;	

	TLSCalcObservationMaker LSCalcObs(params->isSimulation());

	if ( LSCalcObs.processData(obsSet, calcDS) )
	{
		//order and check point list
		fError = calcDS.checkAllPointsAreUsed(data.getWorkingPoints());
		if(fError == "")
		{
			// assign the matrix indices for the unknown parameters
			calcDS.updateIndices();
			if (calcDS.getDimensions().UIndex > calcDS.getDimensions().EIndex)
			{
				fError += "There are more unknowns than observations!\n";
			}
			else
			{
				// calculate constraints for a free network calculation
				if( params->isLibrDetect() )
				{
					// identify the constraints necessary, create them, and add them to the calculation data set
					TLSConstraintIdentifier freeConstraint;
					freeConstraint.initCnstrIdentifier(calcDS, obsSet);
					calcDS.setFreeConstraints(freeConstraint);
				}

				if (params->areAllPointsFixed())
				{
					lastIteration = computeAllPointsFixedResults(calcDS);
				}
				// either perform requested simulations or process the calculation
				else if( params->isSimulation() )
				{
					lastIteration = computeSimulatedLSResults(obsSet, calcDS, data, params);
				}
				else
				{
					lastIteration = processCalculation(calcDS);
				}
			}
		}
	}
	else
	{
		fError = fError + LSCalcObs.getError();
	}

	return lastIteration;
}

bool TLSCalculation::computeAllPointsFixedResults(LSCalcDataSet& calcDS)
{
	TLSInputMatricesFiller inpMtrFiller(fCalcParams.getRefSurface(), fCalcParams.isPdorBearDefined() );

	fResultsMatrices = new TLSResultsMatrices(calcDS.getDimensions(), calcDS.getFreeConstraints().getNumberOfConstraint());
	TLSInputMatrices inputMtr;
	TLSParametricMtdComputer computer;
	TLSResultsMatricesExtractor extractor(&calcDS);
	
	bool lastIteration = iterate2Solution(calcDS, &inpMtrFiller, &inputMtr, &computer, &extractor);
	calcResiduAndVarCovMatrice(calcDS, &inputMtr, &extractor, &computer);
	extractor.extractResiduals(*fResultsMatrices, fCalcParams);

	if(calcDS.getDimensions().UIndex != 0)
	{//pas d'inconnues
		invertMatriceVarCoVar(calcDS);
		extractVarCovarParams(&extractor);
		calcFautParams(&inputMtr, &extractor);
		calcRelErrorParams( &extractor);
	}
	else
	{//pas de probleme de convergence dans un calcul sans inconnues
		lastIteration = true;
		calcDS.setConvergenceResult(lastIteration);
	}

	if (fError == "")
	{
		extractor.extractAllPointsFixedUnknowns(*fResultsMatrices, fCalcParams);
	}

	return true;
}



bool TLSCalculation::computeSimulatedLSResults(const ObservationSet&, LSCalcDataSet& calcDS,  TLGCDataSet&, TLGCCalcParams *params)
{// computes the solution with simulated values for the observations

	bool calcOK = false;
	int numOfSimMade = 0;

    fRndGenerator.seed();

	//initialise the simulation results storage
	calcOK = calcDS.initSimResults();

	// Run through the first simulation
	// avoids unecessary calls to initialise the data parameters
	try {
		calcOK = processSimCalculation(calcDS);
	} catch (std::exception& e) {
		this->fError = e.what();
		calcOK = false;
	}


	numOfSimMade++;

	// repeat simulation calculations until all are completed 
	// and no errors are generated
	while ( calcOK &&  numOfSimMade < params->getNumOfSim() && getError() == "" )
	{
		// re-initialise the LSCalcDataSet to remove all increments to 
		// estimated values, residuals, etc.
		calcDS.reInitialiseForSim();
		
		// compute the ls results for the current simulation
		calcOK = processSimCalculation(calcDS);

		// increment the simulation count
		numOfSimMade++;
	}

	return calcOK;
}

// Computes the solution for a survey network from data in the LSCalcDataSet 
// and stores the results
bool	TLSCalculation::processCalculation(	LSCalcDataSet& calcDS )
{
	bool lastIteration = false;

	TLSInputMatricesFiller inpMtrFiller(fCalcParams.getRefSurface(), fCalcParams.isPdorBearDefined() );

	fResultsMatrices = new TLSResultsMatrices(calcDS.getDimensions(), calcDS.getFreeConstraints().getNumberOfConstraint());
	TLSInputMatrices inputMtr;
	TLSParametricMtdComputer computer;
	TLSResultsMatricesExtractor extractor(&calcDS);

	lastIteration = iterate2Solution(calcDS, &inpMtrFiller, &inputMtr, &computer, &extractor);
	calcResiduAndVarCovMatrice(calcDS, &inputMtr, &extractor, &computer);
	extractor.extractResiduals(*fResultsMatrices, fCalcParams);

	if(calcDS.getDimensions().UIndex != 0)
	{//il y a des inconnues
		invertMatriceVarCoVar(calcDS);
		extractVarCovarParams(&extractor);
		calcFautParams(&inputMtr, &extractor);
		calcRelErrorParams( &extractor);
	}
	else
	{//pas de probleme de convergence dans un calcul sans inconnues
		lastIteration = true;
		calcDS.setConvergenceResult(lastIteration);
	}

	//delete extractor;
	//delete computer;
	//delete inputMtr;
	//delete inpMtrFiller;

	//les erreurs sont prises en compte au niveau du projet
	return lastIteration;
}

// Process a simulation calculation
// generate a new set of simulated measurement values and proceed to calculation,
// then extract the simulation results and store them
bool	TLSCalculation::processSimCalculation(	LSCalcDataSet& calcDS )
{
	bool calcOK = false;

	// simulate measured values for the LS calc observations
	simulateValues(calcDS);

	// calculate the result for the current simulation
	calcOK = processCalculation(calcDS);

	// if solution reached then extract the results from the current simulation
	if( calcOK )
	{
		//
		// store results from the current simulation calculation
		calcOK = calcDS.updateSimResults();
	}

	return calcOK;
}


	
string	TLSCalculation::getError() const 
{/*!@return the error*/
	return fError;
}



bool	TLSCalculation::iterate2Solution(LSCalcDataSet& calcDS,
										 TLSInputMatricesFiller* inpMtrFiller,
										 TLSInputMatrices* inputMtr,
										 TLSParametricMtdComputer* computer,
										 TLSResultsMatricesExtractor* extractor)
{
	fNumberOfMadeIterations = 0;

	bool lastIteration = false;

	// Iterate to find solution
	while((!lastIteration) && (fNumberOfMadeIterations <= fMaxIterations) && (fError == ""))
	{
		bool mtrFilled = inpMtrFiller->fillMatrices(calcDS, *inputMtr);
		if (mtrFilled)
		{
			if (!fCalcParams.usingSigmaAPriori())
			{
				fS0APosterioriVariances = true;
			}
			else 
			{
				fS0APosterioriVariances = false;
			}

#if _DEBUG
//			inputMtr->getFirstDgnMtrxTransposed()->write_matrix_file("C:\\ATOld.txt");
//			inputMtr->getSecondDgnMtrxTransposed()->write_matrix_file("C:\\BTOld.txt");
//			inputMtr->getWeightMtrx()->write_matrix_file("C:\\WOld.txt");
#endif
			// compute solution 
			bool computationOK = computer->computeResults(inputMtr, fResultsMatrices);
	
#if _DEBUG
			//for (int i = 0; i < fResultsMatrices->getSolutionVctr()->size(); i++)
				//printf("%20.15f\n", (double) fResultsMatrices->getSolutionVctrElmt(i));
#endif

			if (computationOK)
			{	// extract solution and residues from the matrices
				extractor->setReferenceSurface(fCalcParams.getRefSurface());
				bool DSextracted(false);
				DSextracted = extractor->extractResults(*fResultsMatrices, fCalcParams);
				if (DSextracted)
				{
					lastIteration = extractor->lastIteration();
				}
				else
				{
					fError += extractor->getError();
					return true;
				}
			}
			else
			{
				fError += "Probleme dans la resolution du systeme";
				fError += '\n';
				fError += computer->getError();
			}
		}
		else
		{
			fError += inpMtrFiller->getError();
		}
		fNumberOfMadeIterations++;
		//cout<<"iteration numero "<<(fNumberOfMadeIterations)<<endl;
	}

	calcDS.setIterationsCount(fNumberOfMadeIterations);
	calcDS.setConvergenceResult(lastIteration);
	return lastIteration;
}


void	TLSCalculation::invertMatriceVarCoVar(LSCalcDataSet& calcDS)
{
	if (fError == "")
	{
		// TODO: The commented code seems to have more variants (but OLD LGC had only inversion) - revisit
		TSparseMatrix * res = new TSparseMatrix();
		*res = TSparseUtils::inverse(*fResultsMatrices->getUnkCovarMtrx(), fError);
		if (fError.length()!=0)
			return;

		if (calcDS.isForDeformationAnalysis())
		{
			calcDS.setUnknownsCovarianceMtrx(res);
		}
		if (fS0APosterioriVariances)
		{
			(*res) *= fResultsMatrices->getSigmaZero2();
		}
		fResultsMatrices->setUnkCovarMtrx(res);

		/*
		TSparseMatrix *res = NULL;
		
		if (calcDS.getDimensions().UIndex == calcDS.getDimensions().OIndex)
		{
			int rows;
			if (calcDS.getFreeConstraints().getNumberOfConstraint() == 0)
			{
				rows = fResultsMatrices->getL()->rowsCount();
			}
			else
			{
				rows = fResultsMatrices->getBigMatrix()->rowsCount();
			}
			int* colptr = new int[rows + 1];
			for (int i = 0; i <= rows; i++)
			{
				colptr[i] = 0;
			}
			res = new TSparseMatrix(rows, rows, NULL, NULL, colptr);
			delete fResultsMatrices->getL();
		}
		else
		{
			if (calcDS.getFreeConstraints().getNumberOfConstraint() == 0)
			{
				TSparseMatrix *chol = fResultsMatrices->getL();
				TSparseMatrix *cholInverse = chol->invert_lower_triangular_cholesky_decomposed_returning_lower_triangular();
				delete chol;
				TSparseMatrix *cholInverseTransposed = cholInverse->transposed();

				if (!calcDS.isForDeformationAnalysis() && !fCalcParams.isFaultDetectToBeSaved() &&
					fCalcParams.getPtNamePairsSize() == 0 && !fCalcParams.isPunchEOrEE())
				{
					TReal *result = cholInverseTransposed->multiply_returning_diagonal(*cholInverse);

					int* rowind = new int[cholInverse->rowsCount() + 1];
					for (int i = 0; i <= cholInverse->rowsCount(); i++)
					{
						rowind[i] = i;
					}
					res = new TSparseMatrix(cholInverse->rowsCount(), cholInverse->rowsCount(), result, rowind, rowind);
				}
				else
				{
					res = cholInverseTransposed->multiply_F(*cholInverse);
				}
				delete cholInverse;
				delete cholInverseTransposed;
			}
			else
			{
				TSparseMatrix *b = fResultsMatrices->getBigMatrix();
				TMatrix* big = b->to_dense();
				for (int i = 0; i < big->numCols(); i++)
				{
					for (int j = i + 1; j < big->numRows(); j++)
					{
						(*big)(i, j) = (*big)(j, i);
					}
				}
				delete b;
				if (!big->invert())
				{
					fError += "Could not find covariance matrix!\n";
					return ;
				}
				res = big->toSparse();
				delete big;
			}

			if (calcDS.isForDeformationAnalysis())
			{
				calcDS.setUnknownsCovarianceMtrx(res);
			}
			
			if (fS0APosterioriVariances)
			{
				res->multiply_by_number(fResultsMatrices->getSigmaZero2());
			}
		}
		
		fResultsMatrices->setUnkCovarMtrx(res);
		*/
	}
	return;
}





void	TLSCalculation::extractVarCovarParams(TLSResultsMatricesExtractor* extractor)
{// extract of parameters variances and covariances
	if (fError == "")
	{
		extractor->extractVarCovarParams(*fResultsMatrices);
		fError += extractor->getError();
	}
}
	

void	TLSCalculation::calcFautParams(	TLSInputMatrices* inputMtr,
										TLSResultsMatricesExtractor* extractor)
{		
	if (fError == "")
	{
		if (fCalcParams.isFaultDetectToBeSaved())
		{	// computation and extraction of reliability parameters
			TVector obsVar = fResultsMatrices->computeVarObs(inputMtr->getFirstDgnMtrx());
			extractor->computeAndExtractReliability(obsVar,fCalcParams);
			fError += extractor->getError();
		}
	}
	return;
}


void	TLSCalculation::calcRelErrorParams(	TLSResultsMatricesExtractor* extractor)
{
	if (fError == "")
	{
		// Relative Error computation 
		extractor->extractRelError(*fResultsMatrices,fCalcParams);	
		fError += extractor->getError();
	}
	return;
}


void	TLSCalculation::calcResiduAndVarCovMatrice(	LSCalcDataSet& calcDS,
													const TLSInputMatrices* inputMtr, 
													TLSResultsMatricesExtractor* ,
													TLSParametricMtdComputer*)
{
	if (fError == "")
	{
		int nbUnk = inputMtr->getNbrUnknowns();
		int nbObs = inputMtr->getNbrObservations();
		TReal sigmaZero2 = LITERAL(0.0);


		auto firstDM = inputMtr->getFirstDgnMtrx();
		auto weightM = inputMtr->getWeightMtrx();
		const TVector & misclV = inputMtr->getMisclosureVctr();

		TVector * solution = fResultsMatrices->getSolutionVctr();
		TVector * residuals = fResultsMatrices->getResidualsVctr();
		
		
		*residuals = *firstDM * (*solution) + misclV;

		//for (int i = 0; i < nbUnk; i++)
		//{
		//	for (int j = firstDM->columnPointers()[i]; j < firstDM->columnPointers()[i + 1]; j++)
		//	{
		//		(*residuals)(firstDM->rowIndices()[j]) += firstDM->values()[j] * (*solution)(i);
		//	}
		//}

		if (!fCalcParams.isFaultDetectToBeSaved())
		{
			delete firstDM;
			//delete inputMtr->getFirstDgnMtrxTransposed();
		}

		if (calcDS.getDimensions().UIndex == calcDS.getDimensions().OIndex)
		{
			sigmaZero2 = 1;
		}
		else
		{
			//for (int i = 0; i < nbObs; i++)
			//{
			//	sigmaZero2 += (*residuals)(i) * weightM->values()[i] * (*residuals)(i);
			//}
			//
			//sigmaZero2 /= nbObs - nbUnk;

			//TODO
			sigmaZero2 = ( residuals->transpose() * (*weightM) * (*residuals) );
			//sigmaZero2 = sigmaZero2*(scaleK*scaleK);
			sigmaZero2 /= (scaleK*scaleK)*(nbObs - nbUnk);
		}

		delete weightM;

		fResultsMatrices->setSigmaZero2(sigmaZero2);

		struct limits fisherLim =calcSigmaZeroLimits(nbObs, nbUnk, sigmaZero2);

		calcDS.setChiUpLimit(fisherLim.s0PostUpLimit);
		calcDS.setChiLoLimit(fisherLim.s0PostLoLimit);

		// setting of the s0 a poseriori boolean
		fResultsMatrices->setS0APosterioriVariances(fS0APosterioriVariances);

		// Setting of covmatrix done in inverse
		TSparseMatrix * unkcov = new TSparseMatrix(fResultsMatrices->getIntermediateMatrix());
		//if (fS0APosterioriVariances)
		//{
		//	*unkcov /= sigmaZero2;
		//}
		
		/*if (fS0APosterioriVariances)
		{
			*unkcov /= (scaleK*scaleK)*sigmaZero2;
		}
		else*/
			*unkcov /= scaleK*scaleK;

		fResultsMatrices->setUnkCovarMtrx(unkcov);
	}
	return;
}


limits	TLSCalculation::calcSigmaZeroLimits(const int nbObs, const int nbUnk, const TReal sigmaZero2)
{
	// computes and saves limits of interval for test chi of S0 validity
	struct limits resultat;
	resultat.s0PostUpLimit = LITERAL(0.0);
	resultat.s0PostLoLimit = LITERAL(0.0);
	TReal d = LITERAL(0.0);
	
	d = nbObs - nbUnk;

	if(d>0)
	{
        double chiUp = deviates_chi_sq(0.975, d);
        double chiLow = deviates_chi_sq(1-0.975, d);

		if (fError == "")
		{
			//Limits
			resultat.s0PostUpLimit = sqrtq(chiUp/d);
			resultat.s0PostLoLimit = sqrtq(chiLow/d);

			if ((sqrtq(sigmaZero2) < resultat.s0PostUpLimit) && (sqrtq(sigmaZero2) > resultat.s0PostLoLimit))
			{
				fS0APosterioriVariances = false;
			}
		}
	}
	return resultat;
}




////////////////////////////////////////////////////////
// SIMULATION ROUTINES
////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//SIMULATION
///////////////////////////////////////////////////////////////////////////////////////////////////////
void TLSCalculation::simulateValues(LSCalcDataSet& calcData)
{//generate simulated values
	getHorAngSimValues(calcData);
	getZenDistSimValues(calcData);
	getHorDistSimValues(calcData);
	getSpaDistSimValues(calcData);
	// Get values for DVER and DLEV
	getVertDistSimValues(calcData, 0);
	getVertDistSimValues(calcData, 1);

	getOffsetToVerLineSimValues(calcData);
	getOffsetToSpaLineSimValues(calcData);
	getOffsetToVerPlaneSimValues(calcData);
	getOffsetToTheoPlaneSimValues(calcData);

	getGyroOrieSimValues(calcData);

	return;
}



void TLSCalculation::getHorAngSimValues(LSCalcDataSet& calcData)
{
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());
	LSHorAngIter	iter, iterEnd;
	iter = calcData.beginLSHorAng();
	iterEnd = calcData.endLSHorAng();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TAngle simHAng;
		TheoStnContrib contribution = cg.getHorAngDsgnMxContributions(iter);
		calcVal = contribution.fCalcMeas.getRadiansValue();
		sigma = iter->getSigmaAPriori().getRadiansValue();
		
		simHAng.setRadiansValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsAngle(simHAng);
		
		iter++;
	}
}


void TLSCalculation::getZenDistSimValues(LSCalcDataSet& calcData)
{// Zen. dist. simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSZenDistIter	iter, iterEnd;
	iter = calcData.beginLSZenDist();
	iterEnd = calcData.endLSZenDist();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TAngle simZDist;
		TheoStnContrib contribution = cg.getZenDistDsgnMxContributions(iter);
		calcVal = contribution.fCalcMeas.getRadiansValue();
		sigma = iter->getSigmaAPriori().getRadiansValue();
		
		simZDist.setRadiansValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsAngle(simZDist);
		
		iter++;
	}
}


void TLSCalculation::getHorDistSimValues(LSCalcDataSet& calcData)
{// Hor. dist. simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSHorDistIter	iter, iterEnd;
	iter = calcData.beginLSHorDist();
	iterEnd = calcData.endLSHorDist();
	while ( iter != iterEnd ) 
	{
		TReal calcVal, sigma;
		TLength simHDist;
		pair<TLength, TFreeVector> contribution = cg.getHorDistDsgnMxContributions(iter);
		calcVal = contribution.first.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simHDist.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simHDist);
		
		iter++;
	}
}


void TLSCalculation::getSpaDistSimValues(LSCalcDataSet& calcData)
{// Spa. dist. simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSSpaDistIter	iter, iterEnd;
	iter = calcData.beginLSSpaDist();
	iterEnd = calcData.endLSSpaDist();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TLength simSDist;
		DistStnContrib contribution = cg.getSpaDistDsgnMxContributions(iter);
		calcVal = contribution.fCalcMeas.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simSDist.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simSDist);
		
		iter++;
	}
}



void TLSCalculation::getVertDistSimValues(LSCalcDataSet& calcData, bool isDLEV)
{// Vert. dist. simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSVertDistIter	iter, iterEnd;
	iter = calcData.beginLSVertDist(isDLEV);
	iterEnd = calcData.endLSVertDist(isDLEV);
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TLength simVDist;
		LevelStnContrib contribution = isDLEV ? cg.getDLEVDsgnMxContributions(iter) : cg.getVertDistDsgnMxContributions(iter);
		calcVal = contribution.fCalcMeas.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simVDist.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simVDist);
		
		iter++;
	}
}


void TLSCalculation::getOffsetToVerLineSimValues(LSCalcDataSet& calcData)
{//offset to vertical line simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSOffsetToVerLineIter	iter, iterEnd;
	iter = calcData.beginLSOffsetToVerLine();
	iterEnd = calcData.endLSOffsetToVerLine();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TLength simOffset;
		pair<TLength, TFreeVector> contribution = cg.getOffsetToVerLineDsgnMxContributions(iter);
		calcVal = contribution.first.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simOffset.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simOffset);
		
		iter++;
	}
}


void TLSCalculation::getOffsetToSpaLineSimValues(LSCalcDataSet& calcData)
{//offset to spatial line simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSOffsetToSpaLineIter	iter, iterEnd;
	iter = calcData.beginLSOffsetToSpaLine();
	iterEnd = calcData.endLSOffsetToSpaLine();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TLength simOffset;
		OffsetStnContrib contribution = cg.getOffsetToSpaLineDsgnMxContributions(iter);
		calcVal = contribution.fCalcMeas.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simOffset.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simOffset);
		
		iter++;
	}
}


void TLSCalculation::getOffsetToVerPlaneSimValues(LSCalcDataSet& calcData)
{//offset to vertical plane simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSOffsetToVerPlaneIter	iter, iterEnd;
	iter = calcData.beginLSOffsetToVerPlane();
	iterEnd = calcData.endLSOffsetToVerPlane();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TLength simOffset;
		OffsetStnContrib contribution = cg.getOffsetToVerPlaneDsgnMxContributions(iter);
		calcVal = contribution.fCalcMeas.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simOffset.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simOffset);
		
		iter++;
	}
}


void TLSCalculation::getOffsetToTheoPlaneSimValues(LSCalcDataSet& calcData)
{//offset to theodolite plane simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSOffsetToTheoPlaneIter	iter, iterEnd;
	iter = calcData.beginLSOffsetToTheoPlane();
	iterEnd = calcData.endLSOffsetToTheoPlane();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TLength simOffset;
		TheoOffsetStnContrib contribution = cg.getOffsetToTheoPlaneDsgnMxContributions(iter);
		calcVal = contribution.fCalcMeas.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simOffset.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simOffset);
		
		iter++;
	}
}


void TLSCalculation::getGyroOrieSimValues(LSCalcDataSet& calcData)
{//gyro. orientation simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSGyroOrieIter	iter, iterEnd;
	iter = calcData.beginLSGyroOrie();
	iterEnd = calcData.endLSGyroOrie();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TAngle simOrie;
		pair<TAngle, TFreeVector> contribution = cg.getGyroOrieDsgnMxContributions(iter);
		calcVal = contribution.first.getRadiansValue();
		sigma = iter->getSigmaAPriori().getRadiansValue();
		
		simOrie.setRadiansValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsAngle(simOrie);
		
		iter++;
	}
}


////////////////////////////////////////////////////////
// Returns the simulated value for a value and its sigma
////////////////////////////////////////////////////////

TReal TLSCalculation::getSimulatedValue(const TReal val, const TReal sigma)
{
	TReal zz;
	
    // zz = val + rndMM->Gaussian(sigma);
    zz = val + std::tr1::normal_distribution<double>(0, sigma)(fRndGenerator);
	
	return zz;
}

/*TReal TLSCalculation::randomNormal(int& z)
{
	static bool spareVal = false;
	static TReal storedValue;
	TReal v1, v2;
	TReal rr = LITERAL(1.0), zz;
	
	if (spareVal == false)
	{
		while (rr >= LITERAL(1.0) || rr == LITERAL(0.0))
		{
			// generate two random uniform numbers in (-1,1)
			v1 = LITERAL(2.0)*randomUniform(z)-LITERAL(1.0);
			v2 = LITERAL(2.0)*randomUniform(z)-LITERAL(1.0);
			rr = v1*v1 + v2*v2;
		}
		
		zz = sqrtq(-LITERAL(2.0)*__logq(rr)/rr);
		storedValue = v1 * zz;
		spareVal = true;
		zz = v2 * zz;
	}
	else
	{
		spareVal = false;
		zz = storedValue;
	}

	return zz;
}*/

/*TReal TLSCalculation::randomUniform(int& z)
{
//Minimal Standard Linear Congruential Generator
// implemented using Schrage's algorithm
// see Numerical Recipes in C++ pp278-283
	
	// params for the generator of number
	const int a = 16807, m = 2147483647, q = 127773, r = 2836;
	const TReal reciprocalM = LITERAL(1.0)/(TReal)m;
	int k;
	TReal ru;

	// generate random number
	k = z/q;
	z = a*(z - k*q) - r*k;
	if (z < 0)
	{
		z += m;
	}

	// scale number to lie between 0 and 1
	ru = z * reciprocalM;
	return ru;
}*/


/* To Finish!
TReal TLSCalculation::randomUniform2(int& z)
{
//Minimal Standard Linear Congruential Generator
// implemented using Schrage's algorithm with a shuffle of return values
// and other safeguards
// see Numerical Recipes in C++ pp278-284
	
	// params for the generator of number
	const int a = 16807, m = 2147483647, q = 127773, r = 2836;
	const TReal vSmNum = 3.0e-16, reciprocalM = LITERAL(1.0)/(TReal)m, maxNum = (LITERAL(1.0) - vSMNum);
	int k;
	TReal ru;

	// generate random number
	k = z/q;
	z = a*(z - k*q) - r*k;
	if (z < 0)
	{
		z += m;
	}

	// scale number to lie between 0 and 1
	ru = z * reciprocalM;
	return ru;
*/




/*
TReal TLSCalculation::getSimulatedValue(const TReal val, const TReal sigma)
{
	TReal v1, v2;
	TReal rr = LITERAL(1.0), zz;
	while (rr >= LITERAL(1.0))
	{
		generateUniformValues(v1, v2);
		rr = v1*v1 + v2*v2;
	}

	zz = val + sigma * v1 * sqrtq(-LITERAL(2.0)*__logq(rr)/rr);
	
	return zz;
}
void TLSCalculation::generateUniformValues(TReal& a1, TReal& a2)
{

	
	// params for the generator of number
	int r = 7,t = 11;
	long rdi, tdi, ai1, ai2;
	rdi = (long)powq(r,r);
	tdi = (long)powq(2,t);
	
	ai1 = (long)(rdi * fUniformLastValue);
	ai1 = ai1%tdi;
	

//	Deux nombres aleatoires distribues selon la loi uniforme (0,1)

	ai2 = ai1*rdi;
	ai2 = ai2%tdi;

	a1 = TReal(ai1)/TReal(tdi);
	a2 = TReal(ai2)/TReal(tdi);

	a1 = LITERAL(2.0) * a1 - LITERAL(1.0);
	a2 = LITERAL(2.0) * a2 - LITERAL(1.0);

	fUniformLastValue = TReal(ai2);
}
*/
	
/////////////////////////////////////////////////////////////////////////////
//END Simulation Routines
/////////////////////////////////////////////////////////////////////////////
