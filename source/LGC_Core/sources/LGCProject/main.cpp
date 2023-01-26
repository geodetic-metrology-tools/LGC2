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
//#include "Moni.h"
using namespace std::chrono;

int main(int argc, char *argv[])
{
//
//	// for random numbers
//	std::ranlux48 engine;
//	engine.seed(1);
//	auto start = high_resolution_clock::now();
//	//std::string inputFilePath = svlTools::getPathFileName("../LB_calcul_3D_CCS_IP_8_HLS_4.lgc");
//	std::string inputFilePath = svlTools::getPathFileName("../SC.lgc2");
//	
//	
//	Moni mockup(inputFilePath);
//	
//	// get the ids so the controlling object (will be the Fras instance) knows them.
//	std::vector<std::string> ecwsIds = mockup.getECWSMeasIds();
//	// first save the original measurements
//	std::unordered_map<std::string, double> originalMeasurements;
//	for (auto id : ecwsIds)
//	{
//		originalMeasurements.insert({id, mockup.getMeas(id)[0]});
//	}
//	// Simulating a monitoring scenario
//	for (int i = 0; i < 1000; i++)
//	{
//		for (auto id: ecwsIds)
//		{
//			// simulate new measurements by taking the old ones and add a perturbation with standard deviation sigma
//			//TReal sigma(0.00003);
//			TReal sigma(0.003);
//			TLength newMeas = TLength(std::normal_distribution<double>(0, sigma)(engine)) + TLength(originalMeasurements.at(id));
//			Eigen::VectorXd new_measurement(1);
//			new_measurement(0)=(double) newMeas;
//			mockup.updateMeas(id, new_measurement);
//		}
//		mockup.adjust();
//		auto currentTime = high_resolution_clock::now();
//		auto duration = duration_cast<milliseconds>(currentTime- start);
//		// get exemplary parameter
//		// not unique!!	"A-TAP.HLS1";
//		std::string parameterName = "B-TAP.WPS2";
//
//		// Print some results
//		std::cout << "\r Fras iteration " << std::setw(5) << i << std::setprecision(5) << ", elapsed time " << (double)duration.count() / 1000 << " s. " << std::endl
//				  << "in Sub frame " << parameterName << " = " << std::setprecision(8) << mockup.getEstimate(parameterName).transpose() << "     Precisions "
//				  << mockup.getEstimateCovar(parameterName).transpose() << " Sigma_0 = " << mockup.getSigma0() << std::endl;
//		std::cout << std::setw(5) << std::setprecision(8) << "in Rootframe " << parameterName << " = " << std::setprecision(8)
//				  << mockup.getEstimate(parameterName, "ROOT").transpose() << "     Precisions " << mockup.getEstimateCovar(parameterName, "ROOT").transpose()
//				  << " Sigma_0 = " << mockup.getSigma0() << std::endl;
//	}
//	auto stop = high_resolution_clock::now();
//	auto duration = duration_cast<seconds>(stop - start);
//	std::cout << "Elapsed time (s): " << duration.count() << std::endl;
//
	return 1;
}
