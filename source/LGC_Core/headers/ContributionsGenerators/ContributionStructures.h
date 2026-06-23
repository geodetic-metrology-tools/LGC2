/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TCONTRIBUTION_STRUCTURES_H
#define TCONTRIBUTION_STRUCTURES_H

/*!
   \file
   \ingroup ContributionsGenerators

	In this header file, structures holding contributions for every observation type are kept.
	Distances and positions are treated in METERS [m], angles, if TAngle class is not used, are treated in RADIANS [rad].

*/

// STL
#include <algorithm>
#include <cmath>
// SURVEYLIB
#include <TAdjustableHelmertTransformation.h>
#include <TFreeVector.h>
#include <TLGCPointConstraintGroup.h>
// LGC
#include <Global.h>

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for transformation parameters (rotations, translations, scale).
*/
struct TransformationContrib
{
	TFreeVector fRotationContrib; //!< Omega (Rx), Phi (Ry) and Kappa (Rz) contributions
	TFreeVector fTranslContrib; //!< Tx, Ty, Tz contributions
	TReal fScaleContrib; //!< Scale contribution
};

/*!
	\ingroup ContributionsGenerators

	\brief  Contributions for a point in observation models consisting of 3 equations (currently used for PLR3D, UVD).
*/
struct Point3DContrib
{
	Eigen::Matrix3d contrib;
};

/*!
	\ingroup ContributionsGenerators

	\brief  Contributions for a transformation in observation models consisting of 3 equations (currently used for PLR3D, UVD).
*/
struct TransformationContrib3D
{
	TransformationContrib firstEquationTransContrib;
	TransformationContrib secondEquationTransContrib;
	TransformationContrib thirdEquationTransContrib;
	// helper for extraction
	TransformationContrib getContrib(size_t j)
	{
		switch (j)
		{
		case 0:
			return firstEquationTransContrib;
		case 1:
			return secondEquationTransContrib;
		case 2:
			return thirdEquationTransContrib;
		default:
			throw std::runtime_error("3D transformation contribution index out of bounds.");
		}
	}
};

/*!
	\ingroup ContributionsGenerators

	\brief  Contribution for a transformation in observation models consisting of 2 equations (currently used for UVEC).
*/
struct TransformationContrib2D
{
	TransformationContrib firstEquationTransContrib;
	TransformationContrib secondEquationTransContrib;
};

// often used to store sensitivities with respect to all transformations along a transformation chain
typedef std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> TransformationContribVector;
typedef std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> TransformationContrib3DVector;

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for Spatial Distance (DIST) measurement made by a total station (TTSTN).
*/
struct DistMeasContrib
{
	TReal fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the DIST measurement is calculated.
	TransformationContribVector fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the DIST measurement is calculated.
	TransformationContribVector fTgTransformContrib;

	TReal fHIContrib; //!< Instrument (station) height contribution
	TReal fDistCorrection; //!< Distance correction

	TReal fObsVariance; //!< Variance of the observation
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions used for both the TANGL and the TZEND measurement types made by a total station (TTSTN).
*/
struct AnglMeasContrib
{
	TAngle fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the angle measurement is calculated.
	TransformationContribVector fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the angle measurement is calculated.
	TransformationContribVector fTgTransformContrib;

	TReal fHIContrib; //!< Instrument (station) height contribution
	TReal fV0Contrib; //!< V0 contribution - orientation angle of the station (around the Z axis)

	TReal fObsVariance; //!< Variance of the observation
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TDHOR measurement made by a total station (TTSTN).
*/
struct HorDistContrib
{
	TReal fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the DHOR measurement is calculated.
	TransformationContribVector fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the DHOR measurement is calculated.
	TransformationContribVector fTgTransformContrib;

	TReal fDistCorrection; //!< Distance correction
	TReal fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TDHOR measurement introduced as a part of the TDLEV measurement
*/
struct HorDistContribLEVEL
{
	TReal fCalcMeas;
	TFreeVector fStaffContrib;
	TFreeVector fRefPtContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STAFF into the node, where the DHOR measurement is calculated.
	TransformationContribVector fStaffTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform Reference Point into the node, where the DHOR measurement is calculated.
	TransformationContribVector fRefPtTransformContrib;

	TReal fRefPtDistContrib; //!< Contribution to the reference point distance from the plane
	TReal fCollAngleContrib; //!< d(DHOR)/d(collAngl); nonzero only in non-OLOC when intersection follows collimation plane
	TReal fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contribution for the levelling (TDLEV) measurement.
*/
struct DLEVContrib
{
	TReal fCalcMeas;
	TFreeVector fStaffContrib;
	TFreeVector fRefPtContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform STAFF into the node, where the DHOR measurement is calculated.
	TransformationContribVector fStaffTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform Reference Point into the node, where the DHOR measurement is calculated.
	TransformationContribVector fRefPtTransformContrib;

	TReal fRefPtDistContrib;
	TReal fCollAngleContrib; //!< Contribution of the collimination angle

	TReal fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Combined DLEV and optional DHOR contributions returned by getDLEVContribCombined.
*/
struct DLEVCombinedContrib
{
	DLEVContrib fDLEV;
	HorDistContribLEVEL fDHOR;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the levelling (TDVER) measurement.
*/
struct DVERContrib
{
	TReal fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the DVER measurement is calculated.
	TransformationContribVector fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the DVER measurement is calculated.
	TransformationContribVector fTgTransformContrib;

	TReal fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TECHO measurement.
*/
struct ECHOContrib
{
	TReal fCalcMeas;
	TFreeVector fStationContrib;
	TReal fThetaPlaneAngleContrib; //!< Contribution to the THETA angle of the plane in radians [rad]
	TReal fRefPtDistContrib; //!< Contribution to a distance of a referenece point from the plane

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECHO measurement is calculated.
	TransformationContribVector fStTransformContrib;

	TReal fObsVariance;
};

/*!
\ingroup ContributionsGenerators

\brief Contributions for the TECVE measurement.
*/
struct ScaleMeasContrib
{
	TReal fCalcMeas;
	TFreeVector fStationContrib;
	TFreeVector fPointLineContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECHO measurement is calculated.
	TransformationContribVector fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECHO measurement is calculated.
	TransformationContribVector fPtLineTransformContrib;

	TReal fObsVariance;
};
/*!
\ingroup ContributionsGenerators

\brief Contributions for the TECSP measurement.
*/
struct ECSPContrib
{
	TReal fCalcMeas;
	TFreeVector fStationContrib;
	TFreeVector fPointLineContrib1;
	TFreeVector fPointLineContrib2;
	// TFreeVector	fLineVecContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECSP measurement is calculated.
	TransformationContribVector fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECSP measurement is calculated.
	TransformationContribVector fPtLineTransformContrib1;
	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECSP measurement is calculated.
	TransformationContribVector fPtLineTransformContrib2;

	TReal fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TECTH measurement.
*/
struct ECTHContrib
{
	TReal fCalcMeas;
	TFreeVector fTSTNPtContrib; /*Reference point for the PLANE*/
	TFreeVector fScaleStationPtContrib;

	TReal fV0Contrib; //!< V0 contribution - orientation angle of the station (around the Z axis)

	TReal fDistanceCorrectionContrib;

	TransformationContribVector fTSTNPtTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the ECTH measurement is calculated.
	TransformationContribVector fStTransformContrib;

	TReal fObsVariance;
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TPLR3D measurement made by a total station (TTSTN).
*/
struct PLR3DContrib
{
	Eigen::Vector3d fCalcMeas;
	Point3DContrib fStCoordContrib;
	Point3DContrib fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the PLR3D measurement is calculated.
	TransformationContrib3DVector fStTransformContrib;
	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the PLR3D measurement is calculated.
	TransformationContrib3DVector fTgTransformContrib;

	Eigen::Vector3d fInstrHeightContrib; //!< Instrument (station) height contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fTargetHeightContrib; //!< Target height contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fV0Contrib; //!< V0 contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fThetaContrib; //!< Theta contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fPhiContrib; //!< Phi contribution for the First, Second and Third equation respectively.
	Eigen::Vector3d fDistAndCsContrib; //!< Distance and distance correction contributions for the First, Second and Third equation respectively.

	Eigen::Vector3d fRxContrib; //!< Contribution of the option rotation about the X axis for the First, Second and Third equation respectively.
	Eigen::Vector3d fRyContrib; //!< Contribution of the option rotation about the Y axis for the First, Second and Third equation respectively.

	Eigen::Vector3d fObsVariance; //!< Variances of the First, Second and Third equation respectively.
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TUVD measurement made by a camera (TCAM).
*/
struct UVDContrib
{
	Eigen::Vector3d fCalcMeas;
	Point3DContrib fStCoordContrib;
	Point3DContrib fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the node, where the UVD measurement is calculated.
	TransformationContrib3DVector fTgTransformContrib;

	Eigen::Vector3d fObsVariance; //!< Variances of the First, Second and Third equation respectively.
};

/*!
	\ingroup ContributionsGenerators

	\brief Contributions for the TUVEC measurement made by a camera (TCAM).
*/
struct UVECContrib
{
	TFreeVector fStCoordContribFirstEq; //!< x,y,z station coordinate contributions for the First equation
	TFreeVector fStCoordContribSecondEq; //!< x,y,z station coordinate contributions for the Second equation

	TFreeVector fTgCoordContribFirstEq; //!< x,y,z target coordinate contributions for the First equation
	TFreeVector fTgCoordContribSecondEq; //!< x,y,z target coordinate contributions for the Second equation

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>> fTgTransformContrib;

	TReal fCalcMeas[2]; //!< Calculated measurement
	TReal fObsVariance[2]; //!< Variances for the First and Second equation respectively.
};

/*!
	\ingroup ContributionsGenerators

	\brief \returns calculation measurement values for the UVD observation.
*/
struct UVDCalcMeas
{
	TFreeVector fMeasuredVector;
	TReal fsDistance;
};

struct PLR3DCalcMeas
{
	TReal dist;
	TReal phi;
	TReal theta;
};

/*!
\ingroup ContributionsGenerators

\brief Contributions for orientation measurement (PDOR, RADI).
*/
struct PtOrientationContrib
{
	TFreeVector oriPointContrib; // contribution for the oritented point

	/// Vector of contributions in pairs with transformations, which are used to transform TARGET into the root.
	TransformationContribVector fRefPtTransformContrib;

	TReal calcmeas; // calculated measurement
};

/*!
\ingroup ContributionsGenerators

\brief Contributions for the point observation OBSXYZ.
*/
struct OBSXYZContrib
{
	TFreeVector fCalcMeas;
	Eigen::Vector3d fObsVariance;
	Point3DContrib fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform the observed point into the observation frame.
	TransformationContrib3DVector fTgTransformContrib;

	TFreeVector fMisclosureVector; //!< Misclosure vector of the First, Second and Third equation respectively.
};
struct modelEval
{
	double modelF;
	Eigen::RowVector3d modelJacobian;
	bool success;
	std::string message;
};
struct ModelAndJacobian
{
	// the raw model function evaluated at the relative position. e.g. distance, horizontal angle
	std::function<modelEval(const Eigen::Vector3d &)> func;
};

/*!
	\ingroup ContributionsGenerators

	\brief Raw polar observation models: the value f(relPos) and its Jacobian df/d(relPos), as a function of the
	relative position relPos = (target - station) expressed in the station frame.

	These are the single source of truth for the distance / angle conventions used by the "TSTN in frame"
	contribution generators (getPolarContribInFrame), kept as pure, non-capturing functions so they can be
	reused and unit-tested in isolation. Each returns a modelEval whose 'success' flag is false (with a
	message) when the Jacobian is undefined for the given geometry.
*/
namespace PolarModels
{
//! Spatial distance: f = ||relPos||
inline modelEval distance(const Eigen::Vector3d &relPos)
{
	const double r = relPos.norm();
	if (isZero(r))
		return {0.0, Eigen::RowVector3d::Zero(), false, "Relative position is zero, cannot evaluate Jacobian of distance function."};
	return {r, relPos.transpose() / r, true, ""};
}

//! Horizontal (direction) angle, convention atan2(x, y)
inline modelEval horizontalAngle(const Eigen::Vector3d &relPos)
{
	const double x = relPos(0);
	const double y = relPos(1);
	const double dist2 = x * x + y * y; // horizontal distance squared
	const double angle = std::atan2(x, y);
	if (isZero(dist2))
		return {angle, Eigen::RowVector3d::Zero(), false, "Horizontal angle Jacobian undefined: (identical x/y coordinates)."};
	Eigen::RowVector3d jac;
	jac << y / dist2, -x / dist2, 0.0;
	return {angle, jac, true, ""};
}

//! Zenith (vertical) angle measured from the local vertical: f = acos(z / ||relPos||)
inline modelEval zenithAngle(const Eigen::Vector3d &relPos)
{
	const double r2 = relPos.squaredNorm();
	if (isZero(r2))
		return {0.0, Eigen::RowVector3d::Zero(), false, "Vertical angle undefined: relative position is zero."};
	const double x = relPos(0);
	const double y = relPos(1);
	const double z = relPos(2);
	const double r = std::sqrt(r2);
	const double c = std::clamp(z / r, -1.0, 1.0); // clamp for numerical safety
	const double angle = std::acos(c);
	const double rho = std::sqrt(x * x + y * y);
	if (isZero(rho))
		return {angle, Eigen::RowVector3d::Zero(), false, "Vertical angle Jacobian undefined: horizontal projection is near zero."};
	const double denom = rho * r2;
	Eigen::RowVector3d jac;
	jac << (x * z) / denom, (y * z) / denom, -rho / r2;
	return {angle, jac, true, ""};
}
} // namespace PolarModels
struct PolarContribInFrame
{
	TReal fModelPrediction; // evalualtion of the model function resulting from relatve position.
	TVector fRelativePosition; // target - station in station frame, reused in the variance calculation of the specific models
	TFreeVector fStationContrib;
	TFreeVector fTargetContrib;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTarget2RootContrib;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fRoot2StationContrib;
	double fFixedParametersVarianceContribution;
};
struct DistMeasContribFrame
{
	TReal fCalcMeas;
	PolarContribInFrame fPolarContrib;
	TReal fHIContrib; //!< Instrument (station) height contribution
	TReal fDistCorrection; //!< Distance correction
	TReal fObsVariance; //!< Variance of the observation
};
struct AnglMeasContribFrame
{
	TAngle fCalcMeas;
	PolarContribInFrame fPolarContrib;
	TReal fV0Contrib; //!< V0 contribution - orientation angle of the station (around the Z axis)
	TReal fHIContrib; //!< Instrument (station) height contribution
	TReal fObsVariance; //!< Variance of the observation
};

struct INCLContrib
{
	TAngle fCalcMeas;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the angle measurement is calculated.
	TransformationContribVector fStTransformContrib;

	TReal fObsVariance; //!< Variance of the observation
};

struct ECWSContrib
{
	TReal fCalcMeas;
	TFreeVector fTgCoordContrib;

	/// Vector of contributions in pairs with transformations, which are used to transform STATION into the node, where the distance measurement is calculated.
	TransformationContribVector fWSTransformContrib;

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
	TransformationContribVector fWireFirstEqTransformContrib;
	TransformationContribVector fWireSecondEqTransformContrib;

	TReal fObsVariance[2];
};

struct ECWICalcMeas
{
	TReal fMeasuredX;
	TReal fMeasuredZ;
};

struct PointGroupConstraintContrib
{
	// this struct can hold the data for a 1D scale constraint
	// current constraint value
	TReal constraintMisclosure;
	// derivatives
	// with respect to frame trafos (for each affected point)
	std::map<std::string, TransformationContribVector> TransformContrib;
	// with respect to involved points
	std::map<std::string, Eigen::Vector3d> PointContrib;
};

struct SagPairContrib
{
	// 3D constraint evaluated in the sag element's base frame:
	// refSag + offset - assocSag = 0
	Eigen::Vector3d constraintMisclosure;
	// derivatives wrt point subframe coordinates (transformed to sag frame)
	Point3DContrib dConstraintdAssocSub, dConstraintdRefSub;
	// derivatives wrt frame transformations (point frames -> sag frame)
	TransformationContrib3DVector dConstraintdAssocHelmert, dConstraintdRefHelmert;
	// derivatives wrt sag parameters (ZS, ZC, XS, XC)
	Eigen::Vector3d dOffsetdZSag, dOffsetdZCurv;
	Eigen::Vector3d dOffsetdXSag, dOffsetdXCurv;
};

struct PointGroupConstraintContrib3D
{
	// this struct can hold the data for a full 3d COG constraint or a full 3d momentum constraint
	// current constraint value
	Eigen::Vector3d constraintMisclosure;
	// derivatives
	// with respect to frame trafos (for each affected point)
	std::map<std::string, TransformationContrib3DVector> TransformContrib;
	// with respect to involved points
	std::map<std::string, Eigen::Matrix3d> PointContrib;
};

struct LIBRPointGroupContrib
{
	// constraintSignature signature;
	PointGroupConstraintContrib3D cogConstraintContrib;
	PointGroupConstraintContrib3D momentumConstraintContrib;
	PointGroupConstraintContrib scaleConstraintContrib;
};

struct pointSigmaContrib
{
	// misclosure is the (rotated) offset, the only contribution that needs to be calculated, all other data (derivatives, weights..) does not change during the iterations
	Eigen::Vector3d misclosure;
};
#endif
