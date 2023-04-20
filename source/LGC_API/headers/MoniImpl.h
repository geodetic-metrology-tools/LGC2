/*
© Copyright CERN 2000-2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TMONIIMPL
#define TMONIIMPL

#include "Moni.h"

// STL
#include <Eigen/Dense>

#include <TLGCData.h>

#include "TVAbstractAlgorithm.h"

class Moni::MoniImpl
{
public:
	// constructor
	MoniImpl(std::string path) : inputFilePath(path) { MoniImpl::initialize(); }
	~MoniImpl() = default;
	void updateMeas(std::string id, Eigen::VectorXd measurementVector);
	// get measurement
	Eigen::VectorXd getMeas(std::string id);
	// triggering the adjustment claculation
	bool adjust();
	// for checking the estimation status
	bool getStatus() { return estimationStatus; };
	// get estimate of parameter
	Eigen::VectorXd getEstimate(std::string);
	// get estimate of parameter in a subframe
	Eigen::VectorXd getEstimate(std::string, std::string frameName);
	// get diagonal elements of covariances of the estimated parameters
	Eigen::VectorXd getEstimatePrec(std::string);
	// get diagonal elements of covariances of the estimated parameters in a subframe, in a first version only Root frame is allowed
	Eigen::VectorXd getEstimatePrec(std::string pointName, std::string frameName);
	// get estimated residual
	Eigen::VectorXd getEstimateResidual(std::string obsName);
	// get the sigma0 after adjustment
	double getSigma0();
	// get Meas IDs
	std::vector<std::string> getECWSMeasIds();

private:
	void initialize();
	void createParameterReferences();
	void createMeasurementReferences();

	// helper methods for concversion to Eigen vector
	Eigen::VectorXd toVectorXd(TFreeVector);
	Eigen::VectorXd toVectorXd(TPositionVector);


	std::string inputFilePath;
	// LGC adjustment algorithm used by adjust method
	std::unique_ptr<TVAbstractAlgorithm> algorithm;
	// containing measurement configuration, observations, estimates
	std::shared_ptr<TLGCData> project;

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
	// should probably be private.
	// maybe needs to be more specific as a universal method a la getEstimate does not exists in LGC, rather there
	// are methods like getEstimatedValue, getEstParam etc..
	struct
	{
		// the parameter types
		std::unordered_map<std::string, std::string> types;
		// the parameter references
		std::unordered_map<std::string, LGCAdjustablePoint &> POINTS;
		std::unordered_map<std::string, LGCAdjustableLine &> LINES;
		std::unordered_map<std::string, TAdjustableAngle &> ANGLES;
		std::unordered_map<std::string, LGCAdjustablePlane &> PLANES;
		std::unordered_map<std::string, TAdjustableLength &> LENGTHS;
		std::unordered_map<std::string, TAdjustableHelmertTransformation &> TRAFOS;
	} paramRefs;

	// status of estimation. True if estimation results are ready for extraction.
	// False if not (after calling updateMeas or if estimation failed)
	bool estimationStatus;


};

#endif
