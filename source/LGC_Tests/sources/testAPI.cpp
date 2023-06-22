#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


//#include <TLGCData.h>
//#include <TReader.h>
//#include "TLGCCalculation.h"
//#include <Behavior.h>
#include "Moni.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std::chrono;
namespace tut
{
	struct test_API 
	{
		//test_API();
	};
	typedef test_group<test_API> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Test API (relative Error) computations");
}

namespace tut
{
	template<>
	template<>
	void object::test<1>()
	{

		set_test_name("Testing standard API use");
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
		// std::string pointName = "B-TAP.WPS2";
		// std::string pointName = "B-TAP.HLS1";
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

	//		TLGCCalculation calcul(projTest);
	//		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	//		Behavior succesCalc = calcul.computeResults(fileWriter);
	//		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	//		const TLGCData& dataset = calcul.getData();
	//		LSRelErrorsContainer lsErrors = dataset.getRelError();
	//		LSRelErrorIter relErrorIt = lsErrors.begin();
	//
	//		TLSCalcRelativeError relError1 = *relErrorIt;
	//		// test if precisions from polar measurement target can be recovered by erel (all in root)
	//		ensure_equals("Zend sigma has to match.", relError1.getSigmaV().getSignedCCValue(), 13, 1e-4);
	//		ensure_equals("Orientation sigma has to to match.", relError1.getSigmaG().getSignedCCValue(), 12, 1e-4);
	//		ensure_equals("Distance sigma has to match.", relError1.getSigmaL().getMMetresValue(), 14, 1e-4);
	//
	//		// test if precisions from polar measurement target can be recovered by erel (point in root observed from uncertain frame) in uncertain frame coordinates (should be independent of frame uncertainty)
	//		relErrorIt++;
	//		TLSCalcRelativeError relError2 = *relErrorIt;
	//		ensure_equals("Zend sigma has to match.", relError2.getSigmaV().getSignedCCValue(), 13, 1e-4);
	//		ensure_equals("Orientation sigma has to to match.", relError2.getSigmaG().getSignedCCValue(), 12, 1e-4);
	//		ensure_equals("Distance sigma has to match.", relError2.getSigmaL().getMMetresValue(), 14, 1e-4);
	//
	//		// test if the relative error between a fixed point in a frame and a point in a different frame measured only by one obsxyz from that frame is equal to the relative error of an equivalent setup in root
	//		relErrorIt++;
	//		TLSCalcRelativeError relErrorReferenceSetup = *relErrorIt;
	//		relErrorIt++;
	//		TLSCalcRelativeError relErrorComplicatedTrSetup = *relErrorIt;
	//		ensure_equals("L relative Error has to match.", relErrorReferenceSetup.getSigmaL(), relErrorComplicatedTrSetup.getSigmaL(), 1e-4);
	//		ensure_equals("G relative Error has to match.", relErrorReferenceSetup.getSigmaG(), relErrorComplicatedTrSetup.getSigmaG(), 1e-4);
	//		ensure_equals("R relative Error has to match.", relErrorReferenceSetup.getSigmaR(), relErrorComplicatedTrSetup.getSigmaR(), 1e-4);
	//		ensure_equals("Z relative Error has to match.", relErrorReferenceSetup.getSigmaZ(), relErrorComplicatedTrSetup.getSigmaZ(), 1e-4);
	//		ensure_equals("V relative Error has to match.", relErrorReferenceSetup.getSigmaV(), relErrorComplicatedTrSetup.getSigmaV(), 1e-4);
	//		relErrorIt++;
	//		TLSCalcRelativeError relErrorComplicatedRotSetup = *relErrorIt;
	//		ensure_equals("L relative Error has to match.", relErrorReferenceSetup.getSigmaL(), relErrorComplicatedRotSetup.getSigmaL(), 1e-4);
	//		ensure_equals("G relative Error has to match.", relErrorReferenceSetup.getSigmaG(), relErrorComplicatedRotSetup.getSigmaG(), 1e-4);
	//		ensure_equals("R relative Error has to match.", relErrorReferenceSetup.getSigmaR(), relErrorComplicatedRotSetup.getSigmaR(), 1e-4);
	//		ensure_equals("Z relative Error has to match.", relErrorReferenceSetup.getSigmaZ(), relErrorComplicatedRotSetup.getSigmaZ(), 1e-4);
	//		ensure_equals("V relative Error has to match.", relErrorReferenceSetup.getSigmaV(), relErrorComplicatedRotSetup.getSigmaV(), 1e-4);
	//
	}

};
