#include "TLGCCalculation.h"
#include "TDataAnalyzer.h"
#include "TLSSimulation.h"
#include "TLSAllfixed.h"
#include <TLSAlgorithm.h>
#include <TLSEvaluator.h>
#include <TLSGaussNewtonSolver.h>
#include "TVAbstractAlgorithm.h"
#include "TLSResultsMatrices.h"
#include <Logger.hpp>
#include <TLSEvaluator.h>
#include <TLSGraph.h>
#include "Serializer_json.hpp"
#include <fstream>
#include <Timer.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLGCCalculation::TLGCCalculation(std::shared_ptr<TLGCData> dat, int maxIterations) : fData(dat), fMaxIterations(maxIterations), fResultsMtr(nullptr)
{}

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
	try{
		// Iteration through the points
		for (auto it(fData->getPoints().begin()); it != fData->getPoints().end(); ++it)
		{
			it->transformProvisionalCoordinates(fData.get());
		}

		algorithm.reset(new TLSAlgorithm(*fData.get()));

		// try to find solution using armijo-stepsize regularization
		// several attempts with different initial values are tried
		// the one that converges to the lowest sigma will be the prefered solution
		// this is a heuristiv that shall reduce the probability to end up in a suboptimal local minimum
		tryArmijoSampling();


		if (fData->getConfig().sim.isActive())
			algorithm.reset(new TLSSimulation(*fData.get(), fMaxIterations, fileWriter));
		else if (fData->getConfig().allfixed.isActive())
			algorithm.reset(new TLSAllfixed(*fData.get(), fMaxIterations));
		
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
	catch (std::exception& e)
	{
		fData->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << e.what();
		successCalculation += Behavior(Behavior::BehaviorCode::ERR_results, L"Problem during computation.");
	}

    // If the calculation succeeded, initialise the observation summaries:
    if(successCalculation)
        initialiseObsSummaries();

	return successCalculation;
}

void TLGCCalculation::tryArmijoSampling()
{
	// set seed for ranwom number generation
	srand(static_cast<unsigned int>(0));
	int dim = fData.get()->fUEOIndices.UIndex;
	TLSEvaluator evaluator(fData);
	std::shared_ptr<TLSEvaluator> evalPtr = std::make_shared<TLSEvaluator>(evaluator);
	solverConfig armijoGN = {2, true, false, 0, 100, 1e-6};
	TLSGaussNewtonSolver gnObject(evalPtr);
	gnObject.setConfig(armijoGN);

	// For more robust Tsunami prototype
	if (fData.get()->fUEOIndices.CIndex == 0 && dim > 0)
	{
		// use Armijo linesearch to find solution
		Eigen::VectorXd provVal = evaluator.getEstParams();

		// create a bunch of random starting values
		int numberSamples = 10;
		std::vector<Eigen::VectorXd> startValues;
		// include the provided provisional value in the sample list
		startValues.push_back(provVal);
		for (int j = 0; j < numberSamples; j++)
		{
			Eigen::VectorXd randVal = Eigen::VectorXd::Random(dim);
			// both try a perturbed version of the supplied provisional value as well as a totally random initial value.
			startValues.push_back(provVal + randVal);
			startValues.push_back(randVal);
		}

		// prepare results
		std::vector<GNresult> results;
		for (auto sval : startValues)
		{
			// set initial value and start armijo GN from this value
			evaluator.setParameters(sval);
			try
			{
				GNresult result = gnObject.solve();
				results.push_back(result);
				if (result.sigma0Aposteriori < 5 && result.success)
				{
					logWarning() << "Solution with Sigma <5 found.";
					break;
				}
			}
			catch (...)
			{
				logWarning() << "Problem occured during attempting solution of problem with randomly generated initial value";
			}
		}

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
			evaluator.setParameters(bestSol);
		}
		else
		{
			logWarning() << "Random initial value sampling was unable to find a solution.";
			evaluator.setParameters(provVal);
		}
	}
	else
	{
		logWarning() << "Armijo stepsize globalization strategy only implemented for unconstrained problems -- continuing with classic LGC";
	}
	// continue LGC normally, if solution was already found only one iteration will be made
}

void TLGCCalculation::initialiseObsSummaries(){

    // Iterate the whole tree and initialise the
    // observation summaries in each node:
    for(auto &node : fData->getTree())
        node->measurements.initialiseObsSummaries();
}

