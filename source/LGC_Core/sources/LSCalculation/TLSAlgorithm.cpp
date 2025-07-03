// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <Logger.hpp>
#include <TLSAlgorithm.h>
#include <TLSEvaluator.h>
#include <TLSTrustRegionLM.h>

#include <memory>
#include "TLSInputMatricesFiller.h"
#include "TLSUniversalMtdComputer.h"

TLSAlgorithm::TLSAlgorithm(TLGCData &data) : fNumberOfIterations(0), fS0APosterioriVariances(false), fPointTransformer(&data.getTree(), data.getConfig().referential)
{
	resultMatrices = std::make_shared<TLSResultsMatrices>(data.fUEOIndices);
}

Behavior TLSAlgorithm::run(TLGCData &data, int fMaxIterations)
{
	std::unique_ptr<TALSComputer> computer;
	std::unique_ptr<TLSInputMatricesFiller> matrFiller(new TLSInputMatricesFiller(&data.getTree(), data.getConfig().referential, data));
	std::unique_ptr<TLSInputMatrices> inputMtr(new TLSInputMatrices());

	fExtractor = std::make_shared<TLSResultsMatricesExtractor>(&data);

	// use the universal LS algorithm for parametric, combined and constrained case.
	computer.reset(new TLSUniversalMtdComputer());

	Behavior computationIsOK;
	bool useLM = data.getConfig().useRegularization.isActive();
	if (useLM)
	{
		std::shared_ptr<TLGCData> dataPtr(&data, [](TLGCData*){});
		tryRegularizedSolve(dataPtr);
		bool onlyStatistics = data.getConfig().useRegularizationOnly.isActive();
		if (onlyStatistics)
		{ // this is the aggressive variant which just takes the regularized solution and does the prostprocessing (covariance matrix etc.)
			logWarning() << "LGC is reporting the solution as reached by the Levenberg-Marquardt method.";
			bool compStatus = computeStatisticsAtCurrentState(&data, matrFiller.get(), inputMtr.get(), computer.get());
		}
		else
		{ // finish computation by doing normal full step Gauss-Newton + all subsequent necessary postprocessing
			computationIsOK = iterate2Solution(data, matrFiller.get(), inputMtr.get(), computer.get(), fMaxIterations, data.getConfig().outPrecision.convCrit);
		}
	}
	else
	{
		computationIsOK = iterate2Solution(data, matrFiller.get(), inputMtr.get(), computer.get(), fMaxIterations, data.getConfig().outPrecision.convCrit);
	}


	return computationIsOK;
}

Behavior TLSAlgorithm::iterate2Solution(TLGCData &data, TLSInputMatricesFiller *matrFiller, TLSInputMatrices *inputMtr, TALSComputer *computer, int fMaxIterations, TReal convCrit)
{
	bool hasReachedCriteria = false;
	fNumberOfIterations = 0;

	TFileLogger &fileLog = data.getFileLogger();

	// Iterate to find solution
	while (!hasReachedCriteria && fNumberOfIterations < fMaxIterations)
	{
		bool fillOK = false;
		fillOK = matrFiller->fillMatrices(&data, inputMtr);

		if (fillOK)
		{
			// compute solution
			bool computationOK = computer->computeResults(inputMtr, resultMatrices.get());

			if (computationOK)
			{
				logDebug() << "Iteration" << fNumberOfIterations << "step: Calculation successfully done\n=============================";

				bool extractOK = false;
				extractOK = fExtractor->extractResults(*resultMatrices, convCrit);

				if (extractOK)
					hasReachedCriteria = fExtractor->lastIteration();
				else
				{
					fileLog << "Problem in LS matrices extraction.\n";
					return Behavior(Behavior::BehaviorCode::ERR_results, L"Problem in LS matrices extraction.\n"); // Error during extraction, errors written out already, STOP the calculation.
				}
			}
			else
			{
				// Write errors which occured in computer of LS methos
				logCritical() << "Problem with LS computation: ended at the iteration step" << fNumberOfIterations;
				fileLog << "Problem with LS computation: " << computer->getError() << " \n";
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
		logCritical() << "Maximal number of iteration steps (" << fMaxIterations << ") has been reached without satisfying the converging criteria!";
		fileLog << "The calculation is not converging \n";
		return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"The calculation is not converging \n"); // Error during the calculation, errors written out already, STOP the calculation.
	}
	else
	{
		if (computeVarCovarAndReliability(&data, inputMtr, computer))
			return Behavior();
		else
		{
			fileLog << TFileLogger::e_logType::LOG_ERROR << "TResidual errors and their related variance-covariance matrix could not be estimated. ";
			return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"TResidual errors and their related variance-covariance matrix could not be estimated!\n");
		}
	}
}

Behavior TLSAlgorithm::computeStatisticsAtCurrentState(TLGCData *data, TLSInputMatricesFiller *matrFiller, TLSInputMatrices *inputMtr, TALSComputer *computer)
{
	bool hasReachedCriteria = false;

	TFileLogger &fileLog = data->getFileLogger();
	bool fillOK = false;
	fillOK = matrFiller->fillMatrices(data, inputMtr);

	if (fillOK)
	{
		// compute solution
		bool computationOK = computer->computeResults(inputMtr, resultMatrices.get());
		logDebug() << "Computed normal matrix for statistics.";

		if (computationOK)
		{
			bool extractOK = false;
			TVector dummySol(data->fUEOIndices.UIndex);
			dummySol.setZero();
			resultMatrices->setSolutionVect(dummySol);
			// now the result extraction but we don't apply the step itself
			double convCritDummy = 1e+6;
			extractOK = fExtractor->extractResults(*resultMatrices, convCritDummy);

			if (extractOK)
				hasReachedCriteria = fExtractor->lastIteration();
			else
			{
				fileLog << "Problem in LS matrices extraction.\n";
				return Behavior(Behavior::BehaviorCode::ERR_results, L"Problem in LS matrices extraction.\n"); // Error during extraction, errors written out already, STOP the calculation.
			}
		}
		else
		{
			// Write errors which occured in computer of LS methos
			logCritical() << "Problem with LS computation: ended at the iteration step" << fNumberOfIterations;
			fileLog << "Problem with LS computation: " << computer->getError() << " \n";
			return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"Problem with LS computation. Matrix not inverted\n");
		}
	}
	else
	{
		fileLog << "Matrices filling was not successful.\n";
		return Behavior(Behavior::BehaviorCode::ERR_inputData, L"Matrices filling was not successful.\n");
	}

	if (computeVarCovarAndReliability(data, inputMtr, computer))
		return Behavior();
	else
	{
		fileLog << TFileLogger::e_logType::LOG_ERROR << "TResidual errors and their related variance-covariance matrix could not be estimated. ";
		return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"TResidual errors and their related variance-covariance matrix could not be estimated!\n");
	}

}

bool TLSAlgorithm::computeVarCovarAndReliability(TLGCData *data, TLSInputMatrices *inputMtr, TALSComputer *computer)
{
	if (!computer->calcResidusAndVarCovMatrix(inputMtr, resultMatrices.get()))
	{
		logWarning() << "Residual errors and their related variance-covariance matrix could not be estimated!";
		return false;
	}

	fExtractor->extractResiduals(*resultMatrices);
	// compute the distance sensitivities
	fExtractor->computeDistSensi();

	TReal S0 = sqrt(resultMatrices->getSigmaZero2());
	// Apply S02 only if APRI is not used and S02 is outside the limits
	if (!data->getConfig().useApriori.isActive() && (S0 > resultMatrices->getSigmaZeroUpLimit() || S0 < resultMatrices->getSigmaZeroLowLimit()))
	{
		TSparseMatrix *UnkCovar = resultMatrices->getUnkCovarMtrx();
		(*UnkCovar) *= resultMatrices->getSigmaZero2();
	}

	if (!fExtractor->extractVarCovarParams(*resultMatrices))
	{
		logWarning() << "Problem during Covariance extraction.";
		return false;
	};
	data->setNumberOfLSIterations(fNumberOfIterations);

	/* Store calculated SIGMA A POSTERIORI in TLGCData and limits for it */
	data->setS0APosteriori(S0);
	data->setChiS0Limits(resultMatrices->getSigmaZeroLowLimit(), resultMatrices->getSigmaZeroUpLimit());

	if ((data->fUEOIndices.UIndex != 0) && data->getConfig().faut.isActive())
	{
		logDebug() << "Computes statistics (Z, W, T, G, NABLA and DELTY)";

		// re-initialize the statictic vectors
		data->getStatistics().initialiseStatVector(inputMtr);
		TReal alpha = data->getConfig().faut.alpha;
		TReal beta = data->getConfig().faut.beta;

		// compute statistics (Z, W, T, G, NABLA and DELTY)
		data->getStatistics().calcReliabilityVector(alpha, beta, inputMtr, resultMatrices.get(), data->getConfig().pdor.isActive());
	}

	if (data->getConfig().fRelErrors.points.size() > 0 || data->getConfig().fRelErrors.frames.size() > 0)
	{
		if (!fExtractor->extractRelError())
		{
			logWarning() << "Problem during relative error computation";
			return false;
		};
		logDebug() << "Statistics have been completed: extracts now the data";
	}

	return true;
}

void TLSAlgorithm::tryRegularizedSolve(std::shared_ptr<TLGCData> dataPtr)
{

	std::shared_ptr<TLSEvaluator> evalPtr = std::make_shared<TLSEvaluator>(dataPtr);

	TLSTrustRegionLM::Config defaultConfig;
	defaultConfig.tolStep = dataPtr->getConfig().outPrecision.convCrit;
	TLSTrustRegionLM trObject(evalPtr, defaultConfig);

	Eigen::VectorXd provVal = evalPtr->getEstParams();

	// create a bunch of random starting values
	int numberSamples = 10;
	std::vector<Eigen::VectorXd> startValues;
	// include the provided provisional value in the sample list
	int totalSamples = 2 * numberSamples + 1;
	startValues.reserve(totalSamples);
	startValues.push_back(provVal);
	for (int j = 0; j < numberSamples; j++)
	{
		Eigen::VectorXd randVal = Eigen::VectorXd::Random(dataPtr->fUEOIndices.UIndex);
		// both try a perturbed version of the supplied provisional value as well as a totally random initial value.
		startValues.push_back(provVal + randVal);
		startValues.push_back(randVal);
	}

	// attempt to solve
	// prepare results
	std::vector<TLSTrustRegionLM::Result> results;
	results.reserve(totalSamples);
	// needed to clean the error logs after each solution attempt
	TFileLogger &fileLog = dataPtr->getFileLogger();
	int j = 0;
	for (const auto &sval : startValues)
	{
		j++;
		// set initial value and start Levenberg Marquardt GN from this value
		try
		{
			logInfo() << "Trying to solve starting from initial guess #" << j;
			TLSTrustRegionLM::Result result = trObject.solve(sval);

			results.push_back(result);
			if (result.sigma0Aposteriori < 5)
			{
				logWarning() << "Candidate with Sigma <5 found.";
				break;
			}
		}
		catch (const std::exception &e)
		{
				// Handles exceptions derived from std::exception.
				logWarning()
				<< "Problem occured during attempting solution of problem with randomly generated initial value: " << std::string(e.what());
		}
		catch (...)
		{
			logWarning() << "Problem occured during attempting solution of problem with randomly generated initial value";
		}
	}

	fileLog.resetErrorCounter();
	// find best solution candidate
	double bestSigma = 1e+12;
	Eigen::VectorXd bestSol;
	bool solFound = false;
	for (const auto &result : results)
	{
		if (result.sigma0Aposteriori < bestSigma && result.status == TLSTrustRegionLM::Status::SuccessStep)
		{
			bestSigma = result.sigma0Aposteriori;
			bestSol = result.sol;
			solFound = true;
		}
	}
	if (solFound)
	{
		logWarning() << "Random initial value sampling found a solution with sigma0 a posteriori = " << bestSigma;
		logWarning() << "LGC will continue with this solution";
		// apply a random perturbation such that the LGC least square method still makes a proper iteration
		evalPtr->setParameters(bestSol);
	}
	else
	{
		logWarning() << "Random initial value sampling was unable to find a solution.";
		evalPtr->setParameters(provVal);
	}
	// continue LGC normally, if solution was already found only one iteration will be made
	// clean errors that potentially happened during input filling
	fileLog.resetErrorCounter();
}

