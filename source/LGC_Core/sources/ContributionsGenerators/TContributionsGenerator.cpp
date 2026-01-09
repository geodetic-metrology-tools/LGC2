// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <LGCAdjustablePoint.h>
#include <TContributionsGenerator.h>
#include <algorithm>

#include "TDist.h"
#include "TTreeEntry.h"
#include "TXYH2CCS.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////
TContributionsGenerator::TContributionsGenerator(TPointTransformer &fPointTransfoFunc) : fPointTransfo(fPointTransfoFunc)
{
}

//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- TSTN measurements
//////////////////////////////////////////////////////////////////////

/*
	All the TSTN contributions are calculated either in ROOT node of the TREE of local frames or in the modified local astronomical
	system of the instrument (station).

	In all cases the STATION and TARGET points can be defined anywhere in the TREE of local frames.
*/

template<typename TPolarMeas>
PolarContribInFrame TContributionsGenerator::getPolarContribInFrame(std::shared_ptr<TTSTN> station, const TPolarMeas &polarMeas, const ModelAndJacobian &model)
{
	fPointTransfo.setMLA(false); // TSTN in Frame measurements never in MLA
	// new version taking target height along local vertical at target position
	const TLOR2LOR &target2RootTrafo = fPointTransfo.getLORTransformation(polarMeas.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	TPositionVector targetPosRoot = polarMeas.targetPos->getEstimatedValue(); // x2
	target2RootTrafo.transform(targetPosRoot);
	// local vertical in root coordinates at target position
	TFreeVector targetVertical(0, 0, 1, TCoordSysFactory::k3DCartesian);
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		fPointTransfo.set2MLATransformation(targetPosRoot);
		fPointTransfo.transformMLA2CGRF(targetVertical);
		fPointTransfo.transformCGRF2CCS(targetVertical);
	}
	// target position with added targetHeight along vertical
	TFreeVector targetHeightVector = targetVertical * polarMeas.target.targetHt;
	TPositionVector targetInRootWithHeight = targetPosRoot + targetHeightVector; // x3
	const TLOR2LOR &root2stationTrafo = fPointTransfo.getLORTransformation(fPointTransfo.getTree()->begin(), station->instrumentPos->getFrameTreePosition());
	TPositionVector targetInStationFrameWithHeight = targetInRootWithHeight; // x4
	root2stationTrafo.transform(targetInStationFrameWithHeight);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();

	// distance
	Eigen::Vector3d relativePosition = (targetInStationFrameWithHeight - stationPos).toRealVector();

	modelEval polarEval = model.func(relativePosition);
	if (!polarEval.success)
	{
		generateContributionError("TContributionGenerator::getPolarContribInFrame: " + polarEval.message + " Points "+ getNameAndLine(*station->instrumentPos) + " and "
			+ getNameAndLine(*polarMeas.targetPos));
	}
	double F = polarEval.modelF;
	Eigen::Matrix<double, 1, 3> dFdT = polarEval.modelJacobian;
	Eigen::Matrix<double, 1, 3> dFdS = -dFdT;
	Eigen::Matrix3d dTargetRoot_dTarget = target2RootTrafo.getPartialDerivativeWrtPosition();
	Eigen::Matrix3d dTargetWithHeightStation_dTargetWithHeightRoot = root2stationTrafo.getPartialDerivativeWrtPosition();
	// derivative with respect to target coordinates
	Eigen::Matrix<double, 1, 3> targetContrib = dFdT * dTargetWithHeightStation_dTargetWithHeightRoot * dTargetRoot_dTarget;
	// derivatives with respect transformation parameters target->root
	Eigen::Matrix<double, 1, 3> TargetToRoot_coefficients = dFdT * dTargetWithHeightStation_dTargetWithHeightRoot;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> target2RootContributions;
	addTransformationsContributions(target2RootTrafo, polarMeas.targetPos->getEstimatedValue(), TargetToRoot_coefficients, target2RootContributions);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> Root2StationContributions;
	addTransformationsContributions(root2stationTrafo, targetInRootWithHeight, dFdT, Root2StationContributions);

	// variance computation (contributions from fixed parameters: target height, target/station centering)
	Eigen::MatrixXd dRelPosdUncertainParameters(3, 5);

	// Transform vectors to station coordinates
	TFreeVector targetVerticalInStationCoord = targetVertical;
	root2stationTrafo.transform(targetVerticalInStationCoord);
	TFreeVector targetX(1, 0, 0, TCoordSysFactory::k3DCartesian);
	root2stationTrafo.transform(targetX);
	TFreeVector targetY(0, 1, 0, TCoordSysFactory::k3DCartesian);
	root2stationTrafo.transform(targetY);

	// Set Jacobian columns (rel pos wrt [T_u x/y, T_H_u, S_u x/y])
	dRelPosdUncertainParameters << targetX.toRealVector(), targetY.toRealVector(), targetVerticalInStationCoord.toRealVector(), -Eigen::Vector3d::UnitX(),
		-Eigen::Vector3d::UnitY();

	// Chain rule for stochastic model Jacobian
	Eigen::MatrixXd stochasticModelJac = dFdT * dRelPosdUncertainParameters;

	// Fixed parameter uncertainties (sigmas) and variances
	Eigen::Vector<double, 5> fixedParameterUncertainties;
	fixedParameterUncertainties << polarMeas.target.sigmaTargetCentering, polarMeas.target.sigmaTargetCentering, polarMeas.target.sigmaTargetHt, station->instrument.sigmaInstrCentering,
		station->instrument.sigmaInstrCentering;
	Eigen::VectorXd fixedParameterVariances = fixedParameterUncertainties.array().square();

	// Variance contribution: Covar Propagation rule
	double fixedParameterVarianceContrib = (stochasticModelJac * fixedParameterVariances.asDiagonal() * stochasticModelJac.transpose())(0, 0);

	return PolarContribInFrame({F, relativePosition, TFreeVector(dFdS), TFreeVector(targetContrib), target2RootContributions, Root2StationContributions, fixedParameterVarianceContrib});
}

// Spatial distance contributions
DistMeasContrib TContributionsGenerator::getSpatialDistanceContrib(std::shared_ptr<TTSTN> station, const TLINE &dist)
{
	// Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(dist.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// local vertical in root coordinates at target position
	TFreeVector targetVertical(0, 0, 1, TCoordSysFactory::k3DCartesian);
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		// temporarily set MLA origin to target, only way to get local vertical
		fPointTransfo.set2MLATransformation(targetPos);
		fPointTransfo.transformMLA2CGRF(targetVertical);
		fPointTransfo.transformCGRF2CCS(targetVertical);
	}


	TPositionVector targetWithHeight = targetPos + targetVertical * dist.target.targetHt;

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true)
	{	// set the correct origin of the station MLA
		fPointTransfo.set2MLATransformation(stationPos);
		fPointTransfo.transformPointsToMLASystem(station->instrumentPos->getName(), stationPos, targetWithHeight);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal hInst = station->instrumentHeightAdjustable->getEstimatedValue();
	// in station system
	TPositionVector instrumentWithHeight = stationPos + TPositionVector(0, 0, hInst, TCoordSysFactory::k3DCartesian);

	TVector relPos = TFreeVector(targetWithHeight - instrumentWithHeight).toRealVector();

	TReal cst;
	if (!dist.target.distCorrectionAdjustable->isFixed())
		cst = dist.target.distCorrectionAdjustable->getEstimatedValue();
	else
		cst = dist.target.distCorrectionValue;

	TReal D = relPos.norm();

	// derivative of the distance with respect to station and target coordinates
	Eigen::Matrix<double, 1, 3> dFdT = relPos.transpose() / D;
	Eigen::Matrix<double, 1, 3> dFdS = -dFdT;
	if (D < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getSpatialDistanceContrib: Division by zero because station and target are identical or have identical coordinates. Points: "
			+ getNameAndLine(*station->instrumentPos) + " and " + getNameAndLine(*dist.targetPos));
	}

	// Calculate and return the contributions
	TReal calcMeas = D - cst; // Calculated measurement value to be returned
	TReal hiContrib = dFdS(2); // Instrument height contribution to be returned

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, dFdS(0), dFdS(1), dFdS(2)); // Contribution to a STATION point
	// Station transformation contribution
	addTransformationsContributions(stLor2RootTrafo, station->instrumentPos->getEstimatedValue(), dFdS, stationTransfContributions);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, dFdT(0), dFdT(1), dFdT(2)); // Contribution to a TARGET point
	// Target transformation contribution
	addTransformationsContributions(tgLor2RootTrafo, dist.targetPos->getEstimatedValue(), dFdT, targetTransfContributions);

	TReal distCorrContrib = -1.0;

	// Variance calculation
	TReal varM = pow2q(dist.target.sigmaDist + calcMeas / 1000 * dist.target.ppmDist);
	TReal varInstHeight = pow2q(station->instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(dist.target.sigmaTargetHt);
	TReal varInstCent = pow2q(station->instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(dist.target.sigmaTargetCentering);
	TReal variance = varM + pow2q(dFdT(2)) * (varInstHeight + varTgHeight) + ((pow2q(relPos(1)) + pow2q(relPos(0))) / pow2q(D)) * (varInstCent + varTgCent);

	DistMeasContrib contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, distCorrContrib, variance};
	return contrib;
}

DistMeasContribFrame TContributionsGenerator::getSpatialDistanceContribInFrame(std::shared_ptr<TTSTN> station, const TLINE &dist)
{
	ModelAndJacobian distanceModel;
	distanceModel.func = [](const Eigen::Vector3d &relPos) -> modelEval {
		if (isZero(relPos.norm()))
		{
			return {0.0, Eigen::RowVector3d::Zero(), false, "Relative position is zero, cannot evaluate Jacobian of distance function."};
		}
		return {relPos.norm(), relPos.transpose() / relPos.norm(), true, ""};
	};

	PolarContribInFrame spatialDistContribInFrame = getPolarContribInFrame(station, dist, distanceModel);

	// distance correction
	TReal cst;
	if (!dist.target.distCorrectionAdjustable->isFixed())
		cst = dist.target.distCorrectionAdjustable->getEstimatedValue();
	else
		cst = dist.target.distCorrectionValue;
	double calcMeas = spatialDistContribInFrame.fModelPrediction - cst;


	// Variance calculation
	TReal varM = pow2q(dist.target.sigmaDist + dist.getDistance() / 1000 * dist.target.ppmDist);
	double varianceFixedParameterContrib = spatialDistContribInFrame.fFixedParametersVarianceContribution;
	TReal variance = varM + varianceFixedParameterContrib;
	// Fill the contribution structure
	DistMeasContribFrame contrib = {calcMeas, spatialDistContribInFrame, 0.0, -1.0, variance};
	return contrib;
}

// Horizontal angle contributions
AnglMeasContrib TContributionsGenerator::getHorAnglContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TANGL &angl)
{
	// Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = angl.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(angl.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// local vertical in root coordinates at target position
	TFreeVector targetVertical(0, 0, 1, TCoordSysFactory::k3DCartesian);
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		// temporarily set MLA origin to target, only way to get local vertical
		fPointTransfo.set2MLATransformation(targetPos);
		fPointTransfo.transformMLA2CGRF(targetVertical);
		fPointTransfo.transformCGRF2CCS(targetVertical);
	}

	TPositionVector targetWithHeight = targetPos + targetVertical * angl.target.targetHt;
	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true)
	{	// set the correct origin of the station MLA
		fPointTransfo.set2MLATransformation(stationPos);
		fPointTransfo.transformPointsToMLASystem(station->instrumentPos->getName(), stationPos, targetWithHeight);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TVector relPos = TFreeVector(targetWithHeight - stationPos).toRealVector();

	// Calculated measurement value, instrument height has no influence
	TAngle calcMeas = TAngle::aTan2(relPos(0), relPos(1)) - rom->v0->getEstimatedValue() - rom->acst; // ACST is the constant orientation of the instrument

	//TReal dist2 = pow2q(dist(xSt, ySt, xTg, yTg));
	TReal dist2 = pow2q(relPos.topRows(2).norm());
	if (dist2 < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getHorAnglContrib: Division by zero because station and target are identical or have identical coordinates. Points: "
			+ getNameAndLine(*station->instrumentPos) + " and " + getNameAndLine(*angl.targetPos));
	}

	Eigen::Matrix<double, 1, 3> dFdT;
	dFdT << relPos(1) / dist2, -relPos(0) / dist2, 0;
	Eigen::Matrix<double, 1, 3> dFdS = -dFdT;

	TReal v0Contrib = -1.0; // contribution for the V0 parameter
	TReal hiContrib = 0.0; // no contribution for the instrument height

	// Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, dFdS(0), dFdS(1), dFdS(2));
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, station->instrumentPos->getEstimatedValue(), dFdS, stationTransfContributions);

	// Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, dFdT(0), dFdT(1), dFdT(2));
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, angl.targetPos->getEstimatedValue(), dFdT, targetTransfContributions);

	// Variance calculation
	TReal variance = pow2q(angl.target.sigmaAngl.getRadiansValue()) + (1.0 / (dist2)) * (pow2q(station->instrument.sigmaInstrCentering) + pow2q(angl.target.sigmaTargetCentering));

	AnglMeasContrib contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

AnglMeasContribFrame TContributionsGenerator::getHorAnglContribInFrame(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TANGL &angl)
{
	ModelAndJacobian horAnglModel;
	horAnglModel.func = [](const Eigen::Vector3d &relPos) -> modelEval {
		const double x = relPos(0);
		const double y = relPos(1);

		// horizontal distance squared
		const double dist2 = x * x + y * y;

		// model value: note your convention is atan2(x, y)
		const double angle = std::atan2(x, y);

		if (isZero(dist2))
		{
			return {angle, Eigen::RowVector3d::Zero(), false, "Horizontal angle Jacobian undefined: (identical x/y coordinates)."};
		}

		Eigen::RowVector3d jac;
		jac << y / dist2, -x / dist2, 0.0;

		return {angle, jac, true, ""};
	};

	PolarContribInFrame horAnglContribInFrame = getPolarContribInFrame(station, angl, horAnglModel);

	// Calculated measurement value
	TAngle calcMeas = TAngle(horAnglContribInFrame.fModelPrediction) - rom->v0->getEstimatedValue() - rom->acst; // ACST is the constant orientation of the instrument
	// Variance calculation
	double varianceFixedParameterContrib = horAnglContribInFrame.fFixedParametersVarianceContribution;
	TReal variance = pow2q(angl.target.sigmaAngl.getRadiansValue()) + varianceFixedParameterContrib;
	AnglMeasContribFrame contrib = {calcMeas, horAnglContribInFrame, -1.0, 0.0, variance};
	return contrib;
}

// Zenith distance (vertical angle) contributions
AnglMeasContrib TContributionsGenerator::getZenDistContrib(std::shared_ptr<TTSTN> station, const TZEND &zend)
{
	// Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = zend.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(zend.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station->instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	// Prepare coefficients (a,b,c) for the points and the transformations contributions
	// PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();

	TReal hTg = zend.target.targetHt;
	TReal hInst = station->instrumentHeightAdjustable->getEstimatedValue();

	TReal dx = xTg - xSt;
	TReal dy = yTg - ySt;
	TReal dz = zTg - zSt - hInst + hTg;

	TReal distance3D = dist3D(xSt, ySt, zSt + hInst, xTg, yTg, zTg + hTg);
	if (distance3D < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getZenDistContrib: Division by zero because station and target are identical or have identical coordinates. Points: "
			+ getNameAndLine(*station->instrumentPos) + " and " + getNameAndLine(*zend.targetPos));
	}

	TAngle calcMeas = TAngle::aCos((zTg - zSt - hInst + hTg) / distance3D);

	TReal sinPhi = sinq(calcMeas.getRadiansValue());

	if (fabs(sinPhi) < nullLimit)
	{
		generateContributionError("TContributionGenerator::getZenDistContrib: Division by zero because Sinus of Zend angle between station and target is zero. Points: "
			+ getNameAndLine(*station->instrumentPos) + " and " + getNameAndLine(*zend.targetPos));
	}

	TReal a, b, c; // station's contributions coefficients (negative values of these give target's coefficients)
	a = (-1.0 * dz * dx) / (powq(distance3D, 3) * sinPhi); // xSt coefficient
	b = (-1.0 * dz * dy) / (powq(distance3D, 3) * sinPhi); // ySt coefficient
	c = (1.0 / (distance3D * sinPhi)) - powq(dz, 2) / (powq(distance3D, 3) * sinPhi); // zSt coefficient

	TReal hiContrib = c; // instrument height contribution
	TReal v0Contrib = 0.0; // no contribution for the v0 parameter

	// Calculate and return the contributions
	// Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	// Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, zend.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	TReal variance = pow2q(zend.target.sigmaZenD.getRadiansValue())
		+ (((pow2q(dx) + pow2q(dy)) * pow2q(dz)) / (powq(distance3D, 6) * pow2q(sinPhi))) * (pow2q(station->instrument.sigmaInstrCentering) + pow2q(zend.target.sigmaTargetCentering))
		+ pow2q(-c) * (pow2q(station->instrument.sigmaInstrHeight) + pow2q(zend.target.sigmaTargetHt));

	AnglMeasContrib contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

AnglMeasContribFrame TContributionsGenerator::getZenDistContribInFrame(std::shared_ptr<TTSTN> station, const TZEND &zend)
{
	ModelAndJacobian vertAnglModel;
	vertAnglModel.func = [](const Eigen::Vector3d &relPos) -> modelEval {
		const double x = relPos(0);
		const double y = relPos(1);
		const double z = relPos(2);

		const double r2 = relPos.squaredNorm();
		if (isZero(r2))
		{
			return {0.0, Eigen::RowVector3d::Zero(), false, "Vertical angle undefined: relative position is zero."};
		}

		const double r = std::sqrt(r2);

		// Clamp for numerical safety
		double c = std::clamp(z / r, -1.0, 1.0);

		const double angle = std::acos(c);

		const double rho = std::sqrt(x * x + y * y);
		if (isZero(rho))
		{
			return {angle, Eigen::RowVector3d::Zero(), false, "Vertical angle Jacobian undefined: horizontal projection is near zero."};
		}

		const double denom = rho * r2;

		Eigen::RowVector3d jac;
		jac << (x * z) / denom, (y * z) / denom, -rho / r2;

		return {angle, jac, true, ""};
	};

	PolarContribInFrame vertAnglContribInFrame = getPolarContribInFrame(station, zend, vertAnglModel);

	TAngle calcMeas(vertAnglContribInFrame.fModelPrediction);

	// Variance calculation
	double varianceFixedParameterContrib = vertAnglContribInFrame.fFixedParametersVarianceContribution;
	TReal variance = pow2q(zend.target.sigmaZenD.getRadiansValue()) + varianceFixedParameterContrib;

	AnglMeasContribFrame contrib = {calcMeas, vertAnglContribInFrame, 0.0, 0.0, variance};
	return contrib;
}

// PLR3D Contribution
PLR3DContrib TContributionsGenerator::getPolar3DContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TPLR3D &plr3D)
{
	// parametric version of polar 3D measurement

	// MISCLOSURE
	// Position of target relative to station
	TPositionVector targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		plr3D.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station->instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TFreeVector relPos(TCoordSysFactory::k3DCartesian);
	relPos = targetPos - stationPos;
	// correct z coordinate using prism height and instrument height
	TReal instrHeight = station->instrumentHeightAdjustable->getEstimatedValue();
	TReal prismHeight = plr3D.target.targetHt;
	TReal distCorrection = plr3D.target.distCorrectionAdjustable->getEstimatedValue();
	relPos.setZ(relPos.getZ() + TLength(prismHeight - instrHeight));
	TReal Rx(0), Ry(0); // Rotation around x/y-axis, default is no rotation

	if (station->rot3D)
	{ // If station can rotate freely get the rotation values
		if (station->rotX == nullptr || station->rotY == nullptr)
			throw std::runtime_error("TContributionGenerator::getPolar3DContrib station can rotate freely, but rotation angles are not defined.");
		Rx = station->rotX->getEstimatedValue().getRadiansValue();
		Ry = station->rotY->getEstimatedValue().getRadiansValue();
	}
	TReal V0((rom->v0->getEstimatedValue() + rom->acst));

	TInverseTransformation ARotationMat;
	//  This represents the "A" matrix in https://edms.cern.ch/document/1465539/3
	ARotationMat.setTransformation(0, 0, 0, Rx, Ry, V0, 1);

	// measuredPos = position as seen from the station, taking into account bearing, Rx, Ry
	Eigen::Vector3d measuredPos = (ARotationMat * relPos).toRealVector();

	// compute "calcMeas"
	// norm of measured pos is the same as rel pos because only rotations are applied
	TReal normMeasuredPos = relPos.length();
	if (normMeasuredPos < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getPolar3DContrib: Division by zero because station and target are identical or have identical coordinates. Points: "
			+ getNameAndLine(*station->instrumentPos) + " and " + getNameAndLine(*plr3D.targetPos));
	}

	// calcMeas = F(measuredPos)-(0,0,distCorrection)^T
	TReal x(measuredPos[0]), y(measuredPos[1]), z(measuredPos[2]);
	Eigen::Vector3d calcMeas(TAngle::aTan2(x, y).getRadiansValue(), // "ANGL"
		TAngle::aCos(z / normMeasuredPos).getRadiansValue(), // "ZEND"
		normMeasuredPos - distCorrection); // "Dist"
	// Jacobian of F with respect to measuredPos
	TDenseMatrix JacF(3, 3);
	TReal normMeasuredPosXY = sqrt(pow2q(x) + pow2q(y));
	JacF << y / pow2q(normMeasuredPosXY), -x / pow2q(normMeasuredPosXY), 0, x * z / (normMeasuredPosXY * pow2q(normMeasuredPos)),
		y * z / (normMeasuredPosXY * pow2q(normMeasuredPos)), -normMeasuredPosXY / pow2q(normMeasuredPos), x / normMeasuredPos, y / normMeasuredPos, z / normMeasuredPos;

	PLR3DContrib contrib;

	// both angles need to be normalized (in the range -2pi, 2pi)
	contrib.fCalcMeas = calcMeas;

	// Derivatives
	// distance correction value cs contribution
	contrib.fDistAndCsContrib = Eigen::Vector3d(0, 0, -1);
	// get the rotation matrix block of the "A" matrix
	Eigen::Matrix3d rotationPart = ARotationMat.getMatrix().block(0, 0, 3, 3);
	Eigen::Matrix3d dFdPosA = JacF * rotationPart;
	Point3DContrib tgContrib, stContrib;
	addPointContributionsPLR3D(tgLor2RootTrafo, dFdPosA, tgContrib, false);
	addPointContributionsPLR3D(stLor2RootTrafo, dFdPosA, stContrib, true);

	// target and station pos contribution
	contrib.fTgCoordContrib = tgContrib;
	contrib.fStCoordContrib = stContrib;

	// target and station Helmert transformation parameter contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector for target transformations contributions
	const TPositionVector &tgPointInLOR = plr3D.targetPos->getEstimatedValue();
	addTransformationsContributions3D(tgLor2RootTrafo, tgPointInLOR, TFreeVector(dFdPosA.row(0)), TFreeVector(dFdPosA.row(1)), TFreeVector(dFdPosA.row(2)), targetTransfContributions);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> stationTransfContributions; // Vector for station transformations contributions
	const TPositionVector &stPointInLOR = station->instrumentPos->getEstimatedValue();
	addTransformationsContributions3D(stLor2RootTrafo, stPointInLOR, TFreeVector(-dFdPosA.row(0)), TFreeVector(-dFdPosA.row(1)), TFreeVector(-dFdPosA.row(2)), stationTransfContributions);
	contrib.fTgTransformContrib = targetTransfContributions;
	contrib.fStTransformContrib = stationTransfContributions;

	// instrument height and target height contribution
	contrib.fInstrHeightContrib = dFdPosA.col(2); // = dFdPosA* [0,0,1]^T
	contrib.fTargetHeightContrib = -contrib.fInstrHeightContrib;

	// Rx Ry V0 contribs
	// derivatives of the rotation matrix
	TDerivativeTransformation dAdRx = ARotationMat.differentiatedTransformationAngle(0);
	TDerivativeTransformation dAdRy = ARotationMat.differentiatedTransformationAngle(1);
	TDerivativeTransformation dAdV0 = ARotationMat.differentiatedTransformationAngle(2);

	contrib.fRxContrib = JacF * (dAdRx * relPos).toRealVector();
	contrib.fRyContrib = JacF * (dAdRy * relPos).toRealVector();
	contrib.fV0Contrib = JacF * (dAdV0 * relPos).toRealVector();

	// Variance calculation -- same as in original combined version
	double dist2 = pow2q(normMeasuredPosXY);
	double dX = relPos[0], dY = relPos[1], dZ = relPos[2];
	double distance3D = relPos.length();
	TReal sinPhi = dist2 / distance3D;
	if (fabs(sinPhi) < nullLimit)
	{
		generateContributionError("TContributionGenerator::getPolar3DContrib: Division by zero because ZEND angle is zero. Points: " + getNameAndLine(*station->instrumentPos)
			+ " and " + getNameAndLine(*plr3D.targetPos));
	}

	TReal c = (1.0 / (distance3D * sinPhi)) - powq(dZ, 2) / (powq(distance3D, 3) * sinPhi);
	TVector obsVariance(3);
	// ANGL
	obsVariance(0) = pow2q(plr3D.target.sigmaAngl.getRadiansValue()) + (1.0 / (dist2)) * (pow2q(station->instrument.sigmaInstrCentering) + pow2q(plr3D.target.sigmaTargetCentering));
	// ZEND
	obsVariance(1) = pow2q(plr3D.target.sigmaZenD.getRadiansValue())
		+ (((pow2q(dX) + pow2q(dY)) * pow2q(dZ)) / (powq(distance3D, 6) * pow2q(sinPhi))) * (pow2q(station->instrument.sigmaInstrCentering) + pow2q(plr3D.target.sigmaTargetCentering))
		+ pow2q(-c) * (pow2q(station->instrument.sigmaInstrHeight) + pow2q(plr3D.target.sigmaTargetHt));
	// DIST
	TReal varM = pow2q(plr3D.target.sigmaDist + calcMeas(2) / 1000 * plr3D.target.ppmDist);
	TReal varInstHeight = pow2q(station->instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(plr3D.target.sigmaTargetHt);
	TReal varInstCent = pow2q(station->instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(plr3D.target.sigmaTargetCentering);
	obsVariance(2) = varM + pow2q((dZ) / distance3D) * (varInstHeight + varTgHeight) + ((pow2q(dY) + pow2q(dX)) / pow2q(distance3D)) * (varInstCent + varTgCent);
	contrib.fObsVariance = obsVariance;

	return contrib;
}

// Horizontal distance contributions, measurement made by TSTN
HorDistContrib TContributionsGenerator::getHorDistContrib(std::shared_ptr<TTSTN> station, const TLINE &dhor)
{
	TReal calcMeas;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

	// Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = dhor.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(dhor.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station->instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	TReal cte;
	if (!dhor.target.distCorrectionAdjustable->isFixed())
		cte = dhor.target.distCorrectionAdjustable->getEstimatedValue();
	else
		cte = dhor.target.distCorrectionValue;

	TReal D = dist(xSt, ySt, xTg, yTg);

	if (D < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getHorDistContrib: Division by zero because station and target are identical or have identical coordinates. Points: "
			+ getNameAndLine(*station->instrumentPos) + " and " + getNameAndLine(*dhor.targetPos));
	}

	TReal a, b, c; // station's contributions coefficients (negative values of these give target's coefficients)
	a = -(xTg - xSt) / D; // xSt coefficient
	b = -(yTg - ySt) / D; // ySt coefficient
	c = 0.0; // zSt coefficient

	// Station can be defined anywhere, get point contributions and transformations contributions
	coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	addTransformationsContributions(stLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	// Target can be defined anywhere, get point contributions and transformations contributions
	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, dhor.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	calcMeas = D - cte;
	TReal distCorrCont = -1.0;

	// Variance
	TReal varM = pow2q(dhor.target.sigmaDist + calcMeas / 1000 * dhor.target.ppmDist);
	TReal variance = varM + (pow2q(station->instrument.sigmaInstrCentering) + pow2q(dhor.target.sigmaTargetCentering));

	HorDistContrib contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, distCorrCont, variance};
	return contrib;
}

// ECTH Contribution
ECTHContrib TContributionsGenerator::getECTHContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TECTH &ecth)
{
	///////////////////Transform TARGET and STATION from their's LOR either to ROOT or to MLA of the station///////////////////////////////
	TPositionVector targetPos = station->instrumentPos->getEstimatedValue(); // position of the scale. Point to measure
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = ecth.targetPos->getEstimatedValue(); // position of the TSTN
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(ecth.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(ecth.targetPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	TAngle theta = ecth.obsHorAngle;
	TAngle Vo = rom->v0->getEstimatedValue();
	TReal a, b, c; // station's contributions coefficients (negative values of these give target's coefficients)
	a = -cos(theta + Vo); // xSt coefficient
	b = sin(theta + Vo); // ySt coefficient
	c = 0.0; // zSt coefficient

	TReal calcMeas = a * (xSt - xTg) + b * (ySt - yTg) - ecth.target.distCorrectionValue.getMetresValue();

	TReal V0Contrib = b * (xSt - xTg) - a * (ySt - yTg); // contribution for the V0 parameter
	TReal distCorrection = -1.0; // Not use for the moment, because it is not adjustable.

	// Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, ecth.targetPos->getEstimatedValue(), -a, -b, -c, stationTransfContributions);

	// Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, a, b, c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, targetTransfContributions);

	// Variance calculation
	TReal varM = pow2q(ecth.target.sigmaD + calcMeas / 1000 * ecth.target.ppmD);
	TReal variance = varM + (pow2q(cos(theta + Vo)) + pow2q(sin(theta + Vo))) * pow2q(ecth.target.sigmaInstrCentering);

	ECTHContrib contrib = {calcMeas, coordContribStation, coordContribTarget, V0Contrib, distCorrection, stationTransfContributions, targetTransfContributions, variance};
	return contrib;
}

// ECDIR contribution
ECTHContrib TContributionsGenerator::getECDIRContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TECDIR &ecdir)
{
	///////////////////Transform TARGET and STATION from their's LOR to ROOT///////////////////////////////
	TPositionVector targetPos = station->instrumentPos->getEstimatedValue(); // position of the scale. Point to measure
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = ecdir.targetPos->getEstimatedValue(); // position of the TSTN
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		ecdir.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(ecdir.targetPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();
	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();

	TAngle theta = ecdir.obsHorAngle;
	TAngle phi = ecdir.obsVertAngle;
	TAngle Vo = rom->v0->getEstimatedValue();
	// line direction at the TSTN position
	TFreeVector l(sin(theta + Vo) * sin(phi), cos(theta + Vo) * sin(phi), cos(phi), TCoordSysFactory::ECoordSys::k3DCartesian);

	// Calcul par le produit scalaire (u^l)^2+(u.l)^2=|v|^2|l|^2
	TReal d, pScal;
	d = sqrt(pow2(xSt - xTg) + pow2(ySt - yTg) + pow2(zSt - zTg)); // distance St-Tg
	pScal = l[0] * (xSt - xTg) + l[1] * (ySt - yTg) + l[2] * (zSt - zTg); // produit scalaire

	TReal calcMeas = sqrt(pow2(d) - pow2(pScal)) - ecdir.target.distCorrectionValue.getMetresValue();

	// contributions
	TReal a, b, c, V0Contrib;
	if (fabs(calcMeas) > nullLimit)
	{
		a = ((xSt - xTg) - l[0] * pScal) / calcMeas;
		b = ((ySt - yTg) - l[1] * pScal) / calcMeas;
		c = ((zSt - zTg) - l[2] * pScal) / calcMeas;
		V0Contrib = -1 * (l[1] * (xSt - xTg) - l[0] * (ySt - yTg)) / calcMeas; /**/
	}
	else
	{
		generateContributionError("TContributionGenerator::getECDIRContrib: Division by zero because the point " + getNameAndLine(*ecdir.targetPos) + " is on the line.");
	}

	TReal distCorrection = -1.0; // Not use for the moment, because it is not adjustable.

	// Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, ecdir.targetPos->getEstimatedValue(), -a, -b, -c, stationTransfContributions);

	// Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, a, b, c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, targetTransfContributions);

	// Variance calculation
	TReal varM = pow2q(ecdir.target.sigmaD + calcMeas / 1000 * ecdir.target.ppmD);
	TReal variance = varM + (pow2q(cos(theta + Vo) * sin(phi)) + pow2q(sin(theta + Vo) * sin(phi))) * pow2q(ecdir.target.sigmaInstrCentering);

	ECTHContrib contrib = {calcMeas, coordContribStation, coordContribTarget, V0Contrib, distCorrection, stationTransfContributions, targetTransfContributions, variance};
	return contrib;
}
//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- individual measurements
//////////////////////////////////////////////////////////////////////

// Spatial distance made by an EDM
DistMeasContrib TContributionsGenerator::getDSPTContrib(const TEDM &edmST, const TDSPT &dspt)
{
	TPositionVector targetPos = dspt.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(dspt.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = edmST.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		edmST.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		fPointTransfo.transformPointsToMLASystem(edmST.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);
	/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();

	TReal hTg = dspt.target.targetHt;
	TReal hInst = edmST.instrument.instrHeight;

	TReal cst;
	if (!dspt.target.distCorrectionAdjustable->isFixed())
		cst = dspt.target.distCorrectionAdjustable->getEstimatedValue();
	else
		cst = dspt.target.distCorrectionValue;

	TReal D = dist3D(xSt, ySt, (zSt + hInst), xTg, yTg, (zTg + hTg));

	if (D < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getDSPTContrib: Division by zero because station and target are identical or have identical coordinates. Points: "
			+ getNameAndLine(*edmST.instrumentPos) + " and " + getNameAndLine(*dspt.targetPos));
	}

	TReal a, b, c; // station's contributions coefficients (negative values of these give target's coefficients)
	a = (xSt - xTg) / D; // xSt coefficient
	b = (ySt - yTg) / D; // ySt coefficient
	c = (zSt + hInst - zTg - hTg) / D; // zSt coefficient

	TReal calcMeas = D - cst;
	TReal hiContrib = 0.0; // instrument height is always FIXED, no contribution!
	TReal distCorrection = -1.0;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	addTransformationsContributions(stLor2RootTrafo, edmST.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	// Target can be defined anywhere, get point contributions and transformations contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, dspt.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	// Variance calculation
	TReal varM = pow2q(dspt.target.sigmaDSpt + calcMeas / 1000 * dspt.target.ppmDSpt);
	TReal varInstHeight = pow2q(edmST.instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(dspt.target.sigmaTargetHt);
	TReal varInstCent = pow2q(edmST.instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(dspt.target.sigmaTargetCentering);
	TReal variance = varM + pow2q((zTg - zSt + hTg - hInst) / D) * (varInstHeight + varTgHeight) + ((pow2q(yTg - ySt) + pow2q(xSt - xTg)) / pow2q(D)) * (varInstCent + varTgCent);

	DistMeasContrib contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, distCorrection, variance};
	return contrib;
}

DLEVCombinedContrib TContributionsGenerator::getDLEVContribCombined(const TLEVEL &levelInstr, const TDLEV &dlev)
{
	// get all the points in the root frame
	TPositionVector referencePointPosition = levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue();
	const TLOR2LOR &refPTLor2RootTrafo = fPointTransfo.getLORTransformation(
		levelInstr.fMeasuredPlane->getReferencePoint()->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	refPTLor2RootTrafo.transform(referencePointPosition);

	TPositionVector staffPosition = dlev.targetPos->getEstimatedValue();
	const TLOR2LOR &staffPTLor2RootTrafo = fPointTransfo.getLORTransformation(dlev.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	staffPTLor2RootTrafo.transform(staffPosition);

	// get all the corrections
	const TReal collAngl = levelInstr.instrument.collAngleAdjustable->getEstimatedValue().getRadiansValue(); // Collimation Angle
	const TReal tanColl = tanq(collAngl);
	const TReal secantSquaredColl = 1.0 + pow2q(tanColl);
	TReal cdz = dlev.target.distCorrectionValue.getMetresValue(); // Correction on the reading
	TReal tgHeight = dlev.target.staffHt.getMetresValue(); // Target Height
	TReal dRef = levelInstr.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(); // Instrument Height over the point

	// Local vertical at the staff position, expressed in CCS (identity in OLOC).
	TFreeVector staffVerticalVector = fPointTransfo.getLocalVerticalInCCS(dlev.targetPos->getName(), staffPosition);

	// If not OLOC => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		// Transform the local vertical into the station MLA
		fPointTransfo.transformVectorToMLASystem(levelInstr.fMeasuredPlane->getReferencePoint()->getName(), referencePointPosition, staffVerticalVector);
		staffVerticalVector.normalize();

		// Transform the target point in the station MLA
		fPointTransfo.transformPointsToMLASystem(levelInstr.fMeasuredPlane->getReferencePoint()->getName(), referencePointPosition, staffPosition);
		fPointTransfo.setMLA(true);
	}
	else
	{
		fPointTransfo.setMLA(false);
	}

	// In MLA the reference point is at the origin; in OLOC these are root-frame coords.
	TReal xSt = referencePointPosition.getX().getMetresValue();
	TReal ySt = referencePointPosition.getY().getMetresValue();
	// vz < 1 in non-OLOC because the staff local vertical is not aligned with the MLA Z-axis.
	TReal vz = staffVerticalVector.getZ().getMetresValue();

	if (std::abs(vz) < nullLimit)
	{
		generateContributionError("TContributionGenerator::getDLEVContribCombined: No intersection found between the horizontal plane and the vertical vector at the "
								  "level of the target. Points: "
			+ getNameAndLine(*levelInstr.fMeasuredPlane->getReferencePoint()) + " and " + getNameAndLine(*dlev.targetPos));
	}

	// Intersect the staff vertical with the horizontal plane at instrument height to get the horizontal distance.
	// The collimation tilt is then applied to that distance. For leveling, epsilon is small so this is accurate.
	TReal scale0 = (dRef - staffPosition.getZ().getMetresValue()) / vz;
	TPositionVector intersection0 = staffPosition + staffVerticalVector * scale0;

	TReal xTgInPlane = intersection0.getX().getMetresValue();
	TReal yTgInPlane = intersection0.getY().getMetresValue();

	TReal dhorGeom = dist(xSt, ySt, xTgInPlane, yTgInPlane);

	TReal dx = xSt - xTgInPlane;
	TReal dy = ySt - yTgInPlane;

	// DLEV observation equation. The division by vz converts the working-frame height difference
	// to the staff reading length (vz = 1 in OLOC).
	TReal calcMeas = (referencePointPosition.getZ().getMetresValue() + dRef - dhorGeom * tanColl - staffPosition.getZ().getMetresValue()) / vz - (cdz + tgHeight);

	// Partial derivatives of calcMeas w.r.t. the horizontal coords of staff/refPt.
	TReal a = 0;
	TReal b = 0;
	if (dhorGeom > nullLimit)
	{
		// not an error if station and target share the same horizontal position
		a = -dx * tanColl / (dhorGeom * vz);
		b = -dy * tanColl / (dhorGeom * vz);
	}
	TReal c = 1 / vz;

	// staff point contribution
	TFreeVector staffContrib = getPointContributions(staffPTLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> staffTransfContributions;
	addTransformationsContributions(staffPTLor2RootTrafo, dlev.targetPos->getEstimatedValue(), -a, -b, -c, staffTransfContributions);

	// reference point contribution
	TFreeVector referencePTContrib = getPointContributions(refPTLor2RootTrafo, a, b, c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> referencePTTransfContributions;
	addTransformationsContributions(refPTLor2RootTrafo, levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue(), a, b, c, referencePTTransfContributions);

	TReal collAngleContrib = -dhorGeom * secantSquaredColl / vz;
	TReal fRefPtDistCont = c; // d(calcMeas)/d(dRef) = 1/vz

	// sigmaInstrHeight is in MLA Z-units; d(calcMeas)/d(IH) = 1/vz so propagate through 1/vz.
	TReal varIH = 0.0;
	if (levelInstr.ihfix)
		varIH = pow2q(levelInstr.instrument.sigmaInstrHeight.getMetresValue() / vz);
	TReal variance = pow2q(dlev.target.sigmaD.getMetresValue() + dhorGeom / 1000 * dlev.target.ppmD.getMetresValue()) + pow2q(dlev.target.sigmaStaffHt.getMetresValue())
		+ varIH + pow2q(dlev.target.sigmaDCorr.getMetresValue());

	DLEVContrib dlevContrib = {calcMeas, staffContrib, referencePTContrib, staffTransfContributions, referencePTTransfContributions, fRefPtDistCont, collAngleContrib, variance};

	HorDistContribLEVEL dhorContrib;
	if (dlev.dhor)
	{
		// obs_DH and DHDCOR are both along the collimation direction (slant distance model).
		if (dhorGeom <= nullLimit)
		{
			generateContributionError(
				"TContributionGenerator::getDLEVContribCombined: Division by zero because x and y coordinates of station and target are identical. Points: "
				+ getNameAndLine(*levelInstr.fMeasuredPlane->getReferencePoint()) + " and " + getNameAndLine(*dlev.dhor->targetPos));
		}
		const TReal cosColl = cosq(collAngl);
		TReal calcMeasDhor = dhorGeom / cosColl - dlev.dhor->target.dhorCorrectionValue.getMetresValue();
		TReal vx = staffVerticalVector.getX().getMetresValue();
		TReal vy = staffVerticalVector.getY().getMetresValue();

		// Partial derivatives of the slant distance w.r.t. target and station coordinates.
		// cDhor is nonzero only in non-OLOC (P = 0 in OLOC).
		const TReal horizontalProjection = dx * vx + dy * vy;
		TReal dhorDenominator = dhorGeom * cosColl;
		TReal aDhor = -dx / dhorDenominator;
		TReal bDhor = -dy / dhorDenominator;
		TReal cDhor = horizontalProjection / (dhorDenominator * vz);

		// staff contributions for DHOR
		TFreeVector staffContribDhor(TCoordSysFactory::k3DCartesian);
		std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> staffTransfContributionsDhor;
		staffContribDhor = getPointContributions(staffPTLor2RootTrafo, aDhor, bDhor, cDhor);
		addTransformationsContributions(staffPTLor2RootTrafo, dlev.targetPos->getEstimatedValue(), aDhor, bDhor, cDhor, staffTransfContributionsDhor);

		// reference point contributions for DHOR
		TFreeVector referencePTContribDhor(TCoordSysFactory::k3DCartesian);
		std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> referencePTTransfContributionsDhor;
		referencePTContribDhor = getPointContributions(refPTLor2RootTrafo, -aDhor, -bDhor, -cDhor);
		addTransformationsContributions(refPTLor2RootTrafo, levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue(), -aDhor, -bDhor, -cDhor, referencePTTransfContributionsDhor);

		// IH derivative for DHOR; zero in OLOC.
		TReal fRefPtDistContDhor = -cDhor;

		// Collimation angle derivative: intersection shift term plus direct cosine term. DHDCOR is treated as exact.
		const TReal intersectionShiftTerm = horizontalProjection * secantSquaredColl / vz;
		const TReal dhorTangentTerm = dhorGeom * tanColl;
		const TReal dhorCollAngleContrib = (intersectionShiftTerm + dhorTangentTerm) / cosColl;

		// Variance: obs_DH reading precision plus IH uncertainty (nonzero only in non-OLOC with IHFIX+IHSE).
		// DHDCOR has no sigma field and is treated as exact.
		TReal varIHDhor = 0.0;
		if (levelInstr.ihfix)
			varIHDhor = pow2q(cDhor * levelInstr.instrument.sigmaInstrHeight.getMetresValue());
		TReal varianceDhor = pow2q(dlev.dhor->target.sigmaDHor.getMetresValue() + calcMeasDhor / 1000 * dlev.dhor->target.ppmDHor) + varIHDhor;
		dhorContrib = {calcMeasDhor, staffContribDhor, referencePTContribDhor, staffTransfContributionsDhor, referencePTTransfContributionsDhor, fRefPtDistContDhor,
			dhorCollAngleContrib, varianceDhor};
	}
	return {dlevContrib, dhorContrib};
}

// ECHO contribution
ECHOContrib TContributionsGenerator::getECHOContrib(const TECHOROM &echoROM, const TECHO &echo)
{
	TReal theta = echoROM.fMeasuredPlane->getThetaEstimatedValue().getRadiansValue(); // Theta angle of the plane
	TReal cEcVp = echo.target.distCorrectionValue; // distance of the target correction value
	TReal dRef = echoROM.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(); // distance of the reference point from the plane

	TPositionVector stationPoint = echo.targetPos->getEstimatedValue();

	const TLOR2LOR &stationPTLor2RootTrafo = fPointTransfo.getLORTransformation(echo.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	stationPTLor2RootTrafo.transform(stationPoint);

	/*Reference point is always defined in ROOT and is fixed (it is implicitly defined),
	i.e. no transformation to ROOT required and no contribution required for its coordinates.*/
	TPositionVector referencePoint = echoROM.fMeasuredPlane->getReferencePoint()->getEstimatedValue();

	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		fPointTransfo.transformPointsToMLASystem(echoROM.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, stationPoint);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal calcMeas = -cosq(theta) * (stationPoint.getX() - referencePoint.getX()).getMetresValue()
		+ sinq(theta) * (stationPoint.getY() - referencePoint.getY()).getMetresValue() + dRef - cEcVp;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector stationContrib = getPointContributions(stationPTLor2RootTrafo, -cosq(theta), sinq(theta), 0.0);
	addTransformationsContributions(stationPTLor2RootTrafo, echo.targetPos->getEstimatedValue(), -cosq(theta), sinq(theta), 0.0, stationTransfContributions);

	TReal thetaContrib = sinq(theta) * (stationPoint.getX() - referencePoint.getX()).getMetresValue() + cosq(theta) * (stationPoint.getY() - referencePoint.getY()).getMetresValue();

	TReal refPtDistContrib = 1.0;
	TReal obsVariance = pow2q(echo.target.sigmaD + calcMeas / 1000 * echo.target.ppmD) + pow2q(echo.target.sigmaInstrCentering);

	ECHOContrib echoContrib = {calcMeas, stationContrib, thetaContrib, refPtDistContrib, stationTransfContributions, obsVariance};
	return echoContrib;
}

// ECVE contribution
ScaleMeasContrib TContributionsGenerator::getECVEContrib(const TECVEROM &ecveROM, const TECVE &ecve)
{
	TReal cEcVp = ecve.target.distCorrectionValue; // distance of the target correction value
	TPositionVector stationPoint = ecve.targetPos->getEstimatedValue();

	const TLOR2LOR &stationPTLor2RootTrafo = fPointTransfo.getLORTransformation(ecve.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	stationPTLor2RootTrafo.transform(stationPoint);

	/*Reference point is always defined in ROOT and is fixed (it is implicitly defined),
	i.e. no transformation to ROOT required and no contribution required for its coordinates.*/
	TPositionVector linePoint = ecveROM.fMeasuredLine->getLinePoint()->getEstimatedValue();
	const TLOR2LOR &linePTLor2RootTrafo = fPointTransfo.getLORTransformation(ecveROM.fMeasuredLine->getLinePoint()->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	linePTLor2RootTrafo.transform(linePoint);

	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		fPointTransfo.transformPointsToMLASystem(ecveROM.fMeasuredLine->getName(), linePoint, stationPoint);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal D = sqrt(pow2(linePoint.getX() - stationPoint.getX()) + pow2(linePoint.getY() - stationPoint.getY()));
	TReal calcMeas = D - cEcVp;

	// coefficient's contribution for the station
	TReal a = -(linePoint.getX() - stationPoint.getX()) / D;
	TReal b = -(linePoint.getY() - stationPoint.getY()) / D;
	TReal c = 0.0;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector stationContrib = getPointContributions(stationPTLor2RootTrafo, a, b, c);
	addTransformationsContributions(stationPTLor2RootTrafo, ecve.targetPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> pointLineTransfContributions;
	TFreeVector pointLineContrib = getPointContributions(linePTLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(linePTLor2RootTrafo, ecve.targetPos->getEstimatedValue(), -a, -b, -c, pointLineTransfContributions);

	// TReal linePointContrib = -1.0;

	TReal obsVariance = pow2q(ecve.target.sigmaD + calcMeas / 1000 * ecve.target.ppmD) + pow2q(ecve.target.sigmaInstrCentering);

	ScaleMeasContrib ecspContrib = {calcMeas, stationContrib, pointLineContrib, stationTransfContributions, pointLineTransfContributions, obsVariance};
	return ecspContrib;
}

// ECSP Contribution
ECSPContrib TContributionsGenerator::getECSPContrib(const TECSPROM &ecspROM, const TECSP &ecsp)
{
	TPositionVector stationPoint = ecsp.targetPos->getEstimatedValue();
	const TLOR2LOR &stationPTLor2RootTrafo = fPointTransfo.getLORTransformation(ecsp.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	stationPTLor2RootTrafo.transform(stationPoint);

	TPositionVector linePoint1 = ecspROM.p1->getEstimatedValue();
	const TLOR2LOR &linePTLor2RootTrafo1 = fPointTransfo.getLORTransformation(ecspROM.p1->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	linePTLor2RootTrafo1.transform(linePoint1);

	TPositionVector linePoint2 = ecspROM.p2->getEstimatedValue();
	const TLOR2LOR &linePTLor2RootTrafo2 = fPointTransfo.getLORTransformation(ecspROM.p2->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	linePTLor2RootTrafo2.transform(linePoint2);

	// contributions can be calculated simply in both the CCS and a local
	// reference frame since no instrument or target heights are involved
	// and the measurement is independent of the local vertical
	// Therefore there are no transformations necessary
	// PARAMETERS IN CARTESIAN REFERENCE SYSTEM

	/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////

	// Calcul par le produit scalaire (u^l)^2+(u.l)^2=|v|^2|l|^2
	TReal dis, pScal, D;
	dis = dist3D(stationPoint.getX(), stationPoint.getY(), stationPoint.getZ(), linePoint1.getX(), linePoint1.getY(), linePoint1.getZ()); // distance P1 - stn
	D = dist3D(linePoint1.getX(), linePoint1.getY(), linePoint1.getZ(), linePoint2.getX(), linePoint2.getY(), linePoint2.getZ()); // distance P1-P2
	pScal = (linePoint2.getX() - linePoint1.getX()).getMetresValue() * (stationPoint.getX() - linePoint1.getX()).getMetresValue()
		+ (linePoint2.getY() - linePoint1.getY()).getMetresValue() * (stationPoint.getY() - linePoint1.getY()).getMetresValue()
		+ (linePoint2.getZ() - linePoint1.getZ()).getMetresValue() * (stationPoint.getZ() - linePoint1.getZ()).getMetresValue(); // produit scalaire

	// contributions
	TReal a, b, c, d, e, f, g, h, i;
	TReal div = sqrt(dis * dis * D * D - pow2(pScal));
	if (div > nullLimit)
	{
		// stn
		a = 1.0 / div * (D * (stationPoint.getX() - linePoint1.getX()) - pScal / D * (linePoint2.getX() - linePoint1.getX()));
		b = 1.0 / div * (D * (stationPoint.getY() - linePoint1.getY()) - pScal / D * (linePoint2.getY() - linePoint1.getY()));
		c = 1.0 / div * (D * (stationPoint.getZ() - linePoint1.getZ()) - pScal / D * (linePoint2.getZ() - linePoint1.getZ()));
		// p2
		d = 1.0 / div * (-pScal / D * (stationPoint.getX() - linePoint1.getX()) + pow2(pScal) / pow(D, 3) * (linePoint2.getX() - linePoint1.getX()));
		e = 1.0 / div * (-pScal / D * (stationPoint.getY() - linePoint1.getY()) + pow2(pScal) / pow(D, 3) * (linePoint2.getY() - linePoint1.getY()));
		f = 1.0 / div * (-pScal / D * (stationPoint.getZ() - linePoint1.getZ()) + pow2(pScal) / pow(D, 3) * (linePoint2.getZ() - linePoint1.getZ()));
		// p1
		g = 1.0 / div
			* (-D * (stationPoint.getX() - linePoint1.getX()) + pScal / D * (linePoint2.getX() - linePoint1.getX() + stationPoint.getX() - linePoint1.getX())
				- pow2(pScal) / pow(D, 3) * (linePoint2.getX() - linePoint1.getX()));
		h = 1.0 / div
			* (-D * (stationPoint.getY() - linePoint1.getY()) + pScal / D * (linePoint2.getY() - linePoint1.getY() + stationPoint.getY() - linePoint1.getY())
				- pow2(pScal) / pow(D, 3) * (linePoint2.getY() - linePoint1.getY()));
		i = 1.0 / div
			* (-D * (stationPoint.getZ() - linePoint1.getZ()) + pScal / D * (linePoint2.getZ() - linePoint1.getZ() + stationPoint.getZ() - linePoint1.getZ())
				- pow2(pScal) / pow(D, 3) * (linePoint2.getZ() - linePoint1.getZ()));
	}
	else
		throw std::logic_error("TContributionGenerator::getECSPContrib: Division by zero.");

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector stationContrib = getPointContributions(stationPTLor2RootTrafo, a, b, c);
	addTransformationsContributions(stationPTLor2RootTrafo, ecsp.targetPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	// first point on the line
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> pointLineTransfContributions1;
	TFreeVector pointLineContrib1 = getPointContributions(linePTLor2RootTrafo1, g, h, i);
	addTransformationsContributions(linePTLor2RootTrafo1, ecsp.targetPos->getEstimatedValue(), g, h, i, pointLineTransfContributions1);
	// second point on the line
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> pointLineTransfContributions2;
	TFreeVector pointLineContrib2 = getPointContributions(linePTLor2RootTrafo2, d, e, f);
	addTransformationsContributions(linePTLor2RootTrafo2, ecsp.targetPos->getEstimatedValue(), d, e, f, pointLineTransfContributions2);

	TReal calcMeas = div / D - ecsp.target.distCorrectionValue;
	// Variance calculation
	TReal varM = pow2q(ecsp.target.sigmaD + calcMeas / 1000 * ecsp.target.ppmD);
	TReal obsVariance = varM + (pow2q((linePoint2.getX() - linePoint1.getX()) / D) + pow2q((linePoint2.getY() - linePoint1.getY()) / D)) * pow2q(ecsp.target.sigmaInstrCentering);

	ECSPContrib ecspContrib = {calcMeas, stationContrib, pointLineContrib1, pointLineContrib2, stationTransfContributions, pointLineTransfContributions1,
		pointLineTransfContributions2, obsVariance};
	return ecspContrib;
}

// DVER contributions
DVERContrib TContributionsGenerator::getDVERContrib(const TDVER &dver)
{
	fPointTransfo.setMLA(false);
	auto k3D = TCoordSysFactory::k3DCartesian;
	/*Contribution if OLOC used, otherwise going to be rewritten*/
	TFreeVector stationC(0, 0, -1, k3D);
	TFreeVector targetC(0, 0, 1, k3D);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

	TPositionVector station(dver.station->getEstimatedValue());
	TPositionVector target(dver.targetPos->getEstimatedValue());
	TPositionVector stationLOR = station;
	TPositionVector targetLOR = target;

	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(dver.station->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Station's position frame
	stLor2RootTrafo.transform(station); // Transform to ROOT(CCS)

	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(dver.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Station's position frame
	tgLor2RootTrafo.transform(target); // Transform to ROOT(CCS)

	TPositionVector stationCCS = station;
	TPositionVector targetCCS = target;

	TReal dh = 0.0;

	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{ /*Needs to be calculated in CGRF.*/
		fPointTransfo.set2MLATransformation(station);
		fPointTransfo.transformMLA2CGRF(stationC); // transform to CGRF

		fPointTransfo.set2MLATransformation(target);
		fPointTransfo.transformMLA2CGRF(targetC); // transform to CGRF

		if (fPointTransfo.getRefFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		{
			TXYH2CCS::CCS2XYHs(station);
			TXYH2CCS::CCS2XYHs(target);
		}
		else if (fPointTransfo.getRefFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		{
			TXYH2CCS::CCS2XYHg2000Machine(station);
			TXYH2CCS::CCS2XYHg2000Machine(target);
		}
		else if (fPointTransfo.getRefFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		{
			TXYH2CCS::CCS2XYHg1985Machine(station);
			TXYH2CCS::CCS2XYHg1985Machine(target);
		}
		/*IMPORTANT ENABLE CGRF, APPLIES IN POINT AND TRANSFORMATION CONTRIBUTIONS*/
		fPointTransfo.setCGRF(true);
		// Calculating the distance meas
		dh = target.getH().getMetresValue() - station.getH().getMetresValue() - dver.getDistanceCorrection();
	}
	else
	{ /*OLOC = Calculated as XYZ, simple case*/
		fPointTransfo.setCGRF(false);
		dh = target.getZ().getMetresValue() - station.getZ().getMetresValue() - dver.getDistanceCorrection();
	}

	TFreeVector stationContrib = getPointContributions(stLor2RootTrafo, stationC.getX().getMetresValue(), stationC.getY().getMetresValue(), stationC.getZ().getMetresValue());
	addTransformationsContributions(stLor2RootTrafo, stationLOR, 0, 0, -1, stationTransfContributions);

	TFreeVector targetContrib = getPointContributions(tgLor2RootTrafo, targetC.getX().getMetresValue(), targetC.getY().getMetresValue(), targetC.getZ().getMetresValue());
	addTransformationsContributions(tgLor2RootTrafo, targetLOR, 0, 0, 1, targetTransfContributions);

	// Reset the CGRF status (can have other effect on the code)
	fPointTransfo.setCGRF(false);

	DVERContrib dverC = {dh, stationContrib, targetContrib, stationTransfContributions, targetTransfContributions, pow2q(dver.getObservedStDev())};

	return dverC;
}

// ORIE contributions
AnglMeasContrib TContributionsGenerator::getOrieContrib(const TORIEROM &orieROM, const TORIE &orie)
{
	// Transform TARGET and STATION in a LOCAL ASTRONOMICAL FRAME
	TPositionVector targetPos = orie.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(orie.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = orieROM.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		orieROM.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		fPointTransfo.transformPointsToMLASystem(orieROM.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.getLA2MLA().transformInverse(stationPos);
		fPointTransfo.getLA2MLA().transformInverse(targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	// Calculated measurement value
	TAngle calcMeas = TAngle::aTan2((xTg - xSt), (yTg - ySt)) - orieROM.fConstantAngle;

	TReal dist2 = pow2q(dist(xSt, ySt, xTg, yTg));
	if (dist2 < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getOrieContrib: Division by zero because station and target are identical or have identical x and y coordinates. Points: "
			+ getNameAndLine(*orieROM.instrumentPos) + " and " + getNameAndLine(*orie.targetPos));
	}

	TReal a, b, c; // station's contributions coefficients (negative values of these give the target coefficients)
	a = (-LITERAL(1.0) * (yTg - ySt)) / dist2; // xSt coefficient
	b = (xTg - xSt) / dist2; // ySt coefficient
	c = 0.0; // zSt coefficient

	TReal v0Contrib = 0.0; // no V0 parameter for a gyro theodolithe
	TReal hiContrib = 0.0; // no contribution for the instrument height

	TFreeVector abc(a, b, c, TCoordSysFactory::k3DCartesian);

	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		fPointTransfo.getLA2MLA().transform(abc);

	// Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, abc.getX().getMetresValue(), abc.getY().getMetresValue(), abc.getZ().getMetresValue());
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, orieROM.instrumentPos->getEstimatedValue(), abc.getX().getMetresValue(), abc.getY().getMetresValue(),
		abc.getZ().getMetresValue(), stationTransfContributions);

	// Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -abc.getX().getMetresValue(), -abc.getY().getMetresValue(), -abc.getZ().getMetresValue());
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, orie.targetPos->getEstimatedValue(), -abc.getX().getMetresValue(), -abc.getY().getMetresValue(),
		-abc.getZ().getMetresValue(), targetTransfContributions);

	// Variance calculation
	TReal variance = pow2q(orie.target.sigmaAngl.getRadiansValue()) + (1.0 / (dist2)) * (pow2q(orieROM.instrument.sigmaInstrCentering) + pow2q(orie.target.sigmaTargetCentering));

	AnglMeasContrib contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

// PDOR contribution
PtOrientationContrib TContributionsGenerator::getPDORContrib(const TPdorObs &pdorObs)
{
	// The fixed point is defined in root
	TPositionVector fixedPt = pdorObs.calaPt->getEstimatedValue();

	TPositionVector oriPt = pdorObs.orientationPt->getEstimatedValue();
	const TLOR2LOR &oriPtLor2RootTrafo = fPointTransfo.getLORTransformation(pdorObs.orientationPt->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transform station to ROOT
	oriPtLor2RootTrafo.transform(oriPt);

	TReal xFix = fixedPt.getX().getMetresValue();
	TReal yFix = fixedPt.getY().getMetresValue();

	TReal xRef = oriPt.getX().getMetresValue();
	TReal yRef = oriPt.getY().getMetresValue();

	TReal D = dist(xFix, yFix, xRef, yRef);
	if (D < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getPDORContrib: Division by zero because station and target are identical or have identical x and y coordinates. Points: "
			+ getNameAndLine(*pdorObs.calaPt) + " and " + getNameAndLine(*pdorObs.orientationPt));
	}

	// When no bearing is set in th input file, one is set in TDataAnalyzer w.r.t provisional values
	TReal calcmeas = (TAngle::aTan2((xRef - xFix), (yRef - yFix))) - pdorObs.getBearing().getRadiansValue();

	// Calculation of the contributions in the local instrument system
	TReal a = (yRef - yFix) / powq(D, 2); // derivative w.r.t xFix
	TReal b = -(xRef - xFix) / powq(D, 2); // derivative w.r.t yFix
	TReal c = 0.0;

	// Target can be defined anywhere, get point contributions and transformations contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fRefPtTransformContrib;
	TFreeVector oriPointContrib = getPointContributions(oriPtLor2RootTrafo, a, b, c);
	addTransformationsContributions(oriPtLor2RootTrafo, pdorObs.orientationPt->getEstimatedValue(), a, b, c, fRefPtTransformContrib);

	return {oriPointContrib, fRefPtTransformContrib, calcmeas};
}

// RADI contribution
PtOrientationContrib TContributionsGenerator::getRADIContrib(const TRADI &radi)
{
	// Constraint made in CCS, later could be extended to subframe
	fPointTransfo.setMLA(false);

	TPositionVector prov = radi.station->getProvisionalValue();
	TPositionVector estimated = radi.station->getEstimatedValue();
	const TLOR2LOR &Lor2RootTrafo = fPointTransfo.getLORTransformation(radi.station->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transform to ROOT
	Lor2RootTrafo.transform(estimated);
	Lor2RootTrafo.transform(prov);

	TReal xp = prov.getX().getMetresValue();
	TReal yp = prov.getY().getMetresValue();

	TReal xe = estimated.getX().getMetresValue();
	TReal ye = estimated.getY().getMetresValue();

	TAngle bear = radi.getAngleCnstr() + radi.getConstAngle();

	// gets calc value and sigma
	TLength calcmeas = TLength(-LITERAL(1.0) * sinq(bear) * (ye - yp) + cosq(bear) * (xe - xp));

	// calculated contibutions in a local system
	TReal a = -LITERAL(1.0) * cosq(bear); // xPt coefficient
	TReal b = sinq(bear); // yPt coefficient
	TReal c = LITERAL(0.0); // zPt coefficient

	// Point can be defined anywhere, get point contributions and transformations contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> festimatedPtTransformContrib;
	TFreeVector estimatedPointContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian);
	addTransformationsContributions(Lor2RootTrafo, radi.station->getEstimatedValue(), a, b, c, festimatedPtTransformContrib);

	return {estimatedPointContrib, festimatedPtTransformContrib, calcmeas};
}

// OBSXYZ contribution
OBSXYZContrib TContributionsGenerator::getOBSXYZContrib(const TOBSXYZ &OBSXYZ)
{
	TPositionVector obsPoint = OBSXYZ.station->getEstimatedValue(); // observed point in frame where point is defined
	TPositionVector obs = OBSXYZ.obsValue; // observation value
	const TLOR2LOR &obsPoint2ObsTrafo = fPointTransfo.getLORTransformation(OBSXYZ.station->getFrameTreePosition(), OBSXYZ.positionInTree); // Transform to frame from which the point is observed
	obsPoint2ObsTrafo.transform(obsPoint);

	Eigen::Vector3d obsVariance;
	obsVariance << pow2q(OBSXYZ.getXObservedStDev()), pow2q(OBSXYZ.getYObservedStDev()), pow2q(OBSXYZ.getZObservedStDev());

	// point transformed to observation frame minus observation (misclosure)
	TFreeVector misclosure = obsPoint - obs;

	// Contributions from the coordinates of the observed point
	Point3DContrib coordContribObsPoint = {obsPoint2ObsTrafo.getPartialDerivativeWrtPosition()};

	// Contributions of transformation parameters
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> transfContributions;
	TFreeVector unitX(1.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector unitY(0.0, 1.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector unitZ(0.0, 0.0, 1.0, TCoordSysFactory::k3DCartesian);

	// Contributions are computed in frame of observation, so no trafo to MLA. Flag used in addTransformationsContributions3D
	fPointTransfo.setMLA(false);
	addTransformationsContributions3D(obsPoint2ObsTrafo, OBSXYZ.station->getEstimatedValue(), unitX, unitY, unitZ, transfContributions);

	return {obsPoint, obsVariance, coordContribObsPoint, transfContributions, misclosure};
}

/*
 * Private Template Helper Function for Common Inclinometer Logic
 *
 * This helper function contains the common logic between getINCLYContrib and getROLLYContrib:
 * transforming the local vertical from root into the station frame and applying the reference
 * angle rotation. It then dispatches to the model-specific function (addINCLYContribution or
 * addROLLYContribution) which computes the calculated measurement, observation variance,
 * gradients and transformation contributions.
 *
 * @param inclST: Station/ROM object (TINCLYROM or TROLLYROM)
 * @param incl: Individual measurement object (TINCLY or TROLLY)
 *               The mathematical model is automatically deduced from the template type:
 *               - TINCLY uses arcsin model
 *               - TROLLY uses atan2 model
 *
 * @return INCLContrib structure with calculated measurement, transformation contributions and obsVariance
 */
template<typename TROM, typename TMeas>
INCLContrib TContributionsGenerator::getINCLContribHelper(const TROM &inclST, const TMeas &incl)
{
	const TPositionVector &targetPos = incl.targetPos->getEstimatedValue();
	const TDataTreeIterator &targetFramePos = incl.targetPos->getFrameTreePosition();
	const TDataTreeIterator &stationFramePos = inclST.positionInTree;

	fPointTransfo.setMLA(false);

	// Transform the point of measure to the Root frame
	TPositionVector stationPos = targetPos;
	const TLOR2LOR &ptLor2RootTrafo = fPointTransfo.getLORTransformation(targetFramePos, fPointTransfo.getTree()->begin());
	ptLor2RootTrafo.transform(stationPos);

	// Creating the Local Vertical vector (no change if OLOC)
	TFreeVector stationV(0, 0, 1, TCoordSysFactory::k3DCartesian);

	// Express the local vertical if MLA is used
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		fPointTransfo.set2MLATransformation(stationPos);
		fPointTransfo.transformMLA2CGRF(stationV);
		fPointTransfo.transformCGRF2CCS(stationV);
	}
	// Keep a copy of the vector in the root for the transformation contributions
	TFreeVector stationVRoot(stationV);

	// Transform the local vertical in the station LOR
	const TLOR2LOR &vert2stTrafo = fPointTransfo.getLORTransformation(fPointTransfo.getTree()->begin(), stationFramePos);
	vert2stTrafo.transform(stationV);

	// Apply reference angle rotation about Y axis
	TRotationMatrix referenceAngleRotation(TRotationMatrix::ERotationType::kRxyz, 0, incl.target.refAngleCorrectionValue, 0);
	TFreeVector stationVTransformed = referenceAngleRotation.inverse() * stationV;

	Eigen::Vector3d locVert = stationVTransformed.toRealVector();

	bool useArcsinModel = std::is_same_v<TMeas, TINCLY>;
	if (useArcsinModel)
		return addINCLYContribution(vert2stTrafo, stationVRoot, locVert, incl.target);
	else
		return addROLLYContribution(vert2stTrafo, stationVRoot, locVert, incl.target);
}

INCLContrib TContributionsGenerator::getINCLYContrib(const TINCLYROM &inclST, const TINCLY &incly)
{
	return getINCLContribHelper(inclST, incly);
}

INCLContrib TContributionsGenerator::getROLLYContrib(const TROLLYROM &rollyST, const TROLLY &rolly)
{
	return getINCLContribHelper(rollyST, rolly);
}

////ECWS contribution
ECWSContrib TContributionsGenerator::getECWSContrib(const TECWSROM &ecwsROM, const TECWS &ecws)
{
	// No need for MLA Transformation in ECWS
	fPointTransfo.setMLA(false);

	// Transforamtions
	const TLOR2LOR &Lor2RootTrafo = fPointTransfo.getLORTransformation(ecws.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	const TLOR2LOR &Root2LorTrafo = fPointTransfo.getLORTransformation(fPointTransfo.getTree()->begin(), ecws.targetPos->getFrameTreePosition());

	// Transform the target Point in the root
	TPositionVector targetPointInRoot = ecws.targetPos->getEstimatedValue();
	Lor2RootTrafo.transform(targetPointInRoot);

	// The WS Height is expressed in the root, can be either in a geodetic system or in a local reference frame
	auto refFrame = fPointTransfo.getRefFrame();
	TPositionVector wsPos(targetPointInRoot.getX(), targetPointInRoot.getY(), ecwsROM.fMeasuredWSHeight->getEstimatedValue().getMetresValue(), TCoordSysFactory::ECoordSys::k3DCartesian);
	if (refFrame != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		wsPos.setCoordSys(TCoordSysFactory::k2DPlusH);
		if (fPointTransfo.getRefFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			TXYH2CCS::XYHg2000Machine2CCS(wsPos);
		else if (fPointTransfo.getRefFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			TXYH2CCS::XYHg1985Machine2CCS(wsPos);
		else
			TXYH2CCS::XYHs2CCS(wsPos);
	}

	// Target must be defined in the frame where the WPS sensor is
	TFreeVector referencePTContrib(0, 0, -1, TCoordSysFactory::ECoordSys::k3DCartesian);

	// WS is defined in the root necessarily
	TReal wsContrib = getPointContributions(Lor2RootTrafo, 0, 0, 1).getZ();

	// Add the contribtion of the WS to the transformation
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> wsContribTransfContributions;
	addTransformationsContributions(Root2LorTrafo, wsPos, 0, 0, 1, wsContribTransfContributions);

	// Obs equation
	Root2LorTrafo.transform(wsPos);
	TPositionVector targetPoint = ecws.targetPos->getEstimatedValue();
	TReal calcMeas = wsPos.getZ().getMetresValue() - targetPoint.getZ().getMetresValue();

	// Compute the variance related to the instrument centering w.r.t water surface
	TFreeVector stationV(0, 0, 1, TCoordSysFactory::k3DCartesian);
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		fPointTransfo.set2MLATransformation(targetPointInRoot);
		fPointTransfo.transformMLA2CGRF(stationV);
		fPointTransfo.transformCGRF2CCS(stationV);
	}
	Root2LorTrafo.transform(stationV);

	// case in test where the z Value is >1 due to numerical errors, makes the acos crash
	auto zValue = abs(stationV.getZ().getMetresValue());
	if (zValue > 1)
		zValue = 1;

	TAngle incliWS(acos(zValue));

	// PI/2 causes a huge Sigma value : OK with the behavior
	TReal instrCenterSigma = tan(incliWS.getRadiansValue()) * ecws.target.sigmaInstrCentering.getMetresValue();

	// Compute the variance of the observation
	TReal obsVariance = pow2q(ecws.target.sigmaDist.getMetresValue()) + pow2q(ecws.target.sigmaInstrHeight.getMetresValue()) + pow2q(ecws.target.sigmaWS.getMetresValue())
		+ pow2q(instrCenterSigma);

	ECWSContrib ecwsContrib = {calcMeas, referencePTContrib, wsContribTransfContributions, wsContrib, obsVariance};
	return ecwsContrib;
}

////ECWI contribution
ECWIContrib TContributionsGenerator::getECWIContrib(const TECWIROM &ecwiROM, const TECWI &ecwi)
{
	fPointTransfo.setMLA(false);

	// Define the transformations for the station
	const TLOR2LOR &StLor2RootTrafo = fPointTransfo.getLORTransformation(ecwi.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	const TLOR2LOR &Root2StLorTrafo = fPointTransfo.getLORTransformation(fPointTransfo.getTree()->begin(), ecwi.targetPos->getFrameTreePosition());

	// Construct the transformation Root <> Wire: 2 succesive transformations root <> firstWire <> secondWire
	// The wire is defined along the second frame Y-axis
	TReal wireBearing = ecwiROM.fWireBearing->getEstimatedValue().getRadiansValue();
	TReal wireSlope = ecwiROM.fWireSlope->getEstimatedValue().getRadiansValue();
	TReal wireDRefX = ecwiROM.fWireDx->getEstimatedValue().getMetresValue();
	TReal wireDRefZ = ecwiROM.fWireDz->getEstimatedValue().getMetresValue();

	TTransformation firstWire2Root;
	firstWire2Root.setTransformation(ecwiROM.referencePoint.getX().getMetresValue(), ecwiROM.referencePoint.getY().getMetresValue(),
		ecwiROM.referencePoint.getZ().getMetresValue(), 0, 0, wireBearing, 1);
	TTransformation root2FirstWire = firstWire2Root.getInversedTransformation();

	TTransformation secondWire2FirstWire;
	secondWire2FirstWire.setTransformation(wireDRefX, 0, wireDRefZ, wireSlope, 0, 0, 1);
	TTransformation firstWire2SecondWire = secondWire2FirstWire.getInversedTransformation();

	// Construct the wire objects defined in the secondWire frame
	TPositionVector wireZero(0, 0, 0, TCoordSysFactory::k3DCartesian); // The oigin of the secondWire frame
	TFreeVector wireY(0, 1, 0, TCoordSysFactory::k3DCartesian); // The orientation vector of the wire along the secondWire Y-axis

	/// First step : compute the modeled observation in the station frame without the SAG
	// Express the wire parameters in the station frame

	secondWire2FirstWire.transform(wireZero);
	firstWire2Root.transform(wireZero);
	Root2StLorTrafo.transform(wireZero);

	secondWire2FirstWire.transform(wireY);
	firstWire2Root.transform(wireY);
	Root2StLorTrafo.transform(wireY);

	// Compute the intersection with the X and Z vectors
	TFreeVector XZPlaneNormal(0, 1, 0, TCoordSysFactory::k3DCartesian);
	auto projPointOnWire = [&ecwiROM, &wireY, &wireZero, &XZPlaneNormal](const TPositionVector &stPoint2Project) {
		TReal numerator = (TFreeVector(stPoint2Project) - TFreeVector(wireZero)).dot(XZPlaneNormal);
		TReal denominator = wireY.dot(XZPlaneNormal);
		if (abs(denominator) < nullLimit)
			throw std::logic_error("TContributionGenerator::getECWIContrib: Division by zero because the wire is parallel to the XZ plane " + ecwiROM.romName);
		TReal vectorFactor = numerator / denominator;
		TPositionVector projSt2Wire(wireY.getX().getMetresValue() * vectorFactor + wireZero.getX().getMetresValue(), stPoint2Project.getY().getMetresValue(),
			wireY.getZ().getMetresValue() * vectorFactor + wireZero.getZ().getMetresValue(), TCoordSysFactory::k3DCartesian);
		return std::tuple<TPositionVector, TReal, TReal>(projSt2Wire, numerator, denominator);
	};

	// Compute the projection of the station onto the wire in the station system
	auto [projSt, projStNumerator, projStDenominator] = projPointOnWire(ecwi.targetPos->getEstimatedValue());
	TReal componentX = projSt.getX().getMetresValue();
	TReal componentZ = projSt.getZ().getMetresValue();

	/// Second step : compute and add the SAG (To refactor when a LGCAdjustableObjects of type wire will be available)

	// Intersect the wire in the anchor frames
	TPositionVector A1 = ecwiROM.anchorPtFirst->getEstimatedValue();
	TPositionVector A2 = ecwiROM.anchorPtSecond->getEstimatedValue();
	StLor2RootTrafo.transform(wireY);
	StLor2RootTrafo.transform(wireZero);

	// Transformation needed
	const TLOR2LOR &Root2AnchorFirstLorTrafo = fPointTransfo.getLORTransformation(fPointTransfo.getTree()->begin(), ecwiROM.anchorPtFirst->getFrameTreePosition());
	const TLOR2LOR &AnchorFirstLor2AnchorSecondLorTrafo = fPointTransfo.getLORTransformation(
		ecwiROM.anchorPtFirst->getFrameTreePosition(), ecwiROM.anchorPtSecond->getFrameTreePosition());
	const TLOR2LOR &AnchorSecondLor2RootTrafo = fPointTransfo.getLORTransformation(ecwiROM.anchorPtSecond->getFrameTreePosition(), fPointTransfo.getTree()->begin());

	// First Anchor
	Root2AnchorFirstLorTrafo.transform(wireY);
	Root2AnchorFirstLorTrafo.transform(wireZero);
	auto [projA1, projA1Numerator, projA1Denominator] = projPointOnWire(A1);

	// Second Anchor
	AnchorFirstLor2AnchorSecondLorTrafo.transform(wireY);
	AnchorFirstLor2AnchorSecondLorTrafo.transform(wireZero);
	AnchorFirstLor2AnchorSecondLorTrafo.transform(projA1);
	auto [projA2, projA2Numerator, projA2Denominator] = projPointOnWire(A2);

	// put again the wire in the station Frame
	AnchorSecondLor2RootTrafo.transform(wireY);
	AnchorSecondLor2RootTrafo.transform(wireZero);
	Root2StLorTrafo.transform(wireY);
	Root2StLorTrafo.transform(wireZero);

	// compute the SAG : need PA1, PA2 and PSt to be in the root frame
	AnchorSecondLor2RootTrafo.transform(projA1);
	AnchorSecondLor2RootTrafo.transform(projA2);
	StLor2RootTrafo.transform(projSt);

	TFreeVector vectorL = projA2 - projA1;
	TFreeVector vectorDi = projSt - projA1;

	// Handling the case where a station point is situates before the projected A1 on the wire
	TReal scalar = vectorL.dot(vectorDi);
	int factor = 1;

	if (vectorL.dot(vectorDi) < 0)
		factor = -1;

	TReal di = vectorDi.getHorDist().getMetresValue();
	TReal l = vectorL.getHorDist().getMetresValue();
	TReal sag = ecwiROM.sagAdjustable->getEstimatedValue().getMetresValue();

	if (l < nullLimit)
		throw std::logic_error("TContributionGenerator::getECWIContrib: Division by zero because anchor points are identical or have identical X and Y coordinates.");

	TReal ratioLength = factor * di / l;
	TReal riOver100 = 4 * sag * (pow2(ratioLength) - ratioLength);

	// Transform the SAG vector and associated precision in the station frame
	TFreeVector sagVector(0, 0, riOver100, TCoordSysFactory::k3DCartesian);

	TReal sigmaRiOver100 = 4 * ecwiROM.instrument.sigmaSagWire.getMetresValue() * (pow2(ratioLength) - ratioLength);
	TFreeVector sigmaSagVector(0, 0, sigmaRiOver100, TCoordSysFactory::k3DCartesian);

	// Express te local vertical  in the CCS if MLA is used, maximal SAG in the middle of the ancor points projected on the wire
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		TPositionVector averageAnchors = (TFreeVector(projA1) + TFreeVector(projA2)) * 0.5;
		fPointTransfo.set2MLATransformation(averageAnchors);
		fPointTransfo.transformMLA2CGRF(sagVector);
		fPointTransfo.transformCGRF2CCS(sagVector);
		fPointTransfo.transformMLA2CGRF(sigmaSagVector);
		fPointTransfo.transformCGRF2CCS(sigmaSagVector);
	}

	Root2StLorTrafo.transform(sagVector);
	Root2StLorTrafo.transform(sigmaSagVector);

	/// CalcMeas
	TReal dx = componentX - ecwi.targetPos->getEstimatedValue().getX().getMetresValue() + sagVector.getX().getMetresValue();
	TReal dz = componentZ - ecwi.targetPos->getEstimatedValue().getZ().getMetresValue() + sagVector.getZ().getMetresValue();

	// Add the contributions

	struct TEqContrib
	{
		TReal eqX = 0;
		TReal eqZ = 0;
	};

	// Station contributions
	TReal stYContribX = wireY.getX().getMetresValue() / projStDenominator;
	TReal stYContribZ = wireY.getZ().getMetresValue() / projStDenominator;
	TFreeVector stFirstEqContrib(-1, stYContribX, 0, TCoordSysFactory::k3DCartesian);
	TFreeVector stSecondEqContrib(0, stYContribZ, -1, TCoordSysFactory::k3DCartesian);

	// Compute SAG Contributions
	TReal sagDeriv = 4 * (pow2(ratioLength) - ratioLength);
	TFreeVector sagVectorDeriv(0, 0, sagDeriv, TCoordSysFactory::k3DCartesian);
	Root2StLorTrafo.transform(sagVectorDeriv);
	TEqContrib sagContrib{sagVectorDeriv.getX().getMetresValue(), sagVectorDeriv.getZ().getMetresValue()};

	// Compute dRefZ Contributions
	TFreeVector dRefZContribWireZero(0, 0, 1, TCoordSysFactory::k3DCartesian);
	firstWire2Root.transform(dRefZContribWireZero);
	Root2StLorTrafo.transform(dRefZContribWireZero);
	TEqContrib dRefZContrib{dRefZContribWireZero.getX().getMetresValue(), dRefZContribWireZero.getZ().getMetresValue()};

	// Compute dRefX Contributions
	TFreeVector dRefXContribWireZero(1, 0, 0, TCoordSysFactory::k3DCartesian);
	firstWire2Root.transform(dRefXContribWireZero);
	Root2StLorTrafo.transform(dRefXContribWireZero);
	TEqContrib dRefXContrib{dRefXContribWireZero.getX().getMetresValue(), dRefXContribWireZero.getZ().getMetresValue()};

	// Compute  Contributions
	TPositionVector stationVector(componentX, ecwi.targetPos->getEstimatedValue().getY().getMetresValue(), componentZ, TCoordSysFactory::k3DCartesian);
	StLor2RootTrafo.transform(stationVector);
	root2FirstWire.transform(stationVector);
	TFreeVector bearingContribVector(-sinq(wireBearing) * (stationVector.getX().getMetresValue()) + cosq(wireBearing) * stationVector.getY().getMetresValue(),
		-cosq(wireBearing) * (stationVector.getX().getMetresValue()) - sinq(wireBearing) * stationVector.getY().getMetresValue(), 0, TCoordSysFactory::k3DCartesian);
	Root2StLorTrafo.transform(bearingContribVector);
	TEqContrib bearingContrib{bearingContribVector.getX().getMetresValue(), bearingContribVector.getZ().getMetresValue()};

	// Compute Slope Contributions
	firstWire2SecondWire.transform(stationVector);
	TFreeVector slopeContribVector(0, -sinq(wireSlope) * stationVector.getY().getMetresValue() + cosq(wireSlope) * stationVector.getZ().getMetresValue(),
		-cosq(wireSlope) * stationVector.getY().getMetresValue() - sinq(wireSlope) * stationVector.getZ().getMetresValue(), TCoordSysFactory::k3DCartesian);
	firstWire2Root.transform(slopeContribVector);
	Root2StLorTrafo.transform(slopeContribVector);
	TEqContrib slopeContrib{slopeContribVector.getX().getMetresValue(), slopeContribVector.getZ().getMetresValue()};

	// Compute Tranformation contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> TransfContributionsDx;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> TransfContributionsDz;
	TPositionVector transfoContrib(componentX + sagVector.getX().getMetresValue(), ecwi.targetPos->getEstimatedValue().getY().getMetresValue() + sagVector.getY().getMetresValue(),
		componentZ + sagVector.getZ().getMetresValue(), TCoordSysFactory::k3DCartesian);
	StLor2RootTrafo.transform(transfoContrib);
	addTransformationsContributions(Root2StLorTrafo, transfoContrib, 1, 0, 0, TransfContributionsDx);
	addTransformationsContributions(Root2StLorTrafo, transfoContrib, 0, 0, 1, TransfContributionsDz);

	// Compute the variances
	TFreeVector sigmaWireVector(ecwiROM.instrument.sigmaWire, 0, ecwiROM.instrument.sigmaWire, TCoordSysFactory::k3DCartesian);
	secondWire2FirstWire.transform(sigmaWireVector);
	firstWire2Root.transform(sigmaWireVector);
	Root2StLorTrafo.transform(sigmaWireVector);
	TEqContrib varianceObs{
		pow2(ecwi.target.sigmaX) + pow2(ecwi.target.sigmaInstrCenteringX) + pow2(sigmaSagVector.getX().getMetresValue()) + pow2(sigmaWireVector.getX().getMetresValue()),
		pow2(ecwi.target.sigmaZ) + pow2(ecwi.target.sigmaInstrCenteringZ) + pow2(sigmaSagVector.getZ().getMetresValue()) + pow2(sigmaWireVector.getZ().getMetresValue())};

	return {{dx, dz}, {stFirstEqContrib, stSecondEqContrib}, {bearingContrib.eqX, bearingContrib.eqZ}, {slopeContrib.eqX, slopeContrib.eqZ},
		{dRefXContrib.eqX, dRefXContrib.eqZ}, {dRefZContrib.eqX, dRefZContrib.eqZ}, {sagContrib.eqX, sagContrib.eqZ}, TransfContributionsDx, TransfContributionsDz,
		{varianceObs.eqX, varianceObs.eqZ}};
}

//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- CAMERA measurements (UVEC/UVD)
//////////////////////////////////////////////////////////////////////
UVECContrib TContributionsGenerator::getUVECContrib(const TCAM &camera, const TUVEC &uvec)
{
	fPointTransfo.setMLA(false); // TCAM measurements are never in MLA

	const TLOR2LOR &tg2stTrafo = fPointTransfo.getLORTransformation(
		uvec.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); // Trafo from from target's LOR to station's LOR
	TPositionVector targetPos = uvec.targetPos->getEstimatedValue();
	tg2stTrafo.transform(targetPos);

	const TFreeVector &unitVec = uvec.getVectorValue(); // observed vector (X and Y), Z is not observed
	TReal i = unitVec.getX().getMetresValue();
	TReal j = unitVec.getY().getMetresValue();
	TReal k = unitVec.getZ().getMetresValue();

	const TPositionVector &instrEstimValue = camera.instrumentPos->getEstimatedValue();
	TReal dx = targetPos.getX().getMetresValue() - instrEstimValue.getX().getMetresValue();
	TReal dy = targetPos.getY().getMetresValue() - instrEstimValue.getY().getMetresValue();
	TReal dz = targetPos.getZ().getMetresValue() - instrEstimValue.getZ().getMetresValue();

	if (fabs(dz) < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getUVECContrib: Division by zero because camera and target are identical or have identical z coordinates. Points: "
			+ getNameAndLine(*camera.instrumentPos) + " and " + getNameAndLine(*uvec.targetPos));
	}

	TFreeVector stFirstEqContrib(-k / dz, 0.0, k * (dx / pow2q(dz)), TCoordSysFactory::k3DCartesian);

	TFreeVector stSecondEqContrib(0, -k / dz, k * (dy / pow2q(dz)), TCoordSysFactory::k3DCartesian);

	TFreeVector tgFirstEqContrib(
		k * (-(dx / pow2q(dz)) * tg2stTrafo.partDerivWRespToX0().getZ().getMetresValue() + (1 / dz) * tg2stTrafo.partDerivWRespToX0().getX().getMetresValue()),
		k * (-(dx / pow2q(dz)) * tg2stTrafo.partDerivWRespToY0().getZ().getMetresValue() + (1 / dz) * tg2stTrafo.partDerivWRespToY0().getX().getMetresValue()),
		k * (-(dx / pow2q(dz)) * tg2stTrafo.partDerivWRespToZ0().getZ().getMetresValue() + (1 / dz) * tg2stTrafo.partDerivWRespToZ0().getX().getMetresValue()),
		TCoordSysFactory::k3DCartesian);

	TFreeVector tgSecondEqContrib(
		k * (-(dy / pow2q(dz)) * tg2stTrafo.partDerivWRespToX0().getZ().getMetresValue() + (1 / dz) * tg2stTrafo.partDerivWRespToX0().getY().getMetresValue()),
		k * (-(dy / pow2q(dz)) * tg2stTrafo.partDerivWRespToY0().getZ().getMetresValue() + (1 / dz) * tg2stTrafo.partDerivWRespToY0().getY().getMetresValue()),
		k * (-(dy / pow2q(dz)) * tg2stTrafo.partDerivWRespToZ0().getZ().getMetresValue() + (1 / dz) * tg2stTrafo.partDerivWRespToZ0().getY().getMetresValue()),
		TCoordSysFactory::k3DCartesian);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>> targetTransfContributions; // Vector with target's transformations contributions

	// use the 3D method to fill contributuons, then make a 2D contrib out of it. Alternative would be to create 2D method
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> auxContribs;
	TFreeVector first((k / dz), 0.0, (-k / pow2q(dz)) * dx, TCoordSysFactory::k3DCartesian);
	TFreeVector second(0.0, (k / dz), -k * ((1 / pow2q(dz))) * dy, TCoordSysFactory::k3DCartesian);
	TFreeVector zeroVector(0.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian);
	addTransformationsContributions3D(tg2stTrafo, uvec.targetPos->getEstimatedValue(), first, second, zeroVector, auxContribs);
	for (auto pair : auxContribs)
	{
		TAdjustableHelmertTransformation trafo = pair.first;
		TransformationContrib2D trafoContrib = {pair.second.firstEquationTransContrib, pair.second.secondEquationTransContrib};
		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>(trafo, trafoContrib));
	}
	////End of filling transformation contributions

	UVECContrib contrib = {stFirstEqContrib, stSecondEqContrib, tgFirstEqContrib, tgSecondEqContrib, targetTransfContributions,
		{(k / dz) * dx, (k / dz) * dy}, // CalcMeas vector for a first and second equation
		{pow2q(uvec.target.sigmaX) + pow2q(k / (dz) * (uvec.target.sigmaTargetCentering + camera.instrument.sigmaInstrCentering)),
			pow2q(uvec.target.sigmaY) + pow2q(k / (dz) * (uvec.target.sigmaTargetCentering + camera.instrument.sigmaInstrCentering))}}; // Obs variances
	return contrib;
}
UVDContrib TContributionsGenerator::getUVDContrib(const TCAM &camera, const TUVD &uvd)
{
	fPointTransfo.setMLA(false); // TCAM measurements never in MLA
	TPositionVector targetPos = uvd.targetPos->getEstimatedValue();
	const TLOR2LOR &tg2stTrafo = fPointTransfo.getLORTransformation(uvd.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	tg2stTrafo.transform(targetPos);

	// the observations
	TReal sDist = uvd.getDistance(); // measured distance
	TReal i = uvd.getVectorValue().getX().getMetresValue(); // X component
	TReal j = uvd.getVectorValue().getY().getMetresValue(); // Y component
	TReal k = uvd.getVectorValue().getZ().getMetresValue(); // Z component not an observation
	TPositionVector stationPos = camera.instrumentPos->getEstimatedValue();
	TFreeVector relPos = targetPos - stationPos;
	double dx(relPos.getX().getMetresValue()), dy(relPos.getY().getMetresValue()), dz(relPos.getZ().getMetresValue()), dzSquare = dz * dz;

	if (fabs(dz) < nullLimit)
	{
		generateContributionError(
			"TContributionGenerator::getUVDContrib: Division by zero because camera and target are identical or have identical z coordinates. Points: "
			+ getNameAndLine(*camera.instrumentPos) + " and " + getNameAndLine(*uvd.targetPos));
	}

	// case k=0 is already catched in the reader
	Eigen::Vector3d calcMeas(k * dx / dz, k * dy / dz, dz / k);

	// Jacobian of CalcMeas
	Eigen::Matrix3d JacCalcMeas;
	JacCalcMeas << k / dz, 0, -k * dx / dzSquare, 0, k / dz, -k * dy / dzSquare, 0, 0, 1 / k;

	// CAM station's contribution is calculated in a LOR system of the station and, therefore, the station's contribution is this
	Point3DContrib coordContribStation = {-JacCalcMeas};

	Point3DContrib coordContribTarget = {JacCalcMeas * tg2stTrafo.getPartialDerivativeWrtPosition()};

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector with target's transformations contributions

	// Parameters: transformation from target's LOR into station's LOR, target position, vector of contributions to be filled
	addTransformationsContributions3D(tg2stTrafo, uvd.targetPos->getEstimatedValue(), TFreeVector(JacCalcMeas.row(0)), TFreeVector(JacCalcMeas.row(1)),
		TFreeVector(JacCalcMeas.row(2)), targetTransfContributions);
	// fill the contribution structure
	Eigen::Vector3d obsVariance(pow2q(uvd.target.sigmaX) + pow2q(uvd.target.sigmaTargetCentering) + pow2(camera.instrument.sigmaInstrCentering),
		pow2q(uvd.target.sigmaY) + pow2q(uvd.target.sigmaTargetCentering) + pow2(camera.instrument.sigmaInstrCentering),
		pow2q(uvd.target.sigmaDist) + pow2q(uvd.target.sigmaTargetCentering) + pow2(camera.instrument.sigmaInstrCentering));

	UVDContrib contrib;
	contrib.fStCoordContrib = coordContribStation;
	contrib.fTgCoordContrib = coordContribTarget;
	contrib.fTgTransformContrib = targetTransfContributions;
	contrib.fCalcMeas = calcMeas;
	contrib.fObsVariance = obsVariance;

	return contrib;
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE / SUPPORTING METHODS
///////////////////////////////////////////////////////////////////////////
TFreeVector TContributionsGenerator::getPointContributions(const TLOR2LOR &lorTrafo, TReal a, TReal b, TReal c)
{
	TFreeVector derX0 = lorTrafo.partDerivWRespToX0();
	TFreeVector derY0 = lorTrafo.partDerivWRespToY0();
	TFreeVector derZ0 = lorTrafo.partDerivWRespToZ0();

	if (fPointTransfo.getMLAused())
	{ // Transform partial derivatives into ILA if necessary
		fPointTransfo.transform2MLA(derX0);
		fPointTransfo.transform2MLA(derY0);
		fPointTransfo.transform2MLA(derZ0);
	}
	else if (fPointTransfo.getCGRFused())
	{
		fPointTransfo.getCCS2CGRF().transform(derX0);
		fPointTransfo.getCCS2CGRF().transform(derY0);
		fPointTransfo.getCCS2CGRF().transform(derZ0);
	}

	TReal xContrib = a * derX0.getX().getMetresValue() + b * derX0.getY().getMetresValue() + c * derX0.getZ().getMetresValue();
	TReal yContrib = a * derY0.getX().getMetresValue() + b * derY0.getY().getMetresValue() + c * derY0.getZ().getMetresValue();
	TReal zContrib = a * derZ0.getX().getMetresValue() + b * derZ0.getY().getMetresValue() + c * derZ0.getZ().getMetresValue();

	return TFreeVector(xContrib, yContrib, zContrib, TCoordSysFactory::k3DCartesian); // Contribution of the point's coordinates
}

void TContributionsGenerator::addTransformationsContributions(const TLOR2LOR &transformations,
	const TPositionVector &pointPos,
	TReal a,
	TReal b,
	TReal c,
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> &transfContrib)
{
	const std::vector<TLOR2LOR::TransformAndParams> &trafoChain = transformations.getTransformationChain();
	TReal omegaContrib, phiContrib, kappaContrib, trans1Contrib, trans2Contrib, trans3Contrib, scaleContrib;

	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
	for (auto it(trafoChain.begin()); it != trafoChain.end(); ++it)
	{
		std::string transformationName = it->adjTrafo->getName();

		TFreeVector omegaPD = transformations.partialDerivativesAngle(transformationName, pointPos, 0); // Partial derivatives of the LOR transformation with respect to omega
		TFreeVector phiPD = transformations.partialDerivativesAngle(transformationName, pointPos, 1);
		TFreeVector kappaPD = transformations.partialDerivativesAngle(transformationName, pointPos, 2);

		TFreeVector trans1PD = transformations.partialDerivativesTranslation(transformationName, pointPos, 0);
		TFreeVector trans2PD = transformations.partialDerivativesTranslation(transformationName, pointPos, 1);
		TFreeVector trans3PD = transformations.partialDerivativesTranslation(transformationName, pointPos, 2);

		TFreeVector scalePD = transformations.partialDerivativesScale(transformationName, pointPos);

		if (fPointTransfo.getMLAused())
		{
			fPointTransfo.transform2MLA(omegaPD);
			fPointTransfo.transform2MLA(phiPD);
			fPointTransfo.transform2MLA(kappaPD);

			fPointTransfo.transform2MLA(trans1PD);
			fPointTransfo.transform2MLA(trans2PD);
			fPointTransfo.transform2MLA(trans3PD);

			fPointTransfo.transform2MLA(scalePD);
		}
		else if (fPointTransfo.getCGRFused())
		{
			fPointTransfo.getCCS2CGRF().transform(omegaPD);
			fPointTransfo.getCCS2CGRF().transform(phiPD);
			fPointTransfo.getCCS2CGRF().transform(kappaPD);

			fPointTransfo.getCCS2CGRF().transform(trans1PD);
			fPointTransfo.getCCS2CGRF().transform(trans2PD);
			fPointTransfo.getCCS2CGRF().transform(trans3PD);

			fPointTransfo.getCCS2CGRF().transform(scalePD);
		}

		omegaContrib = a * omegaPD.getX().getMetresValue() + b * omegaPD.getY().getMetresValue() + c * omegaPD.getZ().getMetresValue();
		phiContrib = a * phiPD.getX().getMetresValue() + b * phiPD.getY().getMetresValue() + c * phiPD.getZ().getMetresValue();
		kappaContrib = a * kappaPD.getX().getMetresValue() + b * kappaPD.getY().getMetresValue() + c * kappaPD.getZ().getMetresValue();

		trans1Contrib = a * trans1PD.getX().getMetresValue() + b * trans1PD.getY().getMetresValue() + c * trans1PD.getZ().getMetresValue();
		trans2Contrib = a * trans2PD.getX().getMetresValue() + b * trans2PD.getY().getMetresValue() + c * trans2PD.getZ().getMetresValue();
		trans3Contrib = a * trans3PD.getX().getMetresValue() + b * trans3PD.getY().getMetresValue() + c * trans3PD.getZ().getMetresValue();

		scaleContrib = a * scalePD.getX().getMetresValue() + b * scalePD.getY().getMetresValue() + c * scalePD.getZ().getMetresValue();

		TransformationContrib trContrib = {TFreeVector(omegaContrib, phiContrib, kappaContrib, TCoordSysFactory::k3DCartesian),
			TFreeVector(trans1Contrib, trans2Contrib, trans3Contrib, TCoordSysFactory::k3DCartesian), scaleContrib};
		transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib>(*it->adjTrafo, trContrib));
	}
}

void TContributionsGenerator::addTransformationsContributions(const TLOR2LOR &lorTrafo,
	const TPositionVector &pointPos,
	const Eigen::Vector3d &vec,
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> &transfContrib)
{
	addTransformationsContributions(lorTrafo, pointPos, vec[0], vec[1], vec[2], transfContrib);
}

void TContributionsGenerator::addTransformationsContributions3D(const TLOR2LOR &lorTrafo,
	const TPositionVector &pointPos,
	const TFreeVector &line1AMat,
	const TFreeVector &line2AMat,
	const TFreeVector &line3AMat,
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> &transfContrib)
{
	const std::vector<TLOR2LOR::TransformAndParams> &trafoChain = lorTrafo.getTransformationChain();

	TFreeVector omegaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector phiDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector kappaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector t1Derivative(TCoordSysFactory::k3DCartesian);
	TFreeVector t2Derivative(TCoordSysFactory::k3DCartesian);
	TFreeVector t3Derivative(TCoordSysFactory::k3DCartesian);
	TFreeVector scaleDeriv(TCoordSysFactory::k3DCartesian);
	// Iterate through the transformations, calculate contributions and store the contributiojn for every transformation
	for (auto it(trafoChain.begin()); it != trafoChain.end(); ++it)
	{
		std::string transformationName = it->adjTrafo->getName();

		// Contributions for rotations : Omega, Phi and Kappa
		omegaDerivative = lorTrafo.partialDerivativesAngle(transformationName, pointPos, 0);
		phiDerivative = lorTrafo.partialDerivativesAngle(transformationName, pointPos, 1);
		kappaDerivative = lorTrafo.partialDerivativesAngle(transformationName, pointPos, 2);

		// Contributions for translation: X, Y and Z coordinate
		t1Derivative = lorTrafo.partialDerivativesTranslation(transformationName, pointPos, 0);
		t2Derivative = lorTrafo.partialDerivativesTranslation(transformationName, pointPos, 1);
		t3Derivative = lorTrafo.partialDerivativesTranslation(transformationName, pointPos, 2);

		scaleDeriv = lorTrafo.partialDerivativesScale(transformationName, pointPos);

		if (fPointTransfo.getMLAused())
		{ // If MLA used, then transform contributions
			fPointTransfo.transform2MLA(omegaDerivative);
			fPointTransfo.transform2MLA(phiDerivative);
			fPointTransfo.transform2MLA(kappaDerivative);

			fPointTransfo.transform2MLA(t1Derivative);
			fPointTransfo.transform2MLA(t2Derivative);
			fPointTransfo.transform2MLA(t3Derivative);

			fPointTransfo.transform2MLA(scaleDeriv);
		}

		TransformationContrib firstEqContribSt = {
			TFreeVector(line1AMat.dot(omegaDerivative), line1AMat.dot(phiDerivative), line1AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian),
			TFreeVector(line1AMat.dot(t1Derivative), line1AMat.dot(t2Derivative), line1AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), line1AMat.dot(scaleDeriv)};

		TransformationContrib secondEqContribSt = {
			TFreeVector(line2AMat.dot(omegaDerivative), line2AMat.dot(phiDerivative), line2AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian),
			TFreeVector(line2AMat.dot(t1Derivative), line2AMat.dot(t2Derivative), line2AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), line2AMat.dot(scaleDeriv)};

		TransformationContrib thirdEqContribSt = {TFreeVector(line3AMat.dot(omegaDerivative), line3AMat.dot(phiDerivative), line3AMat.dot(kappaDerivative),
													  TCoordSysFactory::k3DCartesian), // Tg contribution for a third equation
			TFreeVector(line3AMat.dot(t1Derivative), line3AMat.dot(t2Derivative), line3AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), // Tg contribution for a third equation
			line3AMat.dot(scaleDeriv)};

		TransformationContrib3D stContrib = {firstEqContribSt, secondEqContribSt, thirdEqContribSt};
		transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>(*it->adjTrafo, stContrib));
	}
}

void TContributionsGenerator::addTransformationsContributions3D(const TLOR2LOR &lorTrafo,
	const TPositionVector &pointPos,
	const Eigen::Matrix3d &AMat,
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> &transfContrib)
{
	TFreeVector line1(AMat.row(0)), line2(AMat.row(1)), line3(AMat.row(2));
	addTransformationsContributions3D(lorTrafo, pointPos, line1, line2, line3, transfContrib);
}

void TContributionsGenerator::addPointContributionsPLR3D(const TLOR2LOR &lorTrafo, const Eigen::Matrix3d &Amat, Point3DContrib &pointContrib, bool station)
{
	TDenseMatrix derWrtPos = lorTrafo.getPartialDerivativeWrtPosition();

	if (fPointTransfo.getMLAused())
	{
		fPointTransfo.transform2MLA(derWrtPos, true);
	}

	Eigen::Matrix3d mat = Amat * derWrtPos;
	if (station)
	{
		pointContrib.contrib = -mat;
	}
	else
	{ // Target
		pointContrib.contrib = mat;
	}
}

std::string TContributionsGenerator::getNameAndLine(const LGCAdjustablePoint &point) const
{
	return std::string(point.getName() + " defined in line " + std::to_string(point.line));
}

void TContributionsGenerator::generateContributionError(const std::string &message) const
{
	throw std::logic_error(message);
}

LIBRPointGroupContrib TContributionsGenerator::getPointGroupConstraintContrib(const TLGCPointConstraintGroup pointConstraintGroup, const TLGCData &data)
{
	// all constraints (COG, Momentum and scale) are computed, the inputmatrixfiller will use only the activated ones
	PointGroupConstraintContrib3D resultCOG;
	PointGroupConstraintContrib3D resultMOM;
	PointGroupConstraintContrib resultScale;

	//// Scale Constraint
	//// we use here the version sum |p_i-COG(p)|^2
	//// note d (p_i-COG(p)) / d(p_i) = (1-1/#points)Id

	// preparatory steps before computations are done in a loop over affected points
	std::set<std::string> affectedPoints = pointConstraintGroup.getAffectedPoints();
	int numberOfPoints = affectedPoints.size();
	if (numberOfPoints == 0)
	{
		throw std::logic_error("Contribution of point constraint group undefined for empty group.");
	}
	double averagingFactor = 1.0 / double(numberOfPoints);
	Eigen::Vector3d provCOG = pointConstraintGroup.getProvCOG();
	Eigen::Vector3d estCOG = pointConstraintGroup.computeEstCOG();
	Eigen::Vector3d currentMomentum = Eigen::Vector3d::Zero();
	double provScale = pointConstraintGroup.getProvScale();
	double currentScale = 0;

	// the COG misclosure can be computed outside the loop
	resultCOG.constraintMisclosure = estCOG - provCOG;

	// all derivatives and the misclosure of the Momentum and scale constraint require a loop over the affected points
	for (auto pointName : affectedPoints)
	{
		// preparatory data used in computations
		LGCAdjustablePoint point = data.getPoints().getObject(pointName);
		TLOR2LOR sub2Root(point.getFrameTreePosition(), data.getTree().begin(), "sub2Root");
		TPositionVector pointInSubframe = point.getEstimatedValue();
		TPositionVector pointInRoot = point.getEstimatedValue();
		sub2Root.transform(pointInRoot);
		Eigen::Vector3d positionInRoot = pointInRoot.toRealVector();
		Eigen::Vector3d provisionalInRoot = pointConstraintGroup.getProvRootPos(pointName);

		// COG
		// derivative wrt point coordinates
		resultCOG.PointContrib[pointName] = averagingFactor * sub2Root.getPartialDerivativeWrtPosition();

		// derivative wrt transformations
		Eigen::Matrix3d normalizedIdentity;
		normalizedIdentity.setIdentity();
		normalizedIdentity *= averagingFactor;

		std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> point2RootTransformationsContrib;
		addTransformationsContributions3D(sub2Root, pointInSubframe, normalizedIdentity, point2RootTransformationsContrib);
		resultCOG.TransformContrib[pointName] = point2RootTransformationsContrib;

		// Momentum
		// update momentum sum used in momentum misclosure
		Eigen::Vector3d diff2Root = provCOG - provisionalInRoot;
		currentMomentum += positionInRoot.cross(diff2Root);

		// derivative wrt point coordinates
		// prepare the matrix that is multiplied from the left with the partial derivatives
		Eigen::Matrix3d A = Eigen::Matrix3d::Zero();
		A << 0, -diff2Root[2], diff2Root[1], diff2Root[2], 0, -diff2Root[0], -diff2Root[1], diff2Root[0], 0;
		resultMOM.PointContrib[pointName] = -A * sub2Root.getPartialDerivativeWrtPosition();

		// derivative wrt transformations
		std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> point2RootTransformationsContribMOM;
		addTransformationsContributions3D(sub2Root, pointInSubframe, -A, point2RootTransformationsContribMOM);
		resultMOM.TransformContrib[pointName] = point2RootTransformationsContribMOM;

		// Scale
		Eigen::Vector3d diff2COG = positionInRoot - estCOG;

		// used in scale misclosure
		currentScale += pow2(diff2COG.norm());
		// derivatives with respect to point coordinates:
		Eigen::Vector3d Aline = 2 * diff2COG;
		Eigen::Vector3d derWRTPos = (Aline.transpose() * sub2Root.getPartialDerivativeWrtPosition()).transpose();
		resultScale.PointContrib[pointName] = derWRTPos;
		// derivative wrt transformations
		std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> point2RootTransformationsContribScale;
		addTransformationsContributions(sub2Root, pointInSubframe, Aline, point2RootTransformationsContribScale);
		resultScale.TransformContrib[pointName] = point2RootTransformationsContribScale;
	}

	// set the momentum and scale misclosure which are ready now
	resultMOM.constraintMisclosure = currentMomentum;
	resultScale.constraintMisclosure = currentScale - provScale;

	return LIBRPointGroupContrib{resultCOG, resultMOM, resultScale};
}

pointSigmaContrib TContributionsGenerator::getPointSigmaContrib(LGCAdjustablePoint &pt, const TLGCData *data)
{
	// model is:
	// min |weightMat*V|^2
	// rotMat (est-prov)+V=0
	// the left product is the rotated offset
	const pointSigmaData &ptSigma = pt.getPointSigmaData();
	Eigen::Vector3d rotOffset = ptSigma.calcRotOffset(pt, data);
	return pointSigmaContrib({rotOffset});
}

SagPairContrib TContributionsGenerator::getSagPairContrib(const TLGCSagConstraintPair &sagPair, const TLGCData &data)
{
	SagPairContrib contrib;
	const LGCAdjustableSag &sagObject = sagPair.getSag();

	SagPairBaseFrame bf = sagPair.transformToBaseFrame(data);

	double zSag = sagObject.getZSag().getEstimatedValue().getMetresValue();
	double zCurv = sagObject.getZCurv().getEstimatedValue().getMetresValue();
	double xSag = sagObject.getXSag().getEstimatedValue().getMetresValue();
	double xCurv = sagObject.getXCurv().getEstimatedValue().getMetresValue();

	// dy = y coordinate of reference point in sag frame (longitudinal distance)
	double dy = bf.refSag.getY().getMetresValue();
	double dy2 = pow2q(dy);

	// offset components in sag frame: x and z
	Eigen::Vector3d ex(1, 0, 0);
	Eigen::Vector3d ez(0, 0, 1);
	double relZOffset = zSag + zCurv * dy2;
	double relXOffset = xSag + xCurv * dy2;
	Eigen::Vector3d offset = relXOffset * ex + relZOffset * ez;

	// constraint: refSag + offset - assocSag = 0
	contrib.constraintMisclosure = bf.refSag.toRealVector() + offset - bf.assocSag.toRealVector();

	// derivatives wrt sag parameters
	contrib.dOffsetdZSag = ez;
	contrib.dOffsetdZCurv = dy2 * ez;
	contrib.dOffsetdXSag = ex;
	contrib.dOffsetdXCurv = dy2 * ex;

	// derivatives wrt point coordinates
	// d(offset)/d(refSag) is a rank-1 3x3 matrix: d(offset)/d(dy) * ey^T
	// d(offset)/d(dy) = 2*dy * (xCurv * ex + zCurv * ez)
	Eigen::Vector3d dOffsetdDy = 2 * dy * (xCurv * ex + zCurv * ez);
	Eigen::Vector3d ey(0, 1, 0);
	Eigen::Matrix3d AOffset = dOffsetdDy * ey.transpose(); // rank-1 matrix

	Eigen::Matrix3d id3 = Eigen::Matrix3d::Identity();
	Eigen::Matrix3d R_ref2sag = bf.ref2Sag.getPartialDerivativeWrtPosition();
	Eigen::Matrix3d R_assoc2sag = bf.assoc2Sag.getPartialDerivativeWrtPosition();

	// d(constraint)/d(refSub) = (I + AOffset) * R_ref2sag
	contrib.dConstraintdRefSub = {(id3 + AOffset) * R_ref2sag};
	// d(constraint)/d(assocSub) = -R_assoc2sag
	contrib.dConstraintdAssocSub = {-R_assoc2sag};

	// Frame transformation derivatives
	addTransformationsContributions3D(bf.ref2Sag, bf.refSub, id3 + AOffset, contrib.dConstraintdRefHelmert);
	addTransformationsContributions3D(bf.assoc2Sag, bf.assocSub, -id3, contrib.dConstraintdAssocHelmert);

	return contrib;
}

/*
 * INCL Helper Functions
 *
 * These functions compute transformation contributions for INCLY and ROLLY measurements
 * during least squares adjustment. They calculate how frame transformations affect
 * the measurement residuals and contribute to the normal equations.
 */

/*
 * ROLLY Contribution Calculator (Legacy atan2 Model)
 *
 * Computes the full contribution of ROLLY measurements: calculated measurement, observation variance,
 * and frame transformation contributions using the legacy atan2(X,Z) mathematical model.
 * This function calculates partial derivatives with respect to transformation parameters
 * (omega, phi, kappa, scale) and their contributions to the measurement equation.
 *
 * Mathematical Model: ROLLY = atan2(X, Z) where X and Z are local coordinates
 *
 * @param lorTrafo: Local Object Reference transformation containing the
 *                   transformation chain and parameters
 * @param stationVRoot: Free vector representing the local vertical in root coordinates
 * @param locVert: Local vertical vector [x, y, z] in the station frame (after ref angle rotation)
 * @param target: Instrument data containing stochastic model and correction parameters
 *
 * @return INCLContrib containing:
 *         - Calculated measurement (atan2 model)
 *         - Rotation contributions (omega, phi, kappa) and scale contribution for each transformation
 *         - Observation variance
 */
INCLContrib TContributionsGenerator::addROLLYContribution(const TLOR2LOR &lorTrafo, const TFreeVector &stationVRoot, const Eigen::Vector3d &locVert, const TInstrumentData::TINCL &target)
{
	double x = locVert(0), z = locVert(2);
	double square = pow2q(x) + pow2q(z);
	if (isZero(square))
		throw std::logic_error("Cannot compute the contribution for this observation because both X and Z components are near zero (degenerate case for atan2 model)");

	TAngle calcMeas = TAngle::aTan2(x, z) + target.angleCorrectionValue;

	// Variance propagation (refAngleDerivative is constant 1 for atan2 model)
	TReal refAngleDerivative = 1.0;
	TReal ppmVarianceContribution = target.sigmaPpm.getRadiansValue() * calcMeas;
	TReal obsVariance = pow2q(target.sigmaAngl.getRadiansValue() + ppmVarianceContribution) + pow2q(target.sigmaCorrectionValue.getRadiansValue())
		+ pow2q(target.refSigmaCorrectionValue.getRadiansValue() * refAngleDerivative);

	// Gradient of atan2(x, z) wrt local vertical vector
	Eigen::Vector3d trigoGrad;
	trigoGrad << z / square, 0.0, -x / square;
	TRotationMatrix referenceAngleRotation(TRotationMatrix::ERotationType::kRxyz, 0, target.refAngleCorrectionValue, 0);
	Eigen::Matrix3d refRotMat = referenceAngleRotation.inverse().getMat();
	trigoGrad = refRotMat.transpose() * trigoGrad;

	return {calcMeas, addINCLContribHelper(lorTrafo, stationVRoot, trigoGrad), obsVariance};
}

/*
 * INCLY Contribution Calculator (arcsin Model)
 *
 * Computes the full contribution of INCLY measurements: calculated measurement, observation variance,
 * and frame transformation contributions using the arcsin(X/|V|) mathematical model.
 * This function calculates partial derivatives with respect to transformation parameters
 * (omega, phi, kappa, scale) and their contributions to the measurement equation.
 *
 * Mathematical Model: INCLY = arcsin(X/|V|) where |V| = sqrt(X^2 + Y^2 + Z^2)
 *
 * @param lorTrafo: Local Object Reference transformation containing the
 *                   transformation chain and parameters
 * @param stationVRoot: Free vector representing the local vertical in root coordinates
 * @param locVert: Local vertical vector [x, y, z] in the station frame (after ref angle rotation)
 * @param target: Instrument data containing stochastic model and correction parameters
 *
 * @return INCLContrib containing:
 *         - Calculated measurement (arcsin model)
 *         - Rotation contributions (omega, phi, kappa) and scale contribution for each transformation
 *         - Observation variance
 */
INCLContrib TContributionsGenerator::addINCLYContribution(const TLOR2LOR &lorTrafo, const TFreeVector &stationVRoot, const Eigen::Vector3d &locVert, const TInstrumentData::TINCL &target)
{
	// INCLY model: arcsin(X/|V|)
	double x = locVert(0), y = locVert(1), z = locVert(2);
	double r2 = locVert.squaredNorm();
	if (isZero(r2))
		throw std::logic_error("Cannot compute the contribution for this observation because the local vertical vector has zero length at this position");
	double r2_x2 = r2 - x * x;
	if (isZero(r2_x2))
		throw std::logic_error(
			"Cannot compute the contribution for this observation because the X component equals the vector magnitude (degenerate case for arcsin model)");
	double sqrtR2_x2 = std::sqrt(r2_x2);

	double clampedX = std::clamp(x / std::sqrt(r2), -1.0, 1.0);
	TAngle calcMeas = TAngle(std::asin(clampedX)) + target.angleCorrectionValue;
	TReal refAngleDerivative = z / sqrtR2_x2;

	// Variance propagation
	TReal ppmVarianceContribution = target.sigmaPpm.getRadiansValue() * calcMeas;
	TReal obsVariance = pow2q(target.sigmaAngl.getRadiansValue() + ppmVarianceContribution) + pow2q(target.sigmaCorrectionValue.getRadiansValue())
		+ pow2q(target.refSigmaCorrectionValue.getRadiansValue() * refAngleDerivative);

	// Gradient of arcsin(X/|V|) wrt local vertical vector
	Eigen::Vector3d trigoGrad;
	trigoGrad << (sqrtR2_x2 / r2), (-(x * y) / (r2 * sqrtR2_x2)), (-(x * z) / (r2 * sqrtR2_x2));
	TRotationMatrix referenceAngleRotation(TRotationMatrix::ERotationType::kRxyz, 0, target.refAngleCorrectionValue, 0);
	Eigen::Matrix3d refRotMat = referenceAngleRotation.inverse().getMat();
	trigoGrad = refRotMat.transpose() * trigoGrad;

	return {calcMeas, addINCLContribHelper(lorTrafo, stationVRoot, trigoGrad), obsVariance};
}

/*
 * Unified INCL Contribution Helper
 *
 * This function unifies the common logic between addINCLYContribution and addROLLYContribution
 * to reduce code duplication. The only difference between the two models is the trigoDiff vector.
 *
 * @param lorTrafo: Local Object Reference transformation containing the transformation chain
 * @param vector: Free vector representing the local vertical in root coordinates
 * @param trigoDiff: Jacobian of Model function wrt local vertical vector.
 *
 * @return Transformation contribution structure containing rotation and scale contributions
 */
decltype(INCLContrib::fStTransformContrib) TContributionsGenerator::addINCLContribHelper(const TLOR2LOR &lorTrafo, const TFreeVector &vector, const Eigen::Vector3d &trigoDiff)
{
	const std::vector<TLOR2LOR::TransformAndParams> &trafoChain = lorTrafo.getTransformationChain();
	std::string transformationName;
	TFreeVector scalePD(TCoordSysFactory::k3DCartesian);
	TFreeVector omegaPD(TCoordSysFactory::k3DCartesian);
	TFreeVector phiPD(TCoordSysFactory::k3DCartesian);
	TFreeVector kappaPD(TCoordSysFactory::k3DCartesian);

	TReal omegaContrib, phiContrib, kappaContrib, scaleContrib;
	decltype(INCLContrib::fStTransformContrib) transfContrib;

	for (const auto &it : trafoChain)
	{
		transformationName = it.adjTrafo->getName();
		omegaPD = lorTrafo.partialDerivativesAngle(transformationName, vector, 0);
		phiPD = lorTrafo.partialDerivativesAngle(transformationName, vector, 1);
		kappaPD = lorTrafo.partialDerivativesAngle(transformationName, vector, 2);
		scalePD = lorTrafo.partialDerivativesScale(transformationName, vector);

		omegaContrib = trigoDiff.dot(omegaPD.toRealVector());
		phiContrib = trigoDiff.dot(phiPD.toRealVector());
		kappaContrib = trigoDiff.dot(kappaPD.toRealVector());
		scaleContrib = trigoDiff.dot(scalePD.toRealVector());

		TransformationContrib trContrib = {
			TFreeVector(omegaContrib, phiContrib, kappaContrib, TCoordSysFactory::k3DCartesian), TFreeVector(0, 0, 0, TCoordSysFactory::k3DCartesian), scaleContrib};
		transfContrib.emplace_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib>(*it.adjTrafo, trContrib));
	}
	return transfContrib;
}



// Explicit instantiations for getPolarContribInFrame. To avoid this, the implementation would have to be in the header file
template PolarContribInFrame TContributionsGenerator::getPolarContribInFrame<TANGL>(std::shared_ptr<TTSTN> station, const TANGL &dist, const ModelAndJacobian &model);
template PolarContribInFrame TContributionsGenerator::getPolarContribInFrame<TLINE>(std::shared_ptr<TTSTN> station, const TLINE &dist, const ModelAndJacobian &model);
