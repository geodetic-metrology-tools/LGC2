//
// TLSCalculation.cpp : implementation file
// Class managing the ls calculation from input matrices preparation to final
// results extraction

#include <iostream>
using namespace std;

#include "TLSParametricMtdComputer.h"
#include "TLSResultsMatrices.h"
#include "TLSInputMatrices.h"
#include "TLSResultsMatricesExtractor.h"
#include "TALSComputer.h"
#include "TLSCalcObservationMaker.h"

#include "TLSCalculation.h"

//#include <time.h>
#include	<nag.h>
#include	<nagg01.h>

//////////////////////
// default constructor
//////////////////////
TLSCalculation::TLSCalculation()
: fResultsMatrices(0), fNumberOfMadeIterations(0), fMaxIterations(10), fGeodSys(false)
{
}
/////////////////////////////////////////////
// constructor initializing the calc data set
/////////////////////////////////////////////
/*TLSCalculation::TLSCalculation(const TLGCDataSet& lgcds):
fCalcDataSet(lgcds), fInputMtrFiller(&fCalcDataSet), fComputer(0),
fResultsMtrExtractor(&fCalcDataSet), fNumberOfMadeIterations(0){

	fConvCriteria = 0.00001;
	fMaxIterations = 10;
	fS0APrioriScaleFactor = 1;
	fS0APosterioriVariances = false;
	fResultsMatrices = 0;

}*/

/////////////
// destructor
/////////////
TLSCalculation::~TLSCalculation(){

	//delete fComputer;
	//delete fResultsMatrices;
}

////////////////////////////////////////////////////////////////////
// Constructor taking the calculation parameters for initialisation 
////////////////////////////////////////////////////////////////////
TLSCalculation::TLSCalculation(const TLGCCalcParams& calcParams) : fCalcParams(calcParams),
fResultsMatrices(0), fNumberOfMadeIterations(0), fMaxIterations(10), fGeodSys(false) {
}

/*
//////////////////////////////////////
// sets the LGC calculation parameters
//////////////////////////////////////
void TLSCalculation::setCalcParams(const TLGCCalcParams& params){

	fConvCriteria = params.getConvCriteria();
	fS0APosterioriVariances = !(params.usingSigmaAPriori());
	fNumOfSimToDo = params.getNumOfSim();
}


////////////////////////////////
// sets the convergence criteria
////////////////////////////////
void TLSCalculation::setConvCriteria(double cc){

	fConvCriteria = cc;
}


//////////////////////////////////////////////////////////////////////////
// sets the variances to be calculated with respect to the s0 a posteriori
//////////////////////////////////////////////////////////////////////////
void TLSCalculation::s0APosterioriVariances(){

	fS0APosterioriVariances = true;
}
*/


////////////////////////////////////////
// sets the maximum number of iterations
////////////////////////////////////////
/*void TLSCalculation::setMaxIterations( int maxIter ){

	fMaxIterations = maxIter;
}


////////////////////////////////////////////////
// sets the scale factor for the sigma0 a priori
////////////////////////////////////////////////
void TLSCalculation::setS0APrioriScaleFactor(double scalefac){

	const_cast< TLSInputMatrices *>(fInputMtrFiller.getInputMatrices())->setS0APrioriScaleFactor(scalefac);
	fS0APrioriScaleFactor = scalefac;
}



////////////////////////////////////////////
// sets the calculation type to local system
////////////////////////////////////////////
void TLSCalculation::setLSLocalSysCalcType(){

	fInputMtrFiller.setLSLocalSysCalcType();
}

///////////////////////////////////////////
// sets the observed values to be simulated
///////////////////////////////////////////
void TLSCalculation::simulation(){

	fInputMtrFiller.simulation();
}

/////////////////////////////////////////
// returns a pointer to the Calc Data Set
/////////////////////////////////////////
const TLSCalcDataSet* TLSCalculation::getCalcDataSet() const{

	return &fCalcDataSet;
}
*/
////////////////////////
// computes the solution
////////////////////////
bool TLSCalculation::computeSolution(const ObservationSet& obsSet, LSCalcDataSet& calcDS){

	
	TLSCalcObservationMaker* LSCalcObs = new TLSCalcObservationMaker();
	bool lsObsMade = LSCalcObs->processData(obsSet, calcDS);
	bool lastIteration = false;

	if (lsObsMade)
	{
		calcDS.updateIndices();
		fNumberOfMadeIterations = 0;
		TLSInputMatricesFiller* inpMtrFiller = new TLSInputMatricesFiller(fCalcParams.getRefSurface(), fCalcParams.isPdorBearDefined() );
		TLSInputMatrices* inputMtr = new TLSInputMatrices();
		TLSParametricMtdComputer* computer = new TLSParametricMtdComputer();
		fResultsMatrices = new TLSResultsMatrices(calcDS.getDimensions());
		TLSResultsMatricesExtractor* extractor = new TLSResultsMatricesExtractor(&calcDS);


		while((!lastIteration) && (fNumberOfMadeIterations <= fMaxIterations) && (fError == "")){
			bool mtrFilled = inpMtrFiller->fillMatrices(calcDS, *inputMtr);
			if (mtrFilled)
			{
				if (!fCalcParams.usingSigmaAPriori())
				{computer->s0APosterioriVariances();}
				else {computer->s0APrioriVariances();}
				// compute solution 
				computer->computeResultsMtrs(inputMtr, fResultsMatrices);
				// save limits of interval for test chi of S0 validity
				calcDS.setChiUpLimit(computer->getS0PostUpLimit());
				calcDS.setChiLoLimit(computer->getS0PostLoLimit());

				if (computer->getError() == "")
				{	// extract solution and residues from the matrices
					extractor->setReferenceSurface(fCalcParams.getRefSurface());
					bool DSextracted(false);
					DSextracted = extractor->extractResults(*fResultsMatrices, fCalcParams);
					if (DSextracted) {
						lastIteration = extractor->lastIteration();
					}
					else
					{
						fError += extractor->getError();
					}
				}
				else
				{
					fError += computer->getError();
				}				
			}
			else
			{
				fError += inpMtrFiller->getError();
			}
			fNumberOfMadeIterations++;
		}

		if (fError == "")
		{
			// Normal equation matrix inversion
			fResultsMatrices->getUnkCovarMtrx()->invert();

			if (!fResultsMatrices->getUnkCovarMtrx()->isNull()) 
			{
				// extract of parameters variances and covariances
				extractor->extractVarCovarParams(*fResultsMatrices);
				fError = extractor->getError();
				if (fError == "")
				{	
					if (fCalcParams.isFaultDetectToBeSaved())
					{	// computation and extraction of reliability parameters
						TColumnVector* obsVar = new TColumnVector(inputMtr->getFirstDgnMtrx().numRows());
						(*obsVar) = fResultsMatrices->computeVarObs(inputMtr->getFirstDgnMtrx());
						extractor->computeAndExtractReliability(obsVar,fCalcParams);
						fError += extractor->getError();
						delete obsVar;
					}
					if (fError == "")
					{
						// Relative Error computation 
						extractor->extractRelError(*fResultsMatrices,fCalcParams);				
					}
				}
			}
			else
			{
				fError = fResultsMatrices->getUnkCovarMtrx()->getError();
			}
		}
		delete extractor;
		delete computer;
		delete inputMtr;
		delete inpMtrFiller;
	}
	else
	{
		fError = fError + LSCalcObs->getError();
	}

	delete LSCalcObs;

	return lastIteration;
}




//////////////////////////////////////////////////////////////////
// computes the solution with simulated values for the observations
//////////////////////////////////////////////////////////////////
bool TLSCalculation::computeSimulatedSolution(const ObservationSet& obsSet, LSCalcDataSet& calcDS)
{

	TLSCalcObservationMaker* LSCalcObs = new TLSCalcObservationMaker();
	bool lsObsMade = LSCalcObs->processData(obsSet, calcDS);
	bool sim = false;

	TLSInputMatricesFiller* inpMtrFiller = new TLSInputMatricesFiller(fCalcParams.getRefSurface(), fCalcParams.isPdorBearDefined());
	int numOfMadeSimulations = 0;
	bool lastIteration = false;
			
	//while (numOfMadeSimulations < params.getNumOfSim())
	//{
		if (lsObsMade)
		{// generate the simulatd values
			sim = inpMtrFiller->simulateValues(calcDS);
		}

		if (sim)
		{
			calcDS.updateIndices();

			TLSInputMatrices* inputMtr = new TLSInputMatrices();
			TLSParametricMtdComputer* computer = new TLSParametricMtdComputer();
			fResultsMatrices = new TLSResultsMatrices(calcDS.getDimensions());
			TLSResultsMatricesExtractor* extractor = new TLSResultsMatricesExtractor(&calcDS);

			lastIteration = false;
			fNumberOfMadeIterations = 0;
			// iterate to a solution
			while((!lastIteration) && (fNumberOfMadeIterations <= fMaxIterations) && (fError == ""))
			{
				bool mtrFilled = inpMtrFiller->fillMatrices(calcDS, *inputMtr);

				if (mtrFilled)
				{
					if (!fCalcParams.usingSigmaAPriori())
					{computer->s0APosterioriVariances();}
					else {computer->s0APrioriVariances();}
					// compute solution
					computer->computeResultsMtrs(inputMtr, fResultsMatrices);
					// save limits of interval for test chi of S0 validity
					calcDS.setChiUpLimit(computer->getS0PostUpLimit());
					calcDS.setChiLoLimit(computer->getS0PostLoLimit());

					if (computer->getError() == "")
					{	// extract solution and residues from the matrices
						extractor->simulation();
						bool DSextracted(false);
						DSextracted = extractor->extractResults(*fResultsMatrices, fCalcParams);
						if (DSextracted) {
							lastIteration = extractor->lastIteration();
						}
						else
						{
							fError += extractor->getError();
						}
					}
					else
					{
						fError += computer->getError();
					}				
				}
				else
				{
					fError += inpMtrFiller->getError();
				}
			
				fNumberOfMadeIterations++;
			}
			if (fError == "")
			{
				// Normal equation matrix inversion
				fResultsMatrices->getUnkCovarMtrx()->invert();

				if (!fResultsMatrices->getUnkCovarMtrx()->isNull())
				{
					// extract of parameters variances and covariances
					extractor->extractVarCovarParams(*fResultsMatrices);
					fError = extractor->getError();
					if (fError == "")
					{
						if (fCalcParams.isFaultDetectToBeSaved())
						{	// computation and extraction of reliability parameters
							TColumnVector* obsVar = new TColumnVector(inputMtr->getFirstDgnMtrx().numRows());
							(*obsVar) = fResultsMatrices->computeVarObs(inputMtr->getFirstDgnMtrx());
							extractor->computeAndExtractReliability(obsVar,fCalcParams);
							fError += extractor->getError();
							delete obsVar;
						}
						if (fError == "") 
						{
							// Relative Error computation 
							extractor->extractRelError(*fResultsMatrices,fCalcParams);
						}
					}
				}
				else
				{
					fError = fResultsMatrices->getUnkCovarMtrx()->getError();
				}
			}
			delete extractor;
			delete computer;
			delete inputMtr;
		}
		else
		{
			fError = fError + LSCalcObs->getError();
		}

	//	numOfMadeSimulations++;
	//}
	
	delete inpMtrFiller;
	delete LSCalcObs;
	
	return lastIteration;
}


//////////////////////////////////////////
// computes the solution for a LIBR option
//////////////////////////////////////////
bool TLSCalculation::computeFreeSolution(const ObservationSet& obsSet, LSCalcDataSet& calcDS)
{
	TLSCalcObservationMaker* LSCalcObs = new TLSCalcObservationMaker();
	bool lsObsMade = LSCalcObs->processData(obsSet, calcDS);
	bool lastIteration = false;
	bool s0APriori = false;
	if (lsObsMade)
	{
		calcDS.updateIndices();
		fNumberOfMadeIterations = 0;

		// presently, only local system are used in computation
		TLSConstraintIdentifier freeConstraint;
		freeConstraint.initCnstrIdentifier(calcDS.getLSWorkingPosVector(), obsSet);
		calcDS.setFreeConstraints(freeConstraint);

		TLSInputMatricesFiller* inpMtrFiller = new TLSInputMatricesFiller(fCalcParams.getRefSurface(), fCalcParams.isPdorBearDefined());
		TLSInputMatrices* inputMtr = new TLSInputMatrices();
		TLSParametricMtdComputer* computer = new TLSParametricMtdComputer();
		TLSResultsMatricesExtractor* extractor = new TLSResultsMatricesExtractor(&calcDS);

		// iterate to a solution
		while((!lastIteration) && (fNumberOfMadeIterations <= fMaxIterations) && (fError == ""))
		{
			bool mtrFilled = inpMtrFiller->fillMatrices(calcDS, *inputMtr);

			if (mtrFilled)
			{
				TColumnVector* solut = new TColumnVector(calcDS.getDimensions().UIndex);
				TColumnVector* resid = new TColumnVector(calcDS.getDimensions().OIndex);
				double sigm2(0.0);
				int nb = calcDS.getDimensions().UIndex + inputMtr->getCnstrFirstDgnMtrx().numRows();
				TMatrix* unkcov = new TMatrix(nb,nb); 
				fResultsMatrices = new TLSResultsMatrices(solut,resid,sigm2,unkcov);

				if (!fCalcParams.usingSigmaAPriori())
				{computer->s0APosterioriVariances();}
				else {computer->s0APrioriVariances();s0APriori=true;}
				// compute solution
				computer->computeFreeResultsMtrs(inputMtr, fResultsMatrices);

				if (computer->getError() == "")
				{	// extract solution and residues from the matrices
					bool DSextracted(false);
					DSextracted = extractor->extractResults(*fResultsMatrices, fCalcParams);
					if (DSextracted) {
						lastIteration = extractor->lastIteration();
					}
					else
					{
						fError += extractor->getError();
					}
				}
				else
				{
					fError += computer->getError();
				}				
			}
			else
			{
				fError += inpMtrFiller->getError();
			}
			
			fNumberOfMadeIterations++;
		}

		if(fError == "")
		{

			int nbUnk = inputMtr->getNbrUnknowns();
			int nbObs = inputMtr->getNbrObservations();
	//		int nbCnstr = im->getNbrConstraints();
			int nbCnstrObs = inputMtr->getNbrConstraintObs();



			// computes and saves limits of interval for test chi of S0 validity
			double d =0;
/*			if (TLGCDataSet::usedOrieCnstr())
			{
				d = (inputMtr->getWeightMtrx().numRows()-1)-(fResultsMatrices->getSolutionVctr()->dimension());
			}
			else
			{
				d = (inputMtr->getWeightMtrx().numRows())-(fResultsMatrices->getSolutionVctr()->dimension());
			}*/
			d = nbObs - (nbUnk + nbCnstrObs);

			if(d>0)
			{
				static NagError fail1, fail2;
				fail1.print = false;
				fail2.print = false;
				//test chi
				double chiUp = nag_deviates_chi_sq(0.975, d, &fail1);
				if (fail1.code != NE_NOERROR)
					fError += fail1.message;
				double chiLow = nag_deviates_chi_sq((1-0.975), d, &fail2);
				if (fail2.code != NE_NOERROR)
					fError += fail2.message;

				if (fError == "") {
					//Limits
					computer->setS0PostUpLimit(sqrt(chiUp/d));
					computer->setS0PostLoLimit(sqrt(chiLow/d));

					if ((sqrt(fResultsMatrices->getSigmaZero2()) < computer->getS0PostUpLimit()) && (sqrt(sqrt(fResultsMatrices->getSigmaZero2()) > computer->getS0PostLoLimit())))
					{	
						computer->s0APrioriVariances();
						s0APriori=true;
					}
				}
				calcDS.setChiUpLimit(computer->getS0PostUpLimit());
				calcDS.setChiLoLimit(computer->getS0PostLoLimit());
			}
			else
			{
				fError += "Not enough observations";
			}

			// inversion, and test of s0 a posteriori use.
			*fResultsMatrices->getUnkCovarMtrx() = fResultsMatrices->getUnkCovarMtrx()->inverseSym();
			if (s0APriori)
				*fResultsMatrices->getUnkCovarMtrx() = *fResultsMatrices->getUnkCovarMtrx() * pow(inputMtr->getS0APrioriScaleFactor(),2);
			else
				*fResultsMatrices->getUnkCovarMtrx() = *fResultsMatrices->getUnkCovarMtrx() * pow(inputMtr->getS0APrioriScaleFactor(),2) * fResultsMatrices->getSigmaZero2();

			if (fError == "")
			{
				// extract of parameters variances and covariances
				extractor->extractVarCovarParams(*fResultsMatrices);
				fError = extractor->getError();


				if (fError == "")
				{
					if (fCalcParams.isFaultDetectToBeSaved())
					{ // computation and extraction of reliability parameters
						TColumnVector* obsVar = new TColumnVector(inputMtr->getFirstDgnMtrx().numRows());
						(*obsVar) = fResultsMatrices->computeVarObs(inputMtr->getFirstDgnMtrx());
						extractor->computeAndExtractReliability(obsVar,fCalcParams);
						fError += extractor->getError();
						delete obsVar;
					}
					if (fError == "")
					{
						// Relative Error computation 
						extractor->extractRelError(*fResultsMatrices,fCalcParams);
					}
				}
			}
		}

		delete extractor;
		delete computer;
		delete inputMtr;
		delete inpMtrFiller;
	}
	else
	{
		fError = fError + LSCalcObs->getError();
	}

	delete LSCalcObs;

	return lastIteration;
}


///////////////////
// return the error
///////////////////
string TLSCalculation::getError() const
{
	return fError;
}

///////////////////////////////
// selects the adapted computer
///////////////////////////////
/*void TLSCalculation::selectAdaptedComputer(){

	fComputer = new TLSParametricMtdComputer();
	fComputer->setS0APrioriScaleFactor(fS0APrioriScaleFactor);
	
	if (fS0APosterioriVariances)
		fComputer->s0APosterioriVariances();
}

/////////////////////////////////////////////////////
// makes everything ready for an additional iteration
/////////////////////////////////////////////////////
void TLSCalculation::prepareNextIteration(){

	//The input matrices will be set to zero when fillMatrices is called

	fCalcDataSet.prepareNextIteration();
}

//////////////////////////////////////////////////////
// makes everything ready for an additional simulation
//////////////////////////////////////////////////////
void TLSCalculation::prepareNextSimulation(){

	fCalcDataSet.prepareNextSimulation();
}*/
