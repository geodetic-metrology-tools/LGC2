/*
© Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TCONTRIBUTION_GENERATOR_H
#define TCONTRIBUTION_GENERATOR_H

// LGC
#include <ContributionStructures.h>
#include <TInverseTransformation.h>
#include <TLGCPointConstraintGroup.h>
#include <TPointTransformer.h>
#include <TTSTN.h>

class TUVEC;
class TUVD;
class TPdorObs;
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

	\brief Calculates the contributions for the least squares design matrices for all of the observations processed by LGC2.

	In addition, calculation measurement values can be returned separately (used e.g. in the SIMULATION).
*/
class TContributionsGenerator
{
public:
	/// Constructor taking a reference to a TPointTransformer
	TContributionsGenerator(TPointTransformer &fPointTransfoFunc);

	/*!@name Contributions for the total station (TTSTN) observations.

			All the TSTN contributions are calculated either in ROOT node of the TREE of local frames or in the modified local astronomical
			system of the instrument (station).

			In all cases the STATION and TARGET points can be defined anywhere in the TREE of local frames.
	*/
	//@{

	/// Returns contribution for the Spatial Distance measurement (TDIST)
	DistMeasContrib getSpatialDistanceContrib(std::shared_ptr<TTSTN> station, const TLINE &dist);

	/// Returns contribution for the Spatial Distance measurement (TDIST) for a TSTN in a  frame
	DistMeasContribFrame getSpatialDistanceContribInFrame(std::shared_ptr<TTSTN> station, const TLINE &dist);

	/// Returns contribution for the Horizontal Angle measurement (TANGL)
	AnglMeasContrib getHorAnglContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TANGL &angl);

	/// Returns contribution for the Horizontal Angle measurement (TANGL) for a TSTN in a frame
	AnglMeasContribFrame getHorAnglContribInFrame(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TANGL &angl);

	/// Returns contribution for the Zenith Distance (Vertical Angle) measurement (TZEND)
	AnglMeasContrib getZenDistContrib(std::shared_ptr<TTSTN> station, const TZEND &zend);

	/// Returns contribution for the Zenith Distance (Vertical Angle) measurement (TZEND) for a TSTN in a frame
	AnglMeasContribFrame getZenDistContribInFrame(std::shared_ptr<TTSTN> station, const TZEND &zend);

	/// Returns contribution for the Horizontal Distance measurement (DHOR), made by a total station (TSTN).
	HorDistContrib getHorDistContrib(std::shared_ptr<TTSTN> station, const TLINE &dhor);

	/// Returns contribution for the TPLR3D measurement.
	PLR3DContrib getPolar3DContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TPLR3D &plr3D);

	/// Returns contribution for the offset to a theodolite plane (TECTH) measurement.
	ECTHContrib getECTHContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TECTH &ecth);

	/// Returns contribution for the offset to a theodolite plane (TECDIR) measurement.
	ECTHContrib getECDIRContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TECDIR &ecdir);
	//@}

	/*!@name Contributions to the CAMERA (TCAM) observations. */
	//@{

	/// Returns the contribution for the TUVEC measurement.
	UVECContrib getUVECContrib(const TCAM &camera, const TUVEC &uvec);

	/// Returns the contribution for the TUVD measurement.
	UVDContrib getUVDContrib(const TCAM &camera, const TUVD &uvd);

	//@}

	/*!@name Contributions to the other measurement types (no TSTN nor CAM).*/
	//@{

	///  Returns the contribution for a TDVER measurement
	DVERContrib getDVERContrib(const TDVER &dver);

	///  Returns the contribution for a levelling measurement (TDLEV) measurement
	DLEVContrib getDLEVContrib(const TLEVEL &levelST, const TDLEV &dlev);

	///  Returns the contribution for offset to a vertical plane (TECHO)
	ECHOContrib getECHOContrib(const TECHOROM &echoROM, const TECHO &echo);

	///  Returns the contribution for offset to a vertical line (TECVE)
	ScaleMeasContrib getECVEContrib(const TECVEROM &ecveROM, const TECVE &ecve);

	/// Returns contribution for the offset to a spatial line (TECSP) measurement.
	ECSPContrib getECSPContrib(const TECSPROM &ecspROM, const TECSP &ecsp);

	///  Returns the contribution for a free spatial distance measurement (TDSPT) made by EDM instrument
	DistMeasContrib getDSPTContrib(const TEDM &edmST, const TDSPT &dspt);

	///  Returns the horizontal distance measurement contribution (RDHOR), made as a part of DLEV measurement
	HorDistContribLEVEL getHorDistContrib(const LGCAdjustablePoint *referencePoint, const TDLEV::TDHOR &dhor);

	/// Returns contribution for the Gyro-Theodolithe measurement (TORIE)
	AnglMeasContrib getOrieContrib(const TORIEROM &orieROM, const TORIE &orie);

	///  Returns the contribution for a pdor measurement
	PtOrientationContrib getPDORContrib(const TPdorObs &pdorObs);

	///  Returns the contribution for a RADI measurement
	PtOrientationContrib getRADIContrib(const TRADI &radi);

	/// Returns the contribution for the OBSXYZ measurement
	OBSXYZContrib getOBSXYZContrib(const TOBSXYZ &obsxyz);

	///  Returns the contribution for a roll measurement (TINCLY) made by INCL instrument
	INCLYContrib getINCLYContrib(const TINCLYROM &inclST, const TINCLY &incly);

	///  Returns the contribution the measured distance to a water surface (ECWS) made by the HLS instrument
	ECWSContrib getECWSContrib(const TECWSROM &ecwsROM, const TECWS &ecws);

	///  Returns the contribution for the measured distances to a wire (ECWI) made by the WPS instrument
	ECWIContrib getECWIContrib(const TECWIROM &ecwiROM, const TECWI &ecwi);

	//@}
	LIBRPointGroupContrib getPointGroupConstraintContrib(const TLGCPointConstraintGroup pointConstraintGroup, const TLGCData &data);
	pointSigmaContrib getPointSigmaContrib(LGCAdjustablePoint &, const TLGCData *);

private:
	// Contains references: not assignable
	TContributionsGenerator &operator=(const TContributionsGenerator &);

	/// Pointer to a TPointTransformationFunctions
	TPointTransformer &fPointTransfo;

	/*!@name Supporting functions used in contribution calculation methods */
	//@{
	/*! Returns the point contributions of a given transformation between local object references (lorTrafo), with a given coefficients.
	The pattern is (e.g. for a X coordinate): a*pointXDerivative[0] + b*pointXDerivative[1] + c*pointXDerivative[2]
	*/
	TFreeVector getPointContributions(const TLOR2LOR &lorTrafo, TReal a, TReal b, TReal c);

	/// Adds contribution of a LOR transformations for 1D measurements (ANGL,ZEND,DIST,DHOR) into a 'transfContrib' vector, a,b,c are the coeficcients (see documentation in Mathematical Obsevation Models)
	void addTransformationsContributions(const TLOR2LOR &lorTrafo,
		const TPositionVector &pointPos,
		TReal a,
		TReal b,
		TReal c,
		std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> &transfContrib);
	void addTransformationsContributions(const TLOR2LOR &lorTrafo,
		const TPositionVector &pointPos,
		const Eigen::Vector3d &vec,
		std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> &transfContrib);

	/// Adds contribution of a LOR transformations for PLR3D measurements into a 'transfContrib' vector
	void addTransformationsContributions3D(const TLOR2LOR &lorTrafo,
		const TPositionVector &pointPos,
		const TFreeVector &line1AMat,
		const TFreeVector &line2AMat,
		const TFreeVector &line3AMat,
		std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> &transfContrib);
	// matrix argument version
	void addTransformationsContributions3D(const TLOR2LOR &lorTrafo,
		const TPositionVector &pointPos,
		const Eigen::Matrix3d &AMat,
		std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> &transfContrib);

	/// Adds Point contributions for the PLR3D measurement, returns object representing AMat*dtrafo(point)/dpoint
	void addPointContributionsPLR3D(const TLOR2LOR &lorTrafo, const Eigen::Matrix3d &Amat, Point3DContrib &pointContrib, bool station);

	/// Adds contribution of a the rotation of LOR transformations for 1D measurements (ANGL,ZEND,DIST,DHOR) into a 'transfContrib' vector, a,b,c are the coeficcients (see documentation in Mathematical Obsevation Models)
	decltype(INCLYContrib::fStTransformContrib) addINCLContributions(
		const TLOR2LOR &lorTrafo, const TFreeVector &vector, const Eigen::Vector3d& locVert, bool useWyler = false, bool useCorr = false);

	// helper for error generation
	std::string getNameAndLine(const LGCAdjustablePoint &point) const;
	void generateContributionError(const std::string &message) const;

	//@}
};

#endif
