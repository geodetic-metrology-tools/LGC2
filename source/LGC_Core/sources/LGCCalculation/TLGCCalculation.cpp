#include "TLGCCalculation.h"

#include <Logger.hpp>
#include <TLSAlgorithm.h>
#include <TLSEvaluator.h>
#include <TLSGaussNewton.h>

#include "TDataAnalyzer.h"
#include "TLSAllfixed.h"
#include "TLSResultsMatrices.h"
#include "TLSSimulation.h"
#include "TVAbstractAlgorithm.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLGCCalculation::TLGCCalculation(std::shared_ptr<TLGCData> dat, int maxIterations) : fData(dat), fMaxIterations(maxIterations), fResultsMtr(nullptr)
{
}

///////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////
Behavior TLGCCalculation::computeResults(std::shared_ptr<TSimulationOutputFileWriter> fileWriter)
{
	std::unique_ptr<TVAbstractAlgorithm> algorithm;
	Behavior successCalculation;

	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*fData.get());
	// Checks whether the data are consistent, assign unknown indices and initialize uninitialized objects (points, lines, planes), reference poiints for certain observations, etc.
	if (!analyzer.dataConsistent())
	{
		// throw std::runtime_error("Data are not consistent, see the output file: " + fData->getFileLogger().getOutputFileLocation() + " for more information.");
		return Behavior(Behavior::BehaviorCode::ERR_inputData, L"Data are not consistent, see the log file for more information.");
	}
	try
	{
		// Iteration through the points
		for (auto it(fData->getPoints().begin()); it != fData->getPoints().end(); ++it)
		{
			it->transformProvisionalCoordinates(fData.get());
		}

		algorithm.reset(new TLSAlgorithm(*fData.get()));

		if (fData->getConfig().sim.isActive())
			algorithm.reset(new TLSSimulation(*fData.get(), fMaxIterations, fileWriter));
		else if (fData->getConfig().allfixed.isActive())
			algorithm.reset(new TLSAllfixed(*fData.get(), fMaxIterations));

		tryRegularizedSolve();

		successCalculation = algorithm->run(*fData.get(), fMaxIterations);

		if (successCalculation)
		{
			fResultsMtr = algorithm->resultMatrices;

			// Iteration through the points
			for (auto it(fData->getPoints().begin()); it != fData->getPoints().end(); ++it)
			{
				it->setCovarianceMatrixInRoot(fData.get());
				it->transformEstimatedCoordinates(fData.get());
				it->changeProvValueToCCS(fData.get());
			}
		}
	}
	catch (std::exception &e)
	{
		fData->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << e.what();
		successCalculation += Behavior(Behavior::BehaviorCode::ERR_results, L"Problem during computation.");
	}

	// If the calculation succeeded, initialise the observation summaries:
	if (successCalculation)
		initialiseObsSummaries();

	return successCalculation;
}

void TLGCCalculation::tryRegularizedSolve()
{
	TLSEvaluator evaluator(fData);
	std::shared_ptr<TLSEvaluator> evalPtr = std::make_shared<TLSEvaluator>(evaluator);

	TLSGaussNewton gnObject(evalPtr);

	Eigen::VectorXd provVal = evaluator.getEstParams();

	// create a bunch of random starting values
	int numberSamples = 10;
	std::vector<Eigen::VectorXd> startValues;
	// include the provided provisional value in the sample list
	startValues.push_back(provVal);
	for (int j = 0; j < numberSamples; j++)
	{
		// Eigen::VectorXd randVal = (Eigen::VectorXd::Ones(dim) + Eigen::VectorXd::Random(dim))/2;
		Eigen::VectorXd randVal = Eigen::VectorXd::Random(fData->fUEOIndices.UIndex);
		// both try a perturbed version of the supplied provisional value as well as a totally random initial value.
		startValues.push_back(provVal + randVal);
		startValues.push_back(randVal);
	}

	// attempt to solve
	// prepare results
	std::vector<GNResult> results;
	// needed to clean the error logs after each solution attempt
	TFileLogger &fileLog = fData->getFileLogger();
	int j = 0;
	for (auto sval : startValues)
	{
		j++;
		// set initial value and start armijo GN from this value
		try
		{
			GNResult result = gnObject.solve(sval);

			results.push_back(result);
			if (result.sigma0Aposteriori < 5 && result.success)
			{
				logWarning() << "Solution with Sigma <5 found.";
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

		fileLog.cleanErrors();
	// find best solution candidate
	double bestSigma = 1e+12;
	Eigen::VectorXd bestSol;
	bool solFound = false;
	for (auto result : results)
	{
		if (result.sigma0Aposteriori < bestSigma && result.success)
		{
			bestSigma = result.sigma0Aposteriori;
			bestSol = result.solution;
			solFound = true;
		}
	}
	if (solFound)
	{
		logWarning() << "Random initial value sampling found a solution with sigma a posteriori= " << bestSigma;
		logWarning() << "LGC will continue with this solution";
		// apply a random perturbation such that the LGC least square method still makes a proper iteration
		//evaluator.setParameters(bestSol + 1e-4 * Eigen::VectorXd::Random(fData->fUEOIndices.UIndex));
		evaluator.setParameters(bestSol);
	}
	else
	{
		logWarning() << "Random initial value sampling was unable to find a solution.";
		evaluator.setParameters(provVal);
	}
	// continue LGC normally, if solution was already found only one iteration will be made
	// clean errors that potentially happened during input filling
	fileLog.cleanErrors();
}

void TLGCCalculation::initialiseObsSummaries()
{
	// Iterate the whole tree and initialise the
	// observation summaries in each node:
	for (auto &node : fData->getTree())
		node->measurements.initialiseObsSummaries();
}
