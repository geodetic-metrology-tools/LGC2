#include "TContributionsGenerator.h"
#include "TLGCData.h"
#include "TDist.h" 
#include<Eigen/Dense>
#include "refframetransformations\TXYH2CCS.h"
#include "refframetransformations\GeodeticConstants.h"
#include "refframetransformations\Cartesian2Ellipsoidal.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////
TContributionsGenerator::TContributionsGenerator(TDataTree* tree, TLGCRefFrame::ERefs refFrame): fTree(tree), fRefFrame(refFrame),
fMLAused(false),fCGRFused(false)
{
	fLastStationPtName = "NULL";

	if(refFrame == TLGCRefFrame::ERefs::kSPHE){
		fGeoidModel= TRefSystemFactory::EGeoid::kCGSphere; 
		fccs2cgrf.reInitialize(false); //Implicitly the ccs2cgrf transformation uses ellipsoidal CGRF, if SPHE is used, a SPHE CGRF (cgrfs) shoud be used
	}
	else if(refFrame == TLGCRefFrame::ERefs::kRS2K)
		fGeoidModel= TRefSystemFactory::EGeoid::kCG2000Machine;
	else if(refFrame == TLGCRefFrame::ERefs::kLEP)
		fGeoidModel= TRefSystemFactory::EGeoid::kCG1985Machine;	
	else
		fGeoidModel = TRefSystemFactory::EGeoid::kNoGeoid;
}

//////////////////////////////
//Tree update
//////////////////////////////
//Update transformations between nodes, need to be called, whenever tree changed, typically after every LS calculation run (before running input matrices filler)
void TContributionsGenerator::updateTransformations(){
	for(std::list<TLOR2LOR>::iterator it = fLORTrafo.begin(); it != fLORTrafo.end(); ++it){
		it->updateTree();
	}
	fLastStationPtName = "NULL";
	fMLAused = false;
}
//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- TSTN measurements
//////////////////////////////////////////////////////////////////////
// Returns the calculated observation value and design matrix contributions for the given Spatial Distance observation with the current parameters 
DistMeasContrib	TContributionsGenerator::getSpatialDistanceContrib(const TTSTN& station, const TLINE& dist){
	/* CONTRIBUTION IS CALCULATED EITHER IN ROOT OR IN MLA OF THE STATION(INSTRUMENT)*/
	/*Contribnutions to be returned*/
	TReal calcMeas, hiContrib, distCorrection, variance;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
///////////////////Transform TARGET and STATION from their's LOR either to ROOT or to MLA of the station///////////////////////////////
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dist.targetPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	//If not OLOC used and station can not rotate freely => contributions calculated in MLA of the instrument (station)
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		//If stationed point is different than in the previous call of Cotrib. Gener., or if MLA system was not used in the previous call => set a new origin of the CCS2MLA transformation
		if (!(fLastStationPtName == station.instrumentPos->getName()) || !fMLAused){
			set2MLATransformation(stationPos);
			fLastStationPtName = station.instrumentPos->getName();
		}
		fMLAused = true;
		transform2MLA(targetPos);
		stationPos = TPositionVector(0.0, 0.0, 0.0,TCoordSysFactory::k3DCartesian); //local astronomical system of the station, station's position is the origin of this system, i.e. ( 0 , 0, 0 )  
	}
	else
		fMLAused = false;
/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////
	TReal xSt = stationPos.getX().getValue();
	TReal ySt = stationPos.getY().getValue();
	TReal zSt = stationPos.getZ().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg = targetPos.getY().getValue();
	TReal zTg = targetPos.getZ().getValue();

	TReal hTg = dist.target.targetHt;
	TReal hInst = station.instrumentHeightAdjustable->getEstimatedValue().getValue();
	TReal cst = dist.target.distCorrectionAdjustable->getEstimatedValue().getValue();

	TReal D = dist3D(xSt, ySt, (zSt + hInst), xTg, yTg, (zTg + hTg));

	if (D < nullLimit)
		throw std::logic_error("TContributionGenerator::getSpatialDistanceContrib: Division by zero because observation points have identical coordinates.");

	TReal a,b,c;   //station's contributions coefficients (negative values of these give target's coefficients)		 
	a = (xSt -xTg)/D;  // xSt coefficient
	b = (ySt - yTg)/D;  //ySt coefficient
	c = (zSt + hInst - zTg - hTg)/D;  //zSt coefficient

	calcMeas = D - cst;
	hiContrib = c; //instrument height contribution

	//Station can be defined anywhere, get point contributions and transformations contributions
	coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	addTransformationsContributions(stLor2RootTrafo, station.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, dist.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	distCorrection =  - 1.0;

	// Variance calculation
	TReal varM = pow2q(dist.target.sigmaDist) + pow2q(calcMeas) * pow2q(dist.target.ppmDist);
	TReal varInstHeight = pow2q(station.instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(dist.target.sigmaTargetHt);
	TReal varInstCent = pow2q(station.instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(dist.target.sigmaTargetCentering);
	variance = varM + pow2q((zTg - zSt + hTg - hInst)/D) * (varInstHeight +  varTgHeight) + ((pow2q(yTg - ySt) + pow2q(xSt - xTg))/pow2q(D)) * (varInstCent + varTgCent);

	DistMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, distCorrection, variance};
	return contrib;
}

// Returns the calculated observation value and design matrix contributions for the given Horizontal angle (ANGL) observation with the current parameters 
AnglMeasContrib	TContributionsGenerator::getHorAnglContrib(const TTSTN& station, const TTSTN::TROM& rom, const TANGL& angl){
	LGC::TAngle calcMeas; //Calculated measurement value
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);	
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	TReal hiContrib, v0Contrib;

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = angl.targetPos->getEstimatedValue();

	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(angl.targetPos->getFrameTreePosition(), station.instrumentPos->getFrameTreePosition()); //Station's LOR should be "ROOT"
	tgLor2RootTrafo.transform(targetPos); 

	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	//If not OLOC used and station can not rotate freely => contributions calculated in MLA of the instrument (station)
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		//If station position is different than in previous call of CG, or if MLA system was not used in previous call => set new origin of the CCS2MLA transformation
		if (!(fLastStationPtName == station.instrumentPos->getName()) || !fMLAused){
			set2MLATransformation(stationPos);
			fLastStationPtName = station.instrumentPos->getName();
		}
		fMLAused = true;
		transform2MLA(targetPos); //Transform target to local astronomical system of the station
		stationPos = TPositionVector(0.0, 0.0, 0.0,TCoordSysFactory::k3DCartesian); 	//local astronomical system of the station, station position is the origin of this system
	}
	else
		fMLAused = false;

	TReal xSt = stationPos.getX().getValue();
	TReal ySt = stationPos.getY().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg = targetPos.getY().getValue();

	TReal dist2 = pow2q(dist(xSt, ySt, xTg, yTg));
	
	calcMeas = LGC::TAngle::atan2((xTg - xSt),(yTg - ySt)) + rom.v0->getEstimatedValue() + rom.acst;  //ACST is constant orientation of the instrument

	if (dist2 < nullLimit)
		throw std::logic_error("TContributionGenerator::getHorAnglContrib: Division by zero because observation points have identical coordinates.");

	TReal a,b,c; //station's contributions coefficients (negative values of these give target's coefficients)		
	a = (-LITERAL(1.0) * (yTg - ySt))/dist2;//xSt coefficient
	b = (xTg - xSt)/dist2;//ySt coefficient
	c = 0.0;

	v0Contrib = 1.0; //contribution for the V0 parameter
	hiContrib = 0.0; // no contribution for the instrument height

	//Station can be defined anywhere, get point contributions and transformations contributions
	coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	addTransformationsContributions(stLor2RootTrafo, station.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, angl.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	// Variance calculation
	TReal variance = pow2q(angl.target.sigmaAngl) + (1.0/pow2q(dist2)) * (pow2q(station.instrument.sigmaInstrCentering) + pow2q(angl.target.sigmaTargetCentering));

	AnglMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

//Zenith distance (verical angle) contributions
AnglMeasContrib	TContributionsGenerator::getZenDistContrib(const TTSTN& station, const TZEND& zend){
	LGC::TAngle calcMeas;
	TReal hiContrib, v0Contrib;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = zend.targetPos->getEstimatedValue();

	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(zend.targetPos->getFrameTreePosition(), station.instrumentPos->getFrameTreePosition()); //Station's LOR should be "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	//If not OLOC used and station can not rotate freely => contributions calculated in MLA of the instrument (station)
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		//If station position is different than in previous call of CG, or if MLA system was not used in previous call => set new origin of the CCS2MLA transformation
		if (!(fLastStationPtName == station.instrumentPos->getName()) || !fMLAused){
			set2MLATransformation(stationPos);
			fLastStationPtName = station.instrumentPos->getName();
		}
		fMLAused = true;
		transform2MLA(targetPos);
		stationPos = TPositionVector(0.0,0.0,0.0,TCoordSysFactory::k3DCartesian); 	//local astronomical system of the station, station's position is the origin of this system
	}
	else
		fMLAused = false;

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = stationPos.getX().getValue();
	TReal ySt = stationPos.getY().getValue();
	TReal zSt = stationPos.getZ().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg = targetPos.getY().getValue();
	TReal zTg = targetPos.getZ().getValue();

	TReal hTg = zend.target.targetHt;
	TReal hInst = station.instrumentHeightAdjustable->getEstimatedValue().getValue();

	TReal dx,dy,dz, sinPhi;
	dx = xTg-xSt;
	dy = yTg-ySt;
	dz = zTg-zSt-hInst+hTg;

	TReal distance3D = dist3D(xSt, ySt, zSt+hInst, xTg, yTg, zTg+hTg);
	if (distance3D < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (distance3D).");

	calcMeas = LGC::TAngle::acos((zTg - zSt - hInst + hTg)/distance3D);
	
	//We are taking the currently calculated value noty the measured one!!!
	sinPhi = sinq(calcMeas.rad());
//	sinPhi = sinq(zend.getAngle().rad());

	if (sinPhi < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (sinV).");

	TReal a,b,c; //station's contributions coefficients (negative values of these give target's coefficients)	
	a = (- 1.0 * dz * dx) / (powq(distance3D,3) * sinPhi);//xSt coefficient
	b = (- 1.0 * dz * dy) / (powq(distance3D,3) * sinPhi);//ySt coefficient
	c = (1.0 / (distance3D * sinPhi)) - powq(dz,2)/(powq(distance3D,3) * sinPhi);//zSt coefficient

	hiContrib = c; // instrument height contribution
	v0Contrib = 0.0; // no contribution for the v0 parameter

	//Station can be defined anywhere, get point contributions and transformations contributions
	coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	addTransformationsContributions(stLor2RootTrafo, station.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, zend.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	TReal variance = pow2q(zend.target.sigmaAngl) + (((pow2q(dx) + pow2q(dy))*pow2q(dz))/(powq(distance3D,6)*pow2q(sinPhi))) * 
					(pow2q(station.instrument.sigmaInstrCentering) + pow2q(zend.target.sigmaTargetCentering)) +
					 pow2q(-c) * (pow2q(station.instrument.sigmaInstrHeight) + pow2q(zend.target.sigmaTargetHt));

	AnglMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

//Horizontal distance contribution for a measurement made in DLEV
HorDistContribLEVEL	TContributionsGenerator::getHorDistContrib(const TAdjustablePoint* referencePoint, const TDHOR& dhor){
	TFreeVector staffContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector referencePTContrib(TCoordSysFactory::k3DCartesian);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> staffTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> referencePTTransfContributions;
	TReal calcMeas;

	TPositionVector refPointPos = referencePoint->getEstimatedValue();  // Reference point is a 'target'
	const TLOR2LOR& refPTLor2RootTrafo = getLORTransformation(referencePoint->getFrameTreePosition(), fTree->begin()); 
	refPTLor2RootTrafo.transform(refPointPos);

	TPositionVector staffPos = dhor.targetPos->getEstimatedValue();   // Levelling staff is a 'station'
	const TLOR2LOR& staffPTLor2RootTrafo = getLORTransformation( dhor.targetPos->getFrameTreePosition(), fTree->begin()); 
	refPTLor2RootTrafo.transform(staffPos);

	//If not OLOC used => transform into local astronomical system of the Levelling Staff (this is the station which makes measurements)
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		//If station position is different than in previous call of CG, or if MLA system was not used in previous call => set new origin of the CCS2MLA transformation
		if (!(fLastStationPtName == dhor.targetPos->getName()) || !fMLAused){
			set2MLATransformation(staffPos);
			fLastStationPtName =  dhor.targetPos->getName();
		}
		fMLAused = true;
		transform2MLA(refPointPos);
		staffPos = TPositionVector(0,0,0,TCoordSysFactory::k3DCartesian); 	//local astronomical system of the station, station's position is the origin of this system
	}
	else
		fMLAused = false;

	TReal xSt = staffPos.getX().getValue();
	TReal ySt = staffPos.getY().getValue();

	TReal xTg = refPointPos.getX().getValue();
	TReal yTg = refPointPos.getY().getValue();

	calcMeas = dist(xSt, ySt, xTg, yTg);

	if (calcMeas < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getHorDistContrib: Division by zero because observation points have identical coordinates.");

	TReal a,b,c; //reference point's contributions coefficients (negative values of these give target's coefficients)		
	a = - (xTg - xSt)/calcMeas; // xRp coefficient
	b = - (yTg - ySt)/calcMeas; //yRp coefficient
	c = 0.0; //zRp coefficient

	//Station can be defined anywhere, get point contributions and transformations contributions
	staffContrib = getPointContributions(staffPTLor2RootTrafo, a, b, c);
	addTransformationsContributions(staffPTLor2RootTrafo,  dhor.targetPos->getEstimatedValue(), a, b, c, staffTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	referencePTContrib = getPointContributions(refPTLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(refPTLor2RootTrafo, referencePoint->getEstimatedValue(), -a, -b, -c, referencePTTransfContributions);
	
	HorDistContribLEVEL  contrib = {calcMeas, staffContrib, referencePTContrib, staffTransfContributions, referencePTTransfContributions};

	return contrib;
}

//Horizontal distance contributions, measurement made by TSTN
HorDistContrib	TContributionsGenerator::getHorDistContrib(const TTSTN& station, const TLINE& dhor){
	TReal calcMeas;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = dhor.targetPos->getEstimatedValue();

	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dhor.targetPos->getFrameTreePosition(), station.instrumentPos->getFrameTreePosition()); //Station's LOR should be "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		if (!(fLastStationPtName == station.instrumentPos->getName()) || !fMLAused){
			set2MLATransformation(stationPos);
			fLastStationPtName = station.instrumentPos->getName();
		}
		fMLAused = true;
		transform2MLA(targetPos);
		stationPos = TPositionVector(0,0,0,TCoordSysFactory::k3DCartesian); 	//local astronomical system of the station, station's position is the origin of this system
	}
	else
		fMLAused = false;

	TReal xSt = stationPos.getX().getValue();
	TReal ySt = stationPos.getY().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg = targetPos.getY().getValue();

	TReal cte = dhor.target.distCorrectionAdjustable->getEstimatedValue().getValue();

	TReal D = dist(xSt, ySt, xTg, yTg);

	if (D < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getHorDistDsgnMxContributions: Division by zero because observation points have identical coordinates.");

	TReal a,b,c;  //station's contributions coefficients (negative values of these give target's coefficients)			
	a = - (xTg - xSt)/D;  // xSt coefficient
	b = - (yTg - ySt)/D;  //ySt coefficient
	c = 0.0; //zSt coefficient

	//Station can be defined anywhere, get point contributions and transformations contributions
	coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	addTransformationsContributions(stLor2RootTrafo, station.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, dhor.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	calcMeas = D - cte;
	TReal distCorrCont = - 1.0;

	// Variance
	TReal varM = pow2q(dhor.target.sigmaDist) + pow2q(calcMeas) * pow2q(dhor.target.ppmDist);
	TReal variance = varM + (pow2q(station.instrument.sigmaInstrCentering) + pow2q(dhor.target.sigmaTargetCentering));

	HorDistContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, distCorrCont, variance};
	return contrib;
}


//DLEV contributions
DLEVContrib	TContributionsGenerator::getDLEVContrib(const TLEVEL& levelInstr, const TDLEV& dlev){
	TFreeVector staffContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector referencePTContrib(TCoordSysFactory::k3DCartesian);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> staffTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> referencePTTransfContributions;

	TReal collAngl = levelInstr.instrument.collAngleAdjustable->getEstimatedValue().rad(); //collimination angle in rads
	TReal cdz = dlev.target.distCorrectionValue; //distance of the target correction value
	TReal dRef = levelInstr.fMeasuredPlane->getRefPtDistEstimatedValue().getValue(); 

	TPositionVector referencePoint = levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue();
	const TLOR2LOR& refPTLor2RootTrafo = getLORTransformation(levelInstr.fMeasuredPlane->getReferencePoint()->getFrameTreePosition(), fTree->begin()); 
	refPTLor2RootTrafo.transform(referencePoint);

	TPositionVector staffPosition = dlev.targetPos->getEstimatedValue();  // this Target / Levelling Staff / SCALE assumed to be in ROOT!!!!!
	const TLOR2LOR& staffPTLor2RootTrafo = getLORTransformation(dlev.targetPos->getFrameTreePosition(), fTree->begin()); 
	refPTLor2RootTrafo.transform(staffPosition);

	const std::string& rpName = levelInstr.fMeasuredPlane->getReferencePoint()->getName();
	//If not OLOC used => set up a MLA transformation
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		if (!(fLastStationPtName == rpName) || !fMLAused){
			set2MLATransformation(referencePoint);
			fLastStationPtName = rpName;
		}
		fMLAused = true;
		transform2MLA(staffPosition);
		referencePoint = TPositionVector(0.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian); 	//MLA of the reference point, its origin
	}
	else
		fMLAused = false;


	TReal dTg = sqrtq(pow2q(staffPosition.getX().getValue() - referencePoint.getX().getValue()) + pow2q(staffPosition.getY().getValue() - referencePoint.getY().getValue())); 

	/*TReal calcMeas = staffPosition.getZ().getValue() - referencePoint.getZ().getValue() - dRef - cdz - dTg*tanq(collAngl);*/

	TReal calcMeas = referencePoint.getZ().getValue() - staffPosition.getZ().getValue() + dRef - cdz - dTg*tanq(collAngl);


	//Station can be defined anywhere, get point contributions and transformations contributions
	staffContrib = getPointContributions(staffPTLor2RootTrafo, 0, 0, -1);
	addTransformationsContributions(staffPTLor2RootTrafo, dlev.targetPos->getEstimatedValue(), 0, 0, -1, staffTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	referencePTContrib = getPointContributions(refPTLor2RootTrafo, 0, 0, 1);
	addTransformationsContributions(refPTLor2RootTrafo, levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue(), 0, 0, 1, referencePTTransfContributions);

	TReal collAngleContrib = - dTg*(1.0 + powq(tanq(collAngl),2));
	TReal fRefPtDistCont = 1.0;

	TReal variance = pow2q(dlev.target.sigmaD) + pow2q(dTg) * pow2q(dlev.target.ppmD) +  pow2q(dlev.target.sigmaStaffHt);

	DLEVContrib dlevContrib = {calcMeas, staffContrib, referencePTContrib, staffTransfContributions, referencePTTransfContributions, fRefPtDistCont, collAngleContrib, variance};
	return dlevContrib;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////3D/2D measurements///////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------ PLR3D and DIR3D Contrib for TSTN------------------------------------------------------
PLR3DContrib	TContributionsGenerator::getPolar3DContrib(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D){

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = plr3D.targetPos->getEstimatedValue();

	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(plr3D.targetPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	TReal sDist = plr3D.getScalar();
	TReal cs = plr3D.target.distCorrectionAdjustable->getEstimatedValue().getValue();
	TReal Rx = 0.0; //Rotation around x-axis, default is no rotation
	TReal Ry = 0.0; //Rotation around y-axis, default is no rotation

	if(station.rot3D){ //If station can rotate freely load the rotations
		if(station.rotX == nullptr || station.rotY == nullptr)
			throw std::runtime_error("TContributionGenerator::getPolar3DContrib station can rotate freely, but rotation angles are NULL.");
		Rx = station.rotX->getEstimatedValue().rad();
		Ry = station.rotY->getEstimatedValue().rad();
	}

	TReal sinTheta = plr3D.getAngle(kANGL).sin();
	TReal cosTheta = plr3D.getAngle(kANGL).cos();

	TReal sinPhi = plr3D.getAngle(kZEND).sin();
	TReal cosPhi = plr3D.getAngle(kZEND).cos();

	TReal sinV0 = (rom.v0->getEstimatedValue() - rom.acst).sin();
	TReal cosV0 = (rom.v0->getEstimatedValue() - rom.acst).cos();

	TReal sinRx = sinq(Rx);
	TReal cosRx = cosq(Rx);

	TReal sinRy = sinq(Ry);
	TReal cosRy = cosq(Ry);

	TReal dX = targetPos.getX().getValue() - stationPos.getX().getValue();
	TReal dY = targetPos.getY().getValue() - stationPos.getY().getValue();
	TReal dZ = targetPos.getZ().getValue() + plr3D.target.targetHt - stationPos.getZ().getValue() -  station.instrumentHeightAdjustable->getEstimatedValue().getValue();

	TFreeVector line1AMat( cosV0*cosRy, -sinV0*cosRx+sinRx*cosV0*sinRy, sinV0*sinRx+cosRx*cosV0*sinRy, TCoordSysFactory::k3DCartesian); //first line of the A-matrix
	TFreeVector line2AMat( sinV0*cosRy, cosV0*cosRx+sinRx*sinV0*sinRy, -cosV0*sinRx+cosRx*sinV0*sinRy, TCoordSysFactory::k3DCartesian); //second line of the A-matrix
	TFreeVector line3AMat( -sinRy, sinRx*cosRy, cosRx*cosRy, TCoordSysFactory::k3DCartesian); //third line of the A-matrix

	//If not OLOC used and station is fixed (can not rotate freely) => contributions calculated in MLA
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D == false){
		if (!(fLastStationPtName == station.instrumentPos->getName()) || !fMLAused){
			set2MLATransformation(station.instrumentPos->getEstimatedValue());
			fLastStationPtName = station.instrumentPos->getName();
		}
		fMLAused = true;
		transform2MLA(targetPos); //Transform target coordinates into LA of the station
		stationPos = TPositionVector(0.0, 0.0, 0.0,TCoordSysFactory::k3DCartesian); 	//In local astronomical system of the station, station's position is the origin of this system
	}
	else
		fMLAused = false;

	// TSTN station contribution is calculated in a LOR system of the station, i.e. from LOR's it is an identity transformation
	TFreeVector partDerWRespToX0St = stLor2RootTrafo.partDerivWRespToX0();
	TFreeVector partDerWRespToY0St = stLor2RootTrafo.partDerivWRespToY0();
	TFreeVector partDerWRespToZ0St = stLor2RootTrafo.partDerivWRespToZ0();

	if(fMLAused){
		transform2MLA(partDerWRespToX0St);
		transform2MLA(partDerWRespToY0St);
		transform2MLA(partDerWRespToZ0St);
	}

	Point3DContrib coordContribStation = {
		TFreeVector( line1AMat.dot(partDerWRespToX0St), line1AMat.dot(partDerWRespToY0St), line1AMat.dot(partDerWRespToZ0St), TCoordSysFactory::k3DCartesian),//St contribution for a first equation
		TFreeVector( line2AMat.dot(partDerWRespToX0St), line2AMat.dot(partDerWRespToY0St), line2AMat.dot(partDerWRespToZ0St), TCoordSysFactory::k3DCartesian),//St contribution for a first equation
		TFreeVector( line3AMat.dot(partDerWRespToX0St), line3AMat.dot(partDerWRespToY0St), line3AMat.dot(partDerWRespToZ0St), TCoordSysFactory::k3DCartesian),//St contribution for a first equation
	};

	TFreeVector partDerWRespToX0Tg = tgLor2RootTrafo.partDerivWRespToX0();
	TFreeVector partDerWRespToY0Tg = tgLor2RootTrafo.partDerivWRespToY0();
	TFreeVector partDerWRespToZ0Tg = tgLor2RootTrafo.partDerivWRespToZ0();

	if(fMLAused){
		transform2MLA(partDerWRespToX0Tg);
		transform2MLA(partDerWRespToY0Tg);
		transform2MLA(partDerWRespToZ0Tg);
	}

	Point3DContrib coordContribTarget =  {
		TFreeVector(-line1AMat.dot(partDerWRespToX0Tg),-line1AMat.dot(partDerWRespToY0Tg),-line1AMat.dot(partDerWRespToZ0Tg), TCoordSysFactory::k3DCartesian), //Tg contribution for a first equation
		TFreeVector(-line2AMat.dot(partDerWRespToX0Tg),-line2AMat.dot(partDerWRespToY0Tg),-line2AMat.dot(partDerWRespToZ0Tg), TCoordSysFactory::k3DCartesian), //Tg contribution for a second equation
		TFreeVector(-line3AMat.dot(partDerWRespToX0Tg),-line3AMat.dot(partDerWRespToY0Tg),-line3AMat.dot(partDerWRespToZ0Tg), TCoordSysFactory::k3DCartesian), //Tg contribution for a third equation
	};
/////////////////////////////////////////////////////////////////////////////////
/////This part fills stations transformations contributions
/////////////////////////////////////////////////////////////////////////////////
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> stationTransfContributions; // Vector for target transformations contributions

	const TPositionVector& stPointInLOR = station.instrumentPos->getEstimatedValue();
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChainSt = stLor2RootTrafo.getTransformationChain();

	TFreeVector omegaDerivativeSt(TCoordSysFactory::k3DCartesian); 
	TFreeVector phiDerivativeSt(TCoordSysFactory::k3DCartesian); 
	TFreeVector kappaDerivativeSt(TCoordSysFactory::k3DCartesian); 
	TFreeVector t1DerivativeSt(TCoordSysFactory::k3DCartesian); 
	TFreeVector t2DerivativeSt(TCoordSysFactory::k3DCartesian); 
	TFreeVector t3DerivativeSt(TCoordSysFactory::k3DCartesian); 
	TFreeVector scaleDerivSt(TCoordSysFactory::k3DCartesian); 
	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'targetTransfContributions'
	for(auto it(trafoChainSt.begin()); it != trafoChainSt.end(); ++it){
		std::string transformationName = it->adjTrafo->getName();

		//Contributions for rotations : Omega, Phi and Kappa
		omegaDerivativeSt = stLor2RootTrafo.partialDerivativesAngle(transformationName, stPointInLOR, 0);
		phiDerivativeSt = stLor2RootTrafo.partialDerivativesAngle(transformationName, stPointInLOR, 1);
		kappaDerivativeSt = stLor2RootTrafo.partialDerivativesAngle(transformationName, stPointInLOR, 2);

		//Contributions for translation: X, Y and Z coordinate
		t1DerivativeSt = stLor2RootTrafo.partialDerivativesTranslation(transformationName, stPointInLOR, 0);
		t2DerivativeSt = stLor2RootTrafo.partialDerivativesTranslation(transformationName, stPointInLOR, 1); 
		t3DerivativeSt = stLor2RootTrafo.partialDerivativesTranslation(transformationName, stPointInLOR, 2);

		scaleDerivSt = stLor2RootTrafo.partialDerivativesScale(transformationName, stPointInLOR);

		if(fMLAused){ //If MLA used, then transform contributions
			transform2MLA(omegaDerivativeSt);
			transform2MLA(phiDerivativeSt);
			transform2MLA(kappaDerivativeSt);

			transform2MLA(t1DerivativeSt);
			transform2MLA(t2DerivativeSt);
			transform2MLA(t3DerivativeSt);

			transform2MLA(scaleDerivSt);
		}

		TransformationContrib firstEqContribSt = {	
		TFreeVector(line1AMat.dot(omegaDerivativeSt),line1AMat.dot(phiDerivativeSt),line1AMat.dot(kappaDerivativeSt), TCoordSysFactory::k3DCartesian), //Tg contribution for a first equation
		TFreeVector(line1AMat.dot(t1DerivativeSt),line1AMat.dot(t2DerivativeSt),line1AMat.dot(t3DerivativeSt), TCoordSysFactory::k3DCartesian), //Tg contribution for a first equation
		line1AMat.dot(scaleDerivSt)};


		TransformationContrib secondEqContribSt = {	
		TFreeVector(line2AMat.dot(omegaDerivativeSt),line2AMat.dot(phiDerivativeSt),line2AMat.dot(kappaDerivativeSt), TCoordSysFactory::k3DCartesian), //Tg contribution for a second equation
		TFreeVector(line2AMat.dot(t1DerivativeSt),line2AMat.dot(t2DerivativeSt),line2AMat.dot(t3DerivativeSt), TCoordSysFactory::k3DCartesian), //Tg contribution for a second equation
		line2AMat.dot(scaleDerivSt)};


		TransformationContrib thirdEqContribSt = {	
		TFreeVector(line3AMat.dot(omegaDerivativeSt),line3AMat.dot(phiDerivativeSt),line3AMat.dot(kappaDerivativeSt), TCoordSysFactory::k3DCartesian), //Tg contribution for a third equation
		TFreeVector(line3AMat.dot(t1DerivativeSt),line3AMat.dot(t2DerivativeSt),line3AMat.dot(t3DerivativeSt), TCoordSysFactory::k3DCartesian), //Tg contribution for a third equation
		line3AMat.dot(scaleDerivSt)};

		TransformationContrib3D stContrib = {firstEqContribSt, secondEqContribSt, thirdEqContribSt};
		stationTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, stContrib));
	}
/////////////////////////////////////////////////////////////////////////////////
/////This part fills target transformations contributions
/////////////////////////////////////////////////////////////////////////////////
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector for target transformations contributions

	const TPositionVector& tgPointInLOR = plr3D.targetPos->getEstimatedValue();
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = tgLor2RootTrafo.getTransformationChain();

	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'targetTransfContributions'
	for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
		std::string transformationName = it->adjTrafo->getName();

		//Contributions for rotations : Omega, Phi and Kappa
		TFreeVector omegaDerivative = tgLor2RootTrafo.partialDerivativesAngle(transformationName, tgPointInLOR, 0);
		TFreeVector phiDerivative = tgLor2RootTrafo.partialDerivativesAngle(transformationName, tgPointInLOR, 1);
		TFreeVector kappaDerivative = tgLor2RootTrafo.partialDerivativesAngle(transformationName, tgPointInLOR, 2);

		//Contributions for translation: X, Y and Z coordinate
		TFreeVector t1Derivative = tgLor2RootTrafo.partialDerivativesTranslation(transformationName, tgPointInLOR, 0);
		TFreeVector t2Derivative = tgLor2RootTrafo.partialDerivativesTranslation(transformationName, tgPointInLOR, 1); 
		TFreeVector t3Derivative = tgLor2RootTrafo.partialDerivativesTranslation(transformationName, tgPointInLOR, 2);

		TFreeVector scaleDeriv = tgLor2RootTrafo.partialDerivativesScale(transformationName, tgPointInLOR);

		if(fMLAused){ //If MLA used, then transform contributions
			transform2MLA(omegaDerivative);
			transform2MLA(phiDerivative);
			transform2MLA(kappaDerivative);

			transform2MLA(t1Derivative);
			transform2MLA(t2Derivative);
			transform2MLA(t3Derivative);

			transform2MLA(scaleDeriv);
		}

		TransformationContrib firstEqContrib = {	
		TFreeVector(-line1AMat.dot(omegaDerivative),-line1AMat.dot(phiDerivative),-line1AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a first equation
		TFreeVector(-line1AMat.dot(t1Derivative),-line1AMat.dot(t2Derivative),-line1AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a first equation
		-line1AMat.dot(scaleDeriv)};


		TransformationContrib secondEqContrib = {	
		TFreeVector(-line2AMat.dot(omegaDerivative),-line2AMat.dot(phiDerivative),-line2AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a second equation
		TFreeVector(-line2AMat.dot(t1Derivative),-line2AMat.dot(t2Derivative),-line2AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a second equation
		-line2AMat.dot(scaleDeriv)};


		TransformationContrib thirdEqContrib = {	
		TFreeVector(-line3AMat.dot(omegaDerivative),-line3AMat.dot(phiDerivative),-line3AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a third equation
		TFreeVector(-line3AMat.dot(t1Derivative),-line3AMat.dot(t2Derivative),-line3AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a third equation
		-line3AMat.dot(scaleDeriv)};

		TransformationContrib3D trContrib = {firstEqContrib, secondEqContrib, thirdEqContrib};
		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, trContrib));
	}


	//Contribution
	PLR3DContrib contrib = {coordContribStation, coordContribTarget, stationTransfContributions,  targetTransfContributions, 
	{line1AMat.getZ().getValue(), line2AMat.getZ().getValue(), line3AMat.getZ().getValue()},//Instrument height contribution
	//V0 contribution for a first, second and third equation
	{-(-sinV0*cosRy*dX + (-cosV0*cosRx-sinV0*sinRx*sinRy)*dY + (cosV0*sinRx-sinV0*cosRx*sinRy)*dZ),
	 -(cosV0*cosRy*dX + (-sinV0*cosRx+cosV0*sinRx*sinRy)*dY + (sinV0*sinRx+cosV0*cosRx*sinRy)*dZ), 
	  0},
	//Theta contribution
	{(sDist+cs)*cosTheta*sinPhi,
	 -(sDist+cs)*sinTheta*sinPhi, 
	 0},
	//Phi contribution
	{(sDist+cs)*sinTheta*cosPhi,
	 (sDist+cs)*cosTheta*cosPhi, 
	 -(sDist+cs)*sinPhi},  
	//Distance (s) and distance correction (cs) contribution
	{sinTheta*sinPhi,
	 cosTheta*sinPhi,
	 cosPhi},
	{0.0, 0.0, 0.0}, //default Rx contribution (to be overwritten if rotation of the station is enabled)
	{0.0, 0.0, 0.0}, //default Ry contribution (to be overwritten if rotation of the station is enabled)
	}; 

	if(station.rot3D){
		contrib.fRxContrib[0] = -((sinRx*sinV0 + cosRx*cosV0*sinRy)*dY + (cosRx*sinV0 - cosV0*sinRy*sinRx)*dZ);
		contrib.fRxContrib[1] = -((-sinRx*cosV0 + cosRx*sinV0*sinRy)*dY + (-cosRx*cosV0 - sinV0*sinRy*sinRx)*dZ);
		contrib.fRxContrib[2] = -(cosRx*cosRy*dY - cosRy*sinRx*dZ);

		contrib.fRyContrib[0] = -(-cosV0*sinRy*dX + sinRx*cosV0*cosRy*dY + cosV0*cosRy*cosRx*dZ);
		contrib.fRyContrib[1] = -(-sinV0*sinRy*dX + sinRx*sinV0*cosRy*dY + sinV0*cosRy*cosRx*dZ);
		contrib.fRyContrib[2] = -(-cosRy*dX - sinRx*sinRy*dY - cosRx*sinRy*dZ);
	}

	TFreeVector deltaStTg(dX, dY, dZ,TCoordSysFactory::k3DCartesian);
	//Misclosure vector
	contrib.fMisclosureVector[0] = (sDist+cs)*(sinTheta*sinPhi) - line1AMat.dot(deltaStTg);
	contrib.fMisclosureVector[1] = (sDist+cs)*(cosTheta*sinPhi) - line2AMat.dot(deltaStTg);
	contrib.fMisclosureVector[2] = (sDist+cs)*(cosPhi) - line3AMat.dot(deltaStTg);

	//Variance calcualtion
	TReal varianceInstrCenter = pow2q(station.instrument.sigmaInstrCentering);
	TReal varianceTgCenter = pow2q(plr3D.target.sigmaTargetCentering);
	// ANGL
	TReal varianceANGL = pow2q(plr3D.target.sigmaAngl);
	// ZEND
	TReal varianceZEND = pow2q(plr3D.target.sigmaZenD);
	// DIST
	TReal varianceDIST = pow2q(plr3D.target.sigmaDist);

	contrib.fObsVariance[0] = 0.5*(varianceInstrCenter + varianceTgCenter) + pow2q(contrib.fThetaContrib[0]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[0]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[0]) * varianceDIST;

	contrib.fObsVariance[1] = 0.5*((varianceInstrCenter + varianceTgCenter)) + pow2q(contrib.fThetaContrib[1]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[1]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[1]) * varianceDIST;

	contrib.fObsVariance[2] =  pow2q(plr3D.target.sigmaTargetHt) +  pow2q(station.instrument.sigmaInstrHeight) + pow2q(contrib.fThetaContrib[2]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[2]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[2]) * varianceDIST;
	return contrib;
}

//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- CAMERA measurements (UVEC/UVD)
//////////////////////////////////////////////////////////////////////
UVECContrib	TContributionsGenerator::getUVECContrib(const TCAM& camera, const TUVEC& uvec){
	fMLAused = false;  // TCAM measurements are never in MLA
	const TLOR2LOR& tg2stTrafo = getLORTransformation(uvec.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); //Trafo from from target's LOR to station's LOR
	TPositionVector targetPos = uvec.targetPos->getEstimatedValue(); // Target position
	tg2stTrafo.transform(targetPos);	//Transform from target's LOR into station's LOR

	const TFreeVector& unitVec = uvec.getVectorValue(); // observed vector (X and Y), Z is not observed
	TReal i = unitVec.getX().getValue();
	TReal j = unitVec.getY().getValue();
	TReal k = unitVec.getZ().getValue();


	TReal dx = targetPos.getX().getValue() - camera.instrumentPos->getEstimatedValue().getX().getValue();
	TReal dy = targetPos.getY().getValue() - camera.instrumentPos->getEstimatedValue().getY().getValue();
	TReal dz = targetPos.getZ().getValue() - camera.instrumentPos->getEstimatedValue().getZ().getValue();

	TFreeVector stFirstEqContrib(-k/dz, 
						  0.0, 
						  k*(dx/pow2q(dz)), TCoordSysFactory::k3DCartesian);

	TFreeVector stSecondEqContrib(0, 
						  -k/dz, 
						  k*(dy/pow2q(dz)), TCoordSysFactory::k3DCartesian);


	TFreeVector tgFirstEqContrib(k*(-(dx/pow2q(dz))*tg2stTrafo.partDerivWRespToX0().getZ().getValue() + (1/dz)*tg2stTrafo.partDerivWRespToX0().getX().getValue()), 
						  k*(-(dx/pow2q(dz))*tg2stTrafo.partDerivWRespToY0().getZ().getValue() + (1/dz)*tg2stTrafo.partDerivWRespToY0().getX().getValue()), 
						  k*(-(dx/pow2q(dz))*tg2stTrafo.partDerivWRespToZ0().getZ().getValue() + (1/dz)*tg2stTrafo.partDerivWRespToZ0().getX().getValue()), TCoordSysFactory::k3DCartesian);

	TFreeVector tgSecondEqContrib(k*(-(dy/pow2q(dz))*tg2stTrafo.partDerivWRespToX0().getZ().getValue() + (1/dz)*tg2stTrafo.partDerivWRespToX0().getY().getValue()), 
						  k*(-(dy/pow2q(dz))*tg2stTrafo.partDerivWRespToY0().getZ().getValue() + (1/dz)*tg2stTrafo.partDerivWRespToY0().getY().getValue()), 
						  k*(-(dy/pow2q(dz))*tg2stTrafo.partDerivWRespToZ0().getZ().getValue() + (1/dz)*tg2stTrafo.partDerivWRespToZ0().getY().getValue()), TCoordSysFactory::k3DCartesian);
/////////////////////////////////////////////////////////////////////////////										 
//Fill transformation contributions//////////////////////////////////////////////////////////////////////////
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>> targetTransfContributions; // Vector with target's transformations contributions
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = tg2stTrafo.getTransformationChain();
	std::string transformationName;

	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
	for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
		std::string transformationName = it->adjTrafo->getName();

		//Contributions for rotations : Omega, Phi and Kappa
		TFreeVector omegaPD = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 0);
		TFreeVector phiPD = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 1);
		TFreeVector kappaPD = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 2);

		//Contributions for translation: X, Y and Z coordinate
		TFreeVector trans1PD = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 0);
		TFreeVector trans2PD = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 1); 
		TFreeVector trans3PD = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 2);

		TFreeVector scalePD = tg2stTrafo.partialDerivativesScale(transformationName, targetPos);

		TransformationContrib firstEqContrib = {
			TFreeVector(k*(-(1/pow2q(dz))*omegaPD.getZ().getValue()*dx + (1/dz)*omegaPD.getX().getValue()), 
						  k*(-(1/pow2q(dz))*phiPD.getZ().getValue()*dx + (1/dz)*phiPD.getX().getValue()), 
						  k*(-(1/pow2q(dz))*kappaPD.getZ().getValue()*dx + (1/dz)*kappaPD.getX().getValue()), TCoordSysFactory::k3DCartesian),
			TFreeVector(k*((-1/pow2q(dz))*trans1PD.getZ().getValue()*dx + (1/dz)*trans1PD.getX().getValue()), 
						  k*(-(1/pow2q(dz))*trans2PD.getZ().getValue()*dx + (1/dz)*trans2PD.getX().getValue()), 
						  k*(-(1/pow2q(dz))*trans3PD.getZ().getValue()*dx + (1/dz)*trans3PD.getX().getValue()), TCoordSysFactory::k3DCartesian),
				k*(-(1/pow2q(dz))*scalePD.getZ().getValue()*dx + (1/dz)*scalePD.getX().getValue())
		};

		TransformationContrib secondEqContrib = {
			TFreeVector(k*(-(1/pow2q(dz))*omegaPD.getZ().getValue()*dy + (1/dz)*omegaPD.getY().getValue()), 
						  k*(-(1/pow2q(dz))*phiPD.getZ().getValue()*dy + (1/dz)*phiPD.getY().getValue()), 
						 k*(-(1/pow2q(dz))*kappaPD.getZ().getValue()*dy + (1/dz)*kappaPD.getY().getValue()), TCoordSysFactory::k3DCartesian),
			TFreeVector(k*(-(1/pow2q(dz))*trans1PD.getZ().getValue()*dy + (1/dz)*trans1PD.getY().getValue()), 
						  k*(-(1/pow2q(dz))*trans2PD.getZ().getValue()*dy + (1/dz)*trans2PD.getY().getValue()), 
						  k*(-(1/pow2q(dz))*trans3PD.getZ().getValue()*dy + (1/dz)*trans3PD.getY().getValue()), TCoordSysFactory::k3DCartesian),
						k*(-(1/pow2q(dz))*scalePD.getZ().getValue()*dy + (1/dz)*scalePD.getY().getValue())
		};

		TransformationContrib2D trafoContrib = {firstEqContrib, secondEqContrib};

		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib2D> (*it->adjTrafo, trafoContrib));
	}	
/////END OF FILLING TRANSF. CONTRIB. PART
/////////////////////////////////////////////////////////////////////////////
	UVECContrib contrib = {stFirstEqContrib, stSecondEqContrib, tgFirstEqContrib, tgSecondEqContrib, targetTransfContributions, 
						   {-1.0, 0.0}, // X contribution (i) for a first and second equation
				           {0.0, -1.0}, // Y contribution (j) for a first and second equation
						   {-i+(k/dz)*dx, -j+(k/dz)*dy}, //Misclosure vector for a first and second equation
						   {pow2q(uvec.target.sigmaX), pow2q(uvec.target.sigmaY)}}; //Obs variance
	return contrib;
}

UVDContrib	TContributionsGenerator::getUVDContrib(const TCAM& camera, const TUVD& uvd){
	fMLAused = false; // TCAM measurements never in MLA
	TPositionVector targetPos = uvd.targetPos->getEstimatedValue();
	TPositionVector targetPosOrig = uvd.targetPos->getEstimatedValue();
	const TLOR2LOR& tg2stTrafo = getLORTransformation(uvd.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	tg2stTrafo.transform(targetPos);

	//CAM station's contribution is calculated in a LOR system of the station and, therefore, the station's contribution is this
	Point3DContrib coordContribStation = {TFreeVector(1.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian),
										TFreeVector(0.0, 1.0, 0.0, TCoordSysFactory::k3DCartesian),
										TFreeVector(0.0, 0.0, 1.0, TCoordSysFactory::k3DCartesian)};

	Point3DContrib coordContribTarget = { TFreeVector(-tg2stTrafo.partDerivWRespToX0(0), -tg2stTrafo.partDerivWRespToY0(0), -tg2stTrafo.partDerivWRespToZ0(0), TCoordSysFactory::k3DCartesian),
										  TFreeVector(-tg2stTrafo.partDerivWRespToX0(1), -tg2stTrafo.partDerivWRespToY0(1), -tg2stTrafo.partDerivWRespToZ0(1), TCoordSysFactory::k3DCartesian),
										  TFreeVector(-tg2stTrafo.partDerivWRespToX0(2), -tg2stTrafo.partDerivWRespToY0(2), -tg2stTrafo.partDerivWRespToZ0(2), TCoordSysFactory::k3DCartesian)};

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector with target's transformations contributions

	//Parametres: transformation from target's LOR into station's LOR, target position, vector of contributions to be filled
	addUVDTgTransfContributionsCamera(tg2stTrafo,  targetPosOrig, targetTransfContributions);

	TReal sDist = uvd.getDistance(); //measured distance
	TReal i = uvd.getVectorValue().getX().getValue(); // X component
	TReal j = uvd.getVectorValue().getY().getValue(); // Y component
	TReal k = uvd.getVectorValue().getZ().getValue(); // Z component

	TReal dx = targetPos.getX().getValue() - camera.instrumentPos->getEstimatedValue().getX().getValue();
	TReal dy = targetPos.getY().getValue() - camera.instrumentPos->getEstimatedValue().getY().getValue();
	TReal dz = targetPos.getZ().getValue() - camera.instrumentPos->getEstimatedValue().getZ().getValue();

	UVDContrib contrib = {coordContribStation, coordContribTarget, targetTransfContributions, 
		{sDist,0.0, 0.0}, // i
		{0.0,sDist, 0.0}, //j
		{i, j, k}, // dist contrib
		{sDist*i - dx, sDist*j - dy, sDist*k - dz}, //misclosure vector
		{pow2q(uvd.target.sigmaX), pow2q(uvd.target.sigmaY), pow2q(uvd.target.sigmaDist)}};   //variances

	return contrib;
}


/*ENSURE that this spatial distance measurement made by EDM instrument has the same mathematical model as DIST*/ 
DistMeasContrib	TContributionsGenerator::getDSPTContrib(const TEDM& edmST, const TDSPT& dspt){
    /* CONTRIBUTION IS CALCULATED EITHER IN ROOT OR IN MLA OF THE STATION(INSTRUMENT)*/
	/*Contribnutions to be returned*/
	TReal calcMeas, hiContrib, distCorrection, variance;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
///////////////////Transform TARGET ans STATION from their's LOR either to ROOT or to MLA of the station///////////////////////////////
	TPositionVector targetPos = dspt.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dspt.targetPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = edmST.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(edmST.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	//If not OLOC used and station can not rotate freely => contributions calculated in MLA of the instrument (station)
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		//If stationed point is different than in the previous call of Cotrib. Gener., or if MLA system was not used in the previous call => set a new origin of the CCS2MLA transformation
		if (!(fLastStationPtName == edmST.instrumentPos->getName()) || !fMLAused){
			set2MLATransformation(stationPos);
			fLastStationPtName = edmST.instrumentPos->getName();
		}
		fMLAused = true;
		transform2MLA(targetPos);
		stationPos = TPositionVector(0.0, 0.0, 0.0,TCoordSysFactory::k3DCartesian); //local astronomical system of the station, station's position is the origin of this system, i.e. ( 0 , 0, 0 )  
	}
	else
		fMLAused = false;
/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////
	TReal xSt = stationPos.getX().getValue();
	TReal ySt = stationPos.getY().getValue();
	TReal zSt = stationPos.getZ().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg = targetPos.getY().getValue();
	TReal zTg = targetPos.getZ().getValue();

	TReal hTg = dspt.target.targetHt;
	TReal hInst = edmST.instrumentHeightAdjustable->getEstimatedValue().getValue();
	TReal cst = dspt.target.distCorrectionAdjustable->getEstimatedValue().getValue();

	TReal D = dist3D(xSt, ySt, (zSt + hInst), xTg, yTg, (zTg + hTg));

	if (D < nullLimit)
		throw std::logic_error("TContributionGenerator::getSpatialDistanceContrib: Division by zero because observation points have identical coordinates.");

	TReal a,b,c;   //station's contributions coefficients (negative values of these give target's coefficients)		 
	a = (xSt -xTg)/D;  // xSt coefficient
	b = (ySt - yTg)/D;  //ySt coefficient
	c = (zSt + hInst - zTg - hTg)/D;  //zSt coefficient

	calcMeas = D - cst;
	hiContrib = c; //instrument height contribution

	//Station can be defined anywhere, get point contributions and transformations contributions
	coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	addTransformationsContributions(stLor2RootTrafo, edmST.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, dspt.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	distCorrection =  - 1.0;

	// Variance calculation
	TReal varM = pow2q(dspt.target.sigmaDSpt) + pow2q(calcMeas) * pow2q(dspt.target.ppmDSpt);
	TReal varInstHeight = pow2q(edmST.instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(dspt.target.sigmaTargetHt);
	TReal varInstCent = pow2q(edmST.instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(dspt.target.sigmaTargetCentering);
	variance = varM + pow2q((zTg - zSt + hTg - hInst)/D) * (varInstHeight +  varTgHeight) + ((pow2q(yTg - ySt) + pow2q(xSt - xTg))/pow2q(D)) * (varInstCent + varTgCent);

	DistMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, distCorrection, variance};
	return contrib;
}

////////////////////////////////////////
// Transformations related functions
////////////////////////////////////////
void TContributionsGenerator::transform2MLA(TPositionVector& pv){
	fccs2cgrf.transform(pv);
	fcgrf2ilg.transform(pv);
	filg2ila.transform(pv);
	TTransformation ILA2MILA;
	ILA2MILA.setRotationTransformation(0.0,0.0, -0.5934254894331); //Rotation of 37.77864 about Z-axis
	ILA2MILA.transform(pv);
}

void TContributionsGenerator::transform2MLA(TFreeVector& fv){
	fccs2cgrf.transform(fv);
	fcgrf2ilg.transform(fv);
	filg2ila.transform(fv);
	TTransformation ILA2MILA;
	ILA2MILA.setRotationTransformation(0.0,0.0, -0.5934254894331); //Rotation of 37.77864 about Z-axis
	ILA2MILA.transform(fv);
}

// Returns index of a transformation with given name in the 'fLORTrafo' vector, if vector does not include this transformation, function returns -1
int TContributionsGenerator::getTransformationIndex(const std::string& transfName) const{
	int index = 0;
	for(auto itLOR2LOR(fLORTrafo.begin()); itLOR2LOR != fLORTrafo.end(); ++itLOR2LOR){
		if(itLOR2LOR->getName() == transfName)
			return index;
		index++;
	}
	return -1;
}

// Set a new origin of LA system, initialize the transformation
void	TContributionsGenerator::set2MLATransformation(TPositionVector originInCCS){
	//Position of the origin in the CCS
	auto statPosCCS(originInCCS);
	//Transform origin to CGRF
	fccs2cgrf.transform(originInCCS);
	//Use origin CGRF position as an origin of the Local Geodetic system
	fcgrf2ilg = TCGRF2ILGTransformation(originInCCS);
	//Use origin CCS position as an origin of the Local Astronomical system
	filg2ila = TILG2ILATransformation(statPosCCS, fGeoidModel);
}

TLOR2LOR TContributionsGenerator::getIdentityTransformation(){
	//ROOT2ROOT transformation index == an identity transformation
	auto rootFramePosition(fTree->begin());
	return TLOR2LOR(rootFramePosition,rootFramePosition, "IDENTITY");
}

const TLOR2LOR& TContributionsGenerator::getLORTransformation(TDataTreeIterator originalTreePos, TDataTreeIterator destinationTreePos){
	std::string originalFrameName = originalTreePos->get()->frame.getName();
	std::string destinationFrameName = destinationTreePos->get()->frame.getName();

	int trIndex = getTransformationIndex(originalFrameName +  destinationFrameName);

	//If transformation is not defined yet (i.e. trIndex == -1), it needs to be added into the vector of transformations
	if(trIndex == -1){
		fLORTrafo.emplace_back(TLOR2LOR(originalTreePos, destinationTreePos, originalFrameName  + destinationFrameName));
		trIndex = fLORTrafo.size() - 1; // Index of the last transformation in the vector, i.e. the one we added on the line above
	}
	std::list<TLOR2LOR>::iterator it = fLORTrafo.begin();
    std::advance(it, trIndex);
	return *it;
}

DVERContrib	TContributionsGenerator::getDVERContrib(const TDVER& dver){
	fMLAused = false;
	auto k3D = TCoordSysFactory::k3DCartesian;
	/*Contribution if OLOC used, otherwise going to be rewritten*/
	TFreeVector stationContrib(k3D);
	TFreeVector targetContrib(k3D);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

	TPositionVector station(dver.station->getEstimatedValue());
	TPositionVector target(dver.target->getEstimatedValue());
	TPositionVector stationLOR = station;
	TPositionVector targetLOR = target;

	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(dver.station->getFrameTreePosition(), fTree->begin()); //Station's position frame
	stLor2RootTrafo.transform(station); //Transform to ROOT(CCS)

	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dver.target->getFrameTreePosition(), fTree->begin()); //Station's position frame
	tgLor2RootTrafo.transform(target); //Transform to ROOT(CCS)


	TPositionVector stationCCS = station;
	TPositionVector targetCCS = target;

	TReal  dh = 0.0;

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){  /*Needs to be calculated in CGRF.*/
		if(fRefFrame == TLGCRefFrame::ERefs::kSPHE){
			TXYH2CCS::CCS2XYHs(station);
			TXYH2CCS::CCS2XYHs(target); 
		}
		else if(fRefFrame == TLGCRefFrame::ERefs::kRS2K){
			TXYH2CCS::CCS2XYHg2000Machine(station);
			TXYH2CCS::CCS2XYHg2000Machine(target);
		}
		else if (fRefFrame == TLGCRefFrame::ERefs::kLEP){
			TXYH2CCS::CCS2XYHg1985Machine(station);
			TXYH2CCS::CCS2XYHg1985Machine(target);
		}
		/*IMPORTANT ENABLE CGRF, APPLIES IN POINT AND TRANSFORMATION CONTRIBUTIONS*/
		fCGRFused = true;
		//Calculating the distance meas
		dh = target.getH().getValue() - station.getH().getValue() - dver.getDistanceCorrection(); //Distance correction not provided!!!!!!!

		TPositionVector stationcgrf = fccs2cgrf.getTransformedPt(stationCCS);
		TReal xyDistST = sqrtq(pow2q(stationcgrf.getX().getValue())+ pow2q(stationcgrf.getY().getValue()));
		TReal phiST = Cartesian2Ellipsoidal::getPhi(stationcgrf.getX().getValue(), stationcgrf.getY().getValue() , stationcgrf.getZ().getValue());

		/*Coefficients for Station (with minus sign)*/
		TReal aST = - stationcgrf.getX().getValue() / (cosq(phiST)*xyDistST);
		TReal bST = - stationcgrf.getY().getValue() / (cosq(phiST)*xyDistST);

		stationContrib = getPointContributions(stLor2RootTrafo,aST,bST,0);
		addTransformationsContributions(stLor2RootTrafo,stationLOR,aST,bST,0,stationTransfContributions);

		TPositionVector targetcgrf = fccs2cgrf.getTransformedPt(targetCCS);
		TReal xyDistTG = sqrtq(pow2q(targetcgrf.getX().getValue())+ pow2q(targetcgrf.getY().getValue()));
		TReal phiTG = Cartesian2Ellipsoidal::getPhi(targetcgrf.getX().getValue(), targetcgrf.getY().getValue() , targetcgrf.getZ().getValue());

		/*Coefficients for Target (with minus sign)*/
		TReal aTG = targetcgrf.getX().getValue() / (cosq(phiTG)*xyDistTG);
		TReal bTG = targetcgrf.getY().getValue() / (cosq(phiTG)*xyDistTG);

		targetContrib = getPointContributions(tgLor2RootTrafo, aTG, bTG, 0);
		addTransformationsContributions(tgLor2RootTrafo,targetLOR, aTG, bTG, 0, targetTransfContributions);
	}
	else{ /*OLOC = Calculated as XYZ, simple case*/
		fCGRFused = false;
		dh = target.getZ().getValue() - station.getZ().getValue() - dver.getDistanceCorrection(); //Distance correction not provided!!!!!!!

		stationContrib = getPointContributions(stLor2RootTrafo,0,0,-1);
		addTransformationsContributions(stLor2RootTrafo,stationLOR,0,0,-1,stationTransfContributions);

		targetContrib = getPointContributions(tgLor2RootTrafo, 0, 0, 1);
		addTransformationsContributions(tgLor2RootTrafo,targetLOR, 0, 0, 1, targetTransfContributions);
	}

	DVERContrib dverC =  { dh, stationContrib, targetContrib, stationTransfContributions, targetTransfContributions, pow2q(dver.getObservedStDev())};
	
	return dverC;
}


TFreeVector TContributionsGenerator::getPointContributionDVER(const TLOR2LOR& lorTrafo, const  TPositionVector& cgrfPoint, bool station){
		TReal xyDist = sqrtq(pow2q(cgrfPoint.getX().getValue())+ pow2q(cgrfPoint.getY().getValue()));
		TReal phi = Cartesian2Ellipsoidal::getPhi(cgrfPoint.getX().getValue(), cgrfPoint.getY().getValue() , cgrfPoint.getZ().getValue());

		TReal a = cgrfPoint.getX().getValue() / (cosq(phi)*xyDist);
		TReal b = cgrfPoint.getY().getValue() / (cosq(phi)*xyDist);

		/*STATION CONTRIBUTIONS*/
		TFreeVector derX0 = lorTrafo.partDerivWRespToX0();
		TFreeVector derY0 = lorTrafo.partDerivWRespToY0();
		TFreeVector derZ0 = lorTrafo.partDerivWRespToZ0();

		fccs2cgrf.transform(derX0);
		fccs2cgrf.transform(derY0);
		fccs2cgrf.transform(derZ0);

		TReal x0CoordContrib = a*derX0.getX().getValue() + b*derX0.getY().getValue();
		TReal y0CoordContrib = a*derY0.getX().getValue() + b*derY0.getY().getValue();
		TReal z0CoordContrib = a*derZ0.getX().getValue() + b*derZ0.getY().getValue();

		if(station){
			x0CoordContrib*=-1;
			y0CoordContrib*=-1;
			z0CoordContrib*=-1;
		}

		return TFreeVector(x0CoordContrib, y0CoordContrib, z0CoordContrib,TCoordSysFactory::k3DCartesian );
}
///////////////////////////////////////////////////////////////////////////
// PRIVATE / SUPPORTING METHODS
///////////////////////////////////////////////////////////////////////////
TFreeVector TContributionsGenerator::getPointContributions(const TLOR2LOR& lorTrafo, TReal a, TReal b, TReal c){
	TFreeVector derX0 = lorTrafo.partDerivWRespToX0();
	TFreeVector derY0 = lorTrafo.partDerivWRespToY0();
	TFreeVector derZ0 = lorTrafo.partDerivWRespToZ0();

	if(fMLAused){ //Transform partial derivatives into ILA if necessary
		transform2MLA(derX0);
		transform2MLA(derY0);
		transform2MLA(derZ0);
	}
	else if (fCGRFused){
		fccs2cgrf.transform(derX0);
		fccs2cgrf.transform(derY0);
		fccs2cgrf.transform(derZ0);
	}

	TReal xContrib = a * derX0.getX().getValue() + b * derX0.getY().getValue() + c* derX0.getZ().getValue();
	TReal yContrib = a * derY0.getX().getValue() + b * derY0.getY().getValue() + c* derY0.getZ().getValue();
	TReal zContrib = a * derZ0.getX().getValue() + b * derZ0.getY().getValue() + c* derZ0.getZ().getValue();
	
	return TFreeVector(xContrib, yContrib, zContrib, TCoordSysFactory::k3DCartesian); //Contribution of the point's coordinates
}

void TContributionsGenerator::addTransformationsContributions(const TLOR2LOR& transformations, const TPositionVector& pointPos, TReal a, TReal b, TReal c, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>>& transfContrib){
		const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = transformations.getTransformationChain();
		TReal omegaContrib, phiContrib, kappaContrib, trans1Contrib, trans2Contrib, trans3Contrib, scaleContrib;

		//Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
		for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
			std::string transformationName = it->adjTrafo->getName();

			TFreeVector omegaPD = transformations.partialDerivativesAngle(transformationName, pointPos, 0);  //Partial derivatives of the LOR transformation with respect to omega
			TFreeVector phiPD = transformations.partialDerivativesAngle(transformationName, pointPos, 1);
			TFreeVector kappaPD = transformations.partialDerivativesAngle(transformationName, pointPos, 2);

			TFreeVector trans1PD = transformations.partialDerivativesTranslation(transformationName, pointPos, 0);
			TFreeVector trans2PD = transformations.partialDerivativesTranslation(transformationName, pointPos, 1);
			TFreeVector trans3PD = transformations.partialDerivativesTranslation(transformationName, pointPos, 2);

			TFreeVector scalePD = transformations.partialDerivativesScale(transformationName, pointPos);

			if(fMLAused){
				transform2MLA(omegaPD);
				transform2MLA(phiPD);
				transform2MLA(kappaPD);

				transform2MLA(trans1PD);
				transform2MLA(trans2PD);
				transform2MLA(trans3PD);

				transform2MLA(scalePD);
			}
			else if (fCGRFused){
				fccs2cgrf.transform(omegaPD);
				fccs2cgrf.transform(phiPD);
				fccs2cgrf.transform(kappaPD);

				fccs2cgrf.transform(trans1PD);
				fccs2cgrf.transform(trans2PD);
				fccs2cgrf.transform(trans3PD);

				fccs2cgrf.transform(scalePD);
			}

			omegaContrib = a * omegaPD.getX().getValue() + b * omegaPD.getY().getValue() + c * omegaPD.getZ().getValue();
			phiContrib = a * phiPD.getX().getValue() + b * phiPD.getY().getValue() + c * phiPD.getZ().getValue();
			kappaContrib = a * kappaPD.getX().getValue() + b * kappaPD.getY().getValue() + c * kappaPD.getZ().getValue();

			trans1Contrib = a * trans1PD.getX().getValue() + b * trans1PD.getY().getValue() + c * trans1PD.getZ().getValue();
			trans2Contrib = a * trans2PD.getX().getValue() + b * trans2PD.getY().getValue() + c * trans2PD.getZ().getValue();
			trans3Contrib = a * trans3PD.getX().getValue() + b * trans3PD.getY().getValue() + c * trans3PD.getZ().getValue();

			scaleContrib = a * scalePD.getX().getValue() + b * scalePD.getY().getValue() + c * scalePD.getZ().getValue();

			TransformationContrib trContrib = {TFreeVector(omegaContrib, phiContrib, kappaContrib, TCoordSysFactory::k3DCartesian), TFreeVector(trans1Contrib,trans2Contrib,trans3Contrib,TCoordSysFactory::k3DCartesian),scaleContrib};
			transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib> (*it->adjTrafo, trContrib));
	}
}

void TContributionsGenerator::addUVDTgTransfContributionsCamera(const TLOR2LOR& transformations, const TPositionVector& pointPos, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib){
		const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = transformations.getTransformationChain();
		std::string transformationName;
		TFreeVector scaleDeriv(TCoordSysFactory::k3DCartesian);
		TFreeVector omegaDerivative(TCoordSysFactory::k3DCartesian);
		TFreeVector phiDerivative (TCoordSysFactory::k3DCartesian);
		TFreeVector kappaDerivative(TCoordSysFactory::k3DCartesian);

		TFreeVector t1Derivative = (TCoordSysFactory::k3DCartesian);
		TFreeVector t2Derivative = (TCoordSysFactory::k3DCartesian);
		TFreeVector t3Derivative (TCoordSysFactory::k3DCartesian);

		// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
		for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
			std::string transformationName = it->adjTrafo->getName();

			//Contributions for rotations : Omega, Phi and Kappa
			omegaDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 0);
			phiDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 1);
			kappaDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 2);

			//Contributions for translation: X, Y and Z coordinate
			t1Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 0);
			t2Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 1); 
			t3Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 2);

			scaleDeriv = transformations.partialDerivativesScale(transformationName, pointPos);


			TransformationContrib firstEqContrib = {TFreeVector(-omegaDerivative.getX().getValue(),-phiDerivative.getX().getValue(), -kappaDerivative.getX().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- t1Derivative.getX().getValue(), -t2Derivative.getX().getValue(), -t3Derivative.getX().getValue(),TCoordSysFactory::k3DCartesian),
													- scaleDeriv.getX().getValue()};

			TransformationContrib secondEqContrib = {TFreeVector(-  omegaDerivative.getY().getValue(), -  phiDerivative.getY().getValue(), - kappaDerivative.getY().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- t1Derivative.getY().getValue(), - t2Derivative.getY().getValue(), - t3Derivative.getY().getValue(),TCoordSysFactory::k3DCartesian),
													-  scaleDeriv.getY().getValue()};

			TransformationContrib thirdEqContrib = {TFreeVector(- omegaDerivative.getZ().getValue(), - phiDerivative.getZ().getValue(), -  kappaDerivative.getZ().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- t1Derivative.getZ().getValue(), - t2Derivative.getZ().getValue(), -  t3Derivative.getZ().getValue(),TCoordSysFactory::k3DCartesian),
													- scaleDeriv.getZ().getValue()};

			TransformationContrib3D trContrib = {firstEqContrib, secondEqContrib, thirdEqContrib};

			transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, trContrib));
	}
}