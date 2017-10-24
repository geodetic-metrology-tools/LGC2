#include "LGCAdjustablePoint.h"
#include <TObservationGenerator.h>
#include "TDist.h" 
#include "TTreeEntry.h"
#include "TXYH2CCS.h"


TObservationGenerator::TObservationGenerator(TPointTransformer& pPointTransfo) : fPointTransfo(&pPointTransfo)
{}

//////////////////////////////////////////
/// Functions for Calculation Meas
//////////////////////////////////////////
TReal TObservationGenerator::getANGLCalcMeas(const TTSTN& station, const TTSTN::TROM& rom, const LGCAdjustablePoint* targetAdjPoint){
	TPositionVector targetPos = targetAdjPoint->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo->getLORTransformation(targetAdjPoint->getFrameTreePosition(), fPointTransfo->getTree()->begin()); // Transform target to ROOT
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo->getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); // Transform station to ROOT 
	stLor2RootTrafo.transform(stationPos);

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
		fPointTransfo->transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	return (TAngle::aTan2((xTg - xSt), (yTg - ySt)) - rom.v0->getEstimatedValue() - rom.acst).getRadiansValue();
}

TReal TObservationGenerator::getANGLCalcMeasInFrame(const TTSTN& station, const TTSTN::TROM& rom, const LGCAdjustablePoint* targetAdjPoint) {
	fPointTransfo->setMLA(false); // TSTN in Frame measurements never in MLA
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();

	const TLOR2LOR& tg2stTrafo = fPointTransfo->getLORTransformation(targetAdjPoint->getFrameTreePosition(), station.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	TPositionVector targetPos = targetAdjPoint->getEstimatedValue();
	tg2stTrafo.transform(targetPos);

	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	return (TAngle::aTan2((xTg - xSt), (yTg - ySt)) - rom.v0->getEstimatedValue() - rom.acst).getRadiansValue();
}

TReal TObservationGenerator::getZENDCalcMeas(const TTSTN& station, const LGCAdjustablePoint* targetAdjPoint, TReal targetHt){
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo->getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	TPositionVector targetPos = targetAdjPoint->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo->getLORTransformation(targetAdjPoint->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	tgLor2RootTrafo.transform(targetPos);

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
		fPointTransfo->transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	TReal distance3D = dist3D(stationPos.getX().getMetresValue(), stationPos.getY().getMetresValue(), stationPos.getZ().getMetresValue() + station.instrumentHeightAdjustable->getEstimatedValue(),
		targetPos.getX().getMetresValue(), targetPos.getY().getMetresValue(), targetPos.getZ().getMetresValue() + targetHt);
	if (distance3D < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (distance3D).");

	return (TAngle::aCos(((targetPos.getZ().getMetresValue() + targetHt - stationPos.getZ().getMetresValue() - station.instrumentHeightAdjustable->getEstimatedValue()) / distance3D))).getRadiansValue();
}

TReal TObservationGenerator::getZENDCalcMeasInFrame(const TTSTN& station, const LGCAdjustablePoint* targetAdjPoint, TReal targetHt) {
	fPointTransfo->setMLA(false); // TSTN in Frame measurements never in MLA
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();

	const TLOR2LOR& tg2stTrafo = fPointTransfo->getLORTransformation(targetAdjPoint->getFrameTreePosition(), station.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	TPositionVector targetPos = targetAdjPoint->getEstimatedValue();
	tg2stTrafo.transform(targetPos);

	TReal distance3D = dist3D(stationPos.getX().getMetresValue(), stationPos.getY().getMetresValue(), stationPos.getZ().getMetresValue() + station.instrumentHeightAdjustable->getEstimatedValue(),
		targetPos.getX().getMetresValue(), targetPos.getY().getMetresValue(), targetPos.getZ().getMetresValue() + targetHt);
	if (distance3D < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (distance3D).");

	return (TAngle::aCos(((targetPos.getZ().getMetresValue() + targetHt - stationPos.getZ().getMetresValue() - station.instrumentHeightAdjustable->getEstimatedValue()) / distance3D))).getRadiansValue();
}

TReal TObservationGenerator::getDISTCalcMeas(const TTSTN& station, const LGCAdjustablePoint* targetAdjPoint, TReal targetHt, TReal distanceCorr){
	TPositionVector targetPos = targetAdjPoint->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo->getLORTransformation(targetAdjPoint->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo->getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
		fPointTransfo->transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	return (dist3D(stationPos.getX().getMetresValue(), stationPos.getY().getMetresValue(), (stationPos.getZ().getMetresValue() + station.instrumentHeightAdjustable->getEstimatedValue()),
		targetPos.getX().getMetresValue(), targetPos.getY().getMetresValue(), (targetPos.getZ().getMetresValue() + targetHt)) - distanceCorr);
}

TReal TObservationGenerator::getDISTCalcMeasInFrame(const TTSTN& station, const LGCAdjustablePoint* targetAdjPoint, TReal targetHt, TReal distanceCorr) {
	fPointTransfo->setMLA(false); // TSTN in Frame measurements never in MLA
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();

	const TLOR2LOR& tg2stTrafo = fPointTransfo->getLORTransformation(targetAdjPoint->getFrameTreePosition(), station.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	TPositionVector targetPos = targetAdjPoint->getEstimatedValue();
	tg2stTrafo.transform(targetPos);

	return (dist3D(stationPos.getX().getMetresValue(), stationPos.getY().getMetresValue(), (stationPos.getZ().getMetresValue() + station.instrumentHeightAdjustable->getEstimatedValue()),
		targetPos.getX().getMetresValue(), targetPos.getY().getMetresValue(), (targetPos.getZ().getMetresValue() + targetHt)) - distanceCorr);
}

TReal TObservationGenerator::getDHORCalcMeas(const TTSTN& station, const TLINE& dhor){
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo->getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	TPositionVector targetPos = dhor.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo->getLORTransformation(dhor.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin());  //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
		fPointTransfo->transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	TReal D = dist(stationPos.getX().getMetresValue(), stationPos.getY().getMetresValue(), targetPos.getX().getMetresValue(), targetPos.getY().getMetresValue());
	TReal cte = dhor.target.distCorrectionAdjustable->getEstimatedValue();
	return D - cte;
}

TReal TObservationGenerator::getECTHCalcMeas(const TTSTN& station, const TTSTN::TROM& rom, const TECTH& ecth)
{
	TPositionVector targetPos = ecth.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo->getLORTransformation(ecth.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo->getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	//If not OLOC used and station can not rotate freely => contributions calculated in MLA of the instrument (station)
	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
		//If stationed point is different than in the previous call of Cotrib. Gener., or if MLA system was not used in the previous call => set a new origin of the CCS2MLA transformation
		if (!(fPointTransfo->getLastStnPtName() == station.instrumentPos->getName()) || !fPointTransfo->getMLAused()){
			fPointTransfo->set2MLATransformation(stationPos);
			fPointTransfo->setLastStnPtName(station.instrumentPos->getName());
		}
		fPointTransfo->setMLA(true);
		fPointTransfo->transform2MLA(targetPos);
		stationPos = TPositionVector(0.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian); //local astronomical system of the station, station's position is the origin of this system, i.e. ( 0 , 0, 0 )  
	}
	else
		fPointTransfo->setMLA(false);
	/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();


	TAngle theta = ecth.obsHorAngle;
	TAngle Vo = rom.v0->getEstimatedValue();
	TReal a, b, c;   //station's contributions coefficients (negative values of these give target's coefficients)
	a = -cos(theta + Vo);  // xSt coefficient
	b = sin(theta + Vo);  //ySt coefficient
	c = 0.0;  //zSt coefficient

	return -1.0*(a*(xSt - xTg) + b*(ySt - yTg) - ecth.target.distCorrectionValue.getMetresValue());
}

TReal TObservationGenerator::getECDIRCalcMeas(const TTSTN& station, const TTSTN::TROM& rom, const TECDIR& ecdir)
{
	TPositionVector targetPos = ecdir.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo->getLORTransformation(ecdir.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo->getLORTransformation(station.instrumentPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	//If not OLOC used and station can not rotate freely => contributions calculated in MLA of the instrument (station)
	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station.rot3D != true){
		//If stationed point is different than in the previous call of Cotrib. Gener., or if MLA system was not used in the previous call => set a new origin of the CCS2MLA transformation
		if (!(fPointTransfo->getLastStnPtName() == station.instrumentPos->getName()) || !fPointTransfo->getMLAused()){
			fPointTransfo->set2MLATransformation(stationPos);
			fPointTransfo->setLastStnPtName(station.instrumentPos->getName());
		}
		fPointTransfo->setMLA(true);
		fPointTransfo->transform2MLA(targetPos);
		stationPos = TPositionVector(0.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian); //local astronomical system of the station, station's position is the origin of this system, i.e. ( 0 , 0, 0 )  
	}
	else
		fPointTransfo->setMLA(false);

	/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();
	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();


	TAngle theta = ecdir.obsHorAngle;
	TAngle phi = ecdir.obsVertAngle;
	TAngle Vo = rom.v0->getEstimatedValue();
	//line direction at the TSTN position
	TFreeVector l(sin(theta + Vo) * sin(phi), cos(theta + Vo) * sin(phi), cos(phi), TCoordSysFactory::ECoordSys::k3DCartesian);


	//Calcul par le produit scalaire (u^l)²+(u.l)²=|v|²|l|²
	TReal d, pScal;
	d = sqrt(pow2(xSt - xTg) + pow2(ySt - yTg) + pow2(zSt - zTg));  // distance St-Tg
	pScal = l[0] * (xSt - xTg) + l[1] * (ySt - yTg) + l[2] * (zSt - zTg);  //produit scalaire

	return sqrt(pow2(d) - pow2(pScal)) - ecdir.target.distCorrectionValue.getMetresValue();
}

TReal TObservationGenerator::getDLEVCalcMeas(const TLEVEL& levelInstr, const TDLEV& dlev){
	TPositionVector referencePoint = levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue();
	const TLOR2LOR& refPTLor2RootTrafo = fPointTransfo->getLORTransformation(levelInstr.fMeasuredPlane->getReferencePoint()->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	refPTLor2RootTrafo.transform(referencePoint);

	TPositionVector staffPosition = dlev.targetPos->getEstimatedValue();  // this Target / Levelling Staff / SCALE can be defined anywhere in the tree
	const TLOR2LOR& staffPTLor2RootTrafo = fPointTransfo->getLORTransformation(dlev.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	staffPTLor2RootTrafo.transform(staffPosition);

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo->transformPointsToMLASystem(levelInstr.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, staffPosition);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	TReal collAngl = levelInstr.instrument.collAngleAdjustable->getEstimatedValue().getRadiansValue(); //collimination angle in rads
	TReal cdz = dlev.target.distCorrectionValue; //distance of the target correction value
	TReal dRef = levelInstr.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue();

	TReal dTg = sqrtq(pow2q(staffPosition.getX().getMetresValue() - referencePoint.getX().getMetresValue()) + pow2q(staffPosition.getY().getMetresValue() - referencePoint.getY().getMetresValue()));
	TReal calcMeas = referencePoint.getZ().getMetresValue() - staffPosition.getZ().getMetresValue() + dRef - cdz - dTg*tanq(collAngl);
	return calcMeas;
}

TReal TObservationGenerator::getHorDistCalcMeas(const LGCAdjustablePoint* referencePoint, const TDLEV::TDHOR& dhor){
	TPositionVector refPointPos = referencePoint->getEstimatedValue();  // Reference point is the 'target'.
	const TLOR2LOR& refPTLor2RootTrafo = fPointTransfo->getLORTransformation(referencePoint->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	refPTLor2RootTrafo.transform(refPointPos);

	TPositionVector staffPos = dhor.targetPos->getEstimatedValue();   // Levelling staff is the 'station', can be defined anywhere in the tree.
	const TLOR2LOR& staffPTLor2RootTrafo = fPointTransfo->getLORTransformation(dhor.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	staffPTLor2RootTrafo.transform(staffPos);

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo->transformPointsToMLASystem(dhor.targetPos->getName(), staffPos, refPointPos);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	return dist(staffPos.getX().getMetresValue(), staffPos.getY().getMetresValue(), refPointPos.getX().getMetresValue(), refPointPos.getY().getMetresValue());
}

TReal TObservationGenerator::getDSPTCalcMeas(const TEDM& edmST, const TDSPT& dspt){
	TPositionVector targetPos = dspt.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo->getLORTransformation(dspt.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = edmST.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo->getLORTransformation(edmST.instrumentPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo->transformPointsToMLASystem(edmST.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	TReal cst = dspt.target.distCorrectionAdjustable->getEstimatedValue();

	TReal D = dist3D(stationPos.getX().getMetresValue(), stationPos.getY().getMetresValue(), (stationPos.getZ().getMetresValue() + edmST.instrument.instrHeight),
		targetPos.getX().getMetresValue(), targetPos.getY().getMetresValue(), (targetPos.getZ().getMetresValue() + dspt.target.targetHt));

	return D - cst;
}

TReal TObservationGenerator::getECHOCalcMeas(const TECHOROM& echoROM, const TECHO& echo){
	TPositionVector stationPoint = echo.targetPos->getEstimatedValue();
	const TLOR2LOR& stationPTLor2RootTrafo = fPointTransfo->getLORTransformation(echo.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	stationPTLor2RootTrafo.transform(stationPoint);

	/*Reference point is always defined in ROOT and is fixed, i.e. no transformation to ROOT required and no contribution required for its coordinates.*/
	TPositionVector referencePoint = echoROM.fMeasuredPlane->getReferencePoint()->getEstimatedValue();

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo->transformPointsToMLASystem(echoROM.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, stationPoint);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	TReal theta = echoROM.fMeasuredPlane->getThetaEstimatedValue().getRadiansValue();
	TReal cEcVp = echo.target.distCorrectionValue;
	TReal dRef = echoROM.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue();

	TReal calcMeas = -cosq(theta)*(stationPoint.getX() - referencePoint.getX()).getMetresValue() + sinq(theta)*(stationPoint.getY() - referencePoint.getY()).getMetresValue() + dRef - cEcVp;

	return calcMeas;
}

TReal TObservationGenerator::getECVECalcMeas(const TECVEROM& ecveROM, const TECVE& ecve)
{
	TReal cEcVp = ecve.target.distCorrectionValue; //distance of the target correction value
	TPositionVector stationPoint = ecve.targetPos->getEstimatedValue();

	const TLOR2LOR& stationPTLor2RootTrafo = fPointTransfo->getLORTransformation(ecve.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	stationPTLor2RootTrafo.transform(stationPoint);

	/*Reference point is always defined in ROOT and is fixed (it is implicitly defined),
	i.e. no transformation to ROOT required and no contribution required for its coordinates.*/
	TPositionVector linePoint = ecveROM.fMeasuredLine->getLinePoint()->getEstimatedValue();
	const TLOR2LOR& linePTLor2RootTrafo = fPointTransfo->getLORTransformation(ecveROM.fMeasuredLine->getLinePoint()->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	linePTLor2RootTrafo.transform(linePoint);

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo->transformPointsToMLASystem(ecveROM.fMeasuredLine->getName(), linePoint, stationPoint);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	TReal D = sqrt(pow2(linePoint.getX() - stationPoint.getX()) + pow2(linePoint.getY() - stationPoint.getY()));
	return (D - cEcVp);
}

TReal TObservationGenerator::getECSPCalcMeas(const TECSPROM& ecspROM, const TECSP& ecsp)
{
	// TReal cEcVp = ecsp.target.distCorrectionValue; //distance of the target correction value

	TPositionVector stationPoint = ecsp.targetPos->getEstimatedValue();
	const TLOR2LOR& stationPTLor2RootTrafo = fPointTransfo->getLORTransformation(ecsp.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	stationPTLor2RootTrafo.transform(stationPoint);

	TPositionVector linePoint1 = ecspROM.p1->getEstimatedValue();
	const TLOR2LOR& linePTLor2RootTrafo1 = fPointTransfo->getLORTransformation(ecspROM.p1->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	linePTLor2RootTrafo1.transform(linePoint1);

	TPositionVector linePoint2 = ecspROM.p2->getEstimatedValue();
	const TLOR2LOR& linePTLor2RootTrafo2 = fPointTransfo->getLORTransformation(ecspROM.p2->getFrameTreePosition(), fPointTransfo->getTree()->begin());
	linePTLor2RootTrafo2.transform(linePoint2);

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo->transformPointsToMLASystem(ecspROM.romName, linePoint1, stationPoint);
		fPointTransfo->transformPointsToMLASystem(ecspROM.romName, linePoint2, stationPoint);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);

	/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////


	//Calcul par le produit scalaire (u^l)²+(u.l)²=|v|²|l|²
	TReal dis, pScal, D;
	dis = dist3D(stationPoint.getX(), stationPoint.getY(), stationPoint.getZ(), linePoint1.getX(), linePoint1.getY(), linePoint1.getZ());  // distance P1 - stn
	D = dist3D(linePoint1.getX(), linePoint1.getY(), linePoint1.getZ(), linePoint2.getX(), linePoint2.getY(), linePoint2.getZ());// distance P1-P2
	pScal = (linePoint2.getX() - linePoint1.getX()).getMetresValue() * (stationPoint.getX() - linePoint1.getX()).getMetresValue()
		+ (linePoint2.getY() - linePoint1.getY()).getMetresValue() * (stationPoint.getY() - linePoint1.getY()).getMetresValue()
		+ (linePoint2.getZ() - linePoint1.getZ()).getMetresValue() * (stationPoint.getZ() - linePoint1.getZ()).getMetresValue();  //produit scalaire


	return sqrt(dis*dis*D*D - pow2(pScal)) / D - ecsp.target.distCorrectionValue;


}

TFreeVector TObservationGenerator::getUVECCalcMeas(const TCAM& camera, const TUVEC& uvec){
	fPointTransfo->setMLA(false);
	//Transformation from target into camera LOR
	const TLOR2LOR& tg2camTrafo = fPointTransfo->getLORTransformation(uvec.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition());
	TPositionVector targetPos = uvec.targetPos->getEstimatedValue(); // Target position
	tg2camTrafo.transform(targetPos);	//Transform from target LOR into camera LOR

	const TPositionVector& cameraPos = camera.instrumentPos->getEstimatedValue();

	TFreeVector deltaStTg((targetPos.getX() - cameraPos.getX()).getMetresValue(),
		(targetPos.getY() - cameraPos.getY()).getMetresValue(),
		targetPos.getZ().getMetresValue() - cameraPos.getZ().getMetresValue(),
		TCoordSysFactory::k3DCartesian);

	// s - Distance
	TLength sDist = deltaStTg.length();
	return TFreeVector((deltaStTg.getX() / sDist), (deltaStTg.getY() / sDist), (deltaStTg.getZ() / sDist), TCoordSysFactory::k3DCartesian);
}

UVDCalcMeas TObservationGenerator::getUVDCalcMeas(const TCAM& camera, const TUVD& uvd){
	fPointTransfo->setMLA(false);
	//Transformation from target into camera LOR
	const TLOR2LOR& tg2camTrafo = fPointTransfo->getLORTransformation(uvd.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition());
	TPositionVector targetPos = uvd.targetPos->getEstimatedValue(); // Target position
	tg2camTrafo.transform(targetPos);	//Transform from target LOR into camera LOR

	const TPositionVector& cameraPos = camera.instrumentPos->getEstimatedValue();

	TFreeVector deltaStTg((targetPos.getX() - cameraPos.getX()).getMetresValue(),
		(targetPos.getY() - cameraPos.getY()).getMetresValue(),
		targetPos.getZ().getMetresValue() - cameraPos.getZ().getMetresValue(),
		TCoordSysFactory::k3DCartesian);

	// s - Distance
	TLength sDist = deltaStTg.length();
	TFreeVector vectCalcMeas((deltaStTg.getX() / sDist), (deltaStTg.getY() / sDist), (deltaStTg.getZ() / sDist), TCoordSysFactory::k3DCartesian);
	UVDCalcMeas calMeas = { vectCalcMeas, sDist.getMetresValue() };
	return calMeas;
}

TReal TObservationGenerator::getORIECalcMeas(const TORIEROM& orieROM, const TORIE& orie){
	//Transform TARGET and STATION in a LOCAL ASTRONOMICAL FRAME
	TPositionVector targetPos = orie.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo->getLORTransformation(orie.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = orieROM.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo->getLORTransformation(orieROM.instrumentPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo->transformPointsToMLASystem(orieROM.instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo->getLA2MLA().transformInverse(stationPos);
		fPointTransfo->getLA2MLA().transformInverse(targetPos);
		fPointTransfo->setMLA(true);
	}
	else
		fPointTransfo->setMLA(false);


	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();

	//Calculated measurement value
	return(TAngle::aTan2((xTg - xSt), (yTg - ySt)) - orieROM.fConstantAngle);
}

TReal TObservationGenerator::getDVERCalcMeas(const TDVER& dver){
	fPointTransfo->setMLA(false);
	auto k3D = TCoordSysFactory::k3DCartesian;
	/*Contribution if OLOC used, otherwise going to be rewritten*/
	TFreeVector stationC(0, 0, -1, k3D);
	TFreeVector targetC(0, 0, 1, k3D);

	TPositionVector station(dver.station->getEstimatedValue());
	TPositionVector target(dver.targetPos->getEstimatedValue());
	TPositionVector stationLOR = station;
	TPositionVector targetLOR = target;

	const TLOR2LOR& stLor2RootTrafo = fPointTransfo->getLORTransformation(dver.station->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Station's position frame
	stLor2RootTrafo.transform(station); //Transform to ROOT(CCS)

	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo->getLORTransformation(dver.targetPos->getFrameTreePosition(), fPointTransfo->getTree()->begin()); //Station's position frame
	tgLor2RootTrafo.transform(target); //Transform to ROOT(CCS)


	TPositionVector stationCCS = station;
	TPositionVector targetCCS = target;

	if (fPointTransfo->getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){

		/*Needs to be calculated in CGRF.*/
		fPointTransfo->set2MLATransformation(station);
		fPointTransfo->transformMLA2CGRF(stationC); // transform to CGRF

		fPointTransfo->set2MLATransformation(target);
		fPointTransfo->transformMLA2CGRF(targetC);  // transform to CGRF

		if (fPointTransfo->getRefFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS){
			TXYH2CCS::CCS2XYHs(station);
			TXYH2CCS::CCS2XYHs(target);
		}
		else if (fPointTransfo->getRefFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine){
			TXYH2CCS::CCS2XYHg2000Machine(station);
			TXYH2CCS::CCS2XYHg2000Machine(target);
		}
		else if (fPointTransfo->getRefFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine){
			TXYH2CCS::CCS2XYHg1985Machine(station);
			TXYH2CCS::CCS2XYHg1985Machine(target);
		}
		//Calculating the distance meas
		fPointTransfo->setCGRF(true);
		return target.getH().getMetresValue() - station.getH().getMetresValue() - dver.getDistanceCorrection();

	}
	else{ /*OLOC = Calculated as XYZ, simple case*/
		fPointTransfo->setCGRF(false);
		return target.getZ().getMetresValue() - station.getZ().getMetresValue() - dver.getDistanceCorrection();

	}
}


