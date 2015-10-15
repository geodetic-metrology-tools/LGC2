#include "TLGCCalculation.h"
#include "TDataAnalyzer.h"
#include "LSCalculation/TLSInputMatricesFiller.h"
#include "LSCalculation/TLSResultsMatricesExtractor.h"
#include "lsalgo/TLSInputMatrices.h"

#include "lsalgo/TLSParametricMtdComputer.h"
#include "lsalgo/TLSCombinedMtdComputer.h"
#include "lsalgo/TLSWeightedUnkMtdComputer.h"



//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
//IF MAX NUMBER OF ITERATION SET TO MORE THEN  IT FAILS IN EXTRACTINT VARIANCE COVARIANCE!!!!!!!!!!!!!!!!!!!
TLGCCalculation::TLGCCalculation(TLGCData* dat): fData(*dat), fResultsMatrices(nullptr), fNumberOfMadeIterations(0), fMaxIterations(40), fIsSimulation(false),fConvCriteria(0.000000005)
{ }

///////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////
bool TLGCCalculation::computeLSResults(){
	bool successCalculation = false;

	// Check whether the data are consistent, assign unknown indices and initialize uninitialized planes in DLEV measurements
	TDataAnalyzer analyzer(fData);
	if(!analyzer.dataConsistent())
		throw std::runtime_error("Data are not consistent, see the output file: " + fData.fOutputFileWriter.getOutputFileLocation() + " for more information.");

	//Identify and set calculation type based on configuration options and measurement types
	fCalculationType = analyzer.getCalculationType();

	TLSInputMatricesFiller matrFiller(&fData.tree, fCalculationType);
	fResultsMatrices.reset(new TLSResultsMatrices(fData.fUEOIndices));

	TLSInputMatrices inputMtr;
	TLSResultsMatricesExtractor extractor(&fData);

	std::unique_ptr<TALSComputer> fComputer;

	if(fCalculationType.fCombinedCase)
		fComputer.reset(new TLSCombinedMtdComputer());
	else if(fCalculationType.fStandDevUsed)
		fComputer.reset(new TLSWeightedUnkMtdComputer());
	else
		fComputer.reset(new TLSParametricMtdComputer());

	try{
		successCalculation = iterate2Solution(fData, &matrFiller, &inputMtr, std::move(fComputer), &extractor);
#if 0
		DONE IN THE END OF iterate2Solution() METHOD
		calcResiduAndVarCovMatrice(calcDS, &inputMtr, &extractor, &computer); THIS FUNCTION WAS MOVED INTO LS COMPUTERS ( see TALSCOMPUTER)
		extractor.extractResiduals(*fResultsMatrices, fCalcParams);
#endif
	}
	catch(exception& e){
		fData.fOutputFileWriter << TFileLogger::e_logType::LOG_ERROR << e.what();
		successCalculation = false;
	}

	if(fData.fOutputFileWriter.hasErrors())
		successCalculation = false;

	return successCalculation;
}


bool	TLGCCalculation::iterate2Solution(TLGCData& fData,
						 TLSInputMatricesFiller* inpMtrFiller,
						 TLSInputMatrices* inputMtr,
						 std::unique_ptr<TALSComputer> computer,
						 TLSResultsMatricesExtractor* extractor){
	fNumberOfMadeIterations = 0;

	bool lastIteration = false;

	// Iterate to find solution
	while((!lastIteration) && (fNumberOfMadeIterations < fMaxIterations) && (!fData.fOutputFileWriter.hasErrors()))
	{

		bool mtrFilled = inpMtrFiller->fillMatrices(&fData, inputMtr);
		if (fNumberOfMadeIterations==0)
			inputMtr->saveMatricesToFile(fNumberOfMadeIterations);


		if (mtrFilled)
		{
			if (!fData.cfg.useApriori.isActive())
			{
				fS0APosterioriVariances = true;
			}
			else 
			{
				fS0APosterioriVariances = false;
			}

			// compute solution 
			bool computationOK = computer->computeResults(inputMtr, fResultsMatrices.get());
	
			if (computationOK)
			{	
				//extractor->setReferenceSurface(fCalcParams.getRefSurface());   WHY DID THEY SET UP A REFERENCE SURFACE AFTER EACH RUN????????????
				bool DSextracted = false;
				DSextracted = extractor->extractResults(*fResultsMatrices, fConvCriteria);
				if (DSextracted)
					lastIteration = extractor->lastIteration();
				else
					return false; //Error during extraction, errors written out already, STOP calculation	
			}
			else
			{
				//Write errors which occured in computer of LS methos
				 fData.fOutputFileWriter.writeReportHeader("Problem with LS results\n");
				 fData.fOutputFileWriter << computer->getError();
				 return false;
			}

			//fResultsMatrices.get()->saveMatricesToFile(fNumberOfMadeIterations);
		}
		else
		{
			throw std::runtime_error("Matrices filling was not successful!");
			//WRITE SOME OVERALL ERROR EXPLANATION
		}
		fNumberOfMadeIterations++;
	}

	inputMtr->saveMatricesToFile(fNumberOfMadeIterations);

	computer->calcResiduAndVarCovMatrice(inputMtr, fResultsMatrices.get());
	extractor->extractResiduals(*fResultsMatrices);
	fResultsMatrices.get()->saveMatricesToFile(fNumberOfMadeIterations);


	extractor->extractVarCovarParams(*fResultsMatrices);
	fData.setNumberOfLSIterations(fNumberOfMadeIterations);

	/* Store calculated SIGMA A POSTERIORI in TLGCData and limits for it */
	fData.setS0APosteriori(fResultsMatrices->getSigmaZero2());
	fData.setChiS0Limits(fResultsMatrices->getSigmaZeroLowLimit(), fResultsMatrices->getSigmaZeroUpLimit());


#if _DEBUG
	

#endif

	if(fData.fUEOIndices.UIndex != 0) // && fData.cfg.faut.isActive = true
	{
		//re-initialize the statictic vectors
		computer->clearVectors();
		computer->initialiseStatVector(inputMtr);
		TReal alpha = fData.cfg.faut.alpha/100;
		TReal beta = fData.cfg.faut.beta/100;
		//compute statistics (Z, W, T, G, NABLA and DELTY)
		computer->calcStatisticVector(alpha, beta,inputMtr, fResultsMatrices.get());

		//calcRelErrorParams( &extractor);
		//ellips erros
	}
	else
	{//pas de probleme de convergence dans un calcul sans inconnues
		//calcDS.setConvergenceResult(lastIteration);
	}

	return true;
}