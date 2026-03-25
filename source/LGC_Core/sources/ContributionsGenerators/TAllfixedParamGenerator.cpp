// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TAllfixedParamGenerator.h"

#include "LGCAdjustablePoint.h"
#include "TDist.h"
#include "TTreeEntry.h"
#include "TXYH2CCS.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////
TAllfixedParamGenerator::TAllfixedParamGenerator(TPointTransformer &fPointTransfoFunc) : fPointTransfo(fPointTransfoFunc)
{
}

//////////////////////////////////////////////////////////////////////
// PARAMETER CALCULATION
//////////////////////////////////////////////////////////////////////

TAngle TAllfixedParamGenerator::getV0AllfixedANGL(const TTSTN &station, const TTSTN::TROM &rom, const TANGL &angl)
{
	TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	targetPos = angl.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(angl.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	// Calculated measurement value
	TAngle calcMeas = TAngle::aTan2((xTg - xSt), (yTg - ySt)) - rom.acst; // ACST is the constant orientation of the instrument

	return (calcMeas - angl.getAngle());
}

TAngle *TAllfixedParamGenerator::getV0AllfixedECDIR(const TTSTN &station, const TTSTN::TROM &rom, const TECDIR &ecdir)
{
	TPositionVector targetPos = station.instrumentPos->getEstimatedValue(); // position of the scale. Point to measure
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = ecdir.targetPos->getEstimatedValue(); // position of the TSTN
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		ecdir.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(ecdir.targetPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();
	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();

	TAngle theta = ecdir.obsHorAngle;
	TAngle phi = ecdir.obsVertAngle;
	TAngle Vo = rom.v0->getEstimatedValue();
	// line direction at the TSTN position
	TFreeVector l(sin(theta + Vo) * sin(phi), cos(theta + Vo) * sin(phi), cos(phi), TCoordSysFactory::ECoordSys::k3DCartesian);

	// Calcul par le produit scalaire (u^l)^2+(u.l)^2=|v|^2|l|^2
	TReal d, pScal;
	d = sqrt(pow2(xSt - xTg) + pow2(ySt - yTg) + pow2(zSt - zTg)); // distance St-Tg
	pScal = l[0] * (xSt - xTg) + l[1] * (ySt - yTg) + l[2] * (zSt - zTg); // produit scalaire

	TReal lambda = sqrt(pow2(d) - pow2(ecdir.getDistance() + ecdir.target.distCorrectionValue));

	TReal k = -1.0 * (lambda - (zSt - zTg) * ecdir.obsVertAngle.cosine());

	initSolution();
	solveTrigoEquation((ySt - yTg) * ecdir.obsVertAngle.sine(), (xSt - xTg) * ecdir.obsVertAngle.sine(), k);

	// solveTrigoEquation gives a solution for theta+V0, we search V0 only.
	fSolutionTrigo[0] -= ecdir.obsHorAngle;
	fSolutionTrigo[1] -= ecdir.obsHorAngle;

	return fSolutionTrigo;
}

TAngle TAllfixedParamGenerator::getV0AllfixedECTH(const TTSTN &station, const TECTH &ecth)
{
	TPositionVector targetPos = station.instrumentPos->getEstimatedValue(); // position of the scale. Point to measure
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = ecth.targetPos->getEstimatedValue(); // position of the TSTN
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(ecth.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(ecth.targetPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	TReal d = dist(xSt, ySt, xTg, yTg);

	TAngle theta = ecth.obsHorAngle;
	TAngle bearing = TAngle(atan2((xTg - xSt), (yTg - ySt)), TAngle::EUnits::kRadians);
	TAngle alpha = TAngle(asin(ecth.getDistance().getMetresValue() / d), TAngle::EUnits::kRadians);

	return (bearing + alpha - theta);
}

TAngle *TAllfixedParamGenerator::getV0AllfixedPLR(const TTSTN &station, const TTSTN::TROM &rom, const TPLR3D &plr3D)
{
	TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		plr3D.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal cosTheta = plr3D.getAngle(kANGL).cosine();
	TReal sinPhi = plr3D.getAngle(kZEND).sine();

	TReal dX = targetPos.getX().getMetresValue() - stationPos.getX().getMetresValue();
	TReal dY = targetPos.getY().getMetresValue() - stationPos.getY().getMetresValue();

	// TReal V0 = NO_VALf;

	TReal A, B, C;
	A = -dY; // cos
	B = -dX; // sin
	C = (plr3D.getDistance() + plr3D.target.distCorrectionValue) * cosTheta * sinPhi;

	initSolution();
	solveTrigoEquation(A, B, C);
	// On a les solutions v0 -acst
	fSolutionTrigo[0] += rom.acst;
	fSolutionTrigo[1] += rom.acst;

	return fSolutionTrigo;
}

TAngle *TAllfixedParamGenerator::getRxAllfixedPLR(const TTSTN &station, const TTSTN::TROM & /*rom*/, const TPLR3D &plr3D)
{
	TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		plr3D.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal cosPhi = plr3D.getAngle(kZEND).cosine();
	TReal dY = targetPos.getY().getMetresValue() - stationPos.getY().getMetresValue();
	TReal dZ = targetPos.getZ().getMetresValue() + plr3D.target.targetHt - stationPos.getZ().getMetresValue() - station.instrumentHeightAdjustable->getProvisionalValue();

	TReal A, B, C;
	A = -dZ;
	B = -dY;
	C = (plr3D.getDistance() + plr3D.target.distCorrectionValue) * cosPhi;

	initSolution();
	solveTrigoEquation(A, B, C);

	return fSolutionTrigo;
}

TAngle *TAllfixedParamGenerator::getRyAllfixedPLR(const TTSTN &station, const TTSTN::TROM & /*rom*/, const TPLR3D &plr3D)
{
	TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		plr3D.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal cosPhi = plr3D.getAngle(kZEND).cosine();
	TReal dX = targetPos.getX().getMetresValue() - stationPos.getX().getMetresValue();
	TReal dZ = targetPos.getZ().getMetresValue() + plr3D.target.targetHt - stationPos.getZ().getMetresValue() - station.instrumentHeightAdjustable->getProvisionalValue();

	TReal A, B, C;
	A = -dZ;
	B = -dX;
	C = (plr3D.getDistance() + plr3D.target.distCorrectionValue) * cosPhi;

	initSolution();
	solveTrigoEquation(A, B, C);

	return fSolutionTrigo;
}

TAngle TAllfixedParamGenerator::getCollimationAllfixedDLEV(const TLEVEL &levelInstr, const TDLEV &dlev)
{
	TReal cdz = dlev.target.distCorrectionValue; // distance correction value
	TReal dRef = levelInstr.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(); // Distance of the reference point from the plane

	TPositionVector referencePoint = levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue();
	const TLOR2LOR &refPTLor2RootTrafo = fPointTransfo.getLORTransformation(
		levelInstr.fMeasuredPlane->getReferencePoint()->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	refPTLor2RootTrafo.transform(referencePoint);

	TPositionVector staffPosition = dlev.targetPos->getEstimatedValue(); // this Target / Levelling Staff / SCALE assumed to be in ROOT!!!!!
	const TLOR2LOR &staffPTLor2RootTrafo = fPointTransfo.getLORTransformation(dlev.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	staffPTLor2RootTrafo.transform(staffPosition);

	// If not OLOC => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		fPointTransfo.transformPointsToMLASystem(levelInstr.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, staffPosition);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal dTg = sqrtq(pow2q(staffPosition.getX().getMetresValue() - referencePoint.getX().getMetresValue())
		+ pow2q(staffPosition.getY().getMetresValue() - referencePoint.getY().getMetresValue()));

	// TReal zInst = referencePoint.getZ().getMetresValue();
	// TReal zTg = dlev.targetPos->getEstimatedValue().getZ().getMetresValue();

	TReal calcMeas = referencePoint.getZ().getMetresValue() - staffPosition.getZ().getMetresValue() + dRef - cdz - dlev.getDistance();

	return TAngle(atan(calcMeas / dTg), TAngle::EUnits::kRadians);
}

TLength TAllfixedParamGenerator::getHiAllfixedZEND(const TTSTN &station, const TZEND &zend)
{
	TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	targetPos = zend.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(zend.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	// PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();

	TReal hTg = zend.target.targetHt;

	TReal distance2D = dist(xSt, ySt, xTg, yTg);

	return TLength(zTg + hTg - zSt - distance2D * 1 / tan(zend.getAngle()), TLength::EUnits::kMetres);
}

TLength TAllfixedParamGenerator::getHiAllfixedDIST(const TTSTN &station, const TLINE &dist)
{
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(dist.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	// Prepare coefficients (a,b,c) for the points and the transformations contributions
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();

	TReal hTg = dist.target.targetHt;
	TReal hInst = station.instrumentHeightAdjustable->getEstimatedValue();
	TReal cst = dist.target.distCorrectionAdjustable->getProvisionalValue();

	int sign = 0;
	if (zSt + hInst > zTg + hTg)
		sign = 1;
	else
		sign = -1;

	TReal distance2D = pow2(dist.getDistance() + cst) - pow2(xSt - xTg) - pow2(ySt - yTg);

	return TLength(zTg + hTg - zSt + sign * sqrt(distance2D));
}

TLength TAllfixedParamGenerator::getHiAllfixedPLR(const TTSTN &station, const TTSTN::TROM &rom, const TPLR3D &plr3D)
{
	TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		plr3D.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal Rx = 0.0; // Rotation around x-axis, default is no rotation
	TReal Ry = 0.0; // Rotation around y-axis, default is no rotation

	if (station.rot3D)
	{ // If station can rotate freely get the rotation values
		if (station.rotX == nullptr || station.rotY == nullptr)
			throw std::runtime_error("TAllfixedParamGenerator::getHiAllfixedPLR station can rotate freely, but rotation angles are NULL.");

		Rx = station.rotX->getProvisionalValue().getRadiansValue();
		Ry = station.rotY->getProvisionalValue().getRadiansValue();
	}

	TReal sinV0 = (rom.v0->getProvisionalValue() - rom.acst).sine();
	TReal cosV0 = (rom.v0->getProvisionalValue() - rom.acst).cosine();

	TReal sinRx = sinq(Rx);
	TReal cosRx = cosq(Rx);

	TReal sinRy = sinq(Ry);
	TReal cosRy = cosq(Ry);

	TFreeVector line1AMat(cosV0 * cosRy, -sinV0 * cosRx + sinRx * cosV0 * sinRy, sinV0 * sinRx + cosRx * cosV0 * sinRy, TCoordSysFactory::k3DCartesian); // first line of the A-matrix
	TFreeVector line2AMat(sinV0 * cosRy, cosV0 * cosRx + sinRx * sinV0 * sinRy, -cosV0 * sinRx + cosRx * sinV0 * sinRy, TCoordSysFactory::k3DCartesian); // second line of the A-matrix
	TFreeVector line3AMat(-sinRy, sinRx * cosRy, cosRx * cosRy, TCoordSysFactory::k3DCartesian); // third line of the A-matrix

	TReal sinTheta = plr3D.getAngle(kANGL).sine();
	TReal cosTheta = plr3D.getAngle(kANGL).cosine();

	TReal sinPhi = plr3D.getAngle(kZEND).sine();
	TReal cosPhi = plr3D.getAngle(kZEND).cosine();

	TReal dX = targetPos.getX().getMetresValue() - stationPos.getX().getMetresValue();
	TReal dY = targetPos.getY().getMetresValue() - stationPos.getY().getMetresValue();
	// TReal dZ = targetPos.getZ().getMetresValue() + plr3D.target.targetHt - stationPos.getZ().getMetresValue() - station.instrumentHeightAdjustable->getProvisionalValue();

	TReal Hi = NO_VALf;
	if (fabs(line3AMat[2]) < nullLimit)
		Hi = ((plr3D.getDistance() + plr3D.target.distCorrectionValue) * cosPhi - line3AMat[0] * dX - line3AMat[1] * dY
				 - line3AMat[2] * (targetPos.getZ().getMetresValue() + plr3D.target.targetHt - stationPos.getZ().getMetresValue()))
			/ line3AMat[3];
	else if (fabs(line2AMat[2]) < nullLimit)
		Hi = ((plr3D.getDistance() + plr3D.target.distCorrectionValue) * sinPhi * cosTheta - line2AMat[0] * dX - line2AMat[1] * dY
				 - line2AMat[2] * (targetPos.getZ().getMetresValue() + plr3D.target.targetHt - stationPos.getZ().getMetresValue()))
			/ line2AMat[3];
	else if (fabs(line1AMat[2]) < nullLimit)
		Hi = ((plr3D.getDistance() + plr3D.target.distCorrectionValue) * sinPhi * sinTheta - line1AMat[0] * dX - line1AMat[1] * dY
				 - line1AMat[2] * (targetPos.getZ().getMetresValue() + plr3D.target.targetHt - stationPos.getZ().getMetresValue()))
			/ line1AMat[3];
	else
		throw std::logic_error("TAllfixedParamGenerator::getHiAllfixedPLR:No solution can be determine. Division by 0.");

	return TLength(Hi);
}

TLength TAllfixedParamGenerator::getCsAllfixedDSPT(const TEDM &edmST, const TDSPT &dspt)
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

	// Prepare coefficients (a,b,c) for the points and the transformations contributions
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();
	TReal hInst = edmST.instrument.instrHeight.getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();
	TReal hTg = dspt.target.targetHt;

	TReal D = dist3D(xSt, ySt, (zSt + hInst), xTg, yTg, (zTg + hTg));

	return TLength(D - dspt.getDistance());
}

TLength TAllfixedParamGenerator::getCsAllfixedDHOR(const TTSTN &station, const TLINE &dhor)
{
	TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	targetPos = dhor.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(dhor.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	// Prepare coefficients (a,b,c) for the points and the transformations contributions
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	TReal D = dist(xSt, ySt, xTg, yTg);

	return TLength(D - dhor.getDistance());
}

TLength TAllfixedParamGenerator::getCsAllfixedDIST(const TTSTN &station, const TLINE &dist)
{
	TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	targetPos = dist.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(dist.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	// Prepare coefficients (a,b,c) for the points and the transformations contributions
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();
	TReal hInst = station.instrumentHeightAdjustable->getProvisionalValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();
	TReal hTg = dist.target.targetHt;

	TReal D = dist3D(xSt, ySt, (zSt + hInst), xTg, yTg, (zTg + hTg));

	return TLength(D - dist.getDistance());
}

TLength TAllfixedParamGenerator::getCsAllfixedPLR(const TTSTN &station, const TTSTN::TROM &rom, const TPLR3D &plr3D)
{
	TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
	targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR &tgLor2RootTrafo = fPointTransfo.getLORTransformation(
		plr3D.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR &stLor2RootTrafo = fPointTransfo.getLORTransformation(
		station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true)
	{
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal Rx = 0.0; // Rotation around x-axis, default is no rotation
	TReal Ry = 0.0; // Rotation around y-axis, default is no rotation

	if (station.rot3D)
	{ // If station can rotate freely get the rotation values
		if (station.rotX == nullptr || station.rotY == nullptr)
			throw std::runtime_error("TAllfixedParamGenerator::getCsAllfixedPLR station can rotate freely, but rotation angles are NULL.");
		Rx = station.rotX->getProvisionalValue().getRadiansValue();
		Ry = station.rotY->getProvisionalValue().getRadiansValue();
	}

	TReal sinV0 = (rom.v0->getProvisionalValue() - rom.acst).sine();
	TReal cosV0 = (rom.v0->getProvisionalValue() - rom.acst).cosine();

	TReal sinRx = sinq(Rx);
	TReal cosRx = cosq(Rx);

	TReal sinRy = sinq(Ry);
	TReal cosRy = cosq(Ry);

	TFreeVector line1AMat(cosV0 * cosRy, -sinV0 * cosRx + sinRx * cosV0 * sinRy, sinV0 * sinRx + cosRx * cosV0 * sinRy, TCoordSysFactory::k3DCartesian); // first line of the A-matrix
	TFreeVector line2AMat(sinV0 * cosRy, cosV0 * cosRx + sinRx * sinV0 * sinRy, -cosV0 * sinRx + cosRx * sinV0 * sinRy, TCoordSysFactory::k3DCartesian); // second line of the A-matrix
	TFreeVector line3AMat(-sinRy, sinRx * cosRy, cosRx * cosRy, TCoordSysFactory::k3DCartesian); // third line of the A-matrix

	TReal sinTheta = plr3D.getAngle(kANGL).sine();
	TReal cosTheta = plr3D.getAngle(kANGL).cosine();

	TReal sinPhi = plr3D.getAngle(kZEND).sine();
	TReal cosPhi = plr3D.getAngle(kZEND).cosine();

	TReal dX = targetPos.getX().getMetresValue() - stationPos.getX().getMetresValue();
	TReal dY = targetPos.getY().getMetresValue() - stationPos.getY().getMetresValue();
	TReal dZ = targetPos.getZ().getMetresValue() + plr3D.target.targetHt - stationPos.getZ().getMetresValue() - station.instrumentHeightAdjustable->getProvisionalValue();

	TReal Cs = NO_VALf;

	if (fabs(cosPhi) < nullLimit)
		Cs = (line1AMat[0] * dX + line1AMat[1] * dY + line1AMat[2] * dZ - plr3D.getDistance() * cosPhi) / cosPhi;
	else if (fabs(cosTheta) < nullLimit)
		Cs = (line2AMat[0] * dX + line2AMat[1] * dY + line2AMat[2] * dZ - plr3D.getDistance() * cosTheta * sinPhi) / (cosTheta * sinPhi);
	else
		Cs = (line3AMat[0] * dX + line3AMat[1] * dY + line3AMat[2] * dZ - plr3D.getDistance() * sinTheta * sinPhi) / (sinTheta * sinPhi);

	return TLength(Cs);
}

TLength TAllfixedParamGenerator::getCsAllfixedDISTinFrame(const TTSTN &station, const TLINE &dist)
{
	fPointTransfo.setMLA(false); // TSTN in Frame measurements never in MLA
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();

	const TLOR2LOR &tg2stTrafo = fPointTransfo.getLORTransformation(dist.targetPos->getFrameTreePosition(), station.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	tg2stTrafo.transform(targetPos);

	// Prepare coefficients (a,b,c) for the points and the transformations contributions
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();
	TReal hInst = station.instrumentHeightAdjustable->getProvisionalValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();
	TReal hTg = dist.target.targetHt;

	TReal D = dist3D(xSt, ySt, (zSt + hInst), xTg, yTg, (zTg + hTg));

	return TLength(D - dist.getDistance());
}

// solve second degree equation a*x^2 + b*x + c =0
void TAllfixedParamGenerator::solve2ndDegree(TReal a, TReal b, TReal c)
{
	TReal delta = b * b - 4 * a * c;

	if (/*delta == 0.0*/ delta < 1e-5 && delta > -1e-5)
		fSolution2ndD[0] = -b / (2 * a);
	else if (delta > 0.0)
	{
		fSolution2ndD[0] = (-b - sqrt(delta)) / (2 * a);
		fSolution2ndD[1] = (-b + sqrt(delta)) / (2 * a);
	}
	else
		throw std::runtime_error("TAllfixedParamGenerator::solve2ndDegree no real solution");
}

// solve trigo equation A*cosX + B*sinX + C =0
void TAllfixedParamGenerator::solveTrigoEquation(TReal A, TReal B, TReal C)
{
	TReal a, b, c;
	a = C - A;
	b = 2 * B;
	c = C + A;
	solve2ndDegree(a, b, c);

	if (fSolution2ndD[0] != NO_VALf)
		fSolutionTrigo[0] = TAngle(2 * atan(fSolution2ndD[0]), TAngle::EUnits::kRadians);
	if (fSolution2ndD[1] != NO_VALf)
		fSolutionTrigo[1] = TAngle(2 * atan(fSolution2ndD[1]), TAngle::EUnits::kRadians);
}

void TAllfixedParamGenerator::initSolution()
{
	fSolution2ndD[0] = NO_VALf;
	fSolution2ndD[1] = NO_VALf;
	fSolutionTrigo[0] = TAngle(NO_VALf);
	fSolutionTrigo[1] = TAngle(NO_VALf);
}
