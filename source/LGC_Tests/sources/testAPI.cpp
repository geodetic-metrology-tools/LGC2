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
	ensure("After testObs2 reactivation, P2 needs to be estimated again at 0,0,0", estP2.isApprox(expectedP2));

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
	ensure_equals("tx should be frozen to value of 5. ", estRes[0], 5.0);

	// unfreeze the parameter
	apiObject.unfreezeFrameParameter("testFrame", 0);
	apiObject.adjust();
	estRes = apiObject.getFrameEstimate("testFrame");
	ensure_equals("tx should be back at 1. ", estRes[0], 1.0);

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
	ensure_equals("tx should be frozen to value of 5. ", estRes[0], 5.0);

	// extract precision of frozen parameter
	Eigen::VectorXd frozenPrecision = apiObject.getPointEstimatePrec("P2");
	ensure_equals("x coordinate is frozen so precision should be zero. ", frozenPrecision[0], 0.0);
	ensure("y coordinate is not frozen and there should be a nonzero-precision assigned", (fabs(frozenPrecision[1]) != 0.0));
	ensure("z coordinate is not frozen and there should be a nonzero-precision assigned", (fabs(frozenPrecision[2]) != 0.0));

	// unfreeze the parameter
	apiObject.unfreezePointParameter("P2", 0);
	apiObject.adjust();
	estRes = apiObject.getPointEstimate("P2");
	ensure_equals("x coordinate should be back at 0. ", estRes[0], 0.0);

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
		originalMeasurements.insert({id, mockup.getMeas(id)[0]});
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
	set_test_name("Testing a realistic application scenario for FRAS");
	Moni apiObject("test_files/Updated_LGC_Vincent.lgc");
	apiObject.adjust();

	// create a map from variable name to double (some variable names like "GTAP.A1LX.RST.TX:MM" can't be used directly because : is used by c++)
	std::map<std::string, double> values;

	// W.r.t. NXCALS Variables names available in the last version furnished by Vincent on 12/12/2023:
	// get the FUV/S02
	// call:
	double s = apiObject.getSigma0();
	// returns a double to log into the corresponding NXCALS variable

	// Let's assume the position GTAP.A1LX
	// position transformation info tab:
	// call:
	// the string should always be "RSTRI_" + "Accelerator" "." position
	Eigen::VectorXd pos = apiObject.getFrameEstimate("RSTRI_SCT.GTAP.A1LX");
	// Will return the an Eigen::VectorXd containing 7 parameters : TX, TY, TZ, RX, RY, RZ, Scale
	// The translation are always returned in meter. the NXCALS variable is in mm
	values["GTAP.A1LX.RST.TX:MM"] = pos(0) / 1000;
	values["GTAP.A1LX.RST.TY:MM"] = pos(1) / 1000;
	values["GTAP.A1LX.RST.TZ:MM"] = pos(2) / 1000;
	// The translation are always returned in radians. the NXCALS variable is in mrad
	values["GTAP.A1LX.RST.RX:MRAD"] = pos(3) / 1000;
	values["GTAP.A1LX.RST.RZ:MRAD"] = pos(5) / 1000;
	// Scale factor is unitless
	values["GTAP.A1LX.RST.SCALE:PPM"] = pos(6);

	// Watchout for the roll angle (GTAP.A1LX.RST.RY_MRAD), something specific need to be done due to GEODE integration
	// the string should always be "RSTR_" + "Accelerator" "." position
	Eigen::VectorXd posRoll = apiObject.getFrameEstimate("RSTR_SCT.GTAP.A1LX");
	values["GTAP.A1LX.RST.RY:mRAD"] = posRoll(4) / 1000;

	// position transformation ACCURACY tab:
	// call:
	// the string should always be "RSTRI_" +  accelerator_name + "." position
	Eigen::VectorXd posAcc = apiObject.getFrameEstimatePrec("RSTRI_SCT.GTAP.A1LX");
	// Will return the an Eigen::VectorXd containing 7 parameters precision along: TX, TY, TZ, RX, RY, RZ, Scale
	// The translation are always returned in meter. the NXCALS variable is in mm
	values["GTAP.A1LX.RST.TX:ACCURACY_MM"] = posAcc(0) / 1000;
	values["GTAP.A1LX.RST.TY:ACCURACY_MM"] = posAcc(2) / 1000;
	// The translation are always returned in radians. the NXCALS variable is in mrad
	values["GTAP.A1LX.RST.RX:ACCURACY_MRAD"] = posAcc(3) / 1000;
	values["GTAP.A1LX.RST.RZ:ACCURACY_MRAD"] = posAcc(5) / 1000;
	// Scale factor is unitless
	values["GTAP.A1LX.RST.Scale:ACCURACY_PPM"] = posAcc(6);

	// Watchout for the roll angle (GTAP.A1LX.RST.RY_mRAD), something specific need to be done
	// the string should always be "RSTR_" + accelerator_name + "." position
	Eigen::VectorXd posAccRoll = apiObject.getFrameEstimatePrec("RSTR_SCT.GTAP.A1LX");
	values["GTAP.A1LX.RST.RY:ACCURACY_MRAD"] = posAccRoll(4) / 1000;

	// BeamPoint coordinate offsets Info tab:
	// For the entry point call:
	// the string for the point name should always be "BEAM_" +  accelerator_name + "." position + ".E"
	// the string for the frame name should always be "RSTB_" +  accelerator_name + "." position
	Eigen::VectorXd entry = apiObject.getPointEstimate("BEAM_SCT.GTAP.A1LX.E", "RSTB_SCT.GTAP.A1LX") - apiObject.getPointEstimate("BEAM_SCT.GTAP.A1LX.E");
	// or alternatively this works too:
	// entry = apiObject.getPointEstimate("BEAM_SCT.GTAP.A1LX.E", "RSTB_SCT.GTAP.A1LX") - apiObject.getPointEstimate("BEAM_SCT.GTAP.A1LX.E", "RSTR_SCT.GTAP.A1LX");
	// Will return the an Eigen::VectorXd containing 3 parameters precision along: X, Y, Z
	values["GTAP.A1LX.E:X_MM"] = entry(0) / 1000;
	values["GTAP.A1LX.E:Y_MM"] = entry(1) / 1000;
	values["GTAP.A1LX.E:Z_MM"] = entry(2) / 1000;
	// The dimensions are always returned in meters. the NXCALS variable is in mm

	// for the exit point ("sortie") do the same by calling:
	// the string for the point name should always be "BEAM_" +  accelerator_name + "." position + ".S"
	Eigen::VectorXd sortie = apiObject.getPointEstimate("BEAM_SCT.GTAP.A1LX.S", "RSTB_SCT.GTAP.A1LX") - apiObject.getPointEstimate("BEAM_SCT.GTAP.A1LX.S");
	values["GTAP.A1LX.S:X_MM"] = sortie(0) / 1000;
	values["GTAP.A1LX.S:Y_MM"] = sortie(1) / 1000;
	values["GTAP.A1LX.S:Z_MM"] = sortie(2) / 1000;

	// for motor points:
	// same than beam points with the associated special naming (defined?), if in geode would be something along the lines accelerator_name + "." position + ".MOTOR1" then call
	//	Eigen::VectorXd motor1 = apiObject.getPointEstimate("SCT.GTAP.A1LX.MOTOR1", "RSTB_SCT.GTAP.A1LX") - apiObject.getPointEstimate("SCT.GTAP.A1LX.MOTOR1");

	// BeamPoint coordinate accuracy Info tab:
	// For the entry point call:
	// the string for the point name should always be "BEAM_" +  accelerator_name + "." position + ".E"
	// the string for the frame name should always be "RSTB_" +  accelerator_name + "." position
	Eigen::VectorXd entryAcc = apiObject.getPointEstimatePrec("BEAM_SCT.GTAP.A1LX.E", "RSTB_SCT.GTAP.A1LX");
	// Will return the an Eigen::VectorXd containing 3 parameters precision along: X, Y, Z
	values["GTAP.A1LX.E:X_ACCURACY_MM"] = entryAcc(0) / 1000;
	values["GTAP.A1LX.E:Y_ACCURACY_MM"] = entryAcc(1) / 1000;
	values["GTAP.A1LX.E:Z_ACCURACY_MM"] = entryAcc(2) / 1000;
	// The dimensions are always returned in meters. the NXCALS variable is in mm

	// for the exit point ("sortie") do the same by calling:
	// the string for the point name should always be "BEAM_" +  accelerator_name + "." position + ".S"
	Eigen::VectorXd sortieAcc = apiObject.getPointEstimatePrec("BEAM_SCT.GTAP.A1LX.S", "RSTB_SCT.GTAP.A1LX");
	values["GTAP.A1LX.S:X_ACCURACY_MM"] = sortieAcc(0) / 1000;
	values["GTAP.A1LX.S:Y_ACCURACY_MM"] = sortieAcc(1) / 1000;
	values["GTAP.A1LX.S:Z_ACCURACY_MM"] = sortieAcc(2) / 1000;

	// same for motor points if needed

	// get the WPS/HLS network parameters:
	// for WPS call:
	// The name of the WPS network is given as a functional position in InforEAM.
	wireRom WPSNetwork = apiObject.getECWIData("GIWPN.A1LX.T");
	// returns a custom struct wireRom see declaration in the header file.
	// then w.r.t NXCALS variables:
	// For the info tab:
	// again all the lengths and angles are given in m and radians
	values["GIWPN.A1LX.T:DELTA_X:MM"] = WPSNetwork.estimate(0) / 1000;
	values["GIWPN.A1LX.T:DELTA_Z:MM"] = WPSNetwork.estimate(1) / 1000;
	values["GIWPN.A1LX.T:DELTA_GISEMENT_MRAD"] = WPSNetwork.estimate(2) / 1000;
	values["GIWPN.A1LX.T:DELTA_PENTE_MRAD"] = WPSNetwork.estimate(3) / 1000;
	values["GIWPN.A1LX.T:SAG:MM"] = WPSNetwork.estimate(4) / 1000;
	// For the accuracy tab:
	// again all the lengths and angles are given in m and radians
	values["GIWPN.A1LX.T:DELTA_X_ACCURACY:MM"] = WPSNetwork.prec(0) / 1000;
	values["GIWPN.A1LX.T:DELTA_Z_ACCURACY:MM"] = WPSNetwork.prec(1) / 1000;
	values["GIWPN.A1LX.T:DELTA_GISEMENT_ACCURACY_MRAD"] = WPSNetwork.prec(2) / 1000;
	values["GIWPN.A1LX.T:DELTA_PENTE_ACCURACY_MRAD"] = WPSNetwork.prec(3) / 1000;
	values["GIWPN.A1LX.T:SAG_ACCURACY:MM"] = WPSNetwork.prec(4) / 1000;

	// for HLS call:
	// The name of the HLS network is given as a functional position in InforEAM.
	waterRom HLSNetwork = apiObject.getECWSData("GIHLN.A1LX.A");
	// returns a custom struct waterRom see declaration in the header file.
	// then w.r.t NXCALS variables:
	// For the info tab:
	// again all the lengths are given in m
	values["GIHLN.A1LX.A:HWATER_MM"] = HLSNetwork.estimate / 1000;
	// For the accuracy tab:
	// again all the lengths and angles are given in m and radians
	values["GIHLN.A1LX.A:HWATER_ACCURACY_MM"] = HLSNetwork.prec / 1000;

	// for residuals
	// for HLS:
	// let's take the HLS on functional positon SCT.GISC.A1LX.A, the corresponding observation ID is "SCT.GISC.A1LX.A_HLS"
	// call:
	Eigen::VectorXd HLSRes = apiObject.getEstimateResidual("SCT.GISC.A1LX.A_HLS");
	// returns a Eigen::VectorXd containing 1 parameter
	values["GTAP.A1LX.A_HLS:RESIDUAL_MM"] = HLSRes(0) / 1000;
	// given in meters, to transform to mm

	// for WPS:
	// let's take the WPS on functional positon SCT.GISC.A1LX.A, the corresponding observation ID is "SCT.GISC.A1LX.A_WPS"
	// call:
	Eigen::VectorXd WPSRes = apiObject.getEstimateResidual("SCT.GISC.A1LX.A_WPS");
	// returns a Eigen::VectorXd containing 2 parameters
	values["GTAP.A1LX.A_WPS:X_RESIDUAL_MM"] = WPSRes(0) / 1000;
	values["GTAP.A1LX.A_WPS:Z_RESIDUAL_MM"] = WPSRes(1) / 1000;
	// given in meters, to transform to mm

	// to update an observation:
	// for HLS:
	// let's take the HLS on functional positon SCT.GISC.A1LX.A, the corresponding observation ID is "SCT.GISC.A1LX.A_HLS"
	// update the value:
	// create a  Eigen::VectorXd of 1 element with the value of the observation
	Eigen::VectorXd hlsObs = Eigen::VectorXd::Zero(1);
	hlsObs(0) = 1;
	// update the measurement
	apiObject.updateMeas("SCT.GISC.A1LX.A_HLS", hlsObs);

	// update the state of this observation :
	//  call to turn off;
	apiObject.setActivationStatus("SCT.GISC.A1LX.A_HLS", false);
	apiObject.adjust();
	double sigmaAfterDeactivation =apiObject.getSigma0();
	// call to turn on;
	apiObject.setActivationStatus("SCT.GISC.A1LX.A_HLS", true);
	double sigmaAfterReactivation =apiObject.getSigma0();

	// for WPS:
	// let's take the WPS on functional positon SCT.GISC.A1LX.A, the corresponding observation ID is "SCT.GISC.A1LX.A_WPS"
	// update the value:
	// create a  Eigen::VectorXd of 2 elements with the value of the observations in X and Z
	Eigen::VectorXd wpsObs = Eigen::VectorXd::Zero(2);
	wpsObs(0) = 1;
	wpsObs(1) = 2;
	// update the measurement
	apiObject.updateMeas("SCT.GISC.A1LX.A_WPS", wpsObs);

	// update the state of this observation :
	//  call to turn off;
	apiObject.setActivationStatus("SCT.GISC.A1LX.A_WPS", false);
	// call to turn on;
	apiObject.setActivationStatus("SCT.GISC.A1LX.A_WPS", true);

	// to freeze/unfreeze the a translation parameters.
	// by defaults a parameters can only be freezed if in the full configuration it is a DOF
	// the following NXCALS are available
	// GTAP.A1LX:RST_TX_USE
	// GTAP.A1LX:RST_TY_USE
	// GTAP.A1LX:RST_TZ_USE
	// GTAP.A1LX:RST_RX_USE
	// GTAP.A1LX:RST_RY_USE
	// GTAP.A1LX:RST_RZ_USE
	// GTAP.A1LX:RST_SCALE_USE
	// let's take the GTAP.A1LX:RST_TX_USE and it's a boolean

	// to freeze a parameter a value should be specified, here let's take 10m)
	apiObject.freezeFrameParameter("RSTRI_SCT.GTAP.A1LX", 0, 10);

	// to unfreeze a parameter
	apiObject.unfreezeFrameParameter("RSTRI_SCT.GTAP.A1LX", 0);

	/* the T* are in meter, the R* are in gon, the Scale is unitless
   here is the index map
   GTAP.A1LX:RST_TX_USE		-->	1
   GTAP.A1LX:RST_TY_USE		-->	2
   GTAP.A1LX:RST_TZ_USE		-->	3
   GTAP.A1LX:RST_RX_USE		-->	4
   GTAP.A1LX:RST_RY_USE		-->	5
   GTAP.A1LX:RST_RZ_USE		-->	6
   GTAP.A1LX:RST_SCALE_USE	-->	7
   */

	/*
   For the newer below variables:
   GISCD.A1LX.RST_TX:MM
   GISCD.A1LX.RST_TY:MM
   GISCD.A1LX.RST_TZ:MM
   GISCD.A1LX.RST_RX:MRAD
   GISCD.A1LX.RST_RY:MRAD
   GISCD.A1LX.RST_RZ:MRAD
   GISCD.A1LX.RST_SCALE:PPM

   GISCD.A1LX.RST_TX:ACCURACY_MM
   GISCD.A1LX.RST_TY:ACCURACY_MM
   GISCD.A1LX.RST_TZ:ACCURACY_MM
   GISCD.A1LX.RST_RX:ACCURACY_MRAD
   GISCD.A1LX.RST_RY:ACCURACY_MRAD
   GISCD.A1LX.RST_RZ:ACCURACY_MRAD
   GISCD.A1LX.RST_SCALE:ACCURACY_PPM
   Nothing is available in the DBs yet.
   Methods in LGC will be available quickly for the transforamtion parameters.
   For the accuracy, needed? Will take a bit more time as not that straightforward
   */
}
}; // namespace tut
