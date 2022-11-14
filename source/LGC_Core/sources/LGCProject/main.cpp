#include <chrono>
#include <iostream>
#include <random>

#include <ConsoleLogHandler.hpp>
#include <FileLogHandler.hpp>
#include <Logger.hpp>

#include "Defaults.h"
#include "FileUtils.h"
#include "TFileLogger.h" // Will be obsolete soon
#include "TLGCApp.h"
#include "TMonitor.h"
using namespace std::chrono;

int main(int argc, char *argv[])
{
	// for random numbers
	std::ranlux48 engine;
	engine.seed(1);
	auto start = high_resolution_clock::now();
	std::string inputFilePath = svlTools::getPathFileName("../LB_calcul_3D_CCS_IP_8_HLS_4.lgc");
	TMonitor mockup(inputFilePath);
	// get the ids so the controlling object (will be the Fras instance) knows them.
	std::vector<std::string> ecwsIds = mockup.getECWSMeasIds();
	// first save the original measurements
	std::unordered_map<std::string, double> originalMeasurements;
	for (auto id : ecwsIds)
	{
		originalMeasurements.insert({id, mockup.measRefs.ECWS.at(id).getDistance()});
	}
	for (int i = 0; i < 100; i++)
	{
		for (auto aux : mockup.measRefs.ECWS)
		{
			std::string ecwsId = aux.first;
			// simulate new measurements by taking the old ones and add a perturbation with standard deviation sigma
			//TLength oldMeas = mockup.measRefs.ECWS.at(ecwsId).getDistance();
			TReal sigma = mockup.measRefs.ECWS.at(ecwsId).target.sigmaDist;
			TLength newMeas = TLength(std::normal_distribution<double>(0, sigma)(engine)) + TLength(originalMeasurements.at(ecwsId));
			Eigen::VectorXd new_measurement(1);
			new_measurement(0)=(double) newMeas;
			mockup.updateMeas(ecwsId, new_measurement);
		}
		mockup.adjust();
		auto currentTime = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(currentTime- start);
		std::cout << "\r Fras iteration " << i << ", elapsed time " << (double) duration.count()/1000 << " s";
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stop - start);
	std::cout << std::endl;
	std::cout << "Elapsed time (s): " << duration.count() << std::endl;

	return 1;
}
