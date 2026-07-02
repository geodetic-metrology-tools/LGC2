// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TMONITORIMPL
#define TMONITORIMPL

#include "MonitorCore.h"

// STL
#include <chrono>
#include <iomanip> // put_time

#include <Eigen/Dense>

#include <Behavior.h>
#include <TLGCData.h>
#include <TLSAlgorithm.h>
#include <TReader.h>

#include "FileUtils.h"
#include "TDataAnalyzer.h"
#include "TInputFileWriter.h"
#include "TLGCCalculation.h"
#include "TLSResultsMatrices.h"
#include "TLSSimulation.h"
#include "TSimFileWriter.h"
#include "TStreamFormatterFactory.h"
#include "TVAbstractAlgorithm.h"

#if USE_SERIALIZER
#	include <Serializer_json.hpp>
#endif // USE_SERIALIZER

class Monitor::MonitorImpl
{
public:
	// constructor
	MonitorImpl(const std::string &path) : inputFilePath(path) { MonitorImpl::initialize(); }
	~MonitorImpl() = default;
	void updateMeas(const std::string &id, const Eigen::VectorXd &measurementVector);
	// set & get activationstatus
	void setActivationStatus(const std::string &, bool);
	bool getActivationStatus(const std::string &);
	// set observation standard deviation (optional, otherwise the ones from the inputfile are used)
	void setObsSigma(const std::string &id, const Eigen::VectorXd &sigma);
	// methods for manipulating fixed point and frame parameters
	void setFixedFrameParameter(const std::string &frameName, int idx, double val);
	void setFixedPointParameter(const std::string &pointName, int idx, double val);
	void setFixedSagParameter(const std::string &sagName, int idx, double val);
	// freeze
	void freezeFrameParameter(const std::string &frameName, int idx, double val);
	void freezePointParameter(const std::string &PointName, int idx, double val);
	// bearing of sag is hidden in interface, idx=0 means vertical sags
	void freezeSagParameter(const std::string &sagName, int idx, double val);
	// unfreeze
	void unfreezeFrameParameter(const std::string &frameName, int idx);
	void unfreezePointParameter(const std::string &PointName, int idx);
	void unfreezeSagParameter(const std::string &sagName, int idx);

	// get measurement
	Eigen::VectorXd getMeas(const std::string &id);
	// triggering the adjustment calculation
	bool adjust();
	// for checking the estimation status
	bool getStatus() { return estimationStatus; };
#if USE_SERIALIZER
	// serialized snapshot of the adjusted project (JSON string)
	std::string getResultsJson();
#endif
	// write input file with current measurement data
	void writeLGCInputFile();

	// Parameter Result methods
	// get estimate of point
	Eigen::VectorXd getPointEstimate(const std::string &);
	// get estimate of point in a subframe
	Eigen::VectorXd getPointEstimate(const std::string &, const std::string &frameName);
	Eigen::VectorXd getFrameEstimate(const std::string &);
	Eigen::VectorXd getSagEstimate(const std::string &);
	// precisions
	Eigen::VectorXd getPointEstimatePrec(const std::string &);
	Eigen::VectorXd getPointEstimatePrec(const std::string &, const std::string &detsFrame);
	Eigen::VectorXd getFrameEstimatePrec(const std::string &);
	Eigen::VectorXd getSagEstimatePrec(const std::string &);

	// Residual result methods
	// get estimated residual
	Eigen::VectorXd getEstimateResidual(const std::string &obsName);
	// get calculated measurement
	Eigen::VectorXd getCalcMeas(const std::string &obsName) { return getMeas(obsName) + getEstimateResidual(obsName); };
	// get observation sigma
	Eigen::VectorXd getObsSigma(const std::string &obsName);
	// get the sigma0 after adjustment
	double getSigma0();
	// transform coordinates/directions
	Eigen::Vector3d transformCoordinates(const Eigen::Vector3d& coord, const std::string& from, const std::string& to);
	Eigen::Vector3d transformDirection(const Eigen::Vector3d& dir, const std::string& from, const std::string& to);
	// for water and wire network related data
	waterRom getECWSData(const std::string &ecwsRomName);
	wireRom getECWIData(const std::string &ecwiRomName);

private:
	void initialize();
	void createParameterReferences();
	void createMeasurementReferences();
	TVMeas &getMeasurement(const std::string &id);

	// containing measurement configuration, observations, estimates
	std::shared_ptr<TLGCData> project;
	// helper methods for conversion to Eigen vector
	Eigen::VectorXd toVectorXd(TFreeVector);
	Eigen::VectorXd toVectorXd(TPositionVector);

	std::string inputFilePath;
	// LGC adjustment algorithm used by adjust method
	std::unique_ptr<TVAbstractAlgorithm> algorithm;

	struct
	{
		// single polymorphic handle to every measurement, keyed by observation id
		std::unordered_map<std::string, TVMeas &> meas;
	} measRefs;

	struct
	{
		// Round of Measurement references, for now only used for water and wire
		std::unordered_map<std::string, TECWSROM &> ecwsRoms;
		std::unordered_map<std::string, TECWIROM &> ecwiRoms;

	} romRefs;

	// containing maps to parameter object references
	// maybe needs to be more specific as a universal method a la getEstimate does not exists in LGC, rather there
	// are methods like getEstimatedValue, getEstParam etc..
	struct
	{
		// the parameter types
		// the parameter references
		std::unordered_map<std::string, LGCAdjustablePoint &> POINTS;
		std::unordered_map<std::string, LGCAdjustableLine &> LINES;
		std::unordered_map<std::string, TAdjustableAngle &> ANGLES;
		std::unordered_map<std::string, LGCAdjustablePlane &> PLANES;
		std::unordered_map<std::string, TAdjustableLength &> LENGTHS;
		std::unordered_map<std::string, TAdjustableHelmertTransformation &> FRAMES;
		std::unordered_map<std::string, LGCAdjustableSag&> SAGS;
	} paramRefs;

	// status of estimation. True if estimation results are ready for extraction.
	// False if not (after calling updateMeas or if estimation failed)
	bool estimationStatus;
};

#endif
