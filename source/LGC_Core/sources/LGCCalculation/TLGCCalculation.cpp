#include "TLGCCalculation.h"
#include "TDataAnalyzer.h"
#include "TLSSimulation.h"
#include "TLSAllfixed.h"
#include <TLSAlgorithm.h>
#include "TVAbractAlgorithm.h"
#include "TLSResultsMatrices.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLGCCalculation::TLGCCalculation(std::shared_ptr<TLGCData> dat) : fData(dat), fMaxIterations(800), fResultsMtr(nullptr)
{}

///////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////
Behavior TLGCCalculation::computeResults(std::shared_ptr<TSimulationOutputFileWriter> fileWriter){
	std::unique_ptr<TVAbractAlgorithm> algorithm;
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

		algorithm.reset(new TLSAlgorithm(*fData.get()));

		if (fData->getConfig().sim.isActive())
			algorithm.reset(new TLSSimulation(*fData.get(), fMaxIterations, fileWriter));
		else if (fData->getConfig().allfixed.isActive())
			algorithm.reset(new TLSAllfixed(*fData.get(), fMaxIterations));
		
		successCalculation = algorithm->run(*fData.get(), fMaxIterations);

		if (successCalculation)
			fResultsMtr = algorithm->resultMatrices;

	}
	catch (exception& e)
	{
		fData->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << e.what();
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