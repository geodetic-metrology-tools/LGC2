#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <chrono>
#include <SimpleTimer.h>
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
	double sigmaBeforeDeactivation = apiObject.getSigma0();
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
	double sigmaAfterDeactivation = apiObject.getSigma0();
	estP2 = apiObject.getPointEstimate("P2");
	expectedP2.setConstant(1);
	ensure("After testObs2 deactivation, P2 needs to be estimated at 1,1,1", estP2.isApprox(expectedP2));

	// test reactivation
	apiObject.setActivationStatus("testObs2", true);
	apiObject.adjust();
	double sigmaAfterReactivation = apiObject.getSigma0();
	estP2 = apiObject.getPointEstimate("P2");
	expectedP2.setZero();
	ensure("After testObs2 reactivation, P2 needs to be estimated again at 0,0,0", (estP2 - expectedP2).norm() < 1e-8);

	// check if sigmas were affected by activation and deactivation of measurements
	ensure_equals("Sigma before deactivation and after reactivation should be equal.", sigmaBeforeDeactivation, sigmaAfterReactivation, 1e-6);
	ensure_equals("Sigma after deactivation should be 0 because the remaining observation can be fitted with 0 residual.", sigmaAfterDeactivation, 0.0, 1e-9);
	

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
	Eigen::VectorXd expectedObsxyzSigma = apiObject.getObsSigma("testObs1");
	ensure("Retrieved Sigma has to match newly set sigma.", expectedObsxyzSigma.isApprox(newObsxyzSigma, 1e-12));

	apiObject.adjust();
	expectedFramePrecisions << newObsxyzSigma, 0, 0, 0, 0;
	framePrecisions = apiObject.getFrameEstimatePrec("testFrame");
	ensure("Frame precisions should match updated OBSXYZ weights.", expectedFramePrecisions.isApprox(framePrecisions, 1e-12));
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing methods for manipulating fixed parameters");
	Moni apiObject("test_files/manipulateFixedParameters.lgc2");
	apiObject.adjust();
	Eigen::VectorXd estRes = apiObject.getFrameEstimate("testFrame");
	Eigen::VectorXd expectedRes(7);
	expectedRes << 0, 0, 0, 0, 0, 0, 1;
	ensure("Frame Parameters have to be estimated correctly.", estRes.isApprox(expectedRes, 1e-12));

	for (int j = 0; j < 7; j++)
	{
		// manipulate frame parameter j
		apiObject.setFixedFrameParameter("testFrame", j, 0.1 * j);
		apiObject.adjust();
		Eigen::VectorXd framePars = apiObject.getFrameEstimate("testFrame");
		// j-th entry should be fixed to value 0.1*j now (take 0.1*j because j would be greater then pi and normalization ocurs)
		double estVar = framePars(j);
		ensure_equals("Frame variable index " + std::to_string(j) + " should be fixed to " + std::to_string(j) + " now.", estVar, 0.1 * j);
	}

	// test for changing fixed point variables
	for (int j = 0; j < 3; j++)
	{
		// manipulate point parameter j
		apiObject.setFixedPointParameter("P2", j, j);
		apiObject.adjust();
		Eigen::VectorXd pointPars = apiObject.getPointEstimate("P2");
		double estVar = pointPars(j);
		ensure_equals("Point variable index " + std::to_string(j) + " should be fixed to " + std::to_string(j) + " now.", estVar, j);
	}
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing methods for freezing frame parameters");
	Moni apiObject("test_files/minimalTest.lgc2");
	apiObject.adjust();
	Eigen::VectorXd estRes = apiObject.getFrameEstimate("testFrame");
	Eigen::VectorXd expectedRes(7);
	expectedRes << 1, 2, 3, 0, 0, 0, 1;
	// std::cout << estRes << std::endl;
	ensure("Frame Parameters have to be estimated correctly.", estRes.isApprox(expectedRes, 1e-12));

	// test freezing a parameter
	apiObject.freezeFrameParameter("testFrame", 0, 5.0);
	apiObject.adjust();
	estRes = apiObject.getFrameEstimate("testFrame");
	ensure_equals("tx should be frozen to value of 5. ", estRes(0), 5.0);

	// unfreeze the parameter
	apiObject.unfreezeFrameParameter("testFrame", 0);
	apiObject.adjust();
	estRes = apiObject.getFrameEstimate("testFrame");
	ensure_equals("tx should be back at 1. ", estRes(0), 1.0);

	// try freezing a fixed variable
	try
	{
		apiObject.freezeFrameParameter("testFrame", 3, 1.0);
		fail("exception expected when trying to freeze a fixed varable.");
	}
	catch (const std::runtime_error &ex)
	{
		// ok
	}

	// try unfreeze non frozen variable
	try
	{
		apiObject.unfreezeFrameParameter("testFrame", 4);
		fail("exception expected");
	}
	catch (const std::runtime_error &ex)
	{
		// ok
	}
}

template<>
template<>
void object::test<4>()
{
	set_test_name("Testing methods for freezing point parameters");
	Moni apiObject("test_files/minimalTest.lgc2");
	apiObject.adjust();
	Eigen::VectorXd estRes = apiObject.getPointEstimate("P2");
	Eigen::VectorXd expectedRes(3);
	expectedRes << 0, 0, 0;
	ensure("Point Parameters have to be estimated correctly.", estRes.isApprox(expectedRes, 1e-12));

	// test freezing a parameter
	apiObject.freezePointParameter("P2", 0, 5.0);
	apiObject.adjust();
	estRes = apiObject.getPointEstimate("P2");
	ensure_equals("tx should be frozen to value of 5. ", estRes(0), 5.0);

	// extract precision of frozen parameter
	Eigen::VectorXd frozenPrecision = apiObject.getPointEstimatePrec("P2");
	ensure_equals("x coordinate is frozen so precision should be zero. ", frozenPrecision(0), 0.0);
	ensure("y coordinate is not frozen and there should be a nonzero-precision assigned", (fabs(frozenPrecision(1)) != 0.0));
	ensure("z coordinate is not frozen and there should be a nonzero-precision assigned", (fabs(frozenPrecision(2)) != 0.0));

	// unfreeze the parameter
	apiObject.unfreezePointParameter("P2", 0);
	apiObject.adjust();
	estRes = apiObject.getPointEstimate("P2");
	ensure_equals("x coordinate should be back at 0. ", estRes(0), 0.0);

	// try freezing a fixed variable
	try
	{
		apiObject.freezePointParameter("P1", 1, 1.0);
		fail("exception expected when trying to freeze a fixed varable.");
	}
	catch (const std::runtime_error &ex)
	{
		// ok
	}

	// try unfreeze non frozen variable
	try
	{
		apiObject.unfreezePointParameter("P2", 0);
		fail("exception expected");
	}
	catch (const std::runtime_error &ex)
	{
		// ok
	}
}
template<>
template<>
void object::test<5>()
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
		originalMeasurements.insert({id, mockup.getMeas(id)(0)});
	}
	bool status = false;

	// Simulating a monitoring scenario
	for (int i = 0; i < 10; i++)
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
		status = mockup.getStatus();
		ensure_equals("Estimation status should be false after measurement updates", status, false);
		// test input file writer
		// mockup.writeLGCInputFile();

		status = mockup.adjust();
		ensure_equals("Estimation status should be true after adjustment", status, true);
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

		// test water network data extraction
		waterRom waterNetworkResult = mockup.getECWSData("WaterNetwork1");
		std::cout << "Water network name= " << waterNetworkResult.romName << " water level height = " << waterNetworkResult.estimate << " with sigma "
				  << waterNetworkResult.prec << std::endl;

		// get sigmaZero
		std::cout << "Sigma 0 aposteriori =" << mockup.getSigma0() << std::endl;
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stop - start);
	std::cout << "Elapsed time (s): " << duration.count() << std::endl;
}

template<>
template<>
void object::test<6>()
{
	set_test_name("Testing API on single component test file");
	ensure_equals("Reading file successful", true, true);
	// for simulation of random perturbations
	std::ranlux48 engine;
	// reproducibility
	engine.seed(1);
	auto start = high_resolution_clock::now();
	std::string inputFilePath = "test_files/LGC_SCT_WPS.lgc";

	Moni mockup(inputFilePath);

	// the IDs for the observations we want to manipulate during the monitoring
	std::vector<std::string> wpsIds = {
		"GISCD.B1LX.A_WPS", "GTAP.A1LX.A_WPS", "GTAP.A1LX.L_WPS", "GISCD.A1LX.A_WPS", "GISCD.B1LX.D_WPS", "GTAP.A1LX.D_WPS", "GTAP.A1LX.Q_WPS", "GISCD.A1LX.D_WPS"};

	// first save the original measurements
	std::unordered_map<std::string, Eigen::VectorXd> originalMeasurements;
	for (auto id : wpsIds)
	{
		originalMeasurements.insert({id, mockup.getMeas(id)});
	}
	bool status = false;

	// Simulating a monitoring scenario
	for (int i = 0; i < 10; i++)
	{
		for (auto id : wpsIds)
		{
			// simulate new measurements by taking the original values and add a perturbation with standard deviation sigma
			// ECWS sigma = 0.001 mm = 1e-6m
			double sigma(1e-6);
			Eigen::VectorXd perturbation(2);
			perturbation << std::normal_distribution<double>(0, sigma)(engine), std::normal_distribution<double>(0, sigma)(engine);
			Eigen::VectorXd new_measurement = perturbation + originalMeasurements.at(id);
			mockup.updateMeas(id, new_measurement);
			// Eigen::VectorXd new_obsSigma(1);
			//  testing  setObsSigma
			//  new_obsSigma << 1e-5;
			//  mockup.setObsSigma(id, new_obsSigma);
		}
		status = mockup.getStatus();
		ensure_equals("Estimation status should be false after measurement updates", status, false);
		// test input file writer
		// mockup.writeLGCInputFile();

		status = mockup.adjust();
		ensure_equals("Estimation status should be true after adjustment", status, true);
		auto currentTime = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(currentTime - start);

		// get exemplary parameter estimates
		std::string pointName = "BEAM_SCT.GTAP.A1LX.E";
		std::string frameName = "SCT.GTAP.A1LX.RSTRI";

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
		std::string obsName = "GTAP.A1LX.L_WPS";
		std::cout << "Observed value of " << obsName << " = " << mockup.getMeas(obsName) << std::endl;
		std::cout << "Residual of " << obsName << " = " << mockup.getEstimateResidual(obsName) << std::endl;
		std::cout << "Calc meas of " << obsName << " = " << mockup.getCalcMeas(obsName) << std::endl;

		wireRom wireNetworkResult = mockup.getECWIData("GIWPN.T1LX");
		std::cout << "Wire network name= " << wireNetworkResult.romName << " data (dx,dz,bearing,slope,sag) = " << wireNetworkResult.estimate << " with precision"
				  << wireNetworkResult.prec << std::endl;

		// get sigmaZero
		std::cout << "Sigma 0 aposteriori =" << mockup.getSigma0() << std::endl;
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stop - start);
	std::cout << "Elapsed time (s): " << duration.count() << std::endl;
}

template<>
template<>
void object::test<7>()
{
	set_test_name("Testing API on single component test file with nominal positions for direct offset calculation.");
	ensure_equals("Reading file successful", true, true);
	// for simulation of random perturbations
	std::ranlux48 engine;
	// reproducibility
	engine.seed(1);
	auto start = high_resolution_clock::now();
	std::string inputFilePath = "test_files/LGC_SCT_WPS_withNominalPositions.lgc";

	Moni mockup(inputFilePath);

	// the IDs for the observations we want to manipulate during the monitoring
	std::vector<std::string> wpsIds = {
		"GISCD.B1LX.A_WPS", "GTAP.A1LX.A_WPS", "GTAP.A1LX.L_WPS", "GISCD.A1LX.A_WPS", "GISCD.B1LX.D_WPS", "GTAP.A1LX.D_WPS", "GTAP.A1LX.Q_WPS", "GISCD.A1LX.D_WPS"};

	// first save the original measurements
	std::unordered_map<std::string, Eigen::VectorXd> originalMeasurements;
	for (auto id : wpsIds)
	{
		originalMeasurements.insert({id, mockup.getMeas(id)});
	}
	bool status = false;

	// Simulating a monitoring scenario
	for (int i = 0; i < 10; i++)
	{
		for (auto id : wpsIds)
		{
			// simulate new measurements by taking the original values and add a perturbation with standard deviation sigma
			// ECWI sigmas for x & y = 0.02 mm = 2e-5m
			double sigma(2e-5);
			Eigen::VectorXd perturbation(2);
			perturbation << std::normal_distribution<double>(0, sigma)(engine), std::normal_distribution<double>(0, sigma)(engine);
			Eigen::VectorXd new_measurement = perturbation + originalMeasurements.at(id);
			mockup.updateMeas(id, new_measurement);
		}
		status = mockup.getStatus();
		ensure_equals("Estimation status should be false after measurement updates", status, false);

		status = mockup.adjust();
		ensure_equals("Estimation status should be true after adjustment", status, true);
		auto currentTime = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(currentTime - start);

		// compute offset for SCT.GISCD.A1LX.E
		std::string pointNameAligned = "BEAM_SCT.GTAP.A1LX.E";
		std::string pointNameNominal = "BEAM_SCT.GTAP.A1LX.E_nominal";
		std::string frameNominal = "SCT.GTAP.A1LX.RSTB";
		std::string frameAligned = "SCT.GTAP.A1LX.RSTR";

		// get data and compute offset
		Eigen::VectorXd alignedPos = mockup.getPointEstimate(pointNameAligned, frameNominal);
		Eigen::VectorXd nominalPos = mockup.getPointEstimate(pointNameNominal, frameNominal);
		Eigen::VectorXd alignedPosPrec = mockup.getPointEstimatePrec(pointNameAligned, frameNominal);
		Eigen::VectorXd nominalPosPrec = mockup.getPointEstimatePrec(pointNameNominal, frameNominal); // will be zero because nominal point is cala in nominal frame
		Eigen::MatrixXd offsetData(3, 2);
		offsetData << alignedPos - nominalPos, alignedPosPrec;

		std::cout << "Position of point " << pointNameAligned << " in Frame " << frameNominal << std::endl;
		std::cout << alignedPos << std::endl;
		std::cout << "Position of point " << pointNameNominal << " in Frame " << frameNominal << std::endl;
		std::cout << nominalPos << std::endl;

		std::cout << "Offset and offset precision:" << std::endl;
		std::cout << offsetData << std::endl;

		std::cout << "offset calculation without nominal points " << std::endl;
		std::cout << mockup.getPointEstimate(pointNameAligned, frameNominal) - mockup.getPointEstimate(pointNameAligned, frameAligned) << std::endl;

		//wireRom wireNetworkResult = mockup.getECWIData("GIWPN.T1LX");
		//std::cout << "Wire network name= " << wireNetworkResult.romName << " data (dx,dz,bearing,slope,sag) = " << wireNetworkResult.estimate << " with precision"
		//		  << wireNetworkResult.prec << std::endl;

		//// get sigmaZero
		//std::cout << "Sigma 0 aposteriori =" << mockup.getSigma0() << std::endl;
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stop - start);
	std::cout << "Elapsed time (s): " << duration.count() << std::endl;
}
template<>
template<>
void object::test<8>()
{
	set_test_name("Testing simple sag example: freezing, setFixed, getEst, getEstPrec");
	Moni apiObject("test_files/minimalSag.lgc");
	apiObject.adjust();
	Eigen::Vector3d leftEst = apiObject.getPointEstimate("left");
	Eigen::Vector3d middleEst = apiObject.getPointEstimate("middle");
	Eigen::Vector3d rightEst = apiObject.getPointEstimate("right");
	Eigen::VectorXd sagParEst(4);
	sagParEst = apiObject.getSagEstimate("sag1");
	Eigen::Vector3d leftEstExpected(0.007, -1.0, 0.003);
	Eigen::Vector3d middleEstExpected(0.003, 0.0, 0.001);
	Eigen::Vector3d rightEstExpected(0.007, 1.0, 0.003);
	Eigen::Vector4d sagParEstExpected(4);
	sagParEstExpected << 0.001, 0.002, 0.003, 0.004;
	ensure("position should be estimated correctly", leftEst.isApprox(leftEstExpected));
	ensure("position should be estimated correctly", middleEst.isApprox(middleEstExpected));
	ensure("position should be estimated correctly", rightEst.isApprox(rightEstExpected));
	ensure("sag parameter should be estimated correctly", sagParEst.isApprox(sagParEstExpected));

	// now freeze the curvature values to 0
	// VC =>index 1
	apiObject.freezeSagParameter("sag1", 1, 0.0);
	// apiObject.freezeSagParameter("sag1", 3, 0.0);
	apiObject.adjust();
	leftEst = apiObject.getPointEstimate("left");
	middleEst = apiObject.getPointEstimate("middle");
	rightEst = apiObject.getPointEstimate("right");
	// as the curvature is zero, the z offset is fully determined by the vertical sag
	sagParEst = apiObject.getSagEstimate("sag1");
	ensure_equals("height should be equal", leftEst(2), middleEst(2));
	ensure_equals("height should be equal", leftEst(2), rightEst(2));
	ensure_equals("vertical curvature should be frozen at 0", sagParEst(1), 0.0);

	// change the observations to have equal height for all points. Then the precision of the vertical sag should be recognizble
	Eigen::Vector3d leftObs(0.007, -1, 0.0);
	Eigen::Vector3d middleObs(0.003, 0, 0.0);
	Eigen::Vector3d rightObs(0.007, 1, 0.0);
	apiObject.updateMeas("leftObs", leftObs);
	apiObject.updateMeas("middleObs", middleObs);
	apiObject.updateMeas("rightObs", rightObs);
	apiObject.adjust();

	sagParEst = apiObject.getSagEstimate("sag1");
	ensure_equals("vertical sag should be estimated 0", 0.0, sagParEst(0));
	Eigen::VectorXd sagParEstPrec = apiObject.getSagEstimatePrec("sag1");
	// as the sag paramater is determined by 3 z observatuions each with precision 0.1234mm, its total precision shoudl be 0.000123m / sqrt(3)
	ensure_equals("vertical sag precision should be 0.001234m/sqrt 3", 0.0001234 / sqrt(3), sagParEstPrec(0));

	// now test setFixed method
	Eigen::Vector3d P1Est = apiObject.getPointEstimate("P1");
	Eigen::Vector3d P2Est = apiObject.getPointEstimate("P2");
	Eigen::Vector3d P1EstExpected(0, 0, 0);
	Eigen::Vector3d P2EstExpected(0, 1, 0);
	ensure("point should be estimated correctly", P1Est.isApprox(P1EstExpected));
	ensure("point should be estimated correctly", P2Est.isApprox(P2EstExpected));

	// change the sag element
	apiObject.setFixedSagParameter("sag2", 0, 1.0);
	apiObject.setFixedSagParameter("sag2", 1, 2.0);
	apiObject.setFixedSagParameter("sag2", 2, 3.0);
	apiObject.setFixedSagParameter("sag2", 3, 4.0);
	apiObject.adjust();
	P1Est = apiObject.getPointEstimate("P1");
	P2Est = apiObject.getPointEstimate("P2");
	P1EstExpected << 3, 0, 1;
	P2EstExpected << 7, 1, 3;
	ensure("point should be estimated correctly", P1Est.isApprox(P1EstExpected));
	ensure("point should be estimated correctly", P2Est.isApprox(P2EstExpected));
}
template<>
template<>
void object::test<9>()
{
	set_test_name("Testing simple string example with sag");
	Moni apiObject("test_files/vivienString.lgc");

	SimpleTimer compTimer;
	compTimer.start();
	for (int j = 0; j < 10; j++)
	{
		bool success = apiObject.adjust();
	ensure("StringTest need to be computable", success);
		compTimer.step();
	}
	std::cout << "comp times stringtest" << std::endl;
	compTimer.printSteps();
}
template<>
template<>
void object::test<10>()
{
	set_test_name("Testing transformation methods for user specified vectors");
	Moni apiObject("test_files/vivienString.lgc");
	std::string from("RSTR_STRING.B1M.LQXFE.1SF");
	std::string to("RSTR_STRING.B1M.LQXFG.2SF");
	Eigen::Vector3d testCoord(1, 2, 3);
	Eigen::Vector3d testCoordInFinalFrame = apiObject.transformCoordinates(testCoord, from, to);
	Eigen::Vector3d testCoordBackInOriginalFrame = apiObject.transformCoordinates(testCoordInFinalFrame, to, from);
	ensure("forward/backward transformation should leave coordinates invariant.", (testCoord - testCoordBackInOriginalFrame).norm() < 1e-8);
	Eigen::Vector3d testDir(1, 2, 3);
	Eigen::Vector3d testDirInFinalFrame = apiObject.transformDirection(testDir, from, to);
	Eigen::VectorXd testDirBackInOriginalFrame = apiObject.transformDirection(testDirInFinalFrame, to, from);
	ensure("forward/backward transformation should leave directions invariant.", (testDir - testDirBackInOriginalFrame).norm() < 1e-8);
}
template<>
template<>
void object::test<11>()
{
	set_test_name("Testing reset method after computation failure");
	Moni apiObject("test_files/vivienString.lgc");

	Eigen::VectorXd blunderMeasurement(2);
	blunderMeasurement << 10, 10;
	apiObject.updateMeas("LQXFE.1SF.Q_WPS", blunderMeasurement);
	try
	{
		bool success = apiObject.adjust();
		fail("Computation should have failed due to blunder");
	}
	catch (const std::runtime_error &ex)
	{
		// try to reset the computation
		apiObject.reset();
	}
	bool success = apiObject.adjust();
	double sigma0 = apiObject.getSigma0();
	bool isNormalSigma = (sigma0 < 2) && (sigma0 > 0.5);
	ensure("Computation should have recovered", success);
	ensure("Computation should have normal sigma0", isNormalSigma);

}
template<>
template<>
void object::test<12>()
{
	set_test_name("Testing disabling wire sensor");
	Moni apiObject("test_files/vivienString.lgc");
	bool success = apiObject.adjust();
	std::string wireId1 = "LQXFE.1SF.Q_WPS";
	ensure("Residual of active wire sensor should not be 0.", !(apiObject.getEstimateResidual(wireId1).norm() < 1e-12));
	ensure_equals("observation should be active", apiObject.getActivationStatus(wireId1), true);
	apiObject.setActivationStatus(wireId1, false);
	ensure_equals("observation should be inactive", apiObject.getActivationStatus(wireId1), false);
	success = apiObject.adjust();
	ensure("Residual of deactivated wire sensor should be 0.", apiObject.getEstimateResidual(wireId1).norm() < 1e-12);
}
template<>
template<>
void object::test<13>()
{
	set_test_name("Testing reset method repeatedly");
	Moni apiObjectStress("test_files/vivienString.lgc");

	// stress test with a lot of resets and big file
	for (int j = 0; j < 100; j++)
	{
		ensure("Results should not yet be ready", !apiObjectStress.getStatus());
		bool success = apiObjectStress.adjust();
		ensure("Results should be ready", apiObjectStress.getStatus());
		apiObjectStress.reset();
	}
}
}; // namespace tut
