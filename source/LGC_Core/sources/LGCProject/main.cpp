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
	TMonitor mockup;
	// get the ids so the controlling object (will be the Fras instance) knows them.
	std::vector<std::string> theIds = mockup.getMeasIds();
	for (int i = 0; i < 100; i++)
	{
		for (auto aux : theIds)
		{
			// simulate new measurements by taking the old ones and add a perturbation with standard deviation sigma
			TLength oldMeas = mockup.ecws.at(aux).getDistance();
			TReal sigma = mockup.ecws.at(aux).target.sigmaDist;
			TLength newMeas = TLength(std::normal_distribution<double>(0, sigma)(engine)) + oldMeas;
			mockup.updateMeas(aux, double(newMeas));
		}
		mockup.adjust();
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	std::cout << "Elapsed time (ms): " << duration.count() << std::endl;

	return 1;
}
