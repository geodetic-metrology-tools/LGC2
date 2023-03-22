#include "TLGCCalculation.h"
#include "TDataAnalyzer.h"
#include "TLSSimulation.h"
#include "TLSAllfixed.h"
#include <TLSAlgorithm.h>
#include <TLSEvaluator.h>
#include "TVAbstractAlgorithm.h"
#include "TLSResultsMatrices.h"
#include <Logger.hpp>

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

		//{
		for (int i = 0; i < 1000; i++)
		{
			std::cout << "evaluation nr " << i << std::endl;
			std::shared_ptr<TLGCData> aux = fData->clone();
			TLSEvaluator eval(aux);
			Eigen::VectorXd test(fData->fUEOIndices.UIndex);
			test.setZero();
			Eigen::VectorXd result(fData->fUEOIndices.EIndex);
			result = eval.evaluate(test);
			//std::cout << result << std::endl;
			//}
		}
		algorithm.reset(new TLSAlgorithm(*fData.get()));

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

void TLGCCalculation::initialiseObsSummaries(){

    // Iterate the whole tree and initialise the
    // observation summaries in each node:
    for(auto &node : fData->getTree())
        node->measurements.initialiseObsSummaries();
}