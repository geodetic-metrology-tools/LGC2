/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TCONTRIBUTION_STRUCTURES_H
#define TCONTRIBUTION_STRUCTURES_H

/*!
   \file
   \ingroup ContributionsGenerators

   	In this header file, structures holding contributions for every observation type are kept.
	Distances and positions are treated in METERS [m], angles, if TAngle class is not used, are treated in RADIANS [rad].
	
*/

//SURVEYLIB
#include <TFreeVector.h>
#include <TAdjustableHelmertTransformation.h>

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for transformation parameters (rotations, translations, scale).
*/
struct TransformationContrib{
	TFreeVector fRotationContrib; //!< Omega (Rx), Phi (Ry) and Kappa (Rz) contributions
	TFreeVector fTranslContrib; //!< Tx, Ty, Tz contributions
	TReal		fScaleContrib; //!< Scale contribution
};

/*!
	\ingroup ContributionsGenerators

	\brief  Contributions for a point in observation models consisting of 3 equations (currently used for PLR3D, UVD).
*/
struct Point3DContrib{
	Eigen::Matrix3d contrib;
};

/*!
	\ingroup ContributionsGenerators

	\brief  Contributions for a transformation in observation models consisting of 3 equations (currently used for PLR3D, UVD).
*/
struct TransformationContrib3D{
	TransformationContrib	firstEquationTransContrib; 
	TransformationContrib	secondEquationTransContrib;
	TransformationContrib	thirdEquationTransContrib; 
};

/*!
	\ingroup ContributionsGenerators

	\brief  Contribution for a transformation in observation models consisting of 2 equations (currently used for UVEC).
*/
struct TransformationContrib2D{
	TransformationContrib	firstEquationTransContrib; 
	TransformationContrib	secondEquationTransContrib;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for Spatial Distance (DIST) measurement made by a total station (TTSTN).
*/
struct DistMeasContrib{
	TReal		fCalcMeas; 
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the DIST measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib; 
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the DIST measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib;  

	TReal		fHIContrib; //!< Instrument (station) height contribution
	TReal		fDistCorrection; //!< Distance correction

	TReal		fObsVariance; //!< Variance of the observation
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions used for both the TANGL and the TZEND measurement types made by a total station (TTSTN).
*/
struct AnglMeasContrib{
	TAngle	fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the angle measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib; 
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the angle measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib; 

	TReal		fHIContrib; //!< Instrument (station) height contribution
	TReal		fV0Contrib; //!< V0 contribution - orientation angle of the station (around the Z axis)

	TReal		fObsVariance; //!< Variance of the observation
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TDHOR measurement made by a total station (TTSTN).
*/
struct HorDistContrib{
	TReal		fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the DHOR measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib; 
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the DHOR measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib; 

	TReal		fDistCorrection; //!< Distance correction
	TReal		fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TDHOR measurement introduced as a part of the TDLEV measurement
*/
struct HorDistContribLEVEL{
	TReal		fCalcMeas;
	TFreeVector fStaffContrib; 
	TFreeVector fRefPtContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STAFF into the node, where the DHOR measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStaffTransformContrib;  
	/// Vector of contributions in pairs with transformations, which are used to transform Reference Point into the node, where the DHOR measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fRefPtTransformContrib;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contribution for the levelling (TDLEV) measurement.
*/
struct DLEVContrib{
	TReal		fCalcMeas;
	TFreeVector	fStaffContrib;
	TFreeVector	fRefPtContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform STAFF into the node, where the DHOR measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStaffTransformContrib;  
	/// Vector of contributions in pairs with transformations, which are used to transform Reference Point into the node, where the DHOR measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fRefPtTransformContrib; 

	TReal		fRefPtDistContrib;
	TReal		fCollAngleContrib;   //!< Contribution of the collimination angle

	TReal		fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the levelling (TDVER) measurement.
*/
struct DVERContrib{
	TReal			fCalcMeas;
	TFreeVector	    fStCoordContrib;
	TFreeVector	    fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the DVER measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib; 
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the DVER measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib; 

	TReal		fObsVariance;
};


/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TECHO measurement.
*/
struct ECHOContrib{
	TReal		fCalcMeas;
	TFreeVector	fStationContrib;
	TReal		fThetaPlaneAngleContrib;  //!< Contribution to the THETA angle of the plane in radians [rad]
	TReal		fRefPtDistContrib; //!< Contribution to a distance of a referenece point from the plane

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECHO measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib; 

	TReal		fObsVariance;
};

/*!
\ingroup ContributionsGenerators

\brief Contributions for the TECVE measurement.
*/
struct ScaleMeasContrib{
	TReal		fCalcMeas;
	TFreeVector	fStationContrib;
	TFreeVector	fPointLineContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECHO measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECHO measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fPtLineTransformContrib;

	TReal		fObsVariance;
};
/*!
\ingroup ContributionsGenerators

\brief Contributions for the TECSP measurement.
*/
struct ECSPContrib{
	TReal		fCalcMeas;
	TFreeVector	fStationContrib;
	TFreeVector	fPointLineContrib1;
	TFreeVector	fPointLineContrib2;
	//TFreeVector	fLineVecContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECSP measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECSP measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fPtLineTransformContrib1;
	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECSP measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fPtLineTransformContrib2;

	TReal		fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TECTH measurement.
*/
struct ECTHContrib{
	TReal		fCalcMeas;
	TFreeVector	fTSTNPtContrib; /*Reference point for the PLANE*/
	TFreeVector	fScaleStationPtContrib;

	TReal		fV0Contrib; //!< V0 contribution - orientation angle of the station (around the Z axis)

	TReal		fDistanceCorrectionContrib; 

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTSTNPtTransformContrib;  
	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECTH measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib; 

	TReal		fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TPLR3D measurement made by a total station (TTSTN).
*/
struct PLR3DContrib {
	Eigen::Vector3d fCalcMeas;
	Point3DContrib fStCoordContrib;
	Point3DContrib fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the PLR3D measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the PLR3D measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> fTgTransformContrib;

	Eigen::Vector3d fInstrHeightContrib; //!< Instrument (station) height contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fTargetHeightContrib; //!< Target height contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fV0Contrib; //!< V0 contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fThetaContrib; //!< Theta contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fPhiContrib; //!< Phi contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fDistAndCsContrib; //!< Distance and distance correction contributions for the First, Second and Third equation respectively.

	Eigen::Vector3d fRxContrib; //!< Contribution of the option rotation about the X axis for the First, Second and Third equation respectively.
	Eigen::Vector3d fRyContrib; //!< Contribution of the option rotation about the Y axis for the First, Second and Third equation respectively.

	Eigen::Vector3d fMisclosureVector; //!< Misclosure vector of the First, Second and Third equation respectively.
	Eigen::Vector3d fObsVariance; //!< Variances of the First, Second and Third equation respectively.
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TUVD measurement made by a camera (TCAM).
*/
struct UVDContrib{
	Eigen::Vector3d fCalcMeas;
	Point3DContrib fStCoordContrib;
	Point3DContrib fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the UVD measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> fTgTransformContrib; 			

	Eigen::Vector3d fMisclosureVector; //!< Misclosure vector of the First, Second and Third equation respectively.
	Eigen::Vector3d fObsVariance; //!< Variances of the First, Second and Third equation respectively.
};


/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TUVEC measurement made by a camera (TCAM).
*/
struct UVECContrib{
	TFreeVector fStCoordContribFirstEq;  //!< x,y,z station coordinate contributions for the First equation
	TFreeVector fStCoordContribSecondEq; //!< x,y,z station coordinate contributions for the Second equation

	TFreeVector fTgCoordContribFirstEq; //!< x,y,z target coordinate contributions for the First equation
	TFreeVector fTgCoordContribSecondEq; //!< x,y,z target coordinate contributions for the Second equation

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>> fTgTransformContrib;		

	TReal	    fCalcMeas[2];  //!< Calculated measurement
	TReal	    fMisclosureVector[2];  //!< Misclosure vector of the First and Second equation respectively.
	TReal		fObsVariance[2];  //!< Variances for the First and Second equation respectively.
}; 

/*!
	\ingroup ContributionsGenerators

	\brief \returns calculation measurement values for the UVD observation.
*/
struct UVDCalcMeas{
	TFreeVector	fMeasuredVector;
	TReal		fsDistance;
};


struct PLR3DCalcMeas{
	TReal	dist;
	TReal	phi;
	TReal	theta;
};

/*!
\ingroup ContributionsGenerators

\brief Contributions for orientation measurement (PDOR, RADI).
*/
struct PtOrientationContrib{
	TFreeVector oriPointContrib; //contribution for the oritented point

	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the root.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fRefPtTransformContrib;

	TReal		calcmeas; //calculated measurement
};


/*!
\ingroup ContributionsGenerators

\brief Contributions for the point observation OBSXYZ.
*/
struct OBSXYZContrib{
	Point3DContrib fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform the observed point into the observation frame.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> fTgTransformContrib;

	TFreeVector fMisclosureVector; //!< Misclosure vector of the First, Second and Third equation respectively.
};



struct DistMeasContribFrame{
	TReal		fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the DIST measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib;

	TReal		fHIContrib; //!< Instrument (station) height contribution
	TReal		fDistCorrection; //!< Distance correction

	TReal		fObsVariance; //!< Variance of the observation
};
struct AnglMeasContribFrame{
	TAngle	fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the DIST measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib;

	TReal		fHIContrib; //!< Instrument (station) height contribution
	TReal		fV0Contrib; //!< V0 contribution - orientation angle of the station (around the Z axis)

	TReal		fObsVariance; //!< Variance of the observation
};

struct INCLYContrib {
	TAngle	fCalcMeas;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the angle measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib;

	TReal		fObsVariance; //!< Variance of the observation
};


struct ECWSContrib
{
	TReal fCalcMeas;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the distance measurement is calculated.
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fWSTransformContrib;

	TReal fWSContrib;
	TReal fObsVariance;
};

struct ECWIContrib
{
	TReal fCalcMeas[2];
	TFreeVector fStCoordContrib[2];

	// Wire Contributions
	TReal fBearingWireContrib[2]; //!< Contribution to the Bearing angle of the wire
	TReal fSlopeWireContrib[2]; //!< Contribution to the Slope angle of the wire
	TReal fDRefXDistContrib[2]; //!< Contribution to the X offset from the ref. Point to the wire
	TReal fDRefZDistContrib[2]; //!< Contribution to the Z offset from the ref. Point to the wire
	TReal fSagContrib[2]; //!< Contribution to the sag of the wire

	/// Vector of contributions in pairs with transformations, which are used to transform the wire in the Station Frame
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fWireFirstEqTransformContrib;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fWireSecondEqTransformContrib;

	TReal fObsVariance[2];
};

struct ECWICalcMeas
{
	TReal fMeasuredX;
	TReal fMeasuredZ;
};
#endif
