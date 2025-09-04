/*
© Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TSIMOBSERVATION_GENERATOR_H
#define TSIMOBSERVATION_GENERATOR_H

// LGC
#include <ContributionStructures.h>
#include <TContributionsGenerator.h>
#include <TPointTransformer.h>
#include <TTSTN.h>

class TUVEC;
class TUVD;
class TPdorObs;
class TECWS;
class TOBSXYZ;
struct TLEVEL;
struct TEDM;
struct TECHOROM;
struct TECVEROM;
struct TECSPROM;
struct TORIEROM;
struct TCAM;
struct TINCLYROM;
struct TECWSROM;
struct TECWIROM;

/*!
\ingroup ContributionsGenerators

\brief Simulate the observations for the least squares design matrices for all of the observations processed by LGC2.

*/
class TObservationGenerator
{
public:
	/// Constructor taking a reference to a TPointTransformer
	TObservationGenerator(TPointTransformer &pPointTransfo);

	/*!@name Method for accessing the calculation measurement values only . */
	//@{
	/// Returns the the ANGL measurement calculated meas in RADIANS [rad]
	TReal getANGLCalcMeas(const TTSTN &station, const TTSTN::TROM &rom, const LGCAdjustablePoint *targetAdjPoint);

	/// Returns the the ANGL measurement calculated meas in RADIANS [rad] for a TSTN defined in a subframe
	TReal getANGLCalcMeasInFrame(const TTSTN &station, const TTSTN::TROM &rom, const LGCAdjustablePoint *targetAdjPoint);

	/// Returns the ZEND measurement calculated meas in RADIANS [rad]
	TReal getZENDCalcMeas(const TTSTN &station, const LGCAdjustablePoint *targetAdjPoint, TReal targetHt);

	/// Returns the ZEND measurement calculated meas in RADIANS [rad] for a TSTN defined in a subframe
	TReal getZENDCalcMeasInFrame(const TTSTN &station, const LGCAdjustablePoint *targetAdjPoint, TReal targetHt);

	/// Returns the DIST measurement calculated meas in meters [m]
	TReal getDISTCalcMeas(const TTSTN &station, const LGCAdjustablePoint *targetAdjPoint, TReal targetHt, TReal distanceCorr);

	/// Returns the DIST measurement calculated meas in meters [m] for a TSTN defined in a subframe
	TReal getDISTCalcMeasInFrame(const TTSTN &station, const LGCAdjustablePoint *targetAdjPoint, TReal targetHt, TReal distanceCorr);

	/// Returns the DHOR measurement made by a TSTN, calculated meas in meters [m]
	TReal getDHORCalcMeas(const TTSTN &station, const TLINE &dhor);

	/// Returns the TPLR3D calculated measurement values
	PLR3DCalcMeas getPLR3DCalcMeas(const TTSTN &station, const TTSTN::TROM &rom, const TPLR3D &plr3D);

	/// Returns contribution for the offset to a theodolite plane (TECTH) measurement.
	TReal getECTHCalcMeas(const TTSTN &station, const TTSTN::TROM &rom, const TECTH &ecth);

	/// Returns contribution for the offset to a theodolite line (TECDIR) measurement.
	TReal getECDIRCalcMeas(const TTSTN &station, const TTSTN::TROM &rom, const TECDIR &ecdir);

	/// Returns the the TUVEC measurement calculaded measurement value
	TFreeVector getUVECCalcMeas(const TCAM &camera, const TUVEC &uvec);

	/// Returns the TUVD measurement calculated measurement value
	UVDCalcMeas getUVDCalcMeas(const TCAM &camera, const TUVD &uvec);

	/// Returns the TDLEV measurement calculated meas in meters [m]
	TReal getDLEVCalcMeas(const TLEVEL &levelST, const TDLEV &dlev);

	/// Returns the TDSPT measurement calculated meas in meters [m]
	TReal getDSPTCalcMeas(const TEDM &edmST, const TDSPT &dspt);

	/// Returns the TECHO measurement calculated meas in meters [m]
	TReal getECHOCalcMeas(const TECHOROM &echoROM, const TECHO &echo);

	///  Returns TECVE measurement calculated meas in meters [m]
	TReal getECVECalcMeas(const TECVEROM &ecveROM, const TECVE &ecve);

	/// Returns TECSP measurement calculated meas in meters[m]
	TReal getECSPCalcMeas(const TECSPROM &ecspROM, const TECSP &ecsp);

	/// Returns the TDHOR measurement calculated meas in meters [m]
	TReal getHorDistCalcMeas(const LGCAdjustablePoint *referencePoint, const TDLEV::TDHOR &dhor);

	/// Returns the the ORIE measurement calculated meas in RADIANS [rad]
	TReal getORIECalcMeas(const TORIEROM &orieROM, const TORIE &orie);

	/// Returns the the DVER measurement calculated meas in meter [m]
	TReal getDVERCalcMeas(const TDVER &dver);

	/// Returns the INCLY measurement calculated meas in RADIANS [rad]
	TReal getINCLYCalcMeas(const TINCLYROM &inclyROM, const TINCLY &incly);

	/// Returns the ROLLY measurement calculated meas in RADIANS [rad]
	TReal getROLLYCalcMeas(const TROLLYROM &rollyROM, const TROLLY &rolly);

	/// Returns the TECWS measurement calculated meas in meters [m]
	TReal getECWSCalcMeas(const TECWSROM &ecwsROM, const TECWS &ecws);

	/// Returns the OBSXYZ calculated meas in meters [m]
	TPositionVector getOBSXYZCalcMeas(const TOBSXYZ &obsxyz);

	/// Returns the TECWI measurements calculated meas in meters [m]
	ECWICalcMeas getECWICalcMeas(const TECWIROM &ecwiROM, const TECWI &ecwi);

	//@}

private:
	/// Pointer to a TPointTransformationFunctions
	TPointTransformer *fPointTransfo;

	// Contributions generator, used to calculated the calcmeas.
	TContributionsGenerator fCGenerator;
};
#endif
