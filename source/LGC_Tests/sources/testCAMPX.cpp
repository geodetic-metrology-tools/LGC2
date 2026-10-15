// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <Eigen/Dense>

#include <Behavior.h>
#include <TInstrumentData.h>
#include <TInverseTransformation.h>
#include <TLGCApp.h>
#include <TLGCData.h>
#include <TLSEvaluator.h>
#include <TReader.h>

#include "TLGCCalculation.h"
#include "testCAMPX.h"

using namespace TestCAMPX;

namespace
{
typedef TInstrumentData::TCAMD CamInstr;

/// Interior orientation the observations of the network are generated with: C, CX, CY [px], K1, K2, K3, P1, P2 [-], B1, B2 [px]
const double ioTruth[CamInstr::kNumIOParams] = {7246.4, 2048.0, 1900.0, -0.12, 0.15, -0.05, 3.0e-4, -2.0e-4, 0.8, -0.3};
/// Tolerances of the interior orientation estimated from error-free observations, each below about 1e-5 px in the image
const double ioTolerance[CamInstr::kNumIOParams] = {1e-5, 1e-5, 1e-5, 1e-7, 1e-6, 1e-5, 1e-9, 1e-9, 1e-5, 1e-5};
/// Sigma of the image measurement of a target of the network [px]
const double targetSigma = 0.02;

/// A camera station: the camera sits at the origin of its own frame and looks along the z axis of that frame
struct Station
{
	const char *name;
	double pose[6]; // frame parameters tx, ty, tz [m], rx, ry, rz [gon]
};

/// Four convergent stations looking at the target field, S3 and S4 are rolled about the optical axis
const int numStations = 4;
const Station stations[numStations] = {{"S1", {0.0, 0.0, 0.0, 100.0, 0.0, 0.0}}, {"S2", {1.5, 0.3, 0.2, 103.3, 23.3, 1.2}}, {"S3", {-1.5, 0.2, -0.2, 96.8, -22.8, -98.9}},
	{"S4", {0.3, -0.4, 0.9, 112.3, 4.1, -199.2}}};

struct Target
{
	std::string name;
	Eigen::Vector3d pos; // in ROOT [m]
	bool isControl;
};

/// 5 x 4 targets about 4 m in front of the stations and staggered in depth, every third one is a control point
std::vector<Target> targetField(bool planar)
{
	std::vector<Target> targets;
	for (int ix = 0; ix < 5; ix++)
	{
		for (int iz = 0; iz < 4; iz++)
		{
			const int n = ix * 4 + iz + 1;
			const double depth = planar ? 0.0 : 0.6 * ((ix + 2 * iz) % 3 - 1);
			targets.push_back({(n < 10 ? "T0" : "T") + std::to_string(n), Eigen::Vector3d(-0.9 + 0.45 * ix, 4.2 + depth, -0.6 + 0.4 * iz), n % 3 == 1});
		}
	}
	return targets;
}

/// Pixel coordinates of a point of ROOT, seen by a camera with the given interior orientation in a frame below ROOT
Eigen::Vector2d project(const double io[], const double pose[6], const Eigen::Vector3d &pointInRoot)
{
	// the point in the camera frame, transformed by LGC from the parent frame into the frame
	const TransformParameters frame(
		TAngle(pose[3], TAngle::kGons), TAngle(pose[4], TAngle::kGons), TAngle(pose[5], TAngle::kGons), TLength(pose[0]), TLength(pose[1]), TLength(pose[2]), 1.0);
	const TTransformation &parentToFrame = TInverseTransformation(frame);
	TPositionVector point{TVector(pointInRoot)};
	parentToFrame.transform(point);
	const Eigen::Vector3d p = point.toRealVector();

	// central projection, distortion and pixel mapping, implemented independently of the LGC observation model
	const double xn = p(0) / p(2), yn = p(1) / p(2);
	const double r2 = xn * xn + yn * yn;
	const double radial = 1.0 + io[CamInstr::kK1] * r2 + io[CamInstr::kK2] * r2 * r2 + io[CamInstr::kK3] * r2 * r2 * r2;
	const double xd = xn * radial + 2.0 * io[CamInstr::kP1] * xn * yn + io[CamInstr::kP2] * (r2 + 2.0 * xn * xn);
	const double yd = yn * radial + io[CamInstr::kP1] * (r2 + 2.0 * yn * yn) + 2.0 * io[CamInstr::kP2] * xn * yn;
	return Eigen::Vector2d(io[CamInstr::kCX] + (io[CamInstr::kC] + io[CamInstr::kB1]) * xd + io[CamInstr::kB2] * yd, io[CamInstr::kCY] + io[CamInstr::kC] * yd);
}

/// What differs between the input files of the tests. By default all the provisional values are the true ones.
struct NetworkSetup
{
	std::string options; // option keywords, e.g. "*SIMU 1 3\n"
	std::string sensor; // sensor option of the camera, e.g. " SENSOR 4096 3600"
	std::vector<double> ioStart = std::vector<double>(ioTruth, ioTruth + CamInstr::kNumIOParams); // provisional interior orientation
	std::vector<bool> ioFree = std::vector<bool>(CamInstr::kNumIOParams, true);
	std::vector<double> ioSigma = std::vector<double>(CamInstr::kNumIOParams, 0.0); // a priori sigmas, 0 for none
	std::vector<double> ioGenerate = std::vector<double>(ioTruth, ioTruth + CamInstr::kNumIOParams); // interior orientation the observations are generated with
	std::vector<double> poseOffset = std::vector<double>(6, 0.0); // added to the true frame parameters of the stations [m, gon]
	double pointOffset = 0.0; // size of the offsets added to the true coordinates of the free points [m]
	int numStationsUsed = numStations;
	bool planarControlField = false; // all the targets are control points in a plane perpendicular to the optical axis of S1
	bool withObservations = true; // FALSE: only the observed points are given, which is allowed for simulations
	bool stationPointsInRoot = false; // TRUE: the station points are free points of ROOT and the camera frames have only their rotations free
};

/// The provisional values displaced from the truth: the stations by 2 cm and 0.5 gon, the free points by up to 1 cm
NetworkSetup displacedSetup()
{
	NetworkSetup setup;
	setup.poseOffset = {0.02, -0.02, 0.02, 0.5, -0.5, 0.5};
	setup.pointOffset = 0.01;
	return setup;
}

/// Input file of the network, with the observations generated from setup.ioGenerate, by default the true interior orientation
std::string networkInput(const NetworkSetup &setup)
{
	const std::vector<Target> targets = targetField(setup.planarControlField);

	std::ostringstream in;
	in << std::setprecision(15);
	in << "*TITR\nPixel camera network\n*OLOC\n*PREC 7\n" << setup.options;
	in << "*INSTR\n*CAMPX CAMA TG" << setup.sensor << "\nTG " << targetSigma << " " << targetSigma << "\n*IO\n";
	for (int i = 0; i < CamInstr::kNumIOParams; i++)
	{
		in << CamInstr::ioParamNames[i] << " " << setup.ioStart[i] << (setup.ioFree[i] ? " FREE" : "");
		if (setup.ioSigma[i] > 0.0)
			in << " SE " << setup.ioSigma[i];
		in << "\n";
	}

	in << "*CALA\n";
	for (const auto &target : targets)
		if (target.isControl || setup.planarControlField)
			in << target.name << " " << target.pos(0) << " " << target.pos(1) << " " << target.pos(2) << "\n";

	// the free targets and the free station points
	in << "*POIN\n";
	int i = 0;
	for (const auto &target : targets)
	{
		if (target.isControl || setup.planarControlField)
			continue;
		i++;
		in << target.name << " " << target.pos(0) + setup.pointOffset * sin(1.7 * i) << " " << target.pos(1) + setup.pointOffset * cos(2.3 * i) << " "
		   << target.pos(2) + setup.pointOffset * sin(0.9 * i + 1.0) << "\n";
	}
	if (setup.stationPointsInRoot)
	{
		for (int s = 0; s < setup.numStationsUsed; s++)
		{
			in << "C" << stations[s].name;
			for (int k = 0; k < 3; k++)
				in << " " << stations[s].pose[k] + setup.poseOffset[k];
			in << "\n";
		}
	}

	for (int s = 0; s < setup.numStationsUsed; s++)
	{
		const Station &station = stations[s];
		in << "*FRAME " << station.name;
		if (setup.stationPointsInRoot)
		{
			// the translation of the camera frame would move the station point and the targets alike: it is not estimable
			in << " 0 0 0";
			for (int k = 3; k < 6; k++)
				in << " " << station.pose[k] + setup.poseOffset[k];
			in << " 1 RX RY RZ\n";
		}
		else
		{
			for (int k = 0; k < 6; k++)
				in << " " << station.pose[k] + setup.poseOffset[k];
			in << " 1 TX TY TZ RX RY RZ\n";
			in << "*CALA\nC" << station.name << " 0 0 0\n";
		}
		in << "*CAM C" << station.name << " CAMA\n*UV\n";
		for (const auto &target : targets)
		{
			in << target.name;
			if (setup.withObservations)
			{
				const Eigen::Vector2d uv = project(setup.ioGenerate.data(), station.pose, target.pos);
				in << " " << uv(0) << " " << uv(1);
			}
			in << "\n";
		}
		in << "*ENDFRAME\n";
	}
	in << "*END\n";
	return in.str();
}

/// Returns the station of the test network by the name of its frame
const Station &stationOfFrame(const std::string &frameName)
{
	for (const auto &station : stations)
		if (frameName == station.name)
			return station;
	throw std::logic_error("Unknown station " + frameName);
}

/// Location of a file written by the tests
std::string workFile(const std::string &name)
{
	return (std::filesystem::temp_directory_path() / name).string();
}

std::string fileText(const std::string &path)
{
	std::ifstream file(path);
	std::stringstream content;
	content << file.rdbuf();
	return content.str();
}

bool fileContains(const std::string &path, const std::string &text)
{
	return fileText(path).find(text) != std::string::npos;
}
} // namespace

namespace tut
{
struct test_CAMPX
{
	test_CAMPX() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

	std::shared_ptr<TLGCData> projTest;
	TReader r;

	/// Reads the input into projTest
	bool read(const std::string &input)
	{
		projTest->getFileLogger().writeReportHeader("LGC output file");
		std::stringstream infiler(input);
		return r.read(infiler);
	}

	/// Reads and computes the input, which is also written into <name>.lgc2, with the log file <name>.log; a simulation writes
	/// its summary into <name>.txt. Returns the data.
	std::shared_ptr<TLGCData> compute(const std::string &input, const std::string &name, Behavior::BehaviorCode expected = Behavior::BehaviorCode::ERR_noError)
	{
		std::ofstream(workFile(name + ".lgc2")) << input;
		auto data = std::make_shared<TLGCData>();
		data->getFileLogger().setOutputfileLocation(workFile(name + ".log"));
		data->getFileLogger().writeReportHeader("LGC output file");
		TReader reader(data);
		std::stringstream infiler(input);
		ensure_equals("Reading " + name, reader.read(infiler), true);

		std::unique_ptr<TLGCApp> app;
		std::shared_ptr<TAStreamFormatter> stream;
		std::shared_ptr<TSimulationOutputFileWriter> simulationWriter;
		if (data->getConfig().sim.isActive())
		{
			app = std::make_unique<TLGCApp>("dummy_string", workFile(name + ".txt"), 80);
			app->initializeStream(data, workFile(name + ".txt"), stream);
			simulationWriter = std::make_shared<TSimulationOutputFileWriter>(stream.get(), data.get());
		}
		TLGCCalculation calcul(data);
		ensure_equals("Calculation of " + name, calcul.computeResults(simulationWriter).code(), expected);
		return data;
	}

	/// Writes the input into <name>.lgc2 and runs the application on it, which writes <name>.res and the other output files
	void runApp(const std::string &input, const std::string &name)
	{
		{
			std::ofstream file(workFile(name + ".lgc2"));
			file << input;
			ensure("Input file " + name + " should be written", file.good());
		}
		TLGCApp app(workFile(name + ".lgc2"), workFile(name + ".res"));
		ensure_equals("Application run of " + name, app.exec().code(), Behavior::BehaviorCode::ERR_noError);
	}

	/// Adjustable scalar of an interior orientation parameter of the camera CAMA
	const TAdjustableScalar &ioParam(const TLGCData &data, int param) const
	{
		const std::string name = std::string("Cam_CAMA_") + CamInstr::ioParamNames[param];
		ensure("Interior orientation parameter " + name + " should exist", data.getScalars().doesObjectExist(name));
		return data.getScalars().getObject(name);
	}

	/// Compares the Jacobian of the observation models with finite differences, column by column: the columns range from
	/// pixels per pixel to thousands of pixels per metre, so each is compared relative to its largest element
	void ensureJacobianMatchesFiniteDifferences(const std::string &input, int expectedUnknowns)
	{
		std::stringstream infiler(input);
		TLSEvaluator evaluator(infiler);
		ensure_equals("Number of unknowns", evaluator.getIndices().UIndex, expectedUnknowns);

		evaluator.setParameters(evaluator.getEstParams());
		ensure_equals("evaluate() method should succeed.", evaluator.evaluate(), true);
		const Eigen::MatrixXd jacobian = evaluator.getAMatrix();
		const Eigen::MatrixXd finDiffJacobian = evaluator.getFiniteDifferenceA();
		for (int j = 0; j < jacobian.cols(); j++)
		{
			const double scale = jacobian.col(j).cwiseAbs().maxCoeff();
			ensure("Every unknown should contribute to the Jacobian, column " + std::to_string(j), scale > 0.0);
			const double difference = (jacobian.col(j) - finDiffJacobian.col(j)).cwiseAbs().maxCoeff();
			ensure("Finite difference Jacobian and symbolic Jacobian should be equal, column " + std::to_string(j), difference < 2e-5 * scale);
		}
	}

	/// Evaluates the input at its provisional values, the misclosures are then those of the given observations
	Eigen::VectorXd misclosureAtProvisionalValues(const std::string &input, bool expectSuccess = true)
	{
		std::stringstream infiler(input);
		TLSEvaluator evaluator(infiler);
		evaluator.setParameters(evaluator.getEstParams());
		ensure_equals("evaluate() method", evaluator.evaluate(), expectSuccess);
		return expectSuccess ? evaluator.getMisclosure() : Eigen::VectorXd();
	}
};
typedef test_group<test_CAMPX> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("CAMPX tests");
}

namespace tut
{

template<>
template<>
void object::test<1>()
{
	set_test_name("Testing CAMPX reader: instrument, interior orientation and UV measurements");

	ensure_equals("Reading file successful", read(CAMPX_READER), true);

	const auto &instruments = projTest->getInstruments().fCAMD;
	ensure_equals("One camera instrument", instruments.size(), size_t(1));
	const CamInstr &instrument = *instruments.at("CAMA");
	ensure_equals("Instrument is a pixel camera", instrument.isPixelCamera, true);
	ensure_equals("Default target", instrument.defTarget, std::string("TG1"));
	ensure_equals("Number of targets", instrument.targets.size(), size_t(2));
	ensure_equals("Sigma u of TG1 [px]", instrument.targets.at("TG1")->sigmaX, 0.02, 1e-15);
	ensure_equals("Sigma v of TG1 [px]", instrument.targets.at("TG1")->sigmaY, 0.03, 1e-15);
	ensure_equals("Sensor width [px]", instrument.sensorSize[0], 4096.0, 1e-15);
	ensure_equals("Sensor height [px]", instrument.sensorSize[1], 3000.0, 1e-15);

	const double expectedValue[CamInstr::kNumIOParams] = {7000.0, 2048.5, 1501.0, -0.1, 0.05, 0.0, 0.0, 0.0, 0.0, 0.0};
	const bool expectedFree[CamInstr::kNumIOParams] = {true, false, true, true, true, false, false, false, false, false};
	const double expectedSigma[CamInstr::kNumIOParams] = {0.0, 0.0, 2.5, 0.0, 0.01, 0.0, 0.0, 0.0, 0.0, 0.0};
	for (int i = 0; i < CamInstr::kNumIOParams; i++)
	{
		ensure_equals(std::string("Value of ") + CamInstr::ioParamNames[i], instrument.io[i].value, expectedValue[i], 1e-15);
		ensure_equals(std::string("Free state of ") + CamInstr::ioParamNames[i], instrument.io[i].free, expectedFree[i]);
		ensure_equals(std::string("A priori sigma of ") + CamInstr::ioParamNames[i], instrument.io[i].sigma, expectedSigma[i], 1e-15);
	}

	const auto &cameras = projTest->getTree().begin()->get()->measurements.fCAM;
	ensure_equals("One camera station", cameras.size(), size_t(1));
	const TCAM &camera = cameras.front();
	ensure_equals("Station copies the pixel camera", camera.instrument.isPixelCamera, true);
	ensure_equals("Number of UV measurements", camera.measUV.size(), size_t(2));

	const TUV &first = camera.measUV.front();
	ensure_equals("Observed point", first.targetPos->getName(), std::string("P1"));
	ensure_equals("Observed u [px]", first.getObsVector()(0), 2223.0, 1e-12);
	ensure_equals("Observed v [px]", first.getObsVector()(1), 1850.0, 1e-12);
	ensure_equals("Default target used", first.target.ID, std::string("TG1"));
	ensure_equals("Sigma u of the default target [px]", first.target.sigmaX, 0.02, 1e-15);

	const TUV &second = camera.measUV.back();
	ensure_equals("Target option", second.target.ID, std::string("TG2"));
	ensure_equals("Sigma u option [px]", second.target.sigmaX, 0.5, 1e-15);
	ensure_equals("Sigma v option [px]", second.target.sigmaY, 0.6, 1e-15);
	ensure_equals("Observation ID", second.obsID, std::string("OBS7"));
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing CAMPX reader errors");

	for (const auto &wrongInput : CAMPX_WRONG_INPUTS)
	{
		auto wrongData = std::make_shared<TLGCData>();
		TReader reader(wrongData);
		std::stringstream infiler(wrongInput.second);
		ensure_equals(wrongInput.first, reader.read(infiler), false);
	}

	// a correct pixel camera with a UV measurement is accepted
	ensure_equals("Correct pixel camera", read(CAMPX_CORRECT), true);

	// UV measurements outside the sensor are read with a warning, a measurement on its corner without
	for (const auto &[input, outside] : {std::make_pair(CAMPX_RIGHT_OF_SENSOR, true), std::make_pair(CAMPX_ABOVE_SENSOR, true), std::make_pair(CAMPX_SENSOR_CORNER, false)})
	{
		auto sensorData = std::make_shared<TLGCData>();
		sensorData->getFileLogger().setOutputfileLocation(workFile("CAMPX_sensor.log"));
		TReader sensorReader(sensorData);
		std::stringstream sensorInput(input);
		ensure_equals("Reading file successful", sensorReader.read(sensorInput), true);
		ensure_equals("Warning about a UV measurement outside the sensor", fileContains(workFile("CAMPX_sensor.log"), "lies outside the sensor"), outside);
	}
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing UV Jacobian against finite differences");

	// camera in a free frame: 10 interior orientation parameters, 6 parameters per station, 13 free points
	ensureJacobianMatchesFiniteDifferences(networkInput(displacedSetup()), 10 + 6 * numStations + 3 * 13);

	// station points free points of ROOT, outside the camera frames with only their rotations free: the derivatives go
	// through the frame chains of the target and of the station point, with 3 coordinates and 3 rotations per station
	NetworkSetup setup = displacedSetup();
	setup.stationPointsInRoot = true;
	ensureJacobianMatchesFiniteDifferences(networkInput(setup), 10 + 6 * numStations + 3 * 13);

	// camera on a free point of ROOT, targets in a free frame: 4 interior orientation parameters, 3 coordinates of the camera, 6 frame parameters, 2 free points
	ensureJacobianMatchesFiniteDifferences(CAMPX_CAMERA_IN_ROOT, 4 + 3 + 6 + 3 * 2);
}

template<>
template<>
void object::test<4>()
{
	set_test_name("Testing UV, full interior orientation estimated from generated observations");

	// start far from the truth: no distortion at all, stations and free points displaced; CONSI finds no singularity
	NetworkSetup setup = displacedSetup();
	setup.options = "*CONSI\n";
	setup.ioStart = {7200.0, 2040.0, 1910.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	const auto data = compute(networkInput(setup), "CAMPX_full");
	ensure_equals("Number of unknowns", data->fUEOIndices.UIndex, 10 + 6 * numStations + 3 * 13);
	ensure_equals("Number of observations", data->fUEOIndices.OIndex, 2 * 20 * numStations);

	// the interior orientation the observations were generated with is found
	for (int i = 0; i < CamInstr::kNumIOParams; i++)
		ensure_equals(std::string("Estimated ") + CamInstr::ioParamNames[i], ioParam(*data, i).getEstimatedValue(), ioTruth[i], ioTolerance[i]);

	// ... and so are the free points and the stations
	for (const auto &target : targetField(false))
	{
		if (target.isControl)
			continue;
		const TPositionVector estimated = data->getPoints().getObject(target.name).getEstimatedValue();
		ensure_equals(target.name + " x coordinate", estimated.getX().getMetresValue(), target.pos(0), 1e-8);
		ensure_equals(target.name + " y coordinate", estimated.getY().getMetresValue(), target.pos(1), 1e-8);
		ensure_equals(target.name + " z coordinate", estimated.getZ().getMetresValue(), target.pos(2), 1e-8);
	}
	for (auto frameIt = ++data->getTree().begin(); frameIt != data->getTree().end(); ++frameIt)
	{
		const auto &frame = frameIt.node->data->frame;
		const Station &station = stationOfFrame(frame.getName());
		for (int k = 0; k < 3; k++)
		{
			ensure_equals(frame.getName() + " translation", frame.getEstTranslation(k), station.pose[k], 1e-8);
			// the estimated angles are normalised to [0, 400) gon
			ensure_equals(frame.getName() + " rotation", std::remainder(frame.getEstRotation(k).getGonsValue() - station.pose[3 + k], 400.0), 0.0, 1e-7);
		}

		// the observations are reproduced
		for (const auto &uv : frameIt.node->data->measurements.fCAM.front().measUV)
		{
			ensure_equals("Residual of u", uv.getXCompVectorResidual(), 0.0, 1e-6);
			ensure_equals("Residual of v", uv.getYCompVectorResidual(), 0.0, 1e-6);
		}
	}
}

template<>
template<>
void object::test<5>()
{
	set_test_name("Testing UV simulation and the estimation from the simulated observations");

	// all the provisional values are the true ones, the observations are simulated with the sigma of the target; K3, barely
	// determined by this field of view, is fixed as one would do in practice
	NetworkSetup setup;
	setup.options = "*SIMU 1 7\n";
	setup.ioFree[CamInstr::kK3] = false;
	setup.withObservations = false;
	const auto data = compute(networkInput(setup), "CAMPX_simu");

	// the simulated observations scatter around the true pixel coordinates with the sigma of the target
	std::map<std::string, Eigen::Vector3d> truePos;
	for (const auto &target : targetField(false))
		truePos[target.name] = target.pos;
	double sumSquares = 0.0;
	int numValues = 0;
	for (auto frameIt = ++data->getTree().begin(); frameIt != data->getTree().end(); ++frameIt)
	{
		const Station &station = stationOfFrame(frameIt.node->data->frame.getName());
		for (const auto &uv : frameIt.node->data->measurements.fCAM.front().measUV)
		{
			const Eigen::Vector2d noise = uv.getObsVector() - project(ioTruth, station.pose, truePos.at(uv.targetPos->getName()));
			ensure("Simulated noise should stay below 10 sigma", noise.cwiseAbs().maxCoeff() < 10.0 * targetSigma);
			sumSquares += noise.squaredNorm();
			numValues += 2;
		}
	}
	ensure_equals("All the observations are simulated", numValues, 2 * 20 * numStations);

	// the RMS of n normal values scatters relatively by about 1 / sqrt(2 n), sigma zero likewise with the redundancy
	const double rms = sqrt(sumSquares / numValues);
	ensure("RMS of the simulated noise should be the sigma of the target within 5 times its scatter", fabs(rms / targetSigma - 1.0) < 5.0 / sqrt(2.0 * numValues));
	const int redundancy = data->fUEOIndices.OIndex - data->fUEOIndices.UIndex;
	ensure("Sigma zero a posteriori should be 1 within 5 times its scatter", fabs(data->getS0APosteriori() - 1.0) < 5.0 / sqrt(2.0 * redundancy));

	// the estimated interior orientation agrees with the true one within 5 times its estimated precision
	for (int i = 0; i < CamInstr::kNumIOParams; i++)
	{
		const TAdjustableScalar &param = ioParam(*data, i);
		if (param.isFixed())
			continue;
		ensure(std::string("Precision should be estimated for ") + CamInstr::ioParamNames[i], param.getEstimatedPrecision() > 0.0);
		ensure(std::string("Estimated ") + CamInstr::ioParamNames[i] + " should agree with the truth within 5 sigma",
			fabs(param.getEstimatedValue() - ioTruth[i]) < 5.0 * param.getEstimatedPrecision());
	}
}

template<>
template<>
void object::test<6>()
{
	set_test_name("Testing CAMPX application run: result file, fault file, simulated observation file and its re-computation");

	// a normal calculation writes the interior orientation and the UV observations with their residual summary into the
	// result file, and their reliability into the fault file
	NetworkSetup setup;
	setup.options = "*FAUT .01 .10\n";
	setup.sensor = " SENSOR 4096 3600";
	setup.ioStart[CamInstr::kC] = 7240.0;
	runApp(networkInput(setup), "CAMPX_app");
	ensure("Result file should contain the interior orientation", fileContains(workFile("CAMPX_app.res"), "INTERIOR ORIENTATION"));
	ensure("Result file should contain the sensor size", fileContains(workFile("CAMPX_app.res"), "4096 X 3600 PX"));
	ensure("Result file should contain the UV observations", fileContains(workFile("CAMPX_app.res"), "(UV)"));
	ensure("Result file should contain the summary of the u residuals", fileContains(workFile("CAMPX_app.res"), "UV: U"));
	ensure("Fault file should contain the UV observations", fileContains(workFile("CAMPX_app.err"), "UV observations"));

	// a simulation with SOBS writes an input file with the simulated observations and the simulated a priori value of C
	setup = NetworkSetup();
	setup.options = "*SIMU 1 11\n*SOBS\n";
	setup.sensor = " SENSOR 4096 3600";
	setup.ioFree[CamInstr::kK3] = false;
	setup.ioSigma[CamInstr::kC] = 2.0;
	setup.withObservations = false;
	runApp(networkInput(setup), "CAMPX_sobs");
	ensure("Simulation summary should contain the u residuals", fileContains(workFile("CAMPX_sobs.res"), "(UV): U"));

	// ... which can be read and computed again
	const auto again = compute(fileText(workFile("CAMPX_sobs.sim")), "CAMPX_sobs_again");
	const CamInstr &instrument = *again->getInstruments().fCAMD.at("CAMA");
	ensure_equals("Sensor width of the simulated file", instrument.sensorSize[0], 4096.0, 1e-12);
	ensure_equals("Sensor height of the simulated file", instrument.sensorSize[1], 3600.0, 1e-12);
	for (int i = 0; i < CamInstr::kNumIOParams; i++)
	{
		ensure_equals(std::string("Free state of ") + CamInstr::ioParamNames[i], instrument.io[i].free, setup.ioFree[i]);
		if (i != CamInstr::kC)
			ensure_equals(std::string("Provisional ") + CamInstr::ioParamNames[i], instrument.io[i].value, ioTruth[i], 1e-12 * std::max(1.0, fabs(ioTruth[i])));
		const TAdjustableScalar &param = ioParam(*again, i);
		if (!param.isFixed())
			ensure(std::string("Estimated ") + CamInstr::ioParamNames[i] + " should agree with the truth within 5 sigma",
				fabs(param.getEstimatedValue() - ioTruth[i]) < 5.0 * param.getEstimatedPrecision());
	}
	// the a priori value of C is simulated with its a priori sigma, which the file keeps
	ensure_equals("A priori sigma of C", instrument.io[CamInstr::kC].sigma, 2.0, 1e-12);
	const double priorOffset = instrument.io[CamInstr::kC].value - ioTruth[CamInstr::kC];
	ensure("A priori value of C should be simulated within 5 sigma", priorOffset != 0.0 && fabs(priorOffset) < 5.0 * 2.0);

	// the options of a UV measurement are kept in the simulated file: target, sigmas and identifier
	std::string reader = CAMPX_READER;
	reader.insert(reader.find("*INSTR"), "*SIMU 1 3\n*SOBS\n");
	runApp(reader, "CAMPX_options");
	const auto options = compute(fileText(workFile("CAMPX_options.sim")), "CAMPX_options_again");
	const TUV &second = options->getTree().begin()->get()->measurements.fCAM.front().measUV.back();
	ensure_equals("Target option", second.target.ID, std::string("TG2"));
	ensure_equals("Sigma u option [px]", second.target.sigmaX, 0.5, 1e-15);
	ensure_equals("Sigma v option [px]", second.target.sigmaY, 0.6, 1e-15);
	ensure_equals("Observation ID", second.obsID, std::string("OBS7"));
}

template<>
template<>
void object::test<7>()
{
	set_test_name("Testing CONSI with a pixel camera, and an a priori sigma that resolves the singular configuration");

	// one station without distortion in front of a plane of control points perpendicular to its optical axis: the camera
	// constant cannot be separated from the distance to the plane
	NetworkSetup setup;
	setup.options = "*CONSI\n*APRI\n";
	setup.numStationsUsed = 1;
	setup.planarControlField = true;
	setup.ioGenerate = {ioTruth[CamInstr::kC], ioTruth[CamInstr::kCX], ioTruth[CamInstr::kCY], 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	setup.ioStart = setup.ioGenerate;
	setup.ioFree = std::vector<bool>(CamInstr::kNumIOParams, false);
	setup.ioFree[CamInstr::kC] = true;
	compute(networkInput(setup), "CAMPX_singular", Behavior::BehaviorCode::ERR_inputData);

	// with an a priori sigma C is its a priori value, here 3 px from the truth; *APRI gives the precision from the a priori
	// model, sigma zero a posteriori of the error-free observations would be zero
	setup.ioStart[CamInstr::kC] += 3.0;
	setup.ioSigma[CamInstr::kC] = 1.5;
	const auto data = compute(networkInput(setup), "CAMPX_singular_prior");
	ensure_equals("Number of observations: the UV and the a priori value of C", data->fUEOIndices.OIndex, 2 * 20 + 1);
	const TAdjustableScalar &c = ioParam(*data, CamInstr::kC);
	ensure_equals("Camera constant should be its a priori value", c.getEstimatedValue(), setup.ioStart[CamInstr::kC], 1e-6);
	ensure_equals("Precision of the camera constant should be its a priori sigma", c.getEstimatedPrecision(), 1.5, 1e-6);
}

template<>
template<>
void object::test<8>()
{
	set_test_name("Testing an a priori sigma of the camera constant in a network that determines it");

	// a tight a priori sigma keeps a wrong a priori value, a loose one lets the observations decide
	for (const auto &[sigma, expected, tolerance] : {std::make_tuple(1e-4, ioTruth[CamInstr::kC] + 5.0, 1e-2), std::make_tuple(1e4, ioTruth[CamInstr::kC], 1e-5)})
	{
		NetworkSetup setup;
		setup.ioStart[CamInstr::kC] += 5.0;
		setup.ioSigma[CamInstr::kC] = sigma;
		const auto data = compute(networkInput(setup), "CAMPX_prior");
		ensure_equals("Estimated camera constant with an a priori sigma of " + std::to_string(sigma), ioParam(*data, CamInstr::kC).getEstimatedValue(), expected, tolerance);
	}
}

template<>
template<>
void object::test<9>()
{
	set_test_name("Testing the weights of the UV measurements and of the a priori values");

	std::stringstream infiler(CAMPX_READER);
	TLSEvaluator evaluator(infiler);
	evaluator.setParameters(evaluator.getEstParams());
	ensure_equals("evaluate() method should succeed.", evaluator.evaluate(), true);

	// u, v of P1 with the sigmas of TG1, u, v of P2 with USE and VSE, then the a priori values of CY and K2
	const double sigma[] = {0.02, 0.03, 0.5, 0.6, 2.5, 0.01};
	const Eigen::MatrixXd weights = evaluator.getPMatrix();
	ensure_equals("Number of observations", (int)weights.rows(), 6);
	for (int i = 0; i < 6; i++)
		ensure_equals("Weight of the observation " + std::to_string(i), weights(i, i) * sigma[i] * sigma[i], 1.0, 1e-12);
}

template<>
template<>
void object::test<10>()
{
	set_test_name("Testing UV simulation of a point imaged outside the sensor or behind the camera");

	// the principal point is at u = 2048 px, so a sensor of 2000 px width cuts off the right half of the target field
	NetworkSetup setup;
	setup.options = "*SIMU 1 7\n";
	setup.sensor = " SENSOR 2000 3600";
	setup.withObservations = false;
	compute(networkInput(setup), "CAMPX_outside", Behavior::BehaviorCode::ERR_LSCalculation);
	ensure("Simulation should report the point outside the sensor", fileContains(workFile("CAMPX_outside.log"), "is imaged outside the sensor"));

	// in a simulation the provisional geometry is the true one, in which no target may be behind a camera
	std::string behind = CAMPX_TARGET_BEHIND;
	behind.insert(behind.find("*INSTR"), "*SIMU 1 3\n");
	compute(behind, "CAMPX_behind", Behavior::BehaviorCode::ERR_LSCalculation);
	ensure("Simulation should report the point behind the camera", fileContains(workFile("CAMPX_behind.log"), "is behind the camera"));
}

template<>
template<>
void object::test<11>()
{
	set_test_name("Testing UV with the station point outside the camera frame");

	// the station points are free points of ROOT and the camera frames have only their rotations free: at the true values the
	// observations, generated with the station points as projection centres and the frame axes as camera axes, are reproduced
	NetworkSetup setup;
	setup.stationPointsInRoot = true;
	ensure("Observations should be reproduced at the true values", misclosureAtProvisionalValues(networkInput(setup)).cwiseAbs().maxCoeff() < 1e-6);
}

template<>
template<>
void object::test<12>()
{
	set_test_name("Testing UV with a target behind the camera, with two pixel cameras");

	// each station computes with the interior orientation of its own instrument, T seen from CAM1 is the mirrored projection
	std::stringstream infiler(CAMPX_TARGET_BEHIND);
	TLSEvaluator evaluator(infiler);
	evaluator.setParameters(evaluator.getEstParams());
	ensure_equals("Evaluation with a target behind the camera should succeed", evaluator.evaluate(), true);
	ensure("Computed pixel coordinates should be the observed ones", evaluator.getMisclosure().cwiseAbs().maxCoeff() < 1e-9);
	const auto root = evaluator.getData().getTree().begin();
	for (const auto &camera : root.node->data->measurements.fCAM)
		for (const auto &uv : camera.measUV)
			ensure_equals("Marker of " + uv.targetPos->getName() + " seen from " + camera.instrumentPos->getName(), uv.behindCamera,
				uv.targetPos->getName() == "T" && camera.instrumentPos->getName() == "CAM1");

	// a target in the plane of the projection centre has no projection
	misclosureAtProvisionalValues(CAMPX_TARGET_IN_PLANE, false);
}

}; // namespace tut
