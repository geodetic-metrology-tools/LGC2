#include "TLGCCalculation.h"
#include "TDataAnalyzer.h"
#include "TLSSimulation.h"
#include "TLSAlgorithm.h"
#include "TVAbractAlgorithm.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLGCCalculation::TLGCCalculation(std::shared_ptr<TLGCData> dat): fData(dat), fMaxIterations(40)
{}

///////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////
bool TLGCCalculation::computeResults(std::shared_ptr<TSimulationOutputFileWriter> fileWriter){
	bool successCalculation = false;

	std::unique_ptr<TVAbractAlgorithm> algorithm;

	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*fData.get());
	// Checks whether the data are consistent, assign unknown indices and initialize uninitialized objects (points, lines, planes), reference poiints for certain observations, etc. 
	if(!analyzer.dataConsistent())
		throw std::runtime_error("Data are not consistent, see the output file: " + fData->getFileLogger().getOutputFileLocation() + " for more information.");

	try{
		bool L1NormUsed = false;

		if (L1NormUsed)  //L1Norm
		{
		}
		else  // LS
		{
			algorithm.reset(new TLSAlgorithm());

			if (fData->getConfig().libre.isActive())//LIBR keywork is used
				analyzer.addNetworkConstraints();

			else if (fData->getConfig().sim.isActive()){ //SIMU keywork is used
				algorithm.reset(new TLSSimulation(*fData.get(), fMaxIterations, fileWriter));
			}
			else if (fData->getConfig().allfixed.isActive())//ALLFIXED keyword used
			{
				int i = 0;
				//algorithm.reset(new TLSAllfixed(*fData.get(), fMaxIterations, fileWriter));
			}
			successCalculation = algorithm->run(*fData.get(), fMaxIterations);

		}
		
	}
	catch(exception& e){
		fData->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << e.what();
		successCalculation = false;
	}

	if(fData->getFileLogger().hasErrors())
		successCalculation = false;

	return successCalculation;
}

