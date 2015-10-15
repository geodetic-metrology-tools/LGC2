////////////////////////////////////////////////////////////////////
// TLSCalculation.cpp : implementation file
// Class managing the least squares calculation of an LGC project
// from input matrices preparation to final results extraction
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
using namespace std;
#include "TLSMatricesAdapter.h"
#include "TLSResultsMatricesExtractor.h"
#include "TLSCalcObservationMaker.h"
#include "TLGCSimResults.h"

#include "TLGCObsLSContributionGenerator.h"
#include "TLSFreeCnstrCG.h"

#include "TLSCalculation.h"

#include <QuantileFunctions.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
TLSCalculation::TLSCalculation()
: fResultsMatrices(0), fNumberOfMadeIterations(0), fMaxIterations(10), fGeodSys(false)
{// default constructor
	fIsSimulation = false;

	//fUniformLastValue = nag_random_continuous_uniform();
	// initialise for calls to MathMore library
	//rndMM = 0;
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

bool TLSCalculation::computeLSResults(LSCalcDataSet& calcDS, TLGCDataSet& data, TLGCCalcParams *params)
{// computes the solution
	bool lastIteration = false;	

	TLSCalcObservationMaker* LSCalcObs = new TLSCalcObservationMaker();

	if ( LSCalcObs->processData(data, calcDS) )
	{
		//order and check point list
		fError = calcDS.checkAllPointsAreUsed(data.getWorkingPoints());
		if(fError == "")
		{
			// assign the matrix indices for the unknown parameters
			calcDS.updateIndices();

			if (calcDS.getDimensions().UIndex > calcDS.getDimensions().EIndex + calcDS.getConstraintDimensions().EIndex)
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
					freeConstraint.initCnstrIdentifier(calcDS, data);
					calcDS.setFreeConstraints(freeConstraint);
				}

				if (params->areAllPointsFixed())
				{
					lastIteration = computeAllPointsFixedResults(calcDS);
				}
				// either perform requested simulations or process the calculation
				else if( params->isSimulation() )
				{
					lastIteration = computeSimulatedLSResults(calcDS, data, params);
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
		fError = fError + LSCalcObs->getError();
	}
	delete LSCalcObs;

	return lastIteration;
}

bool TLSCalculation::computeAllPointsFixedResults(LSCalcDataSet& calcDS)
{
	TLSInputMatricesFiller* inpMtrFiller = new TLSInputMatricesFiller(fCalcParams.getRefSurface(), fCalcParams.isPdorBearDefined() );

	fResultsMatrices = new TLSResultsMatricesAdapter(calcDS.getDimensions());
	TLSInputMatricesAdapter* inputMtr = new TLSInputMatricesAdapter();
	TLSParametricMtdComputerAdapter* computer = new TLSParametricMtdComputerAdapter();
	TLSResultsMatricesExtractor* extractor = new TLSResultsMatricesExtractor(&calcDS);

	bool lastIteration = iterate2Solution(calcDS, inpMtrFiller, inputMtr, computer, extractor);
	calcResiduAndVarCovMatrice(calcDS,inputMtr);
	extractor->extractResiduals(*fResultsMatrices);

	if(calcDS.getDimensions().UIndex != 0)
	{//pas d'inconnues
		invertMatriceVarCoVar(calcDS);
		extractVarCovarParams(extractor);
		calcFautParams(fResultsMatrices, extractor);
		calcRelErrorParams( extractor);
	}
	else
	{//pas de probleme de convergence dans un calcul sans inconnues
		lastIteration = true;
		calcDS.setConvergenceResult(lastIteration);
	}

	if (fError == "")
	{
		extractor->extractAllPointsFixedUnknowns();
	}

	return true;
}



bool TLSCalculation::computeSimulatedLSResults(LSCalcDataSet& calcDS,  TLGCDataSet& data, TLGCCalcParams *params)
{// computes the solution with simulated values for the observations

	bool calcOK = false;
	int numOfSimMade = 0;

    fRndGenerator.seed(0);

	//initialise the simulation results storage
	calcOK = calcDS.initSimResults();

	// Run through the first simulation
	// avoids unecessary calls to initialise the data parameters
	calcOK = processSimCalculation(calcDS, data);

	numOfSimMade++;

	// repeat simulation calculations until all are completed 
	// and no errors are generated
	while ( calcOK &&  numOfSimMade < params->getNumOfSim() && getError() == "" )
	{
		// re-initialise the LSCalcDataSet to remove all increments to 
		// estimated values, residuals, etc.
		calcDS.reInitialiseForSim();
		
		// compute the ls results for the current simulation
		calcOK = processSimCalculation(calcDS, data);

		// increment the simulation count
		numOfSimMade++;
	}

	// terminate for calls to mathmore library
	//rndMM->Terminate();
	//delete rndMM;
	//rndMM = 0;

	return calcOK;
}


// Computes the solution for a survey network from data in the LSCalcDataSet 
// and stores the results
bool	TLSCalculation::processCalculation(	LSCalcDataSet& calcDS )
{
	bool lastIteration = false;

	TLSInputMatricesFiller* inpMtrFiller = new TLSInputMatricesFiller(fCalcParams.getRefSurface(), fCalcParams.isPdorBearDefined() );

	fResultsMatrices = new TLSResultsMatricesAdapter(calcDS.getDimensions());
	TLSInputMatricesAdapter* inputMtr = new TLSInputMatricesAdapter();
	TLSParametricMtdComputerAdapter* computer = new TLSParametricMtdComputerAdapter();
	TLSResultsMatricesExtractor* extractor = new TLSResultsMatricesExtractor(&calcDS);

	lastIteration = iterate2Solution(calcDS, inpMtrFiller, inputMtr, computer, extractor);
	calcResiduAndVarCovMatrice(calcDS,inputMtr);
	extractor->extractResiduals(*fResultsMatrices);

	if(calcDS.getDimensions().UIndex != 0)
	{//pas d'inconnues
		invertMatriceVarCoVar(calcDS);
		extractVarCovarParams(extractor);
		calcFautParams(fResultsMatrices, extractor);
		calcRelErrorParams( extractor);
	}
	else
	{//pas de probleme de convergence dans un calcul sans inconnues
		lastIteration = true;
		calcDS.setConvergenceResult(lastIteration);
	}

	delete extractor;
	delete computer;
	delete inputMtr;
	delete inpMtrFiller;

	//les erreurs sont prises en compte au niveau du projet
	return lastIteration;
}

// Process a simulation calculation
// generate a new set of simulated measurement values and proceed to calculation,
// then extract the simulation results and store them
bool	TLSCalculation::processSimCalculation(	LSCalcDataSet& calcDS,  TLGCDataSet& data )
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
		calcOK = calcDS.updateSimResults(calcDS, data);
	}

	return calcOK;
}


	
string	TLSCalculation::getError() const 
{/*!@return the error*/
	return fError;
}



bool	TLSCalculation::iterate2Solution(LSCalcDataSet& calcDS,
										 TLSInputMatricesFiller* inpMtrFiller,
										 TLSInputMatricesAdapter* inputMtr,
										 TLSParametricMtdComputerAdapter* computer,
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
			//inputMtr->getFirstDgnMtrxTransposed()->write_matrix_file("C:\\AT.txt");
			//inputMtr->getSecondDgnMtrxTransposed()->write_matrix_file("C:\\BT.txt");
			//inputMtr->getWeightMtrx()->write_matrix_file("C:\\W.txt");
#endif
			// compute solution 
			bool computationOK = computer->computeResults(inputMtr, fResultsMatrices, calcDS.isCombinedCase(), calcDS.isLibrOptionUsed() || calcDS.numOffsetToVerPlaneObs() > 0, 
				calcDS.getConstraintDimensions().EIndex > 0, fCalcParams.isFaultDetectToBeSaved());
	
#if _DEBUG
			//for (int i = 0; i < fResultsMatrices->getSolutionVctr()->dimension(); i++)
			//	printf("%20.15f\n", (double) fResultsMatrices->getSolutionVctrElmt(i));
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


void	TLSCalculation::invertMatriceVarCoVar(LSCalcDataSet& )
{
// is handled in the matrix computer now
#if 0
	if (fError == "")
	{
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
			if (calcDS.isLibrOptionUsed() || calcDS.numOffsetToVerPlaneObs() > 0)
			{
				TSparseMatrix *b = fResultsMatrices->getBigMatrix();
				/*TReal* fft;
				res = b->ldlt_decompose_lower_triangular_returning_lower_triangular(fft)->invert_lower_triangular_ldlt_decomposed(fft);*/

				// TODO: when using ECHO, LU decomposition DOES NOT work! Implement LDLT (or something) for 
				// a DENSE matrix (in the TMatrix class) and use that instead of "invert" below
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
			else if (calcDS.getConstraintDimensions().EIndex > 0)
			{
				// TODO: implement
			}
			else
			{
				// this is the no-constraints case
				TSparseMatrix *chol = fResultsMatrices->getL();
				TSparseMatrix *cholInverse = chol->invert_lower_triangular_cholesky_decomposed_returning_lower_triangular();
				delete chol;
				TSparseMatrix *cholInverseTransposed = cholInverse->transposed();

				if (!calcDS.isForDeformationAnalysis() && !fCalcParams.isFaultDetectToBeSaved() &&
					fCalcParams.getPtNamePairsSize() == 0 && !fCalcParams.isPunchEOrEE())
				{
					// we need only the diagonal
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
	}
	return;
#endif
}





void	TLSCalculation::extractVarCovarParams(TLSResultsMatricesExtractor* extractor)
{// extract of parameters variances and covariances
	if (fError == "")
	{
		extractor->extractVarCovarParams(*fResultsMatrices);
		fError += extractor->getError();
	}
}
	

void	TLSCalculation::calcFautParams(	TLSResultsMatricesAdapter* rm,
										TLSResultsMatricesExtractor* extractor)
{		
	if (fError == "")
	{
		if (fCalcParams.isFaultDetectToBeSaved())
		{	// computation and extraction of reliability parameters
			extractor->computeAndExtractReliability(rm->getVarObs(), fCalcParams);
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


void	TLSCalculation::calcResiduAndVarCovMatrice(	LSCalcDataSet& ,
													const TLSInputMatricesAdapter* )
{

}




////////////////////////////////////////////////////////
// SIMULATION ROUTINES
////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//SIMULATION
///////////////////////////////////////////////////////////////////////////////////////////////////////
void TLSCalculation::simulateValues(LSCalcDataSet& calcData)
{//generate simulated values

	// TODO: add the new observation types

	getPolarSimValues(calcData);
	getHorAngSimValues(calcData);
	getZenDistSimValues(calcData);
	getHorDistSimValues(calcData);
	getSpaDistSimValues<TheodoliteTarget>(calcData.beginLSSpaDist(), calcData.endLSSpaDist());
	getSpaDistSimValues<EDMTarget>(calcData.beginLSEDMSpaDist(), calcData.endLSEDMSpaDist());
	getVertDistSimValues(calcData);
	getLevelObsSimValues(calcData);
	// TODO: should output error if SIMU and DLEV's without horizontal distances - when reading the file

	getOffsetToVerLineSimValues(calcData);
	getOffsetToSpaLineSimValues(calcData);
	getOffsetToVerPlaneSimValues(calcData);
	getOffsetToTheoPlaneSimValues(calcData);

	getGyroOrieSimValues(calcData);

	return;
}


void TLSCalculation::getPolarSimValues(LSCalcDataSet& calcData)
{
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());
	LSPolarIter	iter, iterEnd;
	iter = calcData.beginLSPolar();
	iterEnd = calcData.endLSPolar();
	while ( iter != iterEnd )
	{
		TReal sigma;
		TAngle simAng;
		TLength simLen;
		PolarContrib contribution = cg.getPolarDsgnMxContributions(iter);

		sigma = iter->getHorizontalAngleObservation()->getSigmaAPriori().getRadiansValue();		
		simAng.setRadiansValue(getSimulatedValue(contribution.haCalcMeas, sigma));
		iter->getHorizontalAngleObservation()->setSimulatedObsAngle(simAng);

		sigma = iter->getZenithDistanceObservation()->getSigmaAPriori().getRadiansValue();		
		simAng.setRadiansValue(getSimulatedValue(contribution.zdCalcMeas, sigma));
		iter->getZenithDistanceObservation()->setSimulatedObsAngle(simAng);

		sigma = iter->getSpatialDistanceObservation()->getSigmaAPriori().getMetresValue();		
		simLen.setMetresValue(getSimulatedValue(contribution.sdCalcMeas, sigma));
		iter->getSpatialDistanceObservation()->setSimulatedObsDist(simLen);
		
		iter++;
	}
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

template <typename T>
void TLSCalculation::getSpaDistSimValues(typename list<TLSCalcSpatialDistObservation<T> >::iterator iter,
										 typename list<TLSCalcSpatialDistObservation<T> >::iterator iterEnd)
{
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TLength simSDist;
		DistStnContrib contribution = cg.getSpaDistDsgnMxContributions<T>(iter);
		calcVal = contribution.fCalcMeas.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simSDist.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simSDist);
		
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
		pair<TLength, TFreeVector> contribution = cg.getHorDistDsgnMxContributions<TheodoliteTarget>(iter);
		calcVal = contribution.first.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simHDist.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simHDist);
		
		iter++;
	}
}


void TLSCalculation::getVertDistSimValues(LSCalcDataSet& calcData)
{// Vert. dist. simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSVertDistIter	iter, iterEnd;
	iter = calcData.beginLSVertDist();
	iterEnd = calcData.endLSVertDist();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TLength simVDist;
		LevelStnContrib contribution = cg.getVertDistDsgnMxContributions(iter);
		calcVal = contribution.fCalcMeas.getMetresValue();
		sigma = iter->getSigmaAPriori().getMetresValue();
		
		simVDist.setMetresValue(getSimulatedValue(calcVal, sigma));
		iter->setSimulatedObsDist(simVDist);
		
		iter++;
	}
}


void TLSCalculation::getLevelObsSimValues(LSCalcDataSet& calcData)
{// Vert. dist. simulated values
	TLGCObsLSContributionGenerator  cg(fCalcParams.getRefSurface());

	LSLevelIter	iter, iterEnd;
	iter = calcData.beginLSLevelObs();
	iterEnd = calcData.endLSLevelObs();
	while ( iter != iterEnd )
	{
		TReal calcVal, sigma;
		TLength simVDist;
		LevelStnContrib contribution = cg.getLevelObsDsgnMxContributions(iter);
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
		OffsetToVerLineContrib contribution = cg.getOffsetToVerLineDsgnMxContributions(iter);
		calcVal = contribution.fCalcMeas.getMetresValue();
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
	
	//zz = val + rndMM->Gaussian(sigma);
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
		
		zz = sqrtq(-LITERAL(2.0)*logq(rr)/rr);
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

	zz = val + sigma * v1 * sqrtq(-LITERAL(2.0)*logq(rr)/rr);
	
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
