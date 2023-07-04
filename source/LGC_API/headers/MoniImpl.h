/*
© Copyright CERN 2000-2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TMONIIMPL
#define TMONIIMPL

#include "Moni.h"

// STL
#include <Eigen/Dense>

#include <Behavior.h>
#include <TLGCData.h>
#include <TLSAlgorithm.h>
#include <TReader.h>

#include "FileUtils.h"
#include "TDataAnalyzer.h"
#include "TLGCCalculation.h"
#include "TLSResultsMatrices.h"
#include "TLSSimulation.h"
#include "TVAbstractAlgorithm.h"
#include "TInputFileWriter.h"
#include "TSimFileWriter.h"
#include "TStreamFormatterFactory.h"
#include <iomanip> // put_time
#include <chrono>




#if USE_SERIALIZER
#	include <Serializer_json.hpp>
#endif // USE_SERIALIZER

class Moni::MoniImpl
{
public:
	// constructor
	MoniImpl(std::string path) : inputFilePath(path) { MoniImpl::initialize(); }
	~MoniImpl() = default;
	void updateMeas(std::string id, Eigen::VectorXd measurementVector);
	// set activtaionstatus
	void setActivationStatus(std::string, bool);
	// set observation standard deviation (optional, otherwise the ones from the inputfile are used)
	void setObsSigma(std::string id, Eigen::VectorXd sigma);
	// methods for manipulating fixed point and frame parameters
	void setFixedFrameParameter(std::string frameName, int idx, double val);
	// get measurement
	Eigen::VectorXd getMeas(std::string id);
	// triggering the adjustment calculation
	bool adjust();
	// for checking the estimation status
	bool getStatus() { return estimationStatus; };
	// write Json output file
	void writeResultFile();
	// write input file with current measurement data
	void writeLGCInputFile();

	// Parameter Result methods
	// get estimate of point
	Eigen::VectorXd getPointEstimate(std::string);
	// get estimate of point in a subframe
	Eigen::VectorXd getPointEstimate(std::string, std::string frameName);
	Eigen::VectorXd getPointEstimatePrec(std::string);
	Eigen::VectorXd getPointEstimatePrec(std::string, std::string detsFrame);
	// get estimate of frame
	Eigen::VectorXd getFrameEstimate(std::string);
	Eigen::VectorXd getFrameEstimatePrec(std::string);

	// Residual result methods
	// get estimated residual
	Eigen::VectorXd getEstimateResidual(std::string obsName);
	// get calculated measurement
	Eigen::VectorXd getCalcMeas(std::string obsName) { return getMeas(obsName) + getEstimateResidual(obsName); };
	// get the sigma0 after adjustment
	double getSigma0();
	// get Meas IDs
	std::vector<std::string> getECWSMeasIds();

private:
	void initialize();
	void createParameterReferences();
	void createMeasurementReferences();
	TStatusObject &getStatusObject(std::string obsId);

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
		// the measurement types
		std::unordered_map<std::string, std::string> types;
		// rather maps to one class higher (TScalar, TPosition??)
		// Polar type
		std::unordered_map<std::string, TANGL &> ANGL;
		std::unordered_map<std::string, TZEND &> ZEND;
		std::unordered_map<std::string, TLINE &> DIST;
		std::unordered_map<std::string, TECTH &> ECTH;
		std::unordered_map<std::string, TECDIR &> ECDIR;
		std::unordered_map<std::string, TLINE &> DHOR;
		std::unordered_map<std::string, TPLR3D &> PLR3D;
		std::unordered_map<std::string, TORIE &> ORIE;
		// CAMD type
		std::unordered_map<std::string, TUVEC &> UVEC;
		std::unordered_map<std::string, TUVD &> UVD;
		// EDM type
		std::unordered_map<std::string, TDSPT &> DSPT;
		// LEVEL type, what about RefPt and RefPt+DHOR config?
		std::unordered_map<std::string, TDLEV &> DLEV;
		// SCALE Type
		std::unordered_map<std::string, TECHO &> ECHO;
		std::unordered_map<std::string, TECSP &> ECSP;
		std::unordered_map<std::string, TECVE &> ECVE;
		// INCL Type
		std::unordered_map<std::string, TINCLY &> INCLY;
		// HLSR Type
		std::unordered_map<std::string, TECWS &> ECWS;
		// WPSR Type
		std::unordered_map<std::string, TECWI &> ECWI;
		// "No" type
		std::unordered_map<std::string, TDVER &> DVER;
		std::unordered_map<std::string, TRADI &> RADI;
		std::unordered_map<std::string, TOBSXYZ &> OBSXYZ;
	} measRefs;
	
	
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
	} paramRefs;

	// status of estimation. True if estimation results are ready for extraction.
	// False if not (after calling updateMeas or if estimation failed)
	bool estimationStatus;


};

#endif
