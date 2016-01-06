#include "TAllfixedParamGenerator.h"
#include "TDist.h" 
#include "TTreeEntry.h"
#include "TXYH2CCS.h"

//#include "TLEVEL.h"
//#include "TEDM.h"


//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////
TAllfixedParamGenerator::TAllfixedParamGenerator(TPointTransformer& fPointTransfoFunc) :fPointTransfo(fPointTransfoFunc)
{}


//////////////////////////////////////////////////////////////////////
// PARAMETER CALCULATION 
//////////////////////////////////////////////////////////////////////

TAngle TAllfixedParamGenerator::getV0AllfixedANGL(const TTSTN& station, const TTSTN::TROM& rom, const TANGL& angl)
{
	TPositionVector targetPos = angl.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(angl.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	//Calculated measurement value
	TAngle calcMeas = TAngle::aTan2((xTg - xSt), (yTg - ySt)) - rom.acst;  //ACST is the constant orientation of the instrument

	return (calcMeas - angl.getAngle());
}

/*
TAngle TAllfixedParamGenerator::getV0AllfixedECSP(const TTSTN& station, const TTSTN::TROM& rom, const TECSP& ecsp)
{}

TAngle TAllfixedParamGenerator::getV0AllfixedECTH(const TTSTN& station, const TTSTN::TROM& rom, const TECTH& ecth)
{}

TAngle TAllfixedParamGenerator::getV0AllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D)
{}

TAngle TAllfixedParamGenerator::getRxAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D)
{}

TAngle TAllfixedParamGenerator::getRyAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D)
{}
*/

TAngle TAllfixedParamGenerator::getCollimationAllfixedDLEV(const TLEVEL& levelInstr, const TDLEV& dlev)
{
	TReal cdz = dlev.target.distCorrectionValue; //distance correction value
	TReal dRef = levelInstr.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(); //Distance of the reference point from the plane

	TPositionVector referencePoint = levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue();
	const TLOR2LOR& refPTLor2RootTrafo = fPointTransfo.getLORTransformation(levelInstr.fMeasuredPlane->getReferencePoint()->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	refPTLor2RootTrafo.transform(referencePoint);

	TPositionVector staffPosition = dlev.targetPos->getEstimatedValue();  // this Target / Levelling Staff / SCALE assumed to be in ROOT!!!!!
	const TLOR2LOR& staffPTLor2RootTrafo = fPointTransfo.getLORTransformation(dlev.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	staffPTLor2RootTrafo.transform(staffPosition);

	// If not OLOC => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo.transformPointsToMLASystem(levelInstr.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, staffPosition);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal dTg = sqrtq(pow2q(staffPosition.getX().getMetresValue() - referencePoint.getX().getMetresValue()) + pow2q(staffPosition.getY().getMetresValue() - referencePoint.getY().getMetresValue()));

	TReal zInst = referencePoint.getZ().getMetresValue();
	TReal zTg = dlev.targetPos->getEstimatedValue().getZ().getMetresValue();

	TReal calcMeas = referencePoint.getZ().getMetresValue() - staffPosition.getZ().getMetresValue() + dRef - cdz;

	return TAngle(atan(calcMeas/ dTg), TAngle::EUnits::kRadians);

}


TLength TAllfixedParamGenerator::getHiAllfixedZEND(const TTSTN& station, const TZEND& zend)
{
	TPositionVector targetPos = zend.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(zend.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();

	TReal hTg = zend.target.targetHt;
	TReal hInst = station.instrumentHeightAdjustable->getEstimatedValue();

	TReal distance2D = dist(xSt, ySt, xTg, yTg);

	return TLength(zTg + hTg - zSt - distance2D * 1 / tan(zend.getAngle()), TLength::EUnits::kMetres);
}

TLength TAllfixedParamGenerator::getHiAllfixedDIST(const TTSTN& station, const TLINE& dist)
{
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(dist.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
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

	TReal D = pow2(dist.getDistance() + dist.target.distCorrectionValue) - pow2(xSt - xTg) - pow2(ySt - yTg);

	return TLength(zTg + hTg - zSt - sqrt(D));

}

//TLength TAllfixedParamGenerator::getHiAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D)
//{}

TLength TAllfixedParamGenerator::getCsAllfixedDSPT(const TEDM& edmST, const TDSPT& dspt)
{
	TPositionVector targetPos = dspt.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(dspt.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = edmST.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(edmST.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
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

TLength TAllfixedParamGenerator::getCsAllfixedDHOR(const TTSTN& station, const TLINE& dhor)
{
	TPositionVector targetPos = dhor.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(dhor.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
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

TLength TAllfixedParamGenerator::getCsAllfixedDIST(const TTSTN& station, const TLINE& dist)
{
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(dist.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
		fPointTransfo.transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	// Prepare coefficients (a,b,c) for the points and the transformations contributions
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();
	TReal hInst = station.instrumentHeightAdjustable->getEstimatedValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();
	TReal hTg = dist.target.targetHt;

	TReal D = dist3D(xSt, ySt, (zSt + hInst), xTg, yTg, (zTg + hTg));

	return TLength(D - dist.getDistance());
}

//TLength TAllfixedParamGenerator::getCsAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D)
//{}
