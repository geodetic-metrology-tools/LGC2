#include "TLGCCalculation.h"
#include "TDataAnalyzer.h"
#include "TSimulationCalculation.h"
#include "TLSSolutionCalculator.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLGCCalculation::TLGCCalculation(std::shared_ptr<TLGCData> dat): fData(dat),  fNumberOfMadeIterations(0), fMaxIterations(40)
{
	fConvCriteria = fData->getConfig().outPrecision.convCrit;
}

///////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////
bool TLGCCalculation::computeResults(std::shared_ptr<TResSimFileWriter> fileWriter){
	bool successCalculation = false;

	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*fData.get());
	// Checks whether the data are consistent, assign unknown indices and initialize uninitialized objects (points, lines, planes), reference poiints for certain observations, etc. 
	if(!analyzer.dataConsistent())
		throw std::runtime_error("Data are not consistent, see the output file: " + fData->getFileLogger().getOutputFileLocation() + " for more information.");

	try{
		if(fData->getConfig().libre.isActive())
			analyzer.addNetworkConstraints();

		if(fData->getConfig().sim.isActive()){ //Process simulation
			TSimulationCalculation simu(*fData.get(), fMaxIterations, fConvCriteria);
			successCalculation = simu.computeSimulatedResults(fileWriter);
		}
		else if (fData->getConfig().allfixed.isActive())//ALLFIXED keyword used
			successCalculation = computeALLFIXEDResults();
		else //No special keyword used
			successCalculation = computeNormalCalcResults();
	}
	catch(exception& e){
		fData->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << e.what();
		successCalculation = false;
	}

	if(fData->getFileLogger().hasErrors())
		successCalculation = false;

	return successCalculation;
}

bool TLGCCalculation::computeALLFIXEDResults(){
	bool successCalc = false;

	/*Set all frame parameters to be fixed -- PROBABLY YES, BUT NEEDS CONFIRMATION (TO DO)*/
	// ... 

	/*If at least one unknown in the data, use Least Squares algorithm.*/
	/*In the future we might add other algorithms than Least Squares.*/
	if(fData->fUEOIndices.UIndex>0){
		TLSSolutionCalculator lsCalc;
		successCalc = lsCalc.iterate2Solution(*fData.get(),fMaxIterations,fConvCriteria);	
	}
	else
		throw runtime_error("There is no unknown in the data, the Least Squares algorithm can not be used!");

	return successCalc;
}

bool TLGCCalculation::computeNormalCalcResults(){
	bool successCalc = false;
	/*If at least one unknown in the data, use Least Squares algorithm.*/
	/*In the future we might add other algorithms than Least Squares.*/
	if(fData->fUEOIndices.UIndex>0){
		TLSSolutionCalculator lsCalc;
		successCalc = lsCalc.iterate2Solution(*fData.get(),fMaxIterations,fConvCriteria);
	}
	else
		throw runtime_error("There is no unknown in the data, the Least Squares algorithm can not be used!");

	return successCalc;
}

