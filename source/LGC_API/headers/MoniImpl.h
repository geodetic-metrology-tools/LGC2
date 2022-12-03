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

private:
	void initialize();
	void Moni::MoniImpl::createParameterReferences();
	void Moni::MoniImpl::createMeasurementReferences();


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
		// "No" type
		std::unordered_map<std::string, TDVER &> DVER;
		std::unordered_map<std::string, TRADI &> RADI;
		std::unordered_map<std::string, TOBSXYZ &> OBSXYZ;
	} measRefs;
	
	
	// containing maps to parameter object references
	// should probably be private.
	// maybe needs to be more specific as a method a la getEstimate does not exists, rather there
	// are methods like getEstimatedValue, getEstParam () helmert trafos
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


};

#endif
