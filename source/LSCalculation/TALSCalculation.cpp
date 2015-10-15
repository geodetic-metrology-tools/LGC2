#include "TALSCalculation.h"
#include "TSparseMatrix.h"
#include "QuantileFunctions.h"

#include <memory>

TALSCalculation::TALSCalculation():fResultsMatrices(0), fNumberOfMadeIterations(0), fMaxIterations(10), fGeodSys(false), fError("")
{
	//default constructor
	fIsSimulation = false;
	count = 1;
}

TALSCalculation::TALSCalculation(const TLGCCalcParams& calcParams):fCalcParams(calcParams), fResultsMatrices(0), fNumberOfMadeIterations(0), fMaxIterations(10), fGeodSys(false)
{
	//constructor taking the calculation parameters for initialisation
	fIsSimulation = false;

}

TALSCalculation::~TALSCalculation()
{
	//destructor
	if(fResultsMatrices != 0)
	{
		delete fResultsMatrices;
	}
}


/////////////////////////////////////////////////////
// MEMBER FONCTION
/////////////////////////////////////////////////////

//Compute the solution for a survey network from data in LSCalcDataSet and store the results
bool	TALSCalculation::processCalculation( TLGCData& calcDS )
{
	bool lastIteration = false;

	TLSInputMatricesFiller inpMtrFiller(fCalcParams.getRefSurface(), fCalcParams.isPdorBearDefined() );

	fResultsMatrices = new TLSResultsMatrices(calcDS.getDimensions(), /*calcDS.getFreeConstraints().getNumberOfConstraint()*/);
	TLSInputMatrices inputMtr;
	//TLSParametricMtdComputer computer;
	TLSResultsMatricesExtractor extractor(&calcDS);

	//this = computer
	lastIteration = iterate2Solution(calcDS, &inpMtrFiller, &inputMtr, &this, &extractor);
	calcResiduAndVarCovMatrice(calcDS, &inputMtr, &extractor /*, &this*/);
	extractor.extractResiduals(*fResultsMatrices, fCalcParams);

	if(calcDS.getDimensions().UIndex != 0)
	{//il y a des inconnues
		invertMatriceCoVar(calcDS);
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

	return lastIteration;
}

bool	TALSCalculation::iterate2Solution(TLGCData& calcDS,
										 TLSInputMatricesFiller* inpMtrFiller,
										 TLSInputMatrices* inputMtr,
										 TALSCalculation* computer,
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
			bool computationOK = computer->computeResultsMtrs(inputMtr, fResultsMatrices);
	
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



void TALSCalculation::invertMatriceCoVar(TLGCData& calcDS)
{
	if (fError == "")
	{
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
	}
	return;
}

void TALSCalculation::extractVarCovarParams(TLSResultsMatricesExtractor* extractor)
{
	//extract of parameters variances and covariances
	if (fError == "")
	{
		extractor->extractVarCovarParams(*fResultsMatrices);
		fError += extractor->getError();
	}
}


void TALSCalculation::calcFautParams( TLSInputMatrices* inputMtr, TLSResultsMatricesExtractor* extractor)
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

void TALSCalculation::calcRelErrorParams( TLSResultsMatricesExtractor* extractor)
{
	if (fError == "")
	{
		// Relative Error computation 
		extractor->extractRelError(*fResultsMatrices,fCalcParams);	
		fError += extractor->getError();
	}
	return;
}

limits	TALSCalculation::calcSigmaZeroLimits(const int nbObs, const int nbUnk, const TReal sigmaZero2)
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
