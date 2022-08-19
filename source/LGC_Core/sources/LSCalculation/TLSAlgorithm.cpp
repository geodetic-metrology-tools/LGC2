#include <TLSAlgorithm.h>
#include "TLSInputMatricesFiller.h"
#include "TLSConsistencyCheck.h"
#include "TLSUniversalMtdComputer.h"
#include <Logger.hpp>
#include <chrono>

TLSAlgorithm::TLSAlgorithm(TLGCData& data)
	: fNumberOfIterations(0)
	, fS0APosterioriVariances(false)
	,fPointTransformer(&data.getTree(), data.getConfig().referential)
	,fLibrCnstrGenerator(fPointTransformer, data)
{
	delete resultMatrices;
	// identify the constraints necessary, create them
	if (data.getConfig().libre.isActive())
	{
		fLibrCnstrGenerator.initCnstrIdentifier(data);
		data.fUEOIndices.CIndex = fLibrCnstrGenerator.getNumberOfConstraint();
	}
	resultMatrices = new TLSResultsMatrices(data.fUEOIndices);
}


Behavior TLSAlgorithm::run(TLGCData& data, int fMaxIterations)
{
	std::unique_ptr<TALSComputer> computer;
	std::unique_ptr<TLSInputMatricesFiller> matrFiller(new TLSInputMatricesFiller(&data.getTree(), data.getConfig().referential));
	std::unique_ptr<TLSInputMatrices> inputMtr(new TLSInputMatrices());

	fExtractor = std::make_shared<TLSResultsMatricesExtractor>(&data);
	


	// use the universal LS algorithm for parametric, combined and constrained case.
	computer.reset(new TLSUniversalMtdComputer());

	
	// FRAS Mockup: loop over 1. manipulate measurements to simulate updated measurements 2. solve with iterate2solution
	// for random numbers
	engine.seed(1);
	Behavior computationIsOK = iterate2Solution(data, matrFiller.get(), inputMtr.get(), computer.get(), fMaxIterations, data.getConfig().outPrecision.convCrit);
	for (int i = 0; i < 1000; i++) {
		std::cout << "Starting Fras Mockup iteration " << i << std::endl;
		// manipulate ECWS measurements in data structure
		manipulate_ECWS_measurements(&data);
		// update matrix filler
		// std::unique_ptr<TLSInputMatricesFiller> matrFiller(new TLSInputMatricesFiller(&data.getTree(), data.getConfig().referential));

		// iterate to new solution from there
		auto start = std::chrono::high_resolution_clock::now();
		computationIsOK = iterate2Solution(data, matrFiller.get(), inputMtr.get(), computer.get(), fMaxIterations, data.getConfig().outPrecision.convCrit);
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		std::cout << "Terminated in " << duration.count() << " ms." << std::endl;

	}

	return computationIsOK;
}

void TLSAlgorithm::manipulate_ECWS_measurements(TLGCData* data) {
	// FRAS-Mockup:
	// apply random perturbation to each ECWS measurement, as it is done in a simulation
	
	// iterate over frame tree
	for (TDataTreeIterator itTree = data->getTree().begin(); itTree != data->getTree().end(); itTree++) {
		// iterate over water level measurement rounds in that frame
		for (auto& itECWSrom : itTree.node->data->measurements.fECWS) {
			// iterate over single measurements
			for (auto itECWS(itECWSrom.measECWS.begin()); itECWS != itECWSrom.measECWS.end(); ++itECWS) {
				TLength oldMeas = itECWS->getDistance();
				// itECWS->setDistance(2 * aux);
				TReal sigma = itECWS->target.sigmaDist;
				TLength newMeas = TLength(std::normal_distribution<double>(0, sigma)(engine)) + oldMeas;
				itECWS->setDistance(newMeas);
			}
		}
	}
}

Behavior	TLSAlgorithm::iterate2Solution(TLGCData& data,
											TLSInputMatricesFiller* matrFiller,
											TLSInputMatrices* inputMtr,
											TALSComputer* computer,
											int fMaxIterations,
											TReal convCrit)
{
	

	bool hasReachedCriteria = false;
	fNumberOfIterations = 0;

	TFileLogger& fileLog = data.getFileLogger();

	// Iterate to find solution
	while (!hasReachedCriteria && fNumberOfIterations < fMaxIterations)
	{
		bool fillOK = false;
		if (fNumberOfIterations == 0)//First iteration, fill also the weight unknown matrix.
		{
			fillOK = matrFiller->fillMatrices(&data, true, inputMtr);
		}
		else//In the following iteration the weight matrix remains unchanged, no need to be filled with the same values again.
			fillOK = matrFiller->fillMatrices(&data, false, inputMtr);

		//fill part of the free constraints
		if (data.getConfig().libre.isActive())
			fLibrCnstrGenerator.processFreeCnstr(*inputMtr);

		if (data.getConfig().consCheck.isActive() && fNumberOfIterations==0){
			// Check for inconsistencies leading to ambiguous least square problems
			// libr constraint matrices are necessary if there is a
			// constraint
			TLSConsCheck consCheck(data, *inputMtr);
			if (!consCheck.getResultStatus()) {
				return Behavior(Behavior::BehaviorCode::ERR_consistencyCheck, L"Problem with measurement configuration.\n");	
			}
		}

		if (fillOK)
		{		

			// compute solution 
			bool computationOK = computer->computeResults(inputMtr, resultMatrices);
	

			if (computationOK)
			{
				logDebug() << "Iteration" << fNumberOfIterations << "step: Calculation successfully done\n=============================";

				bool extractOK = false; 
				if (data.getConfig().libre.isActive())
					extractOK = fExtractor->extractResults(*resultMatrices, convCrit, &fLibrCnstrGenerator);
				else
					extractOK = fExtractor->extractResults(*resultMatrices, convCrit);

				if (extractOK)
					hasReachedCriteria = fExtractor->lastIteration();
				else{
					fileLog << "Problem in LS matrices extraction.\n";
					return Behavior(Behavior::BehaviorCode::ERR_results, L"Problem in LS matrices extraction.\n"); //Error during extraction, errors written out already, STOP the calculation.	
				}
			}
			else
			{
				//Write errors which occured in computer of LS methos
				logCritical() << "Problem with LS computation: ended at the iteration step" << fNumberOfIterations;
				fileLog << "Problem with LS computation: " << computer->getError()<< " \n";
				return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"Problem with LS computation. Matrix not inverted\n");
			}
		}
		else
		{
			fileLog << "Matrices filling was not successful.\n";
			return Behavior(Behavior::BehaviorCode::ERR_inputData, L"Matrices filling was not successful.\n");
		}
		fNumberOfIterations++;
	}

	// Checks if maximal number of iteration steps has been reached without satisfying the converging criteria
	if (fNumberOfIterations == fMaxIterations && !hasReachedCriteria)
	{
		logCritical() << "Maximal number of iteration steps ("<<fMaxIterations<<") has been reached without satisfying the converging criteria!";
		fileLog << "The calculation is not converging \n";
		return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"The calculation is not converging \n"); //Error during the calculation, errors written out already, STOP the calculation.	
	}
	else
	{
		std::cout << "Adjustment terminated after " << fNumberOfIterations << " iterations."<<std::endl;
		if (computeVarCovarAndReliability(&data, inputMtr, computer))
			return Behavior();
		else{
			fileLog << TFileLogger::e_logType::LOG_ERROR << "TResidual errors and their related variance-covariance matrix could not be estimated (Matrix inversion Problem)!";
			return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"TResidual errors and their related variance-covariance matrix could not be estimated!\n");
		}
	}
}

bool TLSAlgorithm::computeVarCovarAndReliability(TLGCData* data, TLSInputMatrices* inputMtr, TALSComputer* computer)
{
	if (!computer->calcResidusAndVarCovMatrix(inputMtr, resultMatrices))
	{
		logWarning() << "Residual errors and their related variance-covariance matrix could not be estimated!";
		return false;
	}
		
	fExtractor->extractResiduals(*resultMatrices);

	TReal S0 = sqrt(resultMatrices->getSigmaZero2());
	// Apply S02 only if APRI is not used and S02 is outside the limits
	if (!data->getConfig().useApriori.isActive() && (S0>resultMatrices->getSigmaZeroUpLimit() || S0<resultMatrices->getSigmaZeroLowLimit()))
	{
		TSparseMatrix* UnkCovar = resultMatrices->getUnkCovarMtrx();
		(*UnkCovar) *= resultMatrices->getSigmaZero2();
		resultMatrices->setUnkCovarMtrx(*UnkCovar);
	}

	fExtractor->extractVarCovarParams(*resultMatrices);
	data->setNumberOfLSIterations(fNumberOfIterations);

	/* Store calculated SIGMA A POSTERIORI in TLGCData and limits for it */
	data->setS0APosteriori(S0);
	data->setChiS0Limits(resultMatrices->getSigmaZeroLowLimit(), resultMatrices->getSigmaZeroUpLimit());


	if ((data->fUEOIndices.UIndex != 0) && data->getConfig().faut.isActive())
	{
		logDebug() << "Computes statistics (Z, W, T, G, NABLA and DELTY)";

		//re-initialize the statictic vectors
		data->getStatistics().initialiseStatVector(inputMtr);
		TReal alpha = data->getConfig().faut.alpha / 100;
		TReal beta = data->getConfig().faut.beta / 100;

		//compute statistics (Z, W, T, G, NABLA and DELTY)
		data->getStatistics().calcReliabilityVector(alpha, beta, inputMtr, resultMatrices, data->getConfig().pdor.isActive());
		
	}

	if ((data->fUEOIndices.UIndex != 0) && !data->getConfig().erelPairs.empty())
	{
		logDebug() << "Statistics have been completed: extracts now the data";
		fExtractor->extractRelError(*resultMatrices);
	}

	return true;
	
}
