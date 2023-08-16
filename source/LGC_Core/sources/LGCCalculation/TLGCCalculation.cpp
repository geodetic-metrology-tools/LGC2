#include "TLGCCalculation.h"

#include <Logger.hpp>
#include <TLSAlgorithm.h>

#include "TDataAnalyzer.h"
#include "TLSAllfixed.h"
#include <TLSAlgorithm.h>
#include <TLSEvaluator.h>
#include <TLSDerivativeTester.h>
#include <TLSGaussNewtonSolver.h>
#include <TLSLMSolver.h>
#include "TVAbstractAlgorithm.h"
#include "TLSResultsMatrices.h"
#include <Logger.hpp>
#include <TLSEvaluator.h>
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
		{
			// only now the constraint dimensions are set.
			TLSEvaluator evaluator(fData);
			TLSDerivativeTester tester(fData);
			// experimental: full step GN using the evaluator,
			// plan: armijo backtracking
			TLSEvaluator auxEval(fData);
			Eigen::VectorXd provPar = auxEval.getEstParams();
			TLSGaussNewtonSolver gnObject(std::make_shared<TLSEvaluator>(auxEval));

			// do nothing if uindex=0
			if (fData.get()->fUEOIndices.UIndex > 0)
			{
				auxEval.currentMask.parameterIndices = std::vector<int>{5,1,2,0,4,3};
				auxEval.currentMask.equationsIndices = std::vector<int>{5,4,3};
				TLSGaussNewtonSolver gnObject2(std::make_shared<TLSEvaluator>(auxEval));
				Eigen::VectorXd solution2 = gnObject2.solve();
				std::cout << solution2 << std::endl;
				std::cout << "global Var=" << auxEval.getEstParams(false) << std::endl<< std::endl;

				Eigen::VectorXd solution = gnObject.solve();
				std::cout << solution << std::endl;
				// should do only one iteration
				//solution = gnObject.solve();
				// test levenberg marquardt
				// reset for fairness vs armijo
				// auxEval.setParameters(provPar);
				// TLSLMSolver lmSolver(fData);
				// Eigen::VectorXd solutionLM = lmSolver.solve();
				// std::cout << "LM sol=" << solutionLM << std::endl;
			}
			// reset parameters - to not interfere with usual LGC calculation
			//auxEval.setParameters(provPar);
		}

		if (fData->getConfig().sim.isActive())
			algorithm.reset(new TLSSimulation(*fData.get(), fMaxIterations, fileWriter));
		else if (fData->getConfig().allfixed.isActive())
			algorithm.reset(new TLSAllfixed(*fData.get(), fMaxIterations));

		successCalculation = algorithm->run(*fData.get(), fMaxIterations);
		// repeat test after convergence	
		// TLSDerivativeTester tester(fData);
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

void TLGCCalculation::initialiseObsSummaries()
{
	// Iterate the whole tree and initialise the
	// observation summaries in each node:
	for (auto &node : fData->getTree())
		node->measurements.initialiseObsSummaries();
}
