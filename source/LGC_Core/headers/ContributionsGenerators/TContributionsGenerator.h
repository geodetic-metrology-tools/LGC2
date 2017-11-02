/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TCONTRIBUTION_GENERATOR_H
#define TCONTRIBUTION_GENERATOR_H


//LGC
#include <ContributionStructures.h>
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


/*!
	\ingroup ContributionsGenerators

	\brief Calculates the contributions for the least squares design matrices for all of the observations processed by LGC2.
	
	In addition, calculation measurement values can be returned separately (used e.g. in the SIMULATION).
*/
class TContributionsGenerator{

public:
	/// Constructor taking a reference to a TPointTransformer
	TContributionsGenerator(TPointTransformer& fPointTransfoFunc);


	/*!@name Contributions for the total station (TTSTN) observations. 
		
			All the TSTN contributions are calculated either in ROOT node of the TREE of local frames or in the modified local astronomical
			system of the instrument (station).

			In all cases the STATION and TARGET points can be defined anywhere in the TREE of local frames.
	*/
	//@{

		/// Returns contribution for the Spatial Distance measurement (TDIST)
		DistMeasContrib	getSpatialDistanceContrib(shared_ptr<TTSTN> station, const TLINE& dist);

		/// Returns contribution for the Horizontal Angle measurement (TANGL)
		AnglMeasContrib	getHorAnglContrib(shared_ptr<TTSTN> station, shared_ptr<TTSTN::TROM> rom, const TANGL& angl);

		/// Returns contribution for the Zenith Distance (Vertical Angle) measurement (TZEND)
		AnglMeasContrib	getZenDistContrib(shared_ptr<TTSTN> station, const TZEND& zend);

		/// Returns contribution for the Horizontal Distance measurement (DHOR), made by a total station (TSTN).
		HorDistContrib	getHorDistContrib(shared_ptr<TTSTN> station, const TLINE& dhor);

		/// Returns contribution for the TPLR3D measurement.
		PLR3DContrib getPolar3DContrib(shared_ptr<TTSTN> station, shared_ptr<TTSTN::TROM> rom, const TPLR3D& plr3D);

		/// Returns contribution for the offset to a theodolite plane (TECTH) measurement.
		ECTHContrib	 getECTHContrib(shared_ptr<TTSTN> station, shared_ptr<TTSTN::TROM> rom, const TECTH& ecth);

		/// Returns contribution for the offset to a theodolite plane (TECDIR) measurement.
		ECTHContrib	 getECDIRContrib(shared_ptr<TTSTN> station, shared_ptr<TTSTN::TROM> rom, const TECDIR& ecdir);
	//@}


	/*!@name Contributions to the CAMERA (TCAM) observations. */
	//@{

		/// Returns the contribution for the TUVEC measurement.
		UVECContrib	getUVECContrib(const TCAM& camera, const TUVEC& uvec);

		/// Returns the contribution for the TUVD measurement.
		UVDContrib	getUVDContrib(const TCAM& camera, const TUVD& uvd);

	//@}

	/*!@name Contributions to the other measurement types (no TSTN nor CAM).*/
	//@{

		///  Returns the contribution for a TDVER measurement
		DVERContrib	getDVERContrib(const TDVER& dver);

		///  Returns the contribution for a levelling measurement (TDLEV) measurement
		DLEVContrib	getDLEVContrib(const TLEVEL& levelST, const TDLEV& dlev);

		///  Returns the contribution for offset to a vertical plane (TECHO)
		ECHOContrib	getECHOContrib(const TECHOROM& echoROM, const TECHO& echo);

		///  Returns the contribution for offset to a vertical line (TECVE)
		ScaleMeasContrib getECVEContrib(const TECVEROM& ecveROM, const TECVE& ecve);

		/// Returns contribution for the offset to a spatial line (TECSP) measurement.
		ECSPContrib getECSPContrib(const TECSPROM& ecspROM, const TECSP& ecsp);

		///  Returns the contribution for a free spatial distance measurement (TDSPT) made by EDM instrument
		DistMeasContrib	getDSPTContrib(const TEDM& edmST, const TDSPT& dspt);

		///  Returns the horizontal distance measurement contribution (RDHOR), made as a part of DLEV measurement
		HorDistContribLEVEL	getHorDistContrib(const LGCAdjustablePoint* referencePoint, const TDLEV::TDHOR& dhor);

		/// Returns contribution for the Gyro-Theodolithe measurement (TORIE)
		AnglMeasContrib	getOrieContrib(const TORIEROM& orieROM, const TORIE& orie);

		///  Returns the contribution for a pdor measurement
		PtOrientationContrib	getPDORContrib(const TPdorObs& pdorObs);

		///  Returns the contribution for a RADI measurement
		PtOrientationContrib	getRADIContrib(const TRADI& radi);

		/// Returns the contribution for the OBSXYZ measurement
		OBSXYZContrib  getOBSXYZContrib(const TOBSXYZ& obsxyz);
	//@}


	/*!@name Method for accessing the calculation measurement values only . */
	//@{
		/// Returns the the ANGL measurement calculated meas in RADIANS [rad]
		TReal getANGLCalcMeas(shared_ptr<TTSTN> station, shared_ptr<TTSTN::TROM> rom, const LGCAdjustablePoint* targetAdjPoint);

		/// Returns the ZEND measurement calculated meas in RADIANS [rad]
		TReal getZENDCalcMeas(shared_ptr<TTSTN> station, const LGCAdjustablePoint* targetAdjPoint, TReal targetHt);

		/// Returns the DIST measurement calculated meas in meters [m]
		TReal getDISTCalcMeas(shared_ptr<TTSTN> station, const LGCAdjustablePoint* targetAdjPoint, TReal targetHt, TReal distanceCorr);

		/// Returns the DHOR measurement made by a TSTN, calculated meas in meters [m]
		TReal getDHORCalcMeas(shared_ptr<TTSTN> station, const TLINE& dhor);

		///  Returns the TPLR3D calculated measurement values
		PLR3DCalcMeas getPLR3DCalcMeas(shared_ptr<TTSTN> station, shared_ptr<TTSTN::TROM> rom, const TPLR3D& plr3D);

		/// Returns contribution for the offset to a theodolite plane (TECTH) measurement.
		TReal	 getECTHCalcMeas(shared_ptr<TTSTN> station, shared_ptr<TTSTN::TROM> rom, const TECTH& ecth);

		/// Returns contribution for the offset to a theodolite line (TECSP) measurement.
		TReal	 getECSPCalcMeas(shared_ptr<TTSTN> station, shared_ptr<TTSTN::TROM> rom, const TECSP& ecsp);

		/// Returns the the TUVEC measurement calculaded measurement value
		TFreeVector getUVECCalcMeas(const TCAM& camera, const TUVEC& uvec);

		/// Returns the TUVD measurement calculaded measurement value
		UVDCalcMeas getUVDCalcMeas(const TCAM& camera, const TUVD& uvec);

		/// Returns the TDLEV measurement calculated meas in meters [m]
		TReal getDLEVCalcMeas(const TLEVEL& levelST, const TDLEV& dlev);

		/// Returns the TDSPT measurement calculated meas in meters [m]
		TReal	getDSPTCalcMeas(const TEDM& edmST, const TDSPT& dspt);

		/// Returns the TECHO measurement calculated meas in meters [m]
		TReal getECHOCalcMeas(const TECHOROM& echoROM, const TECHO& echo);

		///  Returns TECVE measurement calculated meas in meters [m]
		TReal getECVECalcMeas(const TECVEROM& ecveROM, const TECVE& ecve);

		/// Returns the TDHOR measurement calculated meas in meters [m]
		TReal	getHorDistCalcMeas(const LGCAdjustablePoint* referencePoint, const TDLEV::TDHOR& dhor);

		/// Returns the the ORIE measurement calculated meas in RADIANS [rad]
		TReal getORIECalcMeas(const TORIEROM& orieROM, const TORIE& orie);

		/// Returns the the DVER measurement calculated meas in meter [m]
		TReal	getDVERCalcMeas(const TDVER& dver);
	//@}

private:
	// Contains references: not assignable
	TContributionsGenerator& operator=(const TContributionsGenerator&);

	/// Pointer to a TPointTransformationFunctions
	TPointTransformer& fPointTransfo;

	/*!@name Supporting functions used in contribution calculation methods */
	//@{
		/*! Returns the point contributions of a given transformation between local object references (lorTrafo), with a given coefficients.
		The pattern is (e.g. for a X coordinate): a*pointXDerivative[0] + b*pointXDerivative[1] + c*pointXDerivative[2]
		*/
		TFreeVector getPointContributions(const TLOR2LOR& lorTrafo, TReal a, TReal b, TReal c);

		/// Adds contribution of a LOR transformations for 1D measurements (ANGL,ZEND,DIST,DHOR) into a 'transfContrib' vector, a,b,c are the coeficcients (see documentation in Mathematical Obsevation Models)
		void addTransformationsContributions(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, TReal a, TReal b, TReal c, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>>& transfContrib);

		/// Adds contribution of a LOR transformations for PLR3D measurements into a 'transfContrib' vector
		void addTransformationsContributions3D(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, const TFreeVector& line1AMat,  const TFreeVector& line2AMat,  const TFreeVector& line3AMat, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib);
		
		/// Adds Point contributions for the PLR3D measurement
		void addPointContributionsPLR3D(const TLOR2LOR& lorTrafo, const TFreeVector& line1AMat,  const TFreeVector& line2AMat,  const TFreeVector& line3AMat, Point3DContrib& pointContrib, bool station);
	
		///Adds contribution of a every target's LOR transformation in 'lorTrafo' into a 'transfContrib' vector for UVD measurement
		void addUVDTgTransfContributionsCamera(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib);
	//@}

};
#endif 
