#include "TMonitor.h"

#include <iostream>

#include <Behavior.h>
#include <TLGCData.h>
#include <TLSAlgorithm.h>
#include <TReader.h>

#include "FileUtils.h"
#include "TDataAnalyzer.h"
#include "TLGCCalculation.h"
#include "TLSResultsMatrices.h"
#include "TLSSimulation.h"
#include "TVAbstractAlgorithm.h"

// constructor
TMonitor::TMonitor()
{
	initialize();
}

void TMonitor::adjust()
{
	Behavior successCalculation;
	TLSResultsMatrices *results(nullptr);
	successCalculation = TMonitor::algorithm->run(*project.get(), 80);
	if (successCalculation)
	{
		results = algorithm->resultMatrices;
	}
	std::cout << "Adjustment method finished." << std::endl;
}
void TMonitor::initialize()
{
	Behavior successCalculation;

	std::cout << "Creating monitoring object.\n";
	std::shared_ptr<TLGCData> projTest(new TLGCData);
	project = projTest;
	TReader r(project);

	project->getFileLogger().setOutputfileLocation("C:/Temp/Fras_Test.txt");
	project->getFileLogger().writeReportHeader("Fras output file");

	// Testfile is LB_calcul_3D_CCS_IP_8_HLS_4.lgc
	std::string inputFilePath = svlTools::getPathFileName("../LB_calcul_3D_CCS_IP_8_HLS_4.lgc");

	std::ifstream inputFileStream(inputFilePath, std::ifstream::in);
	bool succesReading = r.read(inputFileStream);
	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*project.get());
	std::cout << analyzer.dataConsistent() << std::endl;

	algorithm.reset(new TLSAlgorithm(*project.get()));
	// make measurements easy accessible
	createMeasurementReferences();
	std::cout << "Monitor object initialized." << std::endl;
}

void TMonitor::createMeasurementReferences()
{
	// go through the frame tree, collect all ecws measurements
	for (TDataTreeIterator itTree = project.get()->getTree().begin(); itTree != project.get()->getTree().end(); itTree++)
	{
		// iterate over water level measurement rounds in that frame
		for (auto &itECWSrom : itTree.node->data->measurements.fECWS)
		{
			// iterate over single measurements
			for (auto itECWS(itECWSrom.measECWS.begin()); itECWS != itECWSrom.measECWS.end(); ++itECWS)
			{
				// add reference, Linenumber as key at first, shall be a unique measurement/sensor id at a later stage
				ecws.insert({std::to_string(itECWS->line), *itECWS});
			}
		}
	}
}
void TMonitor::updateMeas(std::string id, double value)
{
	// manipulate the corresponding measurement by accesing it via the reference map.
	// check if id exists
	if (ecws.count(id) > 0)
	{
		ecws.at(id).setDistance(TLength(value));
	}
	else
	{
		std::cout << "Measurement ID " << id << " does not exist." << std::endl;
	}
}

std::vector<std::string> TMonitor::getMeasIds()
{
	std::vector<std::string> theIds;
	for (auto aux : ecws)
	{
		theIds.push_back(aux.first);
	}
	return theIds;
}
