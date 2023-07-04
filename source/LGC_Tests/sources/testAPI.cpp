#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

#include "Moni.h"

using namespace std::chrono;
namespace tut
{
struct test_API
{
	// test_API();
};
typedef test_group<test_API> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test API ");
}

namespace tut
{

template<>
template<>
void object::test<1>()
{
	set_test_name("Testing simple API use");
	Moni apiObject("test_files/minimalTest.lgc2");
	apiObject.adjust();
	Eigen::VectorXd estRes = apiObject.getFrameEstimate("testFrame");
	Eigen::VectorXd expectedRes(7);
	expectedRes << 1, 2, 3, 0, 0, 0, 1;
	ensure("Frame Parameters have to be estimated correctly.", estRes.isApprox(expectedRes, 1e-12));
	
	// test P2 position
	Eigen::VectorXd estP2 = apiObject.getPointEstimate("P2");
	Eigen::VectorXd expectedP2(3);
	expectedP2.setZero();
	ensure("P2 needs to be estimated at 0,0,0", estP2.isApprox(expectedP2));
	// test measurement deactivation
	apiObject.setActivationStatus("testObs2", false);
	apiObject.adjust();
	estP2 = apiObject.getPointEstimate("P2");
	expectedP2.setConstant(1);
	ensure("After testObs2 deactivation, P2 needs to be estimated at 1,1,1", estP2.isApprox(expectedP2));
	
	// test reactivation
	apiObject.setActivationStatus("testObs2", true);
	apiObject.adjust();estP2 = apiObject.getPointEstimate("P2");
	expectedP2.setZero();
	ensure("After testObs2 reactivation, P2 needs to be estimated again at 0,0,0", estP2.isApprox(expectedP2));

	// change the obsxyz observation and make new estimation
	Eigen::VectorXd newMeas(3);
	newMeas << 11, 13, 19;
	apiObject.updateMeas("testObs1", newMeas);
	apiObject.adjust();
	expectedRes << newMeas, 0, 0, 0, 1;
	estRes = apiObject.getFrameEstimate("testFrame");
	ensure("Frame Parameters have to be estimated correctly.", estRes.isApprox(expectedRes, 1e-12));

	//
	Eigen::VectorXd p2 = apiObject.getPointEstimate("P2");
	// test calcMeas method. both observations of P2 should give the same calcmeas
	Eigen::VectorXd calcMeas1 = apiObject.getCalcMeas("testObs2");
	Eigen::VectorXd calcMeas2 = apiObject.getCalcMeas("testObs3");
	ensure("Both OBSXYZ observations of P2 should have the same calcmeas.", calcMeas1.isApprox(calcMeas2, 1e-12));

	// testing precisions
	Eigen::VectorXd framePrecisions = apiObject.getFrameEstimatePrec("testFrame");
	// should be the precisions coming from the obsxyz measurement

	Eigen::VectorXd expectedFramePrecisions(7);
	expectedFramePrecisions << 0.001, 0.001, 0.001, 0, 0, 0, 0;
	ensure("Frame precisions should match OBSXYZ precisions.", expectedFramePrecisions.isApprox(framePrecisions, 1e-12));

	// on the fly change of obsxyz precisions
	Eigen::VectorXd newObsxyzSigma(3);
	newObsxyzSigma << 0.12345, 0.23456, 0.34567;
	apiObject.setObsSigma("testObs1", newObsxyzSigma);
	apiObject.adjust();
	expectedFramePrecisions << newObsxyzSigma, 0, 0, 0, 0;
	framePrecisions = apiObject.getFrameEstimatePrec("testFrame");
	ensure("Frame precisions should match updated OBSXYZ weights.", expectedFramePrecisions.isApprox(framePrecisions, 1e-12));
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing method setFixedFrameParameter(std::string frameName, int idx, double val) ");
	Moni apiObject("test_files/manipulateFrameVars.lgc2");
	apiObject.adjust();
	Eigen::VectorXd estRes = apiObject.getFrameEstimate("testFrame");
	Eigen::VectorXd expectedRes(7);
	expectedRes << 0, 0, 0, 0, 0, 0, 1;
	ensure("Frame Parameters have to be estimated correctly.", estRes.isApprox(expectedRes, 1e-12));

	for (int j = 0; j < 7; j++)
	{
		// manipulate frame parameter j
		apiObject.setFixedFrameParameter("testFrame", j, 0.1*j);
		apiObject.adjust();
		Eigen::VectorXd framePars = apiObject.getFrameEstimate("testFrame");
		// j-th entry should be fixed to value 0.1*j now (take 0.1*j because j would be greater then pi and normalization ocurs)
		double estVar = framePars(j);
		ensure_equals("Frame variable index " + std::to_string(j) + " should be fixed to " + std::to_string(j) + " now.", estVar, 0.1 * j);
	}
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing API on single component test file");
	ensure_equals("Reading file successful", true, true);
	// for simulation of random perturbations
	std::ranlux48 engine;
	// reproducibility
	engine.seed(1);
	auto start = high_resolution_clock::now();
	std::string inputFilePath = "test_files/SC.lgc2";

	Moni mockup(inputFilePath);

	// the IDs for the observations we want to manipulate during the monitoring
	std::vector<std::string> ecwsIds = {"meas1", "meas2", "meas3", "meas4", "meas5", "meas6", "meas7", "meas8"};
	// first save the original measurements
	std::unordered_map<std::string, double> originalMeasurements;
	for (auto id : ecwsIds)
	{
		originalMeasurements.insert({id, mockup.getMeas(id)[0]});
	}
	bool status = false;

	// Simulating a monitoring scenario
	for (int i = 0; i < 100; i++)
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
			Eigen::VectorXd new_obsSigma(1);
			// testing  setObsSigma
			// new_obsSigma << 1e-5;
			// mockup.setObsSigma(id, new_obsSigma);
		}
		// test input file writer
		// mockup.writeLGCInputFile();

		status = mockup.adjust();
		auto currentTime = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(currentTime - start);
		// get exemplary parameter estimates
		std::string pointName = "A-TAP.HLS1";
		std::string frameName = "WIRE.RIGHT.DOF";

		std::cout << "Estimate of " << pointName << " in coordinates of its defining frame: " << std::endl << mockup.getPointEstimate(pointName) << std::endl;
		std::cout << "Estimate of " << pointName << " in coordinates of the \"ROOT\" frame: " << std::endl << mockup.getPointEstimate(pointName, "ROOT") << std::endl;
		// precisions of this point in its defining frame are zero because the point is fixed in this frame -- "CALA" point
		std::cout << "Estimated precision for " << pointName << " in coordinates of its defining frame: " << std::endl
				  << mockup.getPointEstimatePrec(pointName) << std::endl;
		// precisions of this point in "ROOT" frame are non-zero because there are non-trivial Helmert transformations with uncertain parameters involved
		std::cout << "Estimated precision for " << pointName << " in coordinates of the \"ROOT\" frame: " << std::endl
				  << mockup.getPointEstimatePrec(pointName, "ROOT") << std::endl;

		std::cout << "Estimated parameters for frame " << frameName << " : " << std::endl << mockup.getFrameEstimate(frameName) << std::endl;
		std::cout << "Estimated precision for frame " << frameName << " : " << std::endl << mockup.getFrameEstimatePrec(frameName) << std::endl;

		// get exemplary measurement residual
		std::string obsName = "meas1";
		std::cout << "Observed value of " << obsName << " = " << mockup.getMeas(obsName) << std::endl;
		std::cout << "Residual of " << obsName << " = " << mockup.getEstimateResidual(obsName) << std::endl;
		std::cout << "Calc meas of " << obsName << " = " << mockup.getCalcMeas(obsName) << std::endl;

		// get sigmaZero
		std::cout << "Sigma 0 aposteriori =" << mockup.getSigma0() << std::endl;

		// testing json output
		// mockup.writeResultFile();
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stop - start);
	std::cout << "Elapsed time (s): " << duration.count() << std::endl;


}

}; // namespace tut
