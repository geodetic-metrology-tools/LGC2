#include "TContributionsGenerator.h"
#include "TLGCData.h"
#include "TDist.h" 
#include<Eigen/Dense>

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////
TContributionsGenerator::TContributionsGenerator(TDataTree* tree, TLGCRefFrame::ERefs refFrame): fTree(tree), fRefFrame(refFrame),
fMLAused(false)
{
	fLastStationPtName = "NULL";

	if(refFrame == TLGCRefFrame::ERefs::kSPHE)
		fGeoidModel= TRefSystemFactory::EGeoid::kCGSphere;   //Is it the right one or topo??
	else if(refFrame == TLGCRefFrame::ERefs::kRS2K)
		fGeoidModel= TRefSystemFactory::EGeoid::kCG2000Machine;
	else if(refFrame == TLGCRefFrame::ERefs::kSPHE)
		fGeoidModel= TRefSystemFactory::EGeoid::kCG1985Machine;	
	else
		fGeoidModel = TRefSystemFactory::EGeoid::kNoGeoid;
}

//////////////////////////////
//Tree update
//////////////////////////////
//Update transformations between nodes, need to be called, whenever tree changed, typically after every LS calculation run (before running input matrices filler)
void TContributionsGenerator::updateTransformations(){
	for(std::vector<TLOR2LOR>::iterator it = fLORTrafo.begin(); it != fLORTrafo.end(); ++it){
		it->updateTree();
	}
	fLastStationPtName = "NULL";
	fMLAused = false;
}
//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION 
//////////////////////////////////////////////////////////////////////
// Returns the calculated observation value and design matrix contributions for the given Spatial Distance observation with the current parameters 
DistMeasContrib	TContributionsGenerator::getSpatialDistanceContrib(const TTSTN& station, const TLINE& dist){
	//Contributions to be returned
	TReal calcMeas, hiContrib, distCorrection, variance;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
///////////////////Transform TARGET from it's LOR either to ROOT or to MLA of the station///////////////////////////////
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dist.targetPos->getFrameTreePosition(), fTree->begin()); //Station's LOR is 'fTree.begin()'
	tgLor2RootTrafo.transform(targetPos);

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
////////////////////////Calculate contributions for both station and target, and the transformation parameters/////////////////
	//Station is defined in a ROOT node, therefore LOR transformation is an identity transformation
	TLOR2LOR statIdentTransformation = getIdentityTransformation();	 
	coordContribStation = getPointContributions(statIdentTransformation, a, b, c);

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

	DistMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, targetTransfContributions, hiContrib, distCorrection, variance};
	return contrib;
}

// Returns the calculated observation value and design matrix contributions for the given Horizontal angle (ANGL) observation with the current parameters 
AnglMeasContrib	TContributionsGenerator::getHorAnglContrib(const TTSTN& station, const TTSTN::TROM& rom, const TANGL& angl){
	LGC::TAngle calcMeas; //Calculated measurement value
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	TReal hiContrib, v0Contrib;

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = angl.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(angl.targetPos->getFrameTreePosition(), fTree->begin()); // Station is defined in a ROOT node
	tgLor2RootTrafo.transform(targetPos); //Transform to station's reference frame


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

	//Station is defined in a ROOT node, therefore LOR transformation is an identity transformation (contribution is calculated either in ROOT or in station's local astronomical system)
	TLOR2LOR statIdentTransformation = getIdentityTransformation();
	coordContribStation = getPointContributions(statIdentTransformation, a,b,c);

	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, angl.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	// Variance calculation
	TReal variance = pow2q(angl.target.sigmaAngl) + (1.0/pow2q(dist2)) * (pow2q(station.instrument.sigmaInstrCentering) + pow2q(angl.target.sigmaTargetCentering));

	AnglMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

//Zenith distance (verical angle) contributions
AnglMeasContrib	TContributionsGenerator::getZenDistContrib(const TTSTN& station, const TZEND& zend){
	LGC::TAngle calcMeas;
	TReal hiContrib, v0Contrib;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = zend.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(zend.targetPos->getFrameTreePosition(), fTree->begin()); //Station's LOR is 'fTree.begin()'
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

	//Station is defined in a ROOT node, therefore LOR transformation is an identity transformation
	TLOR2LOR statIdentTransformation = getIdentityTransformation();
	coordContribStation = getPointContributions(statIdentTransformation, a,b,c);

	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, zend.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions); //Add transformation's coordinates of the target transformation

	TReal variance = pow2q(zend.target.sigmaAngl) + (((pow2q(dx) + pow2q(dy))*pow2q(dz))/(powq(distance3D,6)*pow2q(sinPhi))) * 
					(pow2q(station.instrument.sigmaInstrCentering) + pow2q(zend.target.sigmaTargetCentering)) +
					 pow2q(-c) * (pow2q(station.instrument.sigmaInstrHeight) + pow2q(zend.target.sigmaTargetHt));

	AnglMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

//Horizontal distance contribution for a measurement made in DLEV
HorDistContribLEVEL	TContributionsGenerator::getHorDistContrib(const TAdjustablePoint* referencePoint, const TDHOR& dhor){
	TReal calcMeas;
	TFreeVector coordContribReferencePoint(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribLevelStaff(TCoordSysFactory::k3DCartesian);
	TPositionVector rpPos = referencePoint->getEstimatedValue();  // Reference point is a 'target'
	TPositionVector dhorTargetPos = dhor.targetPos->getEstimatedValue();   // Levelling staff is a 'station'

	//If not OLOC used => transform into local astronomical system of the Levelling Staff (this is the station which makes measurements)
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		//If station position is different than in previous call of CG, or if MLA system was not used in previous call => set new origin of the CCS2MLA transformation
		if (!(fLastStationPtName == dhor.targetPos->getName()) || !fMLAused){
			set2MLATransformation(dhorTargetPos);
			fLastStationPtName =  dhor.targetPos->getName();
		}
		fMLAused = true;
		transform2MLA(rpPos);
		dhorTargetPos = TPositionVector(0,0,0,TCoordSysFactory::k3DCartesian); 	//local astronomical system of the station, station's position is the origin of this system
	}
	else
		fMLAused = false;

	TReal xSt = dhorTargetPos.getX().getValue();
	TReal ySt = dhorTargetPos.getY().getValue();

	TReal xTg = rpPos.getX().getValue();
	TReal yTg = rpPos.getY().getValue();

	calcMeas = dist(xSt, ySt, xTg, yTg);

	if (calcMeas < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getHorDistContrib: Division by zero because observation points have identical coordinates.");

	TReal a,b,c; //reference point's contributions coefficients (negative values of these give target's coefficients)		
	a = - (xTg - xSt)/calcMeas; // xRp coefficient
	b = - (yTg - ySt)/calcMeas; //yRp coefficient
	c = 0.0; //zRp coefficient

	//Station is defined in a ROOT node, therefore LOR transformation is an identity transformation (contribution is calculated either in ROOT or in station's local astronomical system)
	TLOR2LOR identityTransformation = getIdentityTransformation();
	coordContribLevelStaff = getPointContributions(identityTransformation, a, b, c);
	coordContribReferencePoint = getPointContributions(identityTransformation, -a, -b, -c);
	
	HorDistContribLEVEL  contrib = {calcMeas, coordContribReferencePoint, coordContribLevelStaff};
	return contrib;
}

//Horizontal distance contributions, measurement made by TSTN
HorDistContrib	TContributionsGenerator::getHorDistContrib(const TTSTN& station, const TLINE& dhor){
	TReal calcMeas;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = dhor.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dhor.targetPos->getFrameTreePosition(), fTree->begin()); //LOR of the station is 'fTree.begin()'
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

	//Station is defined in a ROOT node, therefore LOR transformation is an identity transformation
	TLOR2LOR statIdentTransformation = getIdentityTransformation();
	coordContribStation = getPointContributions(statIdentTransformation, a,b,c);

	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, dhor.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	calcMeas = D - cte;
	TReal distCorrCont = 1.0;

	// Variance
	TReal varM = pow2q(dhor.target.sigmaDist) + pow2q(calcMeas) * pow2q(dhor.target.ppmDist);
	TReal variance = varM + (pow2q(station.instrument.sigmaInstrCentering) + pow2q(dhor.target.sigmaTargetCentering));

	HorDistContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, targetTransfContributions, distCorrCont, variance};
	return contrib;
}


//DLEV contributions
DLEVContrib	TContributionsGenerator::getDLEVContrib(const TLEVEL& levelInstr, const TDLEV& dlev){
	TReal collAngl = levelInstr.instrument.collAngleAdjustable->getEstimatedValue().rad(); //collimination angle in rads
	TReal cdz = dlev.target.distCorrectionValue; //distance of the target correction value
	TReal dRef = levelInstr.fMeasuredPlane->getRefPtDistEstimatedValue().getValue(); 

	TPositionVector referencePoint = levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue();
	TPositionVector staffPosition = dlev.targetPos->getEstimatedValue();  // this Target is what is also called Levelling Staff

	TReal dTg = dist(staffPosition.getX().getValue(), staffPosition.getY().getValue(), referencePoint.getX().getValue(), referencePoint.getY().getValue()); 

	std::string rpName = levelInstr.fMeasuredPlane->getReferencePoint()->getName();
	//If not OLOC used => set up a MLA transformation
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		if (!(fLastStationPtName == rpName) || !fMLAused){
			set2MLATransformation(referencePoint);
			fLastStationPtName = rpName;
		}
		fMLAused = true;
		transform2MLA(staffPosition);
		referencePoint = TPositionVector(0.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian); 	//local astronomical system of the station, station's position is the origin of this system
	}
	else
		fMLAused = false;

	TReal calcMeas = referencePoint.getZ().getValue() - staffPosition.getZ().getValue() - dRef - cdz - dTg*tanq(collAngl);

	TLOR2LOR statIdentTransformation = getIdentityTransformation();
	TFreeVector staffTargetContrib = getPointContributions(statIdentTransformation, 0.0, 0.0, -1.0);

	TReal collAngleContrib = - dTg*(1.0 + powq(tanq(collAngl),2));
	TReal fRefPtDistCont = -1.0;

	TReal variance = pow2q(dlev.target.sigmaD) + pow2q(dTg) * pow2q(dlev.target.ppmD) +  pow2q(dlev.target.sigmaStaffHt);

	DLEVContrib dlevContrib = {calcMeas, staffTargetContrib, fRefPtDistCont, collAngleContrib, variance};
	return dlevContrib;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////3D/2D measurements///////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------ PLR3D and DIR3D Contrib for TSTN------------------------------------------------------
PLR3DContrib	TContributionsGenerator::getPolar3DContrib(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D){
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(plr3D.targetPos->getFrameTreePosition(), fTree->begin()); //Transformation from target's LOR to ROOT
	tgLor2RootTrafo.transform(targetPos); //Transform target to ROOT

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
	TFreeVector partDerWRespToX0St(1.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToY0St(0.0, 1.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToZ0St(0.0, 0.0, 1.0, TCoordSysFactory::k3DCartesian);

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
/////This part fills target transformations contributions
/////////////////////////////////////////////////////////////////////////////////
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector for target transformations contributions

	const TPositionVector tgPointInLOR = plr3D.targetPos->getEstimatedValue();
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Contribution
	PLR3DContrib contrib = {coordContribStation, coordContribTarget, targetTransfContributions, 
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

//new DIR3D
DIR3DContrib	TContributionsGenerator::getDIR3DContrib(const TTSTN& station, const TTSTN::TROM& rom, const TDIR3D& dir3D){
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = dir3D.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dir3D.targetPos->getFrameTreePosition(), fTree->begin()); //Transformation from target's LOR to ROOT
	tgLor2RootTrafo.transform(targetPos); //Transform target to ROOT

	TReal Rx = 0.0; //Rotation around x-axis, default is no rotation
	TReal Ry = 0.0; //Rotation around y-axis, default is no rotation

	if(station.rot3D){ //If station can rotate freely load the rotations
		if(station.rotX == nullptr || station.rotY == nullptr)
			throw std::runtime_error("TContributionGenerator::getPolar3DContrib station can rotate freely, but rotation angles are NULL.");
		Rx = station.rotX->getEstimatedValue().rad();
		Ry = station.rotY->getEstimatedValue().rad();
	}

	TReal sinTheta = dir3D.getAngle(kANGL).sin();
	TReal cosTheta = dir3D.getAngle(kANGL).cos();

	TReal sinPhi = dir3D.getAngle(kZEND).sin();
	TReal cosPhi = dir3D.getAngle(kZEND).cos();

	TReal sinV0 = (rom.v0->getEstimatedValue() - rom.acst).sin();
	TReal cosV0 = (rom.v0->getEstimatedValue() - rom.acst).cos();

	TReal sinRx = sinq(Rx);
	TReal cosRx = cosq(Rx);

	TReal sinRy = sinq(Ry);
	TReal cosRy = cosq(Ry);

	TReal dX = targetPos.getX().getValue() - stationPos.getX().getValue();
	TReal dY = targetPos.getY().getValue() - stationPos.getY().getValue();
	TReal dZ = targetPos.getZ().getValue() + dir3D.target.targetHt - stationPos.getZ().getValue() -  station.instrumentHeightAdjustable->getEstimatedValue().getValue();

	TReal s = sqrt(powq(dX,2)+powq(dY,2)+powq(dZ,2));

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
		transform2MLA(targetPos);
		stationPos = TPositionVector(0.0, 0.0, 0.0,TCoordSysFactory::k3DCartesian); 	//local astronomical system of the station, station's position is the origin of this system
	}
	else
		fMLAused = false;
////////////////////////////////////////////////////////////////////////////////
/////STATION COORDINATES PARTIAL DERIVATIVES
//////////////////////////////////////////////////////////////////////////////
	// TSTN station contribution is calculated in a LOR system of the station, i.e. from LOR's it is an identity transformation
	TFreeVector partDerWRespToX0St(1.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToY0St(0.0, 1.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToZ0St(0.0, 0.0, 1.0, TCoordSysFactory::k3DCartesian);

	if(fMLAused){
		transform2MLA(partDerWRespToX0St);
		transform2MLA(partDerWRespToY0St);
		transform2MLA(partDerWRespToZ0St);
	}

	//Vector to be multiplied by A matrix, i.e. A*pdVectorx0St, A*pdVectory0St and , A*pdVectorz0St
	TFreeVector pdVectorx0St = getVectorForDIR3DStationPartDer(dX,dY,dZ,s,partDerWRespToX0St);
	TFreeVector pdVectory0St = getVectorForDIR3DStationPartDer(dX,dY,dZ,s,partDerWRespToY0St);
	TFreeVector pdVectorz0St = getVectorForDIR3DStationPartDer(dX,dY,dZ,s,partDerWRespToZ0St);

	Point3DContrib coordContribStation = {
		TFreeVector( line1AMat.dot(pdVectorx0St), line1AMat.dot(pdVectory0St), line1AMat.dot(pdVectorz0St), TCoordSysFactory::k3DCartesian),//St contribution for a first equation
		TFreeVector( line2AMat.dot(pdVectorx0St), line2AMat.dot(pdVectory0St), line2AMat.dot(pdVectorz0St), TCoordSysFactory::k3DCartesian),//St contribution for a first equation
		TFreeVector( line3AMat.dot(pdVectorx0St), line3AMat.dot(pdVectory0St), line3AMat.dot(pdVectorz0St), TCoordSysFactory::k3DCartesian),//St contribution for a first equation
	};
////////////////////////////////////////////////////////////////////////////////
/////TARGET COORDINATES PARTIAL DERIVATIVES
//////////////////////////////////////////////////////////////////////////////
	TFreeVector partDerWRespToX0Tg = tgLor2RootTrafo.partDerivWRespToX0();
	TFreeVector partDerWRespToY0Tg = tgLor2RootTrafo.partDerivWRespToY0();
	TFreeVector partDerWRespToZ0Tg = tgLor2RootTrafo.partDerivWRespToZ0();

	if(fMLAused){
		transform2MLA(partDerWRespToX0Tg);
		transform2MLA(partDerWRespToY0Tg);
		transform2MLA(partDerWRespToZ0Tg);
	}

	//Vector to be multiplied by A matrix
	TFreeVector pdVectorx0Tg = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s,partDerWRespToX0Tg);
	TFreeVector pdVectory0Tg = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s,partDerWRespToY0Tg);
	TFreeVector pdVectorz0Tg = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s,partDerWRespToZ0Tg);


	Point3DContrib coordContribTarget= {
		TFreeVector( -line1AMat.dot(pdVectorx0Tg), -line1AMat.dot(pdVectory0Tg), -line1AMat.dot(pdVectorz0Tg), TCoordSysFactory::k3DCartesian),//St contribution for a first equation
		TFreeVector( -line2AMat.dot(pdVectorx0Tg), -line2AMat.dot(pdVectory0Tg), -line2AMat.dot(pdVectorz0Tg), TCoordSysFactory::k3DCartesian),//St contribution for a first equation
		TFreeVector( -line3AMat.dot(pdVectorx0Tg), -line3AMat.dot(pdVectory0Tg), -line3AMat.dot(pdVectorz0Tg), TCoordSysFactory::k3DCartesian),//St contribution for a first equation
	};
/////////////////////////////////////////////////////////////////////////////////
/////This part fills target transformations contributions
/////////////////////////////////////////////////////////////////////////////////
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector for target transformations contributions

	const TPositionVector tgPointInLOR = dir3D.targetPos->getEstimatedValue();
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

		TFreeVector omegaDerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, omegaDerivative);
		TFreeVector phiDerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, phiDerivative);
		TFreeVector kappaDerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, kappaDerivative);
		TFreeVector t1DerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, t1Derivative);
		TFreeVector t2DerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, t2Derivative);
		TFreeVector t3DerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, t3Derivative);
		TFreeVector scaleDerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, scaleDeriv);

		TransformationContrib firstEqContrib = {	
		TFreeVector(-line1AMat.dot(omegaDerivVector),-line1AMat.dot(phiDerivVector),-line1AMat.dot(kappaDerivVector), TCoordSysFactory::k3DCartesian), 
		TFreeVector(-line1AMat.dot(t1DerivVector), -line1AMat.dot(t2DerivVector),-line1AMat.dot(t3DerivVector), TCoordSysFactory::k3DCartesian),
		-line1AMat.dot(scaleDerivVector)};


		TransformationContrib secondEqContrib = {	
		TFreeVector(-line2AMat.dot(omegaDerivVector),-line2AMat.dot(phiDerivVector),-line2AMat.dot(kappaDerivVector), TCoordSysFactory::k3DCartesian), 
		TFreeVector(-line2AMat.dot(t1DerivVector), -line2AMat.dot(t2DerivVector),-line2AMat.dot(t3DerivVector), TCoordSysFactory::k3DCartesian), 
		-line2AMat.dot(scaleDerivVector)};


		TransformationContrib thirdEqContrib = {	
		TFreeVector(-line3AMat.dot(omegaDerivVector),-line3AMat.dot(phiDerivVector),-line3AMat.dot(kappaDerivVector), TCoordSysFactory::k3DCartesian), 
		TFreeVector(-line3AMat.dot(t1DerivVector), -line3AMat.dot(t2DerivVector),-line3AMat.dot(t3DerivVector), TCoordSysFactory::k3DCartesian), 
		-line3AMat.dot(scaleDerivVector)};

		TransformationContrib3D trContrib = {firstEqContrib, secondEqContrib, thirdEqContrib};
		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, trContrib));
	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//CONTRIBUTIONS
	DIR3DContrib contrib = {coordContribStation, coordContribTarget, targetTransfContributions, 
	{line1AMat.getZ().getValue(), line2AMat.getZ().getValue(), line3AMat.getZ().getValue()}, //!!!!!!THIS IS NOT CORRECT, BUT NOT IMPORTANTS SO FAR, IT IS NOT CONSIDERED, CHANGE LATER!!!!!!
	//V0 contribution for a first, second and third equation
	{-1/s*(-sinV0*cosRy*dX + (-cosV0*cosRx-sinV0*sinRx*sinRy)*dY + (cosV0*sinRx-sinV0*cosRx*sinRy)*dZ),
	 -1/s*(cosV0*cosRy*dX + (-sinV0*cosRx+cosV0*sinRx*sinRy)*dY + (sinV0*sinRx+cosV0*cosRx*sinRy)*dZ), 
	  0},
	//Theta contribution
	{cosTheta*sinPhi,
	 -sinTheta*sinPhi, 
	 0},
	//Phi contribution
	{sinTheta*cosPhi,
	 cosTheta*cosPhi, 
	 -sinPhi},  
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
	contrib.fMisclosureVector[0] = (sinTheta*sinPhi) - 1/s * line1AMat.dot(deltaStTg);
	contrib.fMisclosureVector[1] = (cosTheta*sinPhi) - 1/s * line2AMat.dot(deltaStTg);
	contrib.fMisclosureVector[2] = (cosPhi) - 1/s * line3AMat.dot(deltaStTg);

	//Variance calcualtion
	TReal varianceInstrCenter = pow2q(station.instrument.sigmaInstrCentering);
	TReal varianceTgCenter = pow2q(dir3D.target.sigmaTargetCentering);
	// ANGL
	TReal varianceANGL = pow2q(dir3D.target.sigmaAngl);
	// ZEND
	TReal varianceZEND = pow2q(dir3D.target.sigmaZenD);
	

	contrib.fObsVariance[0] = 0.5*(varianceInstrCenter + varianceTgCenter) + pow2q(contrib.fThetaContrib[0]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[0]) * varianceZEND ;

	contrib.fObsVariance[1] = 0.5*((varianceInstrCenter + varianceTgCenter)) + pow2q(contrib.fThetaContrib[1]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[1]) * varianceZEND ;

	contrib.fObsVariance[2] =  pow2q(dir3D.target.sigmaTargetHt) +  pow2q(station.instrument.sigmaInstrHeight) + pow2q(contrib.fThetaContrib[2]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[2]) * varianceZEND ;
	return contrib;
}



TFreeVector TContributionsGenerator::getVectorForDIR3DTargetPartDer(TReal dX, TReal dY, TReal dZ, TReal s, TFreeVector partDerwr2Unkn){
	//Partial derivative of 's' with respect to x0
	TReal pdSwr2Unknown = (dX*partDerwr2Unkn.getX().getValue() + dY*partDerwr2Unkn.getY().getValue() + dZ*partDerwr2Unkn.getZ().getValue())/s;
	//Vector to be multiplied by A matrix
	return TFreeVector(- (dX/pow2q(s))*pdSwr2Unknown + (1/s)*partDerwr2Unkn.getX().getValue(),-(dY/pow2q(s))*pdSwr2Unknown + (1/s)*partDerwr2Unkn.getY().getValue(), -(dZ/pow2q(s))*pdSwr2Unknown + (1/s)*partDerwr2Unkn.getZ().getValue(),TCoordSysFactory::k3DCartesian);
}

TFreeVector TContributionsGenerator::getVectorForDIR3DStationPartDer(TReal dX, TReal dY, TReal dZ, TReal s, TFreeVector partDerwr2Unkn){
	//Partial derivative of 's' with respect to x0
	TReal pdSwr2Unknown = -(dX*partDerwr2Unkn.getX().getValue() + dY*partDerwr2Unkn.getY().getValue() + dZ*partDerwr2Unkn.getZ().getValue())/s;
	//Vector to be multiplied by A matrix
	return TFreeVector((dX/pow2q(s))*pdSwr2Unknown + (1/s)*partDerwr2Unkn.getX().getValue(),(dY/pow2q(s))*pdSwr2Unknown + (1/s)*partDerwr2Unkn.getY().getValue(), (dZ/pow2q(s))*pdSwr2Unknown + (1/s)*partDerwr2Unkn.getZ().getValue(),TCoordSysFactory::k3DCartesian);
}

//old DIR3D
#if 0
DIR3DContrib	TContributionsGenerator::getDIR3DContrib(const TTSTN& station, const TTSTN::TROM& rom, const TDIR3D& dir3D){
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = dir3D.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dir3D.targetPos->getFrameTreePosition(), fTree->begin()); //Transformation from target's LOR to ROOT
	tgLor2RootTrafo.transform(targetPos); //Transform target to ROOT

	TReal Rx = 0.0; //Rotation around x-axis, default is no rotation
	TReal Ry = 0.0; //Rotation around y-axis, default is no rotation

	if(station.rot3D){ //If station can rotate freely load the rotations
		if(station.rotX == nullptr || station.rotY == nullptr)
			throw std::runtime_error("TContributionGenerator::getPolar3DContrib station can rotate freely, but rotation angles are NULL.");
		Rx = station.rotX->getEstimatedValue().rad();
		Ry = station.rotY->getEstimatedValue().rad();
	}

	TReal dX = targetPos.getX().getValue() - stationPos.getX().getValue();
	TReal dY = targetPos.getY().getValue() - stationPos.getY().getValue();
	TReal dZ = targetPos.getZ().getValue() + dir3D.target.targetHt - stationPos.getZ().getValue() -  station.instrumentHeightAdjustable->getEstimatedValue().getValue();
	TFreeVector deltaStTg(dX, dY, dZ,TCoordSysFactory::k3DCartesian);

	TReal sinTheta = sinq(dir3D.getAngle(kANGL).rad());
	TReal cosTheta = cosq(dir3D.getAngle(kANGL).rad());

	TReal sinPhi = sinq(dir3D.getAngle(kZEND).rad());
	TReal cosPhi = cosq(dir3D.getAngle(kZEND).rad());

	TReal sinV0 = sinq((rom.v0->getEstimatedValue() - rom.acst).rad());
	TReal cosV0 = cosq((rom.v0->getEstimatedValue() - rom.acst).rad());

	TReal sinRx = sinq(Rx);
	TReal cosRx = cosq(Rx);

	TReal sinRy = sinq(Ry);
	TReal cosRy = cosq(Ry);

	//Coefficients of the 'k' 
	TFreeVector kCoeff(sinRy , -sinRx*cosRy, cosRx*cosRy, TCoordSysFactory::k3DCartesian);
	TReal k = kCoeff.dot(deltaStTg);

	TFreeVector l1AMat( cosV0*cosRy, sinV0*cosRx+sinRx*cosV0*sinRy, sinV0*sinRx-cosRx*cosV0*sinRy, TCoordSysFactory::k3DCartesian); //first line of the A-matrix
	l1AMat *= (cosPhi);

	TFreeVector l2AMat( -sinV0*cosRy, cosV0*cosRx-sinRx*sinV0*sinRy, cosV0*sinRx+cosRx*sinV0*sinRy, TCoordSysFactory::k3DCartesian); //first line of the A-matrix
	l2AMat *= (cosPhi);

	//If not OLOC used and station is fixed (can not rotate freely) => contributions calculated in MLA
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D == false){
		if (!(fLastStationPtName == station.instrumentPos->getName()) || !fMLAused){
			set2MLATransformation(station.instrumentPos->getEstimatedValue());
			fLastStationPtName = station.instrumentPos->getName();
		}
		fMLAused = true;
		transform2MLA(targetPos);
		stationPos = TPositionVector(0.0,0.0,0.0,TCoordSysFactory::k3DCartesian); 	//local astronomical system of the station, station's position is the origin of this system
	}
	else
		fMLAused = false;

	//Contribution to a station coordinates, calculated in the station's system, i.e. for LOR's it is an identity transformation
	TFreeVector partDerWRespToX0St(1.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToY0St(0.0, 1.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToZ0St(0.0, 0.0, 1.0, TCoordSysFactory::k3DCartesian);

	if(fMLAused){
		transform2MLA(partDerWRespToX0St);
		transform2MLA(partDerWRespToY0St);
		transform2MLA(partDerWRespToZ0St);
	}

	TFreeVector	stFirstEqContrib( 
		- kCoeff.dot(partDerWRespToX0St)*(sinTheta*sinPhi) + l1AMat.dot(partDerWRespToX0St),
        - kCoeff.dot(partDerWRespToY0St)*(sinTheta*sinPhi) + l1AMat.dot(partDerWRespToY0St),
		- kCoeff.dot(partDerWRespToZ0St)*(sinTheta*sinPhi) + l1AMat.dot(partDerWRespToZ0St), TCoordSysFactory::k3DCartesian);

	TFreeVector	stSecondEqContrib( 
		- kCoeff.dot(partDerWRespToX0St)*(cosTheta*sinPhi) + l2AMat.dot(partDerWRespToX0St),
        - kCoeff.dot(partDerWRespToY0St)*(cosTheta*sinPhi) + l2AMat.dot(partDerWRespToY0St),
		- kCoeff.dot(partDerWRespToZ0St)*(cosTheta*sinPhi) + l2AMat.dot(partDerWRespToZ0St), TCoordSysFactory::k3DCartesian);

	//Contribution for target coordinates
	TFreeVector partDerWRespToX0Tg = tgLor2RootTrafo.partDerivWRespToX0();
	TFreeVector partDerWRespToY0Tg = tgLor2RootTrafo.partDerivWRespToY0();
	TFreeVector partDerWRespToZ0Tg = tgLor2RootTrafo.partDerivWRespToZ0();

	if(fMLAused){
		transform2MLA(partDerWRespToX0Tg);
		transform2MLA(partDerWRespToY0Tg);
		transform2MLA(partDerWRespToZ0Tg);
	}

	TFreeVector	tgFirstEqContrib( 
	   kCoeff.dot(partDerWRespToX0Tg)*(sinTheta*sinPhi) - l1AMat.dot(partDerWRespToX0Tg),
       kCoeff.dot(partDerWRespToY0Tg)*(sinTheta*sinPhi) - l1AMat.dot(partDerWRespToY0Tg),
	   kCoeff.dot(partDerWRespToZ0Tg)*(sinTheta*sinPhi) - l1AMat.dot(partDerWRespToZ0Tg), TCoordSysFactory::k3DCartesian);

	TFreeVector tgSecondEqContrib( 
		kCoeff.dot(partDerWRespToX0Tg)*(cosTheta*sinPhi) - l2AMat.dot(partDerWRespToX0Tg),
        kCoeff.dot(partDerWRespToY0Tg)*(cosTheta*sinPhi) - l2AMat.dot(partDerWRespToY0Tg),
		kCoeff.dot(partDerWRespToZ0Tg)*(cosTheta*sinPhi) - l2AMat.dot(partDerWRespToZ0Tg), TCoordSysFactory::k3DCartesian);

/////////////////////////////////////////////////////////////////////////////////
/////Fills target's transformations contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>> targetTransfContributions; // Vector with target's transformations contributions

	TPositionVector tgPointInLOR = dir3D.targetPos->getEstimatedValue();

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
		  TFreeVector( kCoeff.dot(omegaDerivative)*(sinTheta*sinPhi) - l1AMat.dot(omegaDerivative), 
					   kCoeff.dot(phiDerivative)*(sinTheta*sinPhi) - l1AMat.dot(phiDerivative),
		               kCoeff.dot(kappaDerivative)*(sinTheta*sinPhi) - l1AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian),//ROTATION

		  TFreeVector( kCoeff.dot(t1Derivative)*(sinTheta*sinPhi) - l1AMat.dot(t1Derivative),
				       kCoeff.dot(t2Derivative)*(sinTheta*sinPhi) - l1AMat.dot(t2Derivative),
					   kCoeff.dot(t3Derivative)*(sinTheta*sinPhi) - l1AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian),//TRANSLATION

		  kCoeff.dot(scaleDeriv)*(sinTheta*sinPhi) - l1AMat.dot(scaleDeriv)};//SCALE

		TransformationContrib secondEqContrib = {
	     TFreeVector(  kCoeff.dot(omegaDerivative)*(cosTheta*sinPhi) - l2AMat.dot(omegaDerivative),
			           kCoeff.dot(phiDerivative)*(cosTheta*sinPhi) - l2AMat.dot(phiDerivative),
		               kCoeff.dot(kappaDerivative)*(cosTheta*sinPhi) - l2AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian),

		 TFreeVector( kCoeff.dot(t1Derivative)*(cosTheta*sinPhi) - l2AMat.dot(t1Derivative),
					  kCoeff.dot(t2Derivative)*(cosTheta*sinPhi) - l2AMat.dot(t2Derivative),
				 	  kCoeff.dot(t3Derivative)*(cosTheta*sinPhi) - l2AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian),

		 kCoeff.dot(scaleDeriv)*(cosTheta*sinPhi) - l2AMat.dot(scaleDeriv)};


		TransformationContrib2D trafoContrib = {firstEqContrib, secondEqContrib};
		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib2D> (*it->adjTrafo, trafoContrib));
	}
/////END OF THE TRANSFORMATIONS CONTRIBUTION FILLING PART/////////////////////////////////////////////////////////////////

	DIR3DContrib contrib = {stFirstEqContrib, stSecondEqContrib, tgFirstEqContrib, tgSecondEqContrib, targetTransfContributions, 
	{ -cosRx*cosRy*sinTheta*sinPhi, -cosRx*cosRy*cosTheta*sinPhi}, //Instrument height contribution
				//V0 contibution for first and second equation
				{-cosPhi*(-sinV0*cosRy*dX + (cosV0*cosRx-sinRx*sinV0*sinRy)*dY + (cosV0*sinRx+cosRx*sinV0*sinRy)*dZ), 
				 -cosPhi*(-cosV0*cosRy*dX + (-sinV0*cosRx-sinRx*cosV0*sinRy)*dY + (-sinV0*sinRx+cosRx*cosV0*sinRy)*dZ)},
				//Theta contibution for first and second equation
	            {k*cosTheta*sinPhi, 
				 -k*sinTheta*sinPhi},
				//Phi contribution for first and second equation
	            {k*sinTheta*cosPhi + sinPhi*(cosV0*cosRy*dX + (sinV0*cosRx+sinRx*cosV0*sinRy)*dY+ (sinV0*sinRx-cosRx*cosV0*sinRy)*dZ), 
				 k*cosTheta*cosPhi + sinPhi*(-sinV0*cosRy*dX + (cosV0*cosRx-sinRx*sinV0*sinRy)*dY+(cosV0*sinRx+cosRx*sinV0*sinRy)*dZ)},
				{0.0, 0.0}, //default Rx contribution (to be overwritten if rotation of the station is enabled)
				{0.0, 0.0} //default Ry contribution (to be overwritten if rotation of the station is enabled)
				};
	// Rx, Ry cantribution, is station can rotate freely
	if(station.rot3D){
		TReal rot1Eqcoeff = -cosRx*cosRy*dY-sinRx*cosRy*dZ;
		contrib.fRxContrib[0] = rot1Eqcoeff*sinTheta*sinPhi - cosPhi* ((-sinV0*sinRx+cosRx*cosV0*sinRy)*dY + (sinV0*cosRx+sinRx*cosV0*sinRy)*dZ);
		contrib.fRxContrib[1] = rot1Eqcoeff*cosTheta*sinPhi - cosPhi* ((-cosV0*sinRx-cosRx*sinV0*sinRy)*dY + (cosV0*cosRx-sinRx*sinV0*sinRy)*dZ);

		TReal rot2Eqcoeff = cosRy*dX+sinRx*sinRy*dY-cosRx*sinRy*dZ;
		contrib.fRyContrib[0] = rot2Eqcoeff*sinTheta*sinPhi - cosPhi*(-cosV0*sinRy*dX + sinRx*cosV0*cosRy*dY - cosRx*cosV0*cosRy*dZ);
		contrib.fRyContrib[1] = rot2Eqcoeff*cosTheta*sinPhi - cosPhi*(sinV0*sinRy*dX - sinRx*sinV0*cosRy*dY + cosRx*sinV0*cosRy*dZ);
	}

	//Misclosure vector
	contrib.fMisclosureVector[0] = k*(sinTheta*sinPhi) - l1AMat.dot(deltaStTg);
	contrib.fMisclosureVector[1] = k*(cosTheta*sinPhi) - l2AMat.dot(deltaStTg);

	
	//Variance calcualtion
	TReal varianceInstrCenter = pow2q(station.instrument.sigmaInstrCentering);
	TReal varianceTgCenter = pow2q(dir3D.target.sigmaTargetCentering);

	TReal varianceInstrHt = pow2q(station.instrument.sigmaInstrHeight);
	TReal varianceTgHt = pow2q(dir3D.target.sigmaTargetHt);

	// ANGL
	TReal varianceANGL = pow2q(dir3D.target.sigmaAngl);
	// ZEND
	TReal varianceZEND = pow2q(dir3D.target.sigmaZenD);

	contrib.fObsVariance[0] = pow2q(k) * 0.5 * (varianceInstrCenter + varianceTgCenter) + pow2q(cosRx*cosRy*sinTheta*sinPhi) * (varianceInstrHt + varianceTgHt) +
							  pow2q(contrib.fThetaContrib[0]) * varianceANGL + pow2q(contrib.fPhiContrib[0]) * varianceZEND; 

	contrib.fObsVariance[1] = pow2q(k) * 0.5 * (varianceInstrCenter + varianceTgCenter) + pow2q(cosRx*cosRy*cosTheta*sinPhi) * (varianceInstrHt + varianceTgHt) + 
							  pow2q(contrib.fThetaContrib[1]) * varianceANGL + pow2q(contrib.fPhiContrib[1]) * varianceZEND; 
	return contrib;
}
#endif
#if 0
PLR3DContrib	TContributionsGenerator::getPolar3DContrib(const TCAM& camera, const TPLR3D& plr3D){
	TPositionVector stationPos = camera.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(plr3D.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); //Transformation from target's LOR to ROOT
	
	tgLor2RootTrafo.transform(targetPos); //Transform target to ROOT

	TReal sDist = plr3D.getScalar();
	TReal cs = plr3D.target.distCorrectionAdjustable->getEstimatedValue().getValue();
	TReal Rx = 0.0; //Rotation around x-axis, default is no rotation
	TReal Ry = 0.0; //Rotation around y-axis, default is no rotation
	TReal v0 = 0.0;	//No V0

	TReal sinTheta = sinq(plr3D.getAngle(kANGL).rad());
	TReal cosTheta = cosq(plr3D.getAngle(kANGL).rad());

	TReal sinPhi = sinq(plr3D.getAngle(kZEND).rad());
	TReal cosPhi = cosq(plr3D.getAngle(kZEND).rad());

	TReal sinV0 = sinq(v0);
	TReal cosV0 = cosq(v0);

	TReal sinRx = sinq(Rx);
	TReal cosRx = cosq(Rx);

	TReal sinRy = sinq(Ry);
	TReal cosRy = cosq(Ry);

	TReal dX = targetPos.getX().getValue() - stationPos.getX().getValue();
	TReal dY = targetPos.getY().getValue() - stationPos.getY().getValue();
	TReal dZ = targetPos.getZ().getValue() + plr3D.target.targetHt - stationPos.getZ().getValue();

	TFreeVector line1AMat( cosV0*cosRy, sinV0*cosRx+sinRx*cosV0*sinRy, sinV0*sinRx-cosRx*cosV0*sinRy, TCoordSysFactory::k3DCartesian); //first line of the A-matrix
	TFreeVector line2AMat( -sinV0*cosRy, cosV0*cosRx-sinRx*sinV0*sinRy, cosV0*sinRx+cosRx*sinV0*sinRy, TCoordSysFactory::k3DCartesian); //second line of the A-matrix
	TFreeVector line3AMat( sinRy, -sinRx*cosRy, cosRx*cosRy, TCoordSysFactory::k3DCartesian); //third line of the A-matrix

	// TSTN station contribution is calculated in a LOR system of the station, i.e. from LOR's it is an identity transformation
	TFreeVector partDerWRespToX0St(1.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToY0St(0.0, 1.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToZ0St(0.0, 0.0, 1.0, TCoordSysFactory::k3DCartesian);

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
/////This part fills target transformations contributions
/////////////////////////////////////////////////////////////////////////////////
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector for target transformations contributions

	const TPositionVector tgPointInLOR = plr3D.targetPos->getEstimatedValue();
	
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
		TFreeVector(-line2AMat.dot(omegaDerivative),-line2AMat.dot(phiDerivative),-line2AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a first equation
		TFreeVector(-line2AMat.dot(t1Derivative),-line2AMat.dot(t2Derivative),-line2AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a first equation
		-line2AMat.dot(scaleDeriv)};


		TransformationContrib thirdEqContrib = {	
		TFreeVector(-line3AMat.dot(omegaDerivative),-line3AMat.dot(phiDerivative),-line3AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a first equation
		TFreeVector(-line3AMat.dot(t1Derivative),-line3AMat.dot(t2Derivative),-line3AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a first equation
		-line3AMat.dot(scaleDeriv)};

		TransformationContrib3D trContrib = {firstEqContrib, secondEqContrib, thirdEqContrib};
		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, trContrib));
	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Contribution
	PLR3DContrib contrib = {coordContribStation, coordContribTarget, targetTransfContributions, 
	{0.0, 0.0, 0.0},//Instrument height contribution
	//V0 contribution for a first, second and third equation
	{0.0,
	 0.0, 
	 0.0},
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

	TFreeVector deltaStTg(dX, dY, dZ,TCoordSysFactory::k3DCartesian);
	//Misclosure vector
	contrib.fMisclosureVector[0] = (sDist+cs)*(sinTheta*sinPhi) - line1AMat.dot(deltaStTg);
	contrib.fMisclosureVector[1] = (sDist+cs)*(cosTheta*sinPhi) - line2AMat.dot(deltaStTg);
	contrib.fMisclosureVector[2] = (sDist+cs)*(cosPhi) - line3AMat.dot(deltaStTg);

	//Variance calcualtion
	TReal varianceInstrCenter = pow2q(camera.instrument.sigmaInstrCentering);
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

	contrib.fObsVariance[2] =  pow2q(plr3D.target.sigmaTargetHt) +  pow2q(camera.instrument.sigmaInstrHeight) + pow2q(contrib.fThetaContrib[2]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[2]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[2]) * varianceDIST;
	return contrib;
}
#endif


//------------------------------------ PLR3D and DIR3D Contrib for CAM------------------------------------------------------
PLR3DContrib	TContributionsGenerator::getPolar3DContrib(const TCAM& camera, const TPLR3D& plr3D){
	fMLAused = false; // TCAM measurements never in MLA
	TPositionVector targetPos = plr3D.targetPos->getEstimatedValue();
	TPositionVector targetPosOrig = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR& tg2stTrafo = getLORTransformation(plr3D.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
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
	addPLR3DTgTransfContributionsCamera(tg2stTrafo,  targetPosOrig, targetTransfContributions);

	TReal sDist = plr3D.getScalar(); //measured distance
	TReal cs = plr3D.target.distCorrectionAdjustable->getEstimatedValue().getValue(); //distance correction
	TReal thetaRad = plr3D.getAngle(kANGL).rad(); //measured theta (horizontal) angle in rads
	TReal phiRad = plr3D.getAngle(kZEND).rad(); //measured phi (vertical) angle in rads

	PLR3DContrib contrib = {coordContribStation, coordContribTarget, targetTransfContributions, 
		{0.0, 0.0, 0.0},	// in CAM there is no instrument height in the equations
		{0.0, 0.0, 0.0},	// in CAM there is no V0 in the equations
		{(sDist+cs)*cosq(thetaRad)*sinq(phiRad),-(sDist+cs)*sinq(thetaRad)*sinq(phiRad), 0.0}, // thetaContrib
		{(sDist+cs)*sinq(thetaRad)*cosq(phiRad),(sDist+cs)*cosq(thetaRad)*cosq(phiRad),-(sDist+cs)*sinq(phiRad)}, //phiContrib
		{sinq(thetaRad)*sinq(phiRad), cosq(thetaRad)*sinq(phiRad), cosq(phiRad)}, // dist contrib and Cs contrib
		{0.0, 0.0, 0.0}, //camera can not rotate freely, no Rx contribution 
		{0.0, 0.0, 0.0}};   //camera can not rotate freely, no Ry contribution

	TReal xSt = camera.instrumentPos->getEstimatedValue().getX().getValue();
	TReal ySt = camera.instrumentPos->getEstimatedValue().getY().getValue();
	TReal zSt = camera.instrumentPos->getEstimatedValue().getZ().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg =	targetPos.getY().getValue();
	TReal zTg = targetPos.getZ().getValue();

	TReal hTg = plr3D.target.targetHt;

	TReal zCoordDist = zTg + hTg - zSt;
//Misclosure vector
	contrib.fMisclosureVector[0] = (sDist+cs)*sinq(thetaRad)*sinq(phiRad) - (xTg - xSt);
	contrib.fMisclosureVector[1] = (sDist+cs)*cosq(thetaRad)*sinq(phiRad) - (yTg - ySt);
	contrib.fMisclosureVector[2] = (sDist+cs)*cosq(phiRad) - zCoordDist;

//Variance calcualtion//////////////////////////////////////
	TReal varianceInstrCenter = pow2q(camera.instrument.sigmaInstrCentering);
	TReal varianceTgCenter = pow2q(plr3D.target.sigmaTargetCentering);

	// ANGL
	TReal varianceANGL = pow2q(plr3D.target.sigmaAngl);
	// ZEND
	TReal varianceZEND = pow2q(plr3D.target.sigmaZenD);
	// DIST
	TReal varianceDIST = pow2q(plr3D.target.sigmaDist);

	contrib.fObsVariance[0] = 0.5*((varianceInstrCenter + varianceTgCenter)) + pow2q(contrib.fThetaContrib[0]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[0]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[0]) * varianceDIST;

	contrib.fObsVariance[1] = 0.5*((varianceInstrCenter + varianceTgCenter)) + pow2q(contrib.fThetaContrib[1]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[1]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[1]) * varianceDIST;

	contrib.fObsVariance[2] = pow2q(plr3D.target.sigmaTargetHt) + pow2q(contrib.fThetaContrib[2]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[2]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[2]) * varianceDIST;
	return contrib;
}


#if 0
//------------------------------------ PLR3D and DIR3D Contrib for CAM------------------------------------------------------
PLR3DContrib	TContributionsGenerator::getPolar3DContrib(const TCAM& camera, const TPLR3D& plr3D){
	fMLAused = false; // TCAM measurements never in MLA
	TPositionVector targetPos = plr3D.targetPos->getEstimatedValue();
	TPositionVector targetPosOrig = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR& tg2stTrafo = getLORTransformation(plr3D.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	tg2stTrafo.transform(targetPos);

	//CAM station's contribution is calculated in a LOR system of the station and, therefore, the station's contribution is this
	Point3DContrib coordContribStation = {TFreeVector(-1.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian),
										TFreeVector(0.0, -1.0, 0.0, TCoordSysFactory::k3DCartesian),
										TFreeVector(0.0, 0.0, -1.0, TCoordSysFactory::k3DCartesian)};

	Point3DContrib coordContribTarget = { TFreeVector(tg2stTrafo.partDerivWRespToX0(0), tg2stTrafo.partDerivWRespToY0(0), tg2stTrafo.partDerivWRespToZ0(0), TCoordSysFactory::k3DCartesian),
										  TFreeVector(tg2stTrafo.partDerivWRespToX0(1), tg2stTrafo.partDerivWRespToY0(1), tg2stTrafo.partDerivWRespToZ0(1), TCoordSysFactory::k3DCartesian),
										  TFreeVector(tg2stTrafo.partDerivWRespToX0(2), tg2stTrafo.partDerivWRespToY0(2), tg2stTrafo.partDerivWRespToZ0(2), TCoordSysFactory::k3DCartesian)};

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector with target's transformations contributions

	//Parametres: transformation from target's LOR into station's LOR, target position, vector of contributions to be filled
	addPLR3DTgTransfContributionsCamera(tg2stTrafo,  targetPosOrig, targetTransfContributions);

	TReal sDist = plr3D.getScalar(); //measured distance
	TReal cs = plr3D.target.distCorrectionAdjustable->getEstimatedValue().getValue(); //distance correction
	TReal thetaRad = plr3D.getAngle(kANGL).rad(); //measured theta (horizontal) angle in rads
	TReal phiRad = plr3D.getAngle(kZEND).rad(); //measured phi (vertical) angle in rads

	PLR3DContrib contrib = {coordContribStation, coordContribTarget, targetTransfContributions, 
		{0.0, 0.0, 0.0},	// in CAM there is no instrument height in the equations
		{0.0, 0.0, 0.0},	// in CAM there is no V0 in the equations
		{-(sDist+cs)*cosq(thetaRad)*sinq(phiRad),(sDist+cs)*sinq(thetaRad)*sinq(phiRad), 0.0}, // thetaContrib
		{-(sDist+cs)*sinq(thetaRad)*cosq(phiRad),-(sDist+cs)*cosq(thetaRad)*cosq(phiRad),(sDist+cs)*sinq(phiRad)}, //phiContrib
		{-sinq(thetaRad)*sinq(phiRad), -cosq(thetaRad)*sinq(phiRad), -cosq(phiRad)}, // dist contrib and Cs contrib
		{0.0, 0.0, 0.0}, //camera can not rotate freely, no Rx contribution 
		{0.0, 0.0, 0.0}};   //camera can not rotate freely, no Ry contribution

	TReal xSt = camera.instrumentPos->getEstimatedValue().getX().getValue();
	TReal ySt = camera.instrumentPos->getEstimatedValue().getY().getValue();
	TReal zSt = camera.instrumentPos->getEstimatedValue().getZ().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg =	targetPos.getY().getValue();
	TReal zTg = targetPos.getZ().getValue();

	TReal hTg = plr3D.target.targetHt;

	TReal zCoordDist = zTg + hTg - zSt;
//Misclosure vector
	contrib.fMisclosureVector[0] = -(sDist+cs)*sinq(thetaRad)*sinq(phiRad) + (xTg - xSt);
	contrib.fMisclosureVector[1] = -(sDist+cs)*cosq(thetaRad)*sinq(phiRad) + (yTg - ySt);
	contrib.fMisclosureVector[2] = -(sDist+cs)*cosq(phiRad) + zCoordDist;

//Variance calcualtion//////////////////////////////////////
	TReal varianceInstrCenter = pow2q(camera.instrument.sigmaInstrCentering);
	TReal varianceTgCenter = pow2q(plr3D.target.sigmaTargetCentering);

	// ANGL
	TReal varianceANGL = pow2q(plr3D.target.sigmaAngl);
	// ZEND
	TReal varianceZEND = pow2q(plr3D.target.sigmaZenD);
	// DIST
	TReal varianceDIST = pow2q(plr3D.target.sigmaDist);

	contrib.fObsVariance[0] = 0.5*((varianceInstrCenter + varianceTgCenter)) + pow2q(contrib.fThetaContrib[0]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[0]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[0]) * varianceDIST;

	contrib.fObsVariance[1] = 0.5*((varianceInstrCenter + varianceTgCenter)) + pow2q(contrib.fThetaContrib[1]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[1]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[1]) * varianceDIST;

	contrib.fObsVariance[2] = pow2q(plr3D.target.sigmaTargetHt) + pow2q(contrib.fThetaContrib[2]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[2]) * varianceZEND + pow2q(contrib.fDistAndCsContrib[2]) * varianceDIST;
	return contrib;
}

#endif

#if 1
//new DIR
DIR3DContrib	TContributionsGenerator::getDIR3DContrib(const TCAM& camera, const TDIR3D& dir3D){
	fMLAused = false; // TCAM measurements never in MLA
	TPositionVector stationPos = camera.instrumentPos->getEstimatedValue();
	TPositionVector targetPos = dir3D.targetPos->getEstimatedValue();
	const TLOR2LOR& tg2stTrafo = getLORTransformation(dir3D.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	tg2stTrafo.transform(targetPos);

	TReal dX = targetPos.getX().getValue() - stationPos.getX().getValue();
	TReal dY = targetPos.getY().getValue() - stationPos.getY().getValue();
	TReal dZ = targetPos.getZ().getValue() + dir3D.target.targetHt - stationPos.getZ().getValue();
	TReal s = sqrtq(powq(dX,2)+powq(dY,2)+powq(dZ,2));

////////////////////////////////////////////////////////////////////////////////
/////STATION COORDINATES PARTIAL DERIVATIVES
//////////////////////////////////////////////////////////////////////////////
	// CAM station contribution is calculated in a LOR system of the camera, i.e. it is an identity transformation
	TFreeVector partDerWRespToX0St(1.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToY0St(0.0, 1.0, 0.0, TCoordSysFactory::k3DCartesian);
	TFreeVector partDerWRespToZ0St(0.0, 0.0, 1.0, TCoordSysFactory::k3DCartesian);

	//Vector to be multiplied by A matrix, i.e. A*pdVectorx0St, A*pdVectory0St and , A*pdVectorz0St
	TFreeVector pdVectorx0St = getVectorForDIR3DStationPartDer(dX,dY,dZ,s,partDerWRespToX0St);
	TFreeVector pdVectory0St = getVectorForDIR3DStationPartDer(dX,dY,dZ,s,partDerWRespToY0St);
	TFreeVector pdVectorz0St = getVectorForDIR3DStationPartDer(dX,dY,dZ,s,partDerWRespToZ0St);

	Point3DContrib coordContribStation = {
		TFreeVector( pdVectorx0St.getX().getValue(), pdVectory0St.getX().getValue(), pdVectorz0St.getX().getValue(), TCoordSysFactory::k3DCartesian),//St contribution for a FIRST equation
		TFreeVector( pdVectorx0St.getY().getValue(), pdVectory0St.getY().getValue(), pdVectorz0St.getY().getValue(), TCoordSysFactory::k3DCartesian),//St contribution for a SECOND equation
		TFreeVector( pdVectorx0St.getZ().getValue(), pdVectory0St.getZ().getValue(), pdVectorz0St.getZ().getValue(), TCoordSysFactory::k3DCartesian),//St contribution for a THIRD equation
	};
////////////////////////////////////////////////////////////////////////////////
/////TARGET COORDINATES PARTIAL DERIVATIVES
//////////////////////////////////////////////////////////////////////////////
	TFreeVector partDerWRespToX0Tg = tg2stTrafo.partDerivWRespToX0();
	TFreeVector partDerWRespToY0Tg = tg2stTrafo.partDerivWRespToY0();
	TFreeVector partDerWRespToZ0Tg = tg2stTrafo.partDerivWRespToZ0();

	//Vector to be multiplied by A matrix
	TFreeVector pdVectorx0Tg = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s,partDerWRespToX0Tg);
	TFreeVector pdVectory0Tg = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s,partDerWRespToY0Tg);
	TFreeVector pdVectorz0Tg = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s,partDerWRespToZ0Tg);


	Point3DContrib coordContribTarget= {
		TFreeVector( -pdVectorx0Tg.getX().getValue(), -pdVectory0Tg.getX().getValue(), -pdVectorz0Tg.getX().getValue(), TCoordSysFactory::k3DCartesian),//Tg contribution for a FIRST equation
		TFreeVector( -pdVectorx0Tg.getY().getValue(), -pdVectory0Tg.getY().getValue(), -pdVectorz0Tg.getY().getValue(), TCoordSysFactory::k3DCartesian),//Tg contribution for a SECOND equation
		TFreeVector( -pdVectorx0Tg.getZ().getValue(), -pdVectory0Tg.getZ().getValue(), -pdVectorz0Tg.getZ().getValue(), TCoordSysFactory::k3DCartesian),//Tg contribution for a THIRD equation
	};

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector with target's transformations contributions

	//Parametres: transformation from target's LOR into station's LOR, target position, vector of contributions to be filled
	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
	const TPositionVector tgPointInLOR = dir3D.targetPos->getEstimatedValue();
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = tg2stTrafo.getTransformationChain();

	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'targetTransfContributions'
	for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
		std::string transformationName = it->adjTrafo->getName();

		//Contributions for rotations : Omega, Phi and Kappa
		TFreeVector omegaDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, tgPointInLOR, 0);
		TFreeVector phiDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, tgPointInLOR, 1);
		TFreeVector kappaDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, tgPointInLOR, 2);

		//Contributions for translation: X, Y and Z coordinate
		TFreeVector t1Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, tgPointInLOR, 0);
		TFreeVector t2Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, tgPointInLOR, 1); 
		TFreeVector t3Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, tgPointInLOR, 2);

		TFreeVector scaleDeriv = tg2stTrafo.partialDerivativesScale(transformationName, tgPointInLOR);

		if(fMLAused){ //If MLA used, then transform contributions
			transform2MLA(omegaDerivative);
			transform2MLA(phiDerivative);
			transform2MLA(kappaDerivative);

			transform2MLA(t1Derivative);
			transform2MLA(t2Derivative);
			transform2MLA(t3Derivative);

			transform2MLA(scaleDeriv);
		}

		TFreeVector omegaDerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, omegaDerivative);
		TFreeVector phiDerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, phiDerivative);
		TFreeVector kappaDerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, kappaDerivative);
		TFreeVector t1DerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, t1Derivative);
		TFreeVector t2DerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, t2Derivative);
		TFreeVector t3DerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, t3Derivative);
		TFreeVector scaleDerivVector = getVectorForDIR3DTargetPartDer(dX,dY,dZ,s, scaleDeriv);

		TransformationContrib firstEqContrib = {	
			TFreeVector(-omegaDerivVector.getX().getValue(),-phiDerivVector.getX().getValue(),-kappaDerivVector.getX().getValue(), TCoordSysFactory::k3DCartesian), 
			TFreeVector(-t1DerivVector.getX().getValue(), -t2DerivVector.getX().getValue(),-t3DerivVector.getX().getValue(), TCoordSysFactory::k3DCartesian),
			-scaleDerivVector.getX().getValue()};


		TransformationContrib secondEqContrib = {	
			TFreeVector(-omegaDerivVector.getY().getValue(),-phiDerivVector.getY().getValue(),-kappaDerivVector.getY().getValue(), TCoordSysFactory::k3DCartesian), 
			TFreeVector(-t1DerivVector.getY().getValue(), -t2DerivVector.getY().getValue(),-t3DerivVector.getY().getValue(), TCoordSysFactory::k3DCartesian),
			-scaleDerivVector.getY().getValue()};


		TransformationContrib thirdEqContrib = {	
			TFreeVector(-omegaDerivVector.getZ().getValue(),-phiDerivVector.getZ().getValue(),-kappaDerivVector.getZ().getValue(), TCoordSysFactory::k3DCartesian), 
			TFreeVector(-t1DerivVector.getZ().getValue(), -t2DerivVector.getZ().getValue(),-t3DerivVector.getZ().getValue(), TCoordSysFactory::k3DCartesian),
			-scaleDerivVector.getZ().getValue()};

		TransformationContrib3D trContrib = {firstEqContrib, secondEqContrib, thirdEqContrib};
		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, trContrib));
		}

	TReal thetaRad = dir3D.getAngle(kANGL).rad(); //measured theta (horizontal) angle in rads
	TReal phiRad = dir3D.getAngle(kZEND).rad(); //measured phi (vertical) angle in rads

	DIR3DContrib contrib = {coordContribStation, coordContribTarget, targetTransfContributions, 
		{0.0, 0.0, 0.0},	// in CAM there is no instrument height in the equations, just for TSTN
		{0.0, 0.0, 0.0},	// in CAM there is no V0 in the equations, just for TSTN
		{cosq(thetaRad)*sinq(phiRad),-sinq(thetaRad)*sinq(phiRad), 0.0}, // thetaContrib
		{sinq(thetaRad)*cosq(phiRad),cosq(thetaRad)*cosq(phiRad),-sinq(phiRad)}, //phiContrib
		{0.0, 0.0, 0.0}, //camera can not rotate freely, no Rx contribution 
		{0.0, 0.0, 0.0}};   //camera can not rotate freely, no Ry contribution


//Misclosure vector
	contrib.fMisclosureVector[0] = sinq(thetaRad)*sinq(phiRad) - 1/s *dX;
	contrib.fMisclosureVector[1] = cosq(thetaRad)*sinq(phiRad) - 1/s *dY;
	contrib.fMisclosureVector[2] = cosq(phiRad) - 1/s * dZ;

//Variance calcualtion//////////////////////////////////////
	TReal varianceInstrCenter = pow2q(camera.instrument.sigmaInstrCentering);
	TReal varianceTgCenter = pow2q(dir3D.target.sigmaTargetCentering);

	// ANGL
	TReal varianceANGL = pow2q(dir3D.target.sigmaAngl);
	// ZEND
	TReal varianceZEND = pow2q(dir3D.target.sigmaZenD);

	contrib.fObsVariance[0] = 0.5*((varianceInstrCenter + varianceTgCenter)) + pow2q(contrib.fThetaContrib[0]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[0]) * varianceZEND ;

	contrib.fObsVariance[1] = 0.5*((varianceInstrCenter + varianceTgCenter)) + pow2q(contrib.fThetaContrib[1]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[1]) * varianceZEND ;

	contrib.fObsVariance[2] = pow2q(dir3D.target.sigmaTargetHt) + pow2q(contrib.fThetaContrib[2]) * varianceANGL + 
							  pow2q(contrib.fPhiContrib[2]) * varianceZEND ;
	return contrib;	
}
#endif

// old DIR
#if 0
DIR3DContrib	TContributionsGenerator::getDIR3DContrib(const TCAM& camera, const TDIR3D& dir3D){
	fMLAused = false;  // TCAM measurements never in MLA
	const TLOR2LOR& tg2stTrafo = getLORTransformation(dir3D.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); //Trafo from from target's LOR to station's LOR
	TPositionVector targetPos = dir3D.targetPos->getEstimatedValue(); // Target position
	tg2stTrafo.transform(targetPos);	//Transform from target's LOR into station's LOR

	TReal thetaRad = dir3D.getAngle(kANGL).rad(); //measured theta (horizontal) angle in rads
	TReal phiRad = dir3D.getAngle(kZEND).rad(); //measured phi (vertical) angle in rads

	TFreeVector stFirstEqContrib(cosq(phiRad), 0.0, -sinq(thetaRad)*sinq(phiRad), TCoordSysFactory::k3DCartesian); //Partial derivatives wr2 x0,y0,z0 are (1 0 0), (0 1 0) and (0 0 1) respectively

	TFreeVector stSecondEqContrib(0.0, cosq(phiRad), -cosq(thetaRad)*sinq(phiRad), TCoordSysFactory::k3DCartesian);

	TFreeVector tgFirstEqContrib(tg2stTrafo.partDerivWRespToX0(2)*sinq(thetaRad)*sinq(phiRad) - tg2stTrafo.partDerivWRespToX0(0)*cosq(phiRad), 
						  tg2stTrafo.partDerivWRespToY0(2)*sinq(thetaRad)*sinq(phiRad) - tg2stTrafo.partDerivWRespToY0(0)*cosq(phiRad), 
						  tg2stTrafo.partDerivWRespToZ0(2)*sinq(thetaRad)*sinq(phiRad) - tg2stTrafo.partDerivWRespToZ0(0)*cosq(phiRad), TCoordSysFactory::k3DCartesian);

	TFreeVector tgSecondEqContrib(tg2stTrafo.partDerivWRespToX0(2)*cosq(thetaRad)*sinq(phiRad) - tg2stTrafo.partDerivWRespToX0(1)*cosq(phiRad), 
						   tg2stTrafo.partDerivWRespToY0(2)*cosq(thetaRad)*sinq(phiRad) - tg2stTrafo.partDerivWRespToY0(1)*cosq(phiRad), 
						   tg2stTrafo.partDerivWRespToZ0(2)*cosq(thetaRad)*sinq(phiRad) - tg2stTrafo.partDerivWRespToZ0(1)*cosq(phiRad), TCoordSysFactory::k3DCartesian);
										 
//Fill transformation contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>> targetTransfContributions; // Vector with target's transformations contributions
	addDIR3DTgTransfContributions(tg2stTrafo, dir3D.targetPos->getEstimatedValue(), thetaRad, phiRad, targetTransfContributions);

	TReal xTg = targetPos.getX().getValue();
	TReal yTg = targetPos.getY().getValue();
	TReal zTg = targetPos.getZ().getValue();

	TReal xSt = camera.instrumentPos->getEstimatedValue().getX().getValue();
	TReal ySt = camera.instrumentPos->getEstimatedValue().getY().getValue();
	TReal zSt = camera.instrumentPos->getEstimatedValue().getZ().getValue();

	TReal hTg   = dir3D.target.targetHt; //target height

	TReal zCoordDist = zTg + hTg - zSt;

	DIR3DContrib contrib = {stFirstEqContrib, stSecondEqContrib, tgFirstEqContrib, tgSecondEqContrib, targetTransfContributions, 
						   {0.0, 0.0}, //No instrument height for camera 
						   {0.0, 0.0},	// in CAM there is no V0 in the equations
						   {zCoordDist*cosq(thetaRad)*sinq(phiRad), -zCoordDist*sinq(thetaRad)*sinq(phiRad)}, //Theta contibution for first and second equation
				           {zCoordDist*sinq(thetaRad)*cosq(phiRad) + sinq(phiRad)*(xTg - xSt), zCoordDist*cosq(thetaRad)*cosq(phiRad) + sinq(phiRad)*(yTg - ySt)}, //Phi contirbution for first and second equation
						   {0.0, 0.0}, //Camera can not rotate freely
						   {0.0, 0.0}}; //Camera can not rotate freely

//Misclosure vector
	contrib.fMisclosureVector[0] = zCoordDist*sinq(thetaRad)*sinq(phiRad) - ((xTg - xSt)*cosq(phiRad));
	contrib.fMisclosureVector[1] = zCoordDist*cosq(thetaRad)*sinq(phiRad) - ((yTg - ySt)*cosq(phiRad));

//Variance calcualtion//////////////////////////////////////
	TReal varianceInstrCenter = pow2q(camera.instrument.sigmaInstrCentering);
	TReal varianceTgCenter = pow2q(dir3D.target.sigmaTargetCentering);

	TReal varianceTgHt = pow2q(dir3D.target.sigmaTargetHt);

	// ANGL
	TReal varianceANGL = pow2q(dir3D.target.sigmaAngl);
	// ZEND
	TReal varianceZEND = pow2q(dir3D.target.sigmaZenD);

	contrib.fObsVariance[0] = 0.5 * pow2q(phiRad) * (varianceInstrCenter + varianceTgCenter) + pow2q(sinq(thetaRad) * sinq(phiRad)) * varianceTgHt + pow2q(contrib.fThetaContrib[0]) * varianceANGL +
							  pow2q(contrib.fPhiContrib[0]) * varianceZEND; 

	contrib.fObsVariance[1] = 0.5 * pow2q(phiRad) * (varianceInstrCenter + varianceTgCenter) + pow2q(cosq(thetaRad) * sinq(phiRad)) * varianceTgHt + pow2q(contrib.fThetaContrib[1]) * varianceANGL +
							  pow2q(contrib.fPhiContrib[1]) * varianceZEND; 
	return contrib;
}
#endif

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
///////////////////////////////////////////////////////////////////////////
//Contribution for DIR3D with a CAM
void TContributionsGenerator::addDIR3DTgTransfContributions(const TLOR2LOR& lorTrafo, const TPositionVector& targetPos, TReal thetaV0Rad, TReal phiRad, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>>& transfContrib){
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = lorTrafo.getTransformationChain();
	std::string transformationName;

	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
	for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
		std::string transformationName = it->adjTrafo->getName();

		//Contributions for rotations : Omega, Phi and Kappa
		TFreeVector omegaPD = lorTrafo.partialDerivativesAngle(transformationName, targetPos, 0);
		TFreeVector phiPD = lorTrafo.partialDerivativesAngle(transformationName, targetPos, 1);
		TFreeVector kappaPD = lorTrafo.partialDerivativesAngle(transformationName, targetPos, 2);

		//Contributions for translation: X, Y and Z coordinate
		TFreeVector trans1PD = lorTrafo.partialDerivativesTranslation(transformationName, targetPos, 0);
		TFreeVector trans2PD = lorTrafo.partialDerivativesTranslation(transformationName, targetPos, 1); 
		TFreeVector trans3PD = lorTrafo.partialDerivativesTranslation(transformationName, targetPos, 2);

		TFreeVector scalePD = lorTrafo.partialDerivativesScale(transformationName, targetPos);

		if(fMLAused){
				transform2MLA(omegaPD);
				transform2MLA(phiPD);
				transform2MLA(kappaPD);

				transform2MLA(trans1PD);
				transform2MLA(trans2PD);
				transform2MLA(trans3PD);

				transform2MLA(scalePD);
		}

		TransformationContrib firstEqContrib = {
			TFreeVector(omegaPD.getZ().getValue()*sinq(thetaV0Rad)*sinq(phiRad) - omegaPD.getX().getValue()*cosq(phiRad),
						phiPD.getZ().getValue()*sinq(thetaV0Rad)*sinq(phiRad) - phiPD.getX().getValue()*cosq(phiRad),					
						kappaPD.getZ().getValue()*sinq(thetaV0Rad)*sinq(phiRad) - kappaPD.getX().getValue()*cosq(phiRad),TCoordSysFactory::k3DCartesian),
			TFreeVector(trans1PD.getZ().getValue()*sinq(thetaV0Rad)*sinq(phiRad) - trans1PD.getX().getValue()*cosq(phiRad),
						trans2PD.getZ().getValue()*sinq(thetaV0Rad)*sinq(phiRad) - trans2PD.getX().getValue()*cosq(phiRad),					
						trans3PD.getZ().getValue()*sinq(thetaV0Rad)*sinq(phiRad) - trans3PD.getX().getValue()*cosq(phiRad),TCoordSysFactory::k3DCartesian),
			scalePD.getZ().getValue()*sinq(thetaV0Rad)*sinq(phiRad) - scalePD.getX().getValue()*cosq(phiRad)
		};

		TransformationContrib secondEqContrib = {
			TFreeVector (omegaPD.getZ().getValue()*cosq(thetaV0Rad)*sinq(phiRad) - omegaPD.getY().getValue()*cosq(phiRad),
							phiPD.getZ().getValue()*cosq(thetaV0Rad)*sinq(phiRad) - phiPD.getY().getValue()*cosq(phiRad),					
							kappaPD.getZ().getValue()*cosq(thetaV0Rad)*sinq(phiRad) - kappaPD.getY().getValue()*cosq(phiRad),TCoordSysFactory::k3DCartesian),



			TFreeVector (trans1PD.getZ().getValue()*cosq(thetaV0Rad)*sinq(phiRad) - trans1PD.getY().getValue()*cosq(phiRad),
							trans2PD.getZ().getValue()*cosq(thetaV0Rad)*sinq(phiRad) - trans2PD.getY().getValue()*cosq(phiRad),					
							trans3PD.getZ().getValue()*cosq(thetaV0Rad)*sinq(phiRad) - trans3PD.getY().getValue()*cosq(phiRad),TCoordSysFactory::k3DCartesian),
			scalePD.getZ().getValue()*cosq(thetaV0Rad)*sinq(phiRad) - scalePD.getY().getValue()*cosq(phiRad)
		};

		TransformationContrib2D trafoContrib = {firstEqContrib, secondEqContrib};

		transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib2D> (*it->adjTrafo, trafoContrib));
	}	
}
#if 0
//Contribution for DIR3D with a TSTN
void TContributionsGenerator::addDIR3DTgROT3DTransfContributions(const TLOR2LOR& lorTrafo, const TPositionVector& targetPos, TReal thetaV0Rad, TReal phiRad, TReal rXRad, TReal rYRad, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>>& transfContrib){
	std::vector<TAdjustableHelmertTransformation> trafoChain = lorTrafo.getTransformationChain();
	std::string transformationName;
	
	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
	for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
		transformationName = it->getName();

		//Contributions for rotations : Omega, Phi and Kappa
		TFreeVector omegaPD = lorTrafo.partialDerivativesAngle(transformationName, targetPos, 0);
		TFreeVector phiPD = lorTrafo.partialDerivativesAngle(transformationName, targetPos, 1);
		TFreeVector kappaPD = lorTrafo.partialDerivativesAngle(transformationName, targetPos, 2);

		//Contributions for translation: X, Y and Z coordinate
		TFreeVector trans1PD = lorTrafo.partialDerivativesTranslation(transformationName, targetPos, 0);
		TFreeVector trans2PD = lorTrafo.partialDerivativesTranslation(transformationName, targetPos, 1); 
		TFreeVector trans3PD = lorTrafo.partialDerivativesTranslation(transformationName, targetPos, 2);

		TFreeVector scalePD = lorTrafo.partialDerivativesScale(transformationName, targetPos);

		if(fMLAused){
				transform2MLA(omegaPD);
				transform2MLA(phiPD);
				transform2MLA(kappaPD);

				transform2MLA(trans1PD);
				transform2MLA(trans2PD);
				transform2MLA(trans3PD);

				transform2MLA(scalePD);
		}

		TReal sinPhi = sinq(phiRad);
		TReal cosPhi = cosq(phiRad);

		TReal sinTheta = sinq(thetaV0Rad);
		TReal cosTheta = cosq(thetaV0Rad);

		TReal sinRx = sinq(rXRad);
		TReal cosRx = cosq(rXRad);

		TReal sinRy = sinq(rYRad);
		TReal cosRy = cosq(rYRad);

		TReal coeffK = -sinTheta*sinPhi*cosRx*sinRy + cosTheta*sinPhi*sinRx + cosPhi*cosRx*cosRy;

		TReal firstEqCoeff = sinTheta*sinPhi*cosRy + cosPhi*sinRy;
		TReal secondEqCoeff = sinTheta*sinPhi*sinRx*sinRy + cosTheta*sinPhi*cosRx - cosPhi*sinRx*cosRy;

		TFreeVector fRotationContribFirstEq(omegaPD.getZ().getValue()*firstEqCoeff - coeffK*omegaPD.getX().getValue(), 
											phiPD.getZ().getValue()*firstEqCoeff - coeffK*phiPD.getX().getValue(), 
											kappaPD.getZ().getValue()*firstEqCoeff - coeffK*kappaPD.getX().getValue(), TCoordSysFactory::k3DCartesian);

		TFreeVector fRotationContribSecondEq(omegaPD.getZ().getValue()*secondEqCoeff - coeffK*omegaPD.getY().getValue(), 
											 phiPD.getZ().getValue()*secondEqCoeff - coeffK*phiPD.getY().getValue(), 
											 kappaPD.getZ().getValue()*secondEqCoeff - coeffK*kappaPD.getY().getValue(), TCoordSysFactory::k3DCartesian);


		TFreeVector fTranslationContribFirstEq(trans1PD.getZ().getValue()*firstEqCoeff - coeffK*trans1PD.getX().getValue(), 
											   trans2PD.getZ().getValue()*firstEqCoeff - coeffK*trans2PD.getX().getValue(), 
											   trans3PD.getZ().getValue()*firstEqCoeff - coeffK*trans3PD.getX().getValue(), TCoordSysFactory::k3DCartesian);

		TFreeVector fTranslationContribSecondEq(trans1PD.getZ().getValue()*secondEqCoeff - coeffK*trans1PD.getY().getValue(), 
											    trans2PD.getZ().getValue()*secondEqCoeff - coeffK*trans2PD.getY().getValue(), 
											    trans3PD.getZ().getValue()*secondEqCoeff - coeffK*trans3PD.getY().getValue(), TCoordSysFactory::k3DCartesian);

		TransformationContrib2D trafoContrib = {fRotationContribFirstEq, fRotationContribSecondEq, fTranslationContribFirstEq, fTranslationContribSecondEq,
		{scalePD.getZ().getValue()*firstEqCoeff - coeffK*scalePD.getX().getValue(),   //Scale contribution for a first equation
		 scalePD.getZ().getValue()*secondEqCoeff - coeffK*scalePD.getY().getValue()} //Scale contribution for a second equation
		};

		transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib2D> (*it, trafoContrib));
	}	
}
#endif
TFreeVector TContributionsGenerator::getPointContributions(const TLOR2LOR& lorTrafo, TReal a, TReal b, TReal c){
	TFreeVector derX0 = lorTrafo.partDerivWRespToX0();
	TFreeVector derY0 = lorTrafo.partDerivWRespToY0();
	TFreeVector derZ0 = lorTrafo.partDerivWRespToZ0();

	if(fMLAused){ //Transform partial derivatives into ILA if necessary
		transform2MLA(derX0);
		transform2MLA(derY0);
		transform2MLA(derZ0);
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

//new for DIR
#if 0
void TContributionsGenerator::addPLR3DTgTransfContributionsCamera(const TLOR2LOR& transformations,const TPositionVector& stationPos, const TPositionVector& pointPos, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib){
		const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = transformations.getTransformationChain();
		std::string transformationName;
		TFreeVector scaleDeriv(TCoordSysFactory::k3DCartesian);

		TReal dX = pointPos.getX().getValue() - stationPos.getX().getValue();
		TReal dY = pointPos.getY().getValue() - stationPos.getY().getValue();
		TReal dZ = pointPos.getZ().getValue() + dir3D.target.targetHt - stationPos.getZ().getValue();
		TReal s = sqrt(powq(dX,2)+powq(dY,2)+powq(dZ,2));

		TReal coeffX = 1/s*(1 - powq(dX,2)/powq(s,2));
		TReal coeffY = 1/s*(1 - powq(dY,2)/powq(s,2));
		TReal coeffZ = 1/s*(1 - powq(dZ,2)/powq(s,2));
		
		// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
		for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
			std::string transformationName = it->adjTrafo->getName();

			//Contributions for rotations : Omega, Phi and Kappa
			TFreeVector omegaDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 0);
			TFreeVector phiDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 1);
			TFreeVector kappaDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 2);

			//Contributions for translation: X, Y and Z coordinate
			TFreeVector t1Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 0);
			TFreeVector t2Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 1); 
			TFreeVector t3Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 2);

			scaleDeriv = transformations.partialDerivativesScale(transformationName, pointPos);


			TransformationContrib firstEqContrib = {TFreeVector(-coeffX * omegaDerivative.getX().getValue(), -coeffX *  phiDerivative.getX().getValue(), -coeffX *  kappaDerivative.getX().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- coeffX * t1Derivative.getX().getValue(), -coeffX *  t2Derivative.getX().getValue(), -coeffX *  t3Derivative.getX().getValue(),TCoordSysFactory::k3DCartesian),
													- coeffX * scaleDeriv.getX().getValue()};

			TransformationContrib secondEqContrib = {TFreeVector(- coeffY * omegaDerivative.getY().getValue(), - coeffY * phiDerivative.getY().getValue(), -coeffY *  kappaDerivative.getY().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- coeffY * t1Derivative.getY().getValue(), - coeffY * t2Derivative.getY().getValue(), -coeffY *  t3Derivative.getY().getValue(),TCoordSysFactory::k3DCartesian),
													-coeffY *  scaleDeriv.getY().getValue()};

			TransformationContrib thirdEqContrib = {TFreeVector(- coeffZ * omegaDerivative.getZ().getValue(), - coeffZ * phiDerivative.getZ().getValue(), -coeffZ *  kappaDerivative.getZ().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(-coeffZ *  t1Derivative.getZ().getValue(), - coeffZ * t2Derivative.getZ().getValue(), -coeffZ *  t3Derivative.getZ().getValue(),TCoordSysFactory::k3DCartesian),
													- coeffZ * scaleDeriv.getZ().getValue()};

			TransformationContrib3D trContrib = {firstEqContrib, secondEqContrib, thirdEqContrib};

			transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, trContrib));
	}
}
#endif
//old for TSTN
#if 1
void TContributionsGenerator::addPLR3DTgTransfContributionsCamera(const TLOR2LOR& transformations, const TPositionVector& pointPos, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib){
		const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = transformations.getTransformationChain();
		std::string transformationName;
		TFreeVector scaleDeriv(TCoordSysFactory::k3DCartesian);

		
		// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
		for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
			std::string transformationName = it->adjTrafo->getName();

			//Contributions for rotations : Omega, Phi and Kappa
			TFreeVector omegaDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 0);
			TFreeVector phiDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 1);
			TFreeVector kappaDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 2);

			//Contributions for translation: X, Y and Z coordinate
			TFreeVector t1Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 0);
			TFreeVector t2Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 1); 
			TFreeVector t3Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 2);

			scaleDeriv = transformations.partialDerivativesScale(transformationName, pointPos);


			TransformationContrib firstEqContrib = {TFreeVector(-omegaDerivative.getX().getValue(), - phiDerivative.getX().getValue(), - kappaDerivative.getX().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- t1Derivative.getX().getValue(), - t2Derivative.getX().getValue(), - t3Derivative.getX().getValue(),TCoordSysFactory::k3DCartesian),
													- scaleDeriv.getX().getValue()};

			TransformationContrib secondEqContrib = {TFreeVector(- omegaDerivative.getY().getValue(), - phiDerivative.getY().getValue(), - kappaDerivative.getY().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- t1Derivative.getY().getValue(), - t2Derivative.getY().getValue(), - t3Derivative.getY().getValue(),TCoordSysFactory::k3DCartesian),
													- scaleDeriv.getY().getValue()};

			TransformationContrib thirdEqContrib = {TFreeVector(- omegaDerivative.getZ().getValue(), - phiDerivative.getZ().getValue(), - kappaDerivative.getZ().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- t1Derivative.getZ().getValue(), - t2Derivative.getZ().getValue(), - t3Derivative.getZ().getValue(),TCoordSysFactory::k3DCartesian),
													- scaleDeriv.getZ().getValue()};

/*
			TransformationContrib firstEqContrib = {TFreeVector(omegaDerivative.getX().getValue(),  phiDerivative.getX().getValue(),  kappaDerivative.getX().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector( t1Derivative.getX().getValue(),  t2Derivative.getX().getValue(),  t3Derivative.getX().getValue(),TCoordSysFactory::k3DCartesian),
													scaleDeriv.getX().getValue()};

			TransformationContrib secondEqContrib = {TFreeVector( omegaDerivative.getY().getValue(),  phiDerivative.getY().getValue(),  kappaDerivative.getY().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector( t1Derivative.getY().getValue(),  t2Derivative.getY().getValue(),  t3Derivative.getY().getValue(),TCoordSysFactory::k3DCartesian),
													 scaleDeriv.getY().getValue()};

			TransformationContrib thirdEqContrib = {TFreeVector( omegaDerivative.getZ().getValue(),  phiDerivative.getZ().getValue(),  kappaDerivative.getZ().getValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector( t1Derivative.getZ().getValue(),  t2Derivative.getZ().getValue(),  t3Derivative.getZ().getValue(),TCoordSysFactory::k3DCartesian),
													 scaleDeriv.getZ().getValue()};
*/
			TransformationContrib3D trContrib = {firstEqContrib, secondEqContrib, thirdEqContrib};

			transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, trContrib));
	}
}
#endif

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
	for(auto itLOR2LOR(fLORTrafo.begin()); itLOR2LOR != fLORTrafo.end(); ++itLOR2LOR){
		if(itLOR2LOR->getName() == transfName)
			return itLOR2LOR - fLORTrafo.begin();
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
	return fLORTrafo.at(trIndex);
}