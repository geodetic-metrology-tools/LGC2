#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

// the monitoring class
#include "Moni.h"
using namespace std::chrono;

int main(int argc, char *argv[])
{
	// for simulation of random perturbations
	std::ranlux48 engine;
	// reproducibility
	engine.seed(1);
	auto start = high_resolution_clock::now();
	std::string inputFilePath = "../SC.lgc2";

	Moni mockup(inputFilePath);

	// the IDs for the observations we want to maniupualte during the monitoring
	std::vector<std::string> ecwsIds = {"meas1", "meas2", "meas3", "meas4", "meas5", "meas6", "meas7", "meas8"};
	// first save the original measurements
	std::unordered_map<std::string, double> originalMeasurements;
	for (auto id : ecwsIds)
	{
		originalMeasurements.insert({id, mockup.getMeas(id)[0]});
	}
	bool status = false;

	// Simulating a monitoring scenario
	for (int i = 0; i < 1000; i++)
	{
		for (auto id : ecwsIds)
		{
			// simulate new measurements by taking the original values and add a perturbation with standard deviation sigma
			// ECWS sigma = 0.001 mm = 1e-6m
			double sigma(1e-6);
			double newMeas(std::normal_distribution<double>(0, sigma)(engine) + originalMeasurements.at(id));
			Eigen::VectorXd new_measurement(1);
			new_measurement(0) = newMeas;
			mockup.updateMeas(id, new_measurement);
		}

		status = mockup.adjust();
		auto currentTime = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(currentTime - start);
		// get exemplary parameter estimates
		// std::string pointName = "B-TAP.WPS2";
		// std::string pointName = "B-TAP.HLS1";
		std::string pointName = "A-TAP.HLS1";
		std::string frameName = "WIRE.RIGHT.DOF";

		std::cout << "Estimate of " << pointName << " in coordinates of its defining frame: " << std::endl << mockup.getPointEstimate(pointName) << std::endl;
		std::cout << "Estimate of " << pointName << " in coordinates of the \"ROOT\" frame: " << std::endl << mockup.getPointEstimate(pointName, "ROOT") << std::endl;
		// precisions of this point in its defining frame are zero because the point is fixed in this frame -- "CALA" point
		std::cout << "Estimated precision for " << pointName << " in coordinates of its defining frame: " << std::endl << mockup.getPointEstimatePrec(pointName) << std::endl;
		// precisions of this point in "ROOT" frame are non-zero because there are non-trivial Helmert transformations with unceratin parameters involved
		std::cout << "Estimated precision for " << pointName << " in coordinates of the \"ROOT\" frame: " << std::endl
				  << mockup.getPointEstimatePrec(pointName, "ROOT") << std::endl;
		
		std::cout << "Estimated parameters for frame " << frameName << " : " << std::endl << mockup.getFrameEstimate(frameName) << std::endl;
		std::cout << "Estimated precision for frame " << frameName << " : " << std::endl << mockup.getFrameEstimatePrec(frameName) << std::endl;

		// get exemplary measurement residual
		std::string obsName = "meas1";
		std::cout << "Residual of " << obsName << " = " << mockup.getEstimateResidual(obsName) << std::endl;

		// get sigmaZero
		std::cout << "Sigma 0 aposteriori =" << mockup.getSigma0() << std::endl;

	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stop - start);
	std::cout << "Elapsed time (s): " << duration.count() << std::endl;

	return 1;
}
