#include <iostream>
#include "TMonitor.h"
#include <TLGCData.h>
#include <TReader.h>
#include "TLGCCalculation.h"
#include "TVAbstractAlgorithm.h"
#include <TLSAlgorithm.h>
#include "TLSSimulation.h"
#include <Behavior.h>
#include "FileUtils.h"
#include "TDataAnalyzer.h"
#include "TLSResultsMatrices.h"

using namespace std;

void TMonitor::BasicObject()
{	// for random numbers
	engine.seed(1);

	cout << "Starting monitor!\n";
	std::shared_ptr<TLGCData> projTest(new TLGCData);
	TReader r(projTest);

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/Fras_Test.txt");
	projTest->getFileLogger().writeReportHeader("Fras output file");

	// Testfile is LB_calcul_3D_CCS_IP_8_HLS_4.lgc
	std::string inputFilePath = svlTools::getPathFileName("../LB_calcul_3D_CCS_IP_8_HLS_4.lgc");

	std::ifstream inputFileStream (inputFilePath, std::ifstream::in);
	bool succesReading = r.read(inputFileStream);
	//TLGCCalculation calcul(projTest);
	//std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	//Behavior succesCalc = calcul.computeResults(fileWriter);
	//for (int i = 0; i < 1000; i++) {
	//	// compute
	//	//Behavior succesCalc = calcul.computeResults(fileWriter);
	//	succesCalc = calcul.computeResults(fileWriter);
	//	// manipulate measurements
	//	TMonitor::manipulate_ECWS_measurements(projTest.get());
	//}
	std::unique_ptr<TVAbstractAlgorithm> algorithm;
	Behavior successCalculation;
	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*projTest.get());
	std::cout << analyzer.dataConsistent()<<std::endl;
	algorithm.reset(new TLSAlgorithm(*projTest.get()));
	TLSResultsMatrices *results(nullptr);
	int n = 10000;
	for (int i = 0; i < n; i++)
	{
		successCalculation = algorithm->run(*projTest.get(), 80);
		TMonitor::manipulate_ECWS_measurements(projTest.get());
		std::cout << "Iteration " << i << std::endl;
		if (successCalculation)
		{
			results = algorithm->resultMatrices;
		}
	}

	// if (successCalculation)
    //     fResultsMtr = algorithm->resultMatrices;



}


void TMonitor::manipulate_ECWS_measurements(TLGCData *data) {
	// FRAS-Mockup:
	// apply random perturbation to each ECWS measurement, as it is done in a simulation
	int i = 0;
	// iterate over frame tree
	for (TDataTreeIterator itTree = data->getTree().begin(); itTree != data->getTree().end(); itTree++) {
		// iterate over water level measurement rounds in that frame
		for (auto& itECWSrom : itTree.node->data->measurements.fECWS) {
			// iterate over single measurements
			for (auto itECWS(itECWSrom.measECWS.begin()); itECWS != itECWSrom.measECWS.end(); ++itECWS) {
				TLength oldMeas = itECWS->getDistance();
                // if (i == 0) {
                // std:cout << oldMeas << std::endl;
                // }
				// itECWS->setDistance(2 * aux);
				TReal sigma = itECWS->target.sigmaDist;
				TLength newMeas = TLength(std::normal_distribution<double>(0, sigma)(engine)) + oldMeas;
				itECWS->setDistance(newMeas);
				i++;
			}
		}
	}
}
