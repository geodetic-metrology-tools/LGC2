#include<Eigen/Dense>

#include "TContributionsGenerator.h"
#include "TDist.h" 
#include "TTreeEntry.h"
#include "TXYH2CCS.h"
#include "GeodeticConstants.h"
#include "TCAM.h"
#include "TAdjustablePoint.h"


//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////
TContributionsGenerator::TContributionsGenerator(const TDataTree* tree, const TLGCRefFrame::ERefs& refFrame): fTree(tree), fRefFrame(refFrame),
fMLAused(false),fCGRFused(false), fIsSphere(false)
{
	fLastStationPtName = ""; //No point can have empty name

	if(refFrame == TLGCRefFrame::ERefs::kSPHE){
		fIsSphere = true;
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
void TContributionsGenerator::updateTransformations(){
	for(std::list<TLOR2LOR>::iterator it = fLORTrafo.begin(); it != fLORTrafo.end(); ++it){
		it->updateTree();
	}
	fLastStationPtName = "";
	fMLAused = false;
}

void TContributionsGenerator::transformPointsToMLASystem(std::string originName, TPositionVector& originOfMLAPos, TPositionVector& additPointPos){
	if (!(fLastStationPtName == originName) || !fMLAused){
			set2MLATransformation(originOfMLAPos);
			fLastStationPtName = originName;
		}
		transform2MLA(additPointPos);
		originOfMLAPos.setX(0.0);
		originOfMLAPos.setY(0.0);
		originOfMLAPos.setZ(0.0);
}
//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- TSTN measurements
//////////////////////////////////////////////////////////////////////

/*
	All the TSTN contributions are calculated either in ROOT node of the TREE of local frames or in the modified local astronomical
	system of the instrument (station).

	In all cases the STATION and TARGET points can be defined anywhere in the TREE of local frames.
*/

//Spatial distance contributions
DistMeasContrib	TContributionsGenerator::getSpatialDistanceContrib(const TTSTN& station, const TLINE& dist){
//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dist.targetPos->getFrameTreePosition(), fTree->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

// Prepare coefficients (a,b,c) for the points and the transformations contributions
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

	TReal a,b,c;   //station's contributions coefficients (negative values of these give coefficients of the TARGET)		 
	a = (xSt - xTg)/D;  // xSt coefficient
	b = (ySt - yTg)/D;  //ySt coefficient
	c = (zSt + hInst - zTg - hTg)/D; //zSt coefficient

//Calculate and return the contributions
	TReal calcMeas = D - cst; // Calculated measurement value to be returned
	TReal hiContrib = c; // Instrument height contribution to be returned

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c); // Contribution to a STATION point
	// Station transformation contribution
	addTransformationsContributions(stLor2RootTrafo, station.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c); // Contribution to a TARGET point
	// Target transformation contribution
	addTransformationsContributions(tgLor2RootTrafo, dist.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	TReal distCorrContrib =  - 1.0;

	// Variance calculation
	TReal varM = pow2q(dist.target.sigmaDist + dist.getDistance()/1000 * dist.target.ppmDist);
	TReal varInstHeight = pow2q(station.instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(dist.target.sigmaTargetHt);
	TReal varInstCent = pow2q(station.instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(dist.target.sigmaTargetCentering);
	TReal variance = varM + pow2q((zTg - zSt + hTg - hInst)/D) * (varInstHeight +  varTgHeight) + ((pow2q(yTg - ySt) + pow2q(xSt - xTg))/pow2q(D)) * (varInstCent + varTgCent);

	DistMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, distCorrContrib, variance};
	return contrib;
}

// Horizontal angle contributions
AnglMeasContrib	TContributionsGenerator::getHorAnglContrib(const TTSTN& station, const TTSTN::TROM& rom, const TANGL& angl){
//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = angl.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(angl.targetPos->getFrameTreePosition(), fTree->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal xSt = stationPos.getX().getValue();
	TReal ySt = stationPos.getY().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg = targetPos.getY().getValue();

	//Calculated measurement value
	LGC::TAngle calcMeas = LGC::TAngle::atan2((xTg - xSt),(yTg - ySt)) - rom.v0->getEstimatedValue() - rom.acst;  //ACST is the constant orientation of the instrument

	TReal dist2 = pow2q(dist(xSt, ySt, xTg, yTg));
	if (dist2 < nullLimit)
		throw std::logic_error("TContributionGenerator::getHorAnglContrib: Division by zero because observation points have identical coordinates.");

	TReal a,b,c; //station's contributions coefficients (negative values of these give the target coefficients)		
	a = (-LITERAL(1.0) * (yTg - ySt))/dist2; //xSt coefficient
	b = (xTg - xSt)/dist2; //ySt coefficient
	c = 0.0; //zSt coefficient

	TReal v0Contrib = -1.0; //contribution for the V0 parameter
	TReal hiContrib = 0.0; // no contribution for the instrument height

	//Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, station.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, angl.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	// Variance calculation
	TReal variance = pow2q(angl.target.sigmaAngl) + (1.0/pow2q(dist2)) * (pow2q(station.instrument.sigmaInstrCentering) + pow2q(angl.target.sigmaTargetCentering));

	AnglMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

//Zenith distance (vertical angle) contributions
AnglMeasContrib	TContributionsGenerator::getZenDistContrib(const TTSTN& station, const TZEND& zend){
//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = zend.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(zend.targetPos->getFrameTreePosition(), fTree->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

// Prepare coefficients (a,b,c) for the points and the transformations contributions
	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = stationPos.getX().getValue();
	TReal ySt = stationPos.getY().getValue();
	TReal zSt = stationPos.getZ().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg = targetPos.getY().getValue();
	TReal zTg = targetPos.getZ().getValue();

	TReal hTg = zend.target.targetHt;
	TReal hInst = station.instrumentHeightAdjustable->getEstimatedValue().getValue();

	TReal dx = xTg-xSt;
	TReal dy = yTg-ySt;
	TReal dz = zTg-zSt-hInst+hTg;

	TReal distance3D = dist3D(xSt, ySt, zSt+hInst, xTg, yTg, zTg+hTg);
	if (distance3D < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (distance3D).");

	LGC::TAngle calcMeas = LGC::TAngle::acos((zTg - zSt - hInst + hTg)/distance3D);
	
	//We are taking the currently calculated value not the measured one (zend.getAngle().rad()), do not know what is better to take
	TReal sinPhi = sinq(calcMeas.rad());

	if (sinPhi < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (sinV).");

	TReal a,b,c; //station's contributions coefficients (negative values of these give target's coefficients)	
	a = (- 1.0 * dz * dx) / (powq(distance3D,3) * sinPhi);//xSt coefficient
	b = (- 1.0 * dz * dy) / (powq(distance3D,3) * sinPhi);//ySt coefficient
	c = (1.0 / (distance3D * sinPhi)) - powq(dz,2)/(powq(distance3D,3) * sinPhi);//zSt coefficient

	TReal hiContrib = c; // instrument height contribution
	TReal v0Contrib = 0.0; // no contribution for the v0 parameter

//Calculate and return the contributions
	//Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, station.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, zend.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	TReal variance = pow2q(zend.target.sigmaZenD) + (((pow2q(dx) + pow2q(dy))*pow2q(dz))/(powq(distance3D,6)*pow2q(sinPhi))) * 
					(pow2q(station.instrument.sigmaInstrCentering) + pow2q(zend.target.sigmaTargetCentering)) +
					 pow2q(-c) * (pow2q(station.instrument.sigmaInstrHeight) + pow2q(zend.target.sigmaTargetHt));

	AnglMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

//PLR3D Contribution
PLR3DContrib	TContributionsGenerator::getPolar3DContrib(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D){
//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(plr3D.targetPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal Rx = 0.0; //Rotation around x-axis, default is no rotation
	TReal Ry = 0.0; //Rotation around y-axis, default is no rotation

	if(station.rot3D){ //If station can rotate freely get the rotation values
		if(station.rotX == nullptr || station.rotY == nullptr)
			throw std::runtime_error("TContributionGenerator::getPolar3DContrib station can rotate freely, but rotation angles are NULL.");
		Rx = station.rotX->getEstimatedValue().rad();
		Ry = station.rotY->getEstimatedValue().rad();
	}

	TReal sinV0 = (rom.v0->getEstimatedValue() - rom.acst).sin();
	TReal cosV0 = (rom.v0->getEstimatedValue() - rom.acst).cos();

	TReal sinRx = sinq(Rx);
	TReal cosRx = cosq(Rx);

	TReal sinRy = sinq(Ry);
	TReal cosRy = cosq(Ry);

	TFreeVector line1AMat( cosV0*cosRy, -sinV0*cosRx+sinRx*cosV0*sinRy, sinV0*sinRx+cosRx*cosV0*sinRy, TCoordSysFactory::k3DCartesian); //first line of the A-matrix
	TFreeVector line2AMat( sinV0*cosRy, cosV0*cosRx+sinRx*sinV0*sinRy, -cosV0*sinRx+cosRx*sinV0*sinRy, TCoordSysFactory::k3DCartesian); //second line of the A-matrix
	TFreeVector line3AMat( -sinRy, sinRx*cosRy, cosRx*cosRy, TCoordSysFactory::k3DCartesian); //third line of the A-matrix

	// Contributions for the station coordinates
	TFreeVector zeroVect(0,0,0,TCoordSysFactory::k3DCartesian);
	Point3DContrib coordContribStation = {zeroVect, zeroVect, zeroVect};
	addPointContributionsPLR3D(stLor2RootTrafo, line1AMat,  line2AMat,  line3AMat, coordContribStation, true);

	// Contributions for the station coordinates
	Point3DContrib coordContribTarget = {zeroVect, zeroVect, zeroVect};
	addPointContributionsPLR3D(tgLor2RootTrafo, line1AMat,  line2AMat,  line3AMat, coordContribTarget, false);

	//Fill station transformation contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> stationTransfContributions; // Vector for target transformations contributions
	const TPositionVector& stPointInLOR = station.instrumentPos->getEstimatedValue();
	addTransformationsContributions3D(stLor2RootTrafo, stPointInLOR, line1AMat,  line2AMat,  line3AMat, stationTransfContributions);

	//Fill target transformation contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector for target transformations contributions
	const TPositionVector& tgPointInLOR = plr3D.targetPos->getEstimatedValue();
	addTransformationsContributions3D(tgLor2RootTrafo, tgPointInLOR, line1AMat,  line2AMat,  line3AMat, targetTransfContributions);


	TReal sDistPlusCs = plr3D.getDistance() + plr3D.target.distCorrectionAdjustable->getEstimatedValue().getValue();
	TReal sinTheta = plr3D.getAngle(kANGL).sin();
	TReal cosTheta = plr3D.getAngle(kANGL).cos();

	TReal sinPhi = plr3D.getAngle(kZEND).sin();
	TReal cosPhi = plr3D.getAngle(kZEND).cos();

	TReal dX = targetPos.getX().getValue() - stationPos.getX().getValue();
	TReal dY = targetPos.getY().getValue() - stationPos.getY().getValue();
	TReal dZ = targetPos.getZ().getValue() + plr3D.target.targetHt - stationPos.getZ().getValue() -  station.instrumentHeightAdjustable->getEstimatedValue().getValue();

	TReal dist2 = pow2q(dist(stationPos.getX().getValue(), stationPos.getY().getValue(), targetPos.getX().getValue(), targetPos.getY().getValue()));
	TReal distance3D = sqrt(pow2q(dX) + pow2q(dY)+pow2q(dZ));
	TReal c = (1.0 / (distance3D * sinPhi)) - powq(dZ,2)/(powq(distance3D,3) * sinPhi);

	//Contribution to be returned
	PLR3DContrib contrib = {coordContribStation, coordContribTarget, stationTransfContributions,  targetTransfContributions, 
	{line1AMat.getZ().getValue(), line2AMat.getZ().getValue(), line3AMat.getZ().getValue()},//Instrument height contribution
	//V0 contribution for a first, second and third equation
	{-(-sinV0*cosRy*dX + (-cosV0*cosRx-sinV0*sinRx*sinRy)*dY + (cosV0*sinRx-sinV0*cosRx*sinRy)*dZ),
	 -(cosV0*cosRy*dX + (-sinV0*cosRx+cosV0*sinRx*sinRy)*dY + (sinV0*sinRx+cosV0*cosRx*sinRy)*dZ), 
	  0},
	//Theta contribution
	{(sDistPlusCs)*cosTheta*sinPhi,
	 -(sDistPlusCs)*sinTheta*sinPhi, 
	 0},
	//Phi contribution
	{(sDistPlusCs)*sinTheta*cosPhi,
	 (sDistPlusCs)*cosTheta*cosPhi, 
	 -(sDistPlusCs)*sinPhi},  
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

	const TFreeVector deltaStTg(dX, dY, dZ,TCoordSysFactory::k3DCartesian);
	//Misclosure vector
	contrib.fMisclosureVector[0] = (sDistPlusCs)*(sinTheta*sinPhi) - line1AMat.dot(deltaStTg);
	contrib.fMisclosureVector[1] = (sDistPlusCs)*(cosTheta*sinPhi) - line2AMat.dot(deltaStTg);
	contrib.fMisclosureVector[2] = (sDistPlusCs)*(cosPhi) - line3AMat.dot(deltaStTg);

	//Variance calcualtion
	// ANGL
	contrib.fObsVariance[0] = pow2q(plr3D.target.sigmaAngl) + (1.0/pow2q(dist2)) * (pow2q(station.instrument.sigmaInstrCentering) + pow2q(plr3D.target.sigmaTargetCentering));
	// ZEND
	contrib.fObsVariance[1] = pow2q(plr3D.target.sigmaZenD) + (((pow2q(dX) + pow2q(dY))*pow2q(dZ))/(powq(distance3D,6)*pow2q(sinPhi))) * 
					(pow2q(station.instrument.sigmaInstrCentering) + pow2q(plr3D.target.sigmaTargetCentering)) +
					 pow2q(-c) * (pow2q(station.instrument.sigmaInstrHeight) + pow2q(plr3D.target.sigmaTargetHt));
	// DIST
	TReal varM = pow2q(plr3D.target.sigmaDist + plr3D.getDistance()/1000 *plr3D.target.ppmDist);
	TReal varInstHeight = pow2q(station.instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(plr3D.target.sigmaTargetHt);
	TReal varInstCent = pow2q(station.instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(plr3D.target.sigmaTargetCentering);
	contrib.fObsVariance[2] = varM + pow2q((dZ)/distance3D) * (varInstHeight +  varTgHeight) + ((pow2q(dY) + pow2q(dX))/pow2q(distance3D)) * (varInstCent + varTgCent);

	return contrib;
}

//Horizontal distance contributions, measurement made by TSTN
HorDistContrib	TContributionsGenerator::getHorDistContrib(const TTSTN& station, const TLINE& dhor){
	TReal calcMeas;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = dhor.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dhor.targetPos->getFrameTreePosition(), fTree->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
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
	TReal varM = pow2q(dhor.target.sigmaDist + dhor.getDistance()/1000*dhor.target.ppmDist);
	TReal variance = varM + (pow2q(station.instrument.sigmaInstrCentering) + pow2q(dhor.target.sigmaTargetCentering));

	HorDistContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, distCorrCont, variance};
	return contrib;
}

//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- individual measurements
//////////////////////////////////////////////////////////////////////
// Spatial distance made by an EDM
DistMeasContrib	TContributionsGenerator::getDSPTContrib(const TEDM& edmST, const TDSPT& dspt){
	TPositionVector targetPos = dspt.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dspt.targetPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = edmST.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(edmST.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		transformPointsToMLASystem(edmST.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
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
	TReal hInst = edmST.instrument.instrHeight;
	TReal cst = dspt.target.distCorrectionAdjustable->getEstimatedValue().getValue();

	TReal D = dist3D(xSt, ySt, (zSt + hInst), xTg, yTg, (zTg + hTg));

	if (D < nullLimit)
		throw std::logic_error("TContributionGenerator::getSpatialDistanceContrib: Division by zero because observation points have identical coordinates.");

	TReal a,b,c;   //station's contributions coefficients (negative values of these give target's coefficients)		 
	a = (xSt -xTg)/D;  // xSt coefficient
	b = (ySt - yTg)/D;  //ySt coefficient
	c = (zSt + hInst - zTg - hTg)/D;  //zSt coefficient

	TReal calcMeas = D - cst;
	TReal hiContrib = 0.0; //instrument height is always FIXED, no contribution!
	TReal distCorrection =  - 1.0;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	addTransformationsContributions(stLor2RootTrafo, edmST.instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, dspt.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	// Variance calculation
	TReal varM = pow2q(dspt.target.sigmaDSpt + dspt.getDistance()/1000 * dspt.target.ppmDSpt);
	TReal varInstHeight = pow2q(edmST.instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(dspt.target.sigmaTargetHt);
	TReal varInstCent = pow2q(edmST.instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(dspt.target.sigmaTargetCentering);
	TReal variance = varM + pow2q((zTg - zSt + hTg - hInst)/D) * (varInstHeight +  varTgHeight) + ((pow2q(yTg - ySt) + pow2q(xSt - xTg))/pow2q(D)) * (varInstCent + varTgCent);

	DistMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, distCorrection, variance};
	return contrib;
}

//Horizontal distance contribution for a measurement made in DLEV
HorDistContribLEVEL	TContributionsGenerator::getHorDistContrib(const TAdjustablePoint* referencePoint, const TDLEV::TDHOR& dhor){
	TFreeVector staffContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector referencePTContrib(TCoordSysFactory::k3DCartesian);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> staffTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> referencePTTransfContributions;
	TReal calcMeas;

	TPositionVector refPointPos = referencePoint->getEstimatedValue();  // Reference point
	const TLOR2LOR& refPTLor2RootTrafo = getLORTransformation(referencePoint->getFrameTreePosition(), fTree->begin()); 
	refPTLor2RootTrafo.transform(refPointPos);

	TPositionVector staffPos = dhor.targetPos->getEstimatedValue();   // Levelling staff is the 'target'
	const TLOR2LOR& staffPTLor2RootTrafo = getLORTransformation( dhor.targetPos->getFrameTreePosition(), fTree->begin()); 
	refPTLor2RootTrafo.transform(staffPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		transformPointsToMLASystem(dhor.targetPos->getName(), staffPos, refPointPos);
		fMLAused = true;
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

	TReal a,b,c; 	
	a = - (xTg - xSt)/calcMeas; // xRp coefficient
	b = - (yTg - ySt)/calcMeas; //yRp coefficient
	c = 0.0; //zRp coefficient

	//Staff can be defined anywhere, get point contributions and transformations contributions
	staffContrib = getPointContributions(staffPTLor2RootTrafo, a, b, c);
	addTransformationsContributions(staffPTLor2RootTrafo,  dhor.targetPos->getEstimatedValue(), a, b, c, staffTransfContributions);

	//Reference point can be defined anywhere, get point contributions and transformations contributions
	referencePTContrib = getPointContributions(refPTLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(refPTLor2RootTrafo, referencePoint->getEstimatedValue(), -a, -b, -c, referencePTTransfContributions);
	
	HorDistContribLEVEL  contrib = {calcMeas, staffContrib, referencePTContrib, staffTransfContributions, referencePTTransfContributions};

	return contrib;
}


//DLEV contributions
DLEVContrib	TContributionsGenerator::getDLEVContrib(const TLEVEL& levelInstr, const TDLEV& dlev){
	TReal collAngl = levelInstr.instrument.collAngleAdjustable->getEstimatedValue().rad(); //collimination angle in rads
	TReal cdz = dlev.target.distCorrectionValue; //distance correction value
	TReal dRef = levelInstr.fMeasuredPlane->getRefPtDistEstimatedValue().getValue(); //Distance of the reference point from the plane

	TPositionVector referencePoint = levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue();
	const TLOR2LOR& refPTLor2RootTrafo = getLORTransformation(levelInstr.fMeasuredPlane->getReferencePoint()->getFrameTreePosition(), fTree->begin()); 
	refPTLor2RootTrafo.transform(referencePoint);

	TPositionVector staffPosition = dlev.targetPos->getEstimatedValue();  // this Target / Levelling Staff / SCALE assumed to be in ROOT!!!!!
	const TLOR2LOR& staffPTLor2RootTrafo = getLORTransformation(dlev.targetPos->getFrameTreePosition(), fTree->begin()); 
	staffPTLor2RootTrafo.transform(staffPosition);

	// If not OLOC => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		transformPointsToMLASystem( levelInstr.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, staffPosition);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal dTg = sqrtq(pow2q(staffPosition.getX().getValue() - referencePoint.getX().getValue()) + pow2q(staffPosition.getY().getValue() - referencePoint.getY().getValue())); 
	TReal calcMeas = referencePoint.getZ().getValue() - staffPosition.getZ().getValue() + dRef - cdz - dTg*tanq(collAngl);

	//Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector staffContrib = getPointContributions(staffPTLor2RootTrafo, 0, 0, -1);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> staffTransfContributions;
	addTransformationsContributions(staffPTLor2RootTrafo, dlev.targetPos->getEstimatedValue(), 0, 0, -1, staffTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector referencePTContrib = getPointContributions(refPTLor2RootTrafo, 0, 0, 1);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> referencePTTransfContributions;
	addTransformationsContributions(refPTLor2RootTrafo, levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue(), 0, 0, 1, referencePTTransfContributions);

	TReal collAngleContrib = - dTg*(1.0 + powq(tanq(collAngl),2));
	TReal fRefPtDistCont = 1.0;

	TReal variance = pow2q(dlev.target.sigmaD + dTg/1000*dlev.target.ppmD) +  pow2q(dlev.target.sigmaStaffHt);

	DLEVContrib dlevContrib = {calcMeas, staffContrib, referencePTContrib, staffTransfContributions, referencePTTransfContributions, fRefPtDistCont, collAngleContrib, variance};
	return dlevContrib;
}


//ECHO contribution
ECHOContrib	TContributionsGenerator::getECHOContrib(const TECHOROM& echoROM, const TECHO& echo){
	TReal theta = echoROM.fMeasuredPlane->getThetaEstimatedValue().rad(); // Theta angle of the plane
	TReal cEcVp = echo.target.distCorrectionValue; //distance of the target correction value
	TReal dRef = echoROM.fMeasuredPlane->getRefPtDistEstimatedValue().getValue();  // distance of the reference point from the plane

	TPositionVector stationPoint= echo.targetPos->getEstimatedValue();

	const TLOR2LOR& stationPTLor2RootTrafo = getLORTransformation(echo.targetPos->getFrameTreePosition(), fTree->begin()); 
	stationPTLor2RootTrafo.transform(stationPoint);

	/*Reference point is always defined in ROOT and is fixed (it is implicitly defined),
	i.e. no transformation to ROOT required and no contribution required for its coordinates.*/
	TPositionVector referencePoint = echoROM.fMeasuredPlane->getReferencePoint()->getEstimatedValue();

	const std::string& rpName = echoROM.fMeasuredPlane->getReferencePoint()->getName();
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		transformPointsToMLASystem( echoROM.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, stationPoint);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal calcMeas = -cosq(theta)*(stationPoint.getX() - referencePoint.getX() ).getValue() + sinq(theta)*(stationPoint.getY() - referencePoint.getY()).getValue() + dRef - cEcVp;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector stationContrib = getPointContributions(stationPTLor2RootTrafo,  -cosq(theta), sinq(theta), 0.0);
	addTransformationsContributions(stationPTLor2RootTrafo, echo.targetPos->getEstimatedValue(),  -cosq(theta), sinq(theta), 0.0, stationTransfContributions);
	
	TReal thetaContrib = sinq(theta)*(stationPoint.getX() - referencePoint.getX()).getValue() + cosq(theta)*(stationPoint.getY() - referencePoint.getY()).getValue();

	TReal refPtDistContrib = 1.0;
	TReal obsVariance = pow2q(echo.target.sigmaD + echo.getDistance()/1000*echo.target.ppmD) + pow2q(echo.target.sigmaInstrCentering);

	ECHOContrib echoContrib = {calcMeas, stationContrib, thetaContrib, refPtDistContrib, stationTransfContributions, obsVariance};
	return echoContrib;
}

//DVER contributions
DVERContrib	TContributionsGenerator::getDVERContrib(const TDVER& dver){
	fMLAused = false;
	auto k3D = TCoordSysFactory::k3DCartesian;
	/*Contribution if OLOC used, otherwise going to be rewritten*/
		TFreeVector stationC(0,0,-1,k3D);
		TFreeVector targetC(0,0,1,k3D);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

	TPositionVector station(dver.station->getEstimatedValue());
	TPositionVector target(dver.targetPos->getEstimatedValue());
	TPositionVector stationLOR = station;
	TPositionVector targetLOR = target;

	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(dver.station->getFrameTreePosition(), fTree->begin()); //Station's position frame
	stLor2RootTrafo.transform(station); //Transform to ROOT(CCS)

	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dver.targetPos->getFrameTreePosition(), fTree->begin()); //Station's position frame
	tgLor2RootTrafo.transform(target); //Transform to ROOT(CCS)


	TPositionVector stationCCS = station;
	TPositionVector targetCCS = target;

	TReal  dh = 0.0;

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){  /*Needs to be calculated in CGRF.*/
		set2MLATransformation(station);	
		transformMLA2CGRF(stationC); // transform to CGRF

		set2MLATransformation(target);
		transformMLA2CGRF(targetC);  // transform to CGRF

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
		dh = target.getH().getValue() - station.getH().getValue() - dver.getDistanceCorrection();

	}
	else{ /*OLOC = Calculated as XYZ, simple case*/
		fCGRFused = false;
		dh = target.getZ().getValue() - station.getZ().getValue() - dver.getDistanceCorrection();
		
	}
	
	TFreeVector stationContrib = getPointContributions(stLor2RootTrafo,stationC.getX().getValue(),stationC.getY().getValue(),stationC.getZ().getValue());
	addTransformationsContributions(stLor2RootTrafo,stationLOR,0,0,-1,stationTransfContributions);
	
	TFreeVector targetContrib = getPointContributions(tgLor2RootTrafo, targetC.getX().getValue(),targetC.getY().getValue(),targetC.getZ().getValue());
	addTransformationsContributions(tgLor2RootTrafo,targetLOR, 0, 0, 1, targetTransfContributions);

	DVERContrib dverC =  { dh, stationContrib, targetContrib, stationTransfContributions, targetTransfContributions, pow2q( dver.getObservedStDev())};
	
	return dverC;
}

//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- CAMERA measurements (UVEC/UVD)
UVECContrib	TContributionsGenerator::getUVECContrib(const TCAM& camera, const TUVEC& uvec){
	fMLAused = false;  // TCAM measurements are never in MLA

	const TLOR2LOR& tg2stTrafo = getLORTransformation(uvec.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); //Trafo from from target's LOR to station's LOR
	TPositionVector targetPos = uvec.targetPos->getEstimatedValue();
	tg2stTrafo.transform(targetPos);	

	const TFreeVector& unitVec = uvec.getVectorValue(); // observed vector (X and Y), Z is not observed
	TReal i = unitVec.getX().getValue();
	TReal j = unitVec.getY().getValue();
	TReal k = unitVec.getZ().getValue();

	const TPositionVector& instrEstimValue = camera.instrumentPos->getEstimatedValue();
	TReal dx = targetPos.getX().getValue() - instrEstimValue.getX().getValue();
	TReal dy = targetPos.getY().getValue() - instrEstimValue.getY().getValue();
	TReal dz = targetPos.getZ().getValue() - instrEstimValue.getZ().getValue();

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


//Fill transformation contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>> targetTransfContributions; // Vector with target's transformations contributions
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = tg2stTrafo.getTransformationChain();
	
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
//End of filling transformation contributions

	UVECContrib contrib = {stFirstEqContrib, stSecondEqContrib, tgFirstEqContrib, tgSecondEqContrib, targetTransfContributions, 
						   {-1.0, 0.0}, // X contribution (i) for a first and second equation
				           {0.0, -1.0}, // Y contribution (j) for a first and second equation
						   {-i+(k/dz)*dx, -j+(k/dz)*dy}, //Misclosure vector for a first and second equation
						   {pow2q(uvec.target.sigmaX), pow2q(uvec.target.sigmaY)}}; //Obs variances
	return contrib;
}

UVDContrib	TContributionsGenerator::getUVDContrib(const TCAM& camera, const TUVD& uvd){
	fMLAused = false; // TCAM measurements never in MLA
	TPositionVector targetPos = uvd.targetPos->getEstimatedValue();
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
	addUVDTgTransfContributionsCamera(tg2stTrafo,  uvd.targetPos->getEstimatedValue(), targetTransfContributions);

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

////////////////////////////////////////
// Transformations related functions
////////////////////////////////////////
void TContributionsGenerator::transform2MLA(TPositionVector& pv){
	fccs2cgrf.transform(pv);
	fcgrf2ilg.transform(pv);
	filg2ila.transform(pv);
	TTransformation ILA2MILA;
	if (!fIsSphere)
		ILA2MILA.setRotationTransformation(0.0,0.0, -0.5934254894331); //Rotation of -37.77864gons about Z-axis (Azimut of Y axis for the MLA)
	else
		ILA2MILA.setRotationTransformation(0.0,0.0, -0.5934254894331 - 0.00025507972476); //For the sphere a correction of 0.01623887931 gons is apply to the azimut
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

// used only for the dver measurements
void TContributionsGenerator::transformMLA2CGRF(TFreeVector& fv){
	TTransformation ILA2MILA;
	ILA2MILA.setRotationTransformation(0.0,0.0, -0.5934254894331); //Rotation of 37.77864 about Z-axis
	TTransformation otherSense = ILA2MILA.getInversedTransformation();
	otherSense.transform(fv);

	filg2ila.transformInverse(fv);
	fcgrf2ilg.transformInverse(fv);

	/*fccs2cgrf.transformInverse(fv);*/
}


// \returns index of a transformation with given name in the 'fLORTrafo' vector, if vector does not include this transformation, function \returns -1
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
	fcgrf2ilg = TCGRF2LGTransformation(originInCCS, fIsSphere);
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

void TContributionsGenerator::addTransformationsContributions3D(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, const TFreeVector& line1AMat,  const TFreeVector& line2AMat,  const TFreeVector& line3AMat, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib){
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = lorTrafo.getTransformationChain();

	TFreeVector omegaDerivative(TCoordSysFactory::k3DCartesian); 
	TFreeVector phiDerivative(TCoordSysFactory::k3DCartesian); 
	TFreeVector kappaDerivative(TCoordSysFactory::k3DCartesian); 
	TFreeVector t1Derivative(TCoordSysFactory::k3DCartesian); 
	TFreeVector t2Derivative(TCoordSysFactory::k3DCartesian); 
	TFreeVector t3Derivative(TCoordSysFactory::k3DCartesian); 
	TFreeVector scaleDeriv(TCoordSysFactory::k3DCartesian); 
	// Iterate through the transformations, calculate contributions and store the contributiojn for every transformation
	for(auto it(trafoChain.begin()); it != trafoChain.end(); ++it){
		std::string transformationName = it->adjTrafo->getName();

		//Contributions for rotations : Omega, Phi and Kappa
		omegaDerivative = lorTrafo.partialDerivativesAngle(transformationName, pointPos, 0);
		phiDerivative = lorTrafo.partialDerivativesAngle(transformationName, pointPos, 1);
		kappaDerivative = lorTrafo.partialDerivativesAngle(transformationName, pointPos, 2);

		//Contributions for translation: X, Y and Z coordinate
		t1Derivative = lorTrafo.partialDerivativesTranslation(transformationName, pointPos, 0);
		t2Derivative = lorTrafo.partialDerivativesTranslation(transformationName, pointPos, 1); 
		t3Derivative = lorTrafo.partialDerivativesTranslation(transformationName, pointPos, 2);

		scaleDeriv = lorTrafo.partialDerivativesScale(transformationName, pointPos);

		if(fMLAused){ //If MLA used, then transform contributions
			transform2MLA(omegaDerivative);
			transform2MLA(phiDerivative);
			transform2MLA(kappaDerivative);

			transform2MLA(t1Derivative);
			transform2MLA(t2Derivative);
			transform2MLA(t3Derivative);

			transform2MLA(scaleDeriv);
		}

		TransformationContrib firstEqContribSt = {	
		TFreeVector(line1AMat.dot(omegaDerivative),line1AMat.dot(phiDerivative),line1AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian), 
		TFreeVector(line1AMat.dot(t1Derivative),line1AMat.dot(t2Derivative),line1AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), 
		line1AMat.dot(scaleDeriv)};


		TransformationContrib secondEqContribSt = {	
		TFreeVector(line2AMat.dot(omegaDerivative),line2AMat.dot(phiDerivative),line2AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian),
		TFreeVector(line2AMat.dot(t1Derivative),line2AMat.dot(t2Derivative),line2AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), 
		line2AMat.dot(scaleDeriv)};


		TransformationContrib thirdEqContribSt = {	
		TFreeVector(line3AMat.dot(omegaDerivative),line3AMat.dot(phiDerivative),line3AMat.dot(kappaDerivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a third equation
		TFreeVector(line3AMat.dot(t1Derivative),line3AMat.dot(t2Derivative),line3AMat.dot(t3Derivative), TCoordSysFactory::k3DCartesian), //Tg contribution for a third equation
		line3AMat.dot(scaleDeriv)};

		TransformationContrib3D stContrib = {firstEqContribSt, secondEqContribSt, thirdEqContribSt};
		transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, stContrib));
}
}

void TContributionsGenerator::addPointContributionsPLR3D(const TLOR2LOR& lorTrafo, const TFreeVector& line1AMat,  const TFreeVector& line2AMat,  const TFreeVector& line3AMat, Point3DContrib& pointContrib, bool station){
	TFreeVector partDerWRespToX0 = lorTrafo.partDerivWRespToX0();
	TFreeVector partDerWRespToY0 = lorTrafo.partDerivWRespToY0();
	TFreeVector partDerWRespToZ0 = lorTrafo.partDerivWRespToZ0();

	if(fMLAused){
		transform2MLA(partDerWRespToX0);
		transform2MLA(partDerWRespToY0);
		transform2MLA(partDerWRespToZ0);
	}

	if(station){
		pointContrib.firstEqPtContrib = TFreeVector( line1AMat.dot(partDerWRespToX0), line1AMat.dot(partDerWRespToY0), line1AMat.dot(partDerWRespToZ0), TCoordSysFactory::k3DCartesian);
		pointContrib.secondEqPtContrib = TFreeVector( line2AMat.dot(partDerWRespToX0), line2AMat.dot(partDerWRespToY0), line2AMat.dot(partDerWRespToZ0), TCoordSysFactory::k3DCartesian);
		pointContrib.thirdEqPtContrib = TFreeVector( line3AMat.dot(partDerWRespToX0), line3AMat.dot(partDerWRespToY0), line3AMat.dot(partDerWRespToZ0), TCoordSysFactory::k3DCartesian);
	}
	else{	//Target
		pointContrib.firstEqPtContrib = TFreeVector( -line1AMat.dot(partDerWRespToX0), -line1AMat.dot(partDerWRespToY0), -line1AMat.dot(partDerWRespToZ0), TCoordSysFactory::k3DCartesian);
		pointContrib.secondEqPtContrib = TFreeVector( -line2AMat.dot(partDerWRespToX0), -line2AMat.dot(partDerWRespToY0), -line2AMat.dot(partDerWRespToZ0), TCoordSysFactory::k3DCartesian);
		pointContrib.thirdEqPtContrib = TFreeVector( -line3AMat.dot(partDerWRespToX0), -line3AMat.dot(partDerWRespToY0), -line3AMat.dot(partDerWRespToZ0), TCoordSysFactory::k3DCartesian);
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


//////////////////////////////////////////
/// Functions for Calculation Meas
//////////////////////////////////////////
TReal TContributionsGenerator::getANGLCalcMeas(const TTSTN& station, const TTSTN::TROM& rom, const TAdjustablePoint* targetAdjPoint){
	TPositionVector targetPos = targetAdjPoint->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(targetAdjPoint->getFrameTreePosition(), fTree->begin()); // Transform target to ROOT
	tgLor2RootTrafo.transform(targetPos); 

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); // Transform station to ROOT 
	stLor2RootTrafo.transform(stationPos);

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal xSt = stationPos.getX().getValue();
	TReal ySt = stationPos.getY().getValue();

	TReal xTg = targetPos.getX().getValue();
	TReal yTg = targetPos.getY().getValue();

	return (LGC::TAngle::atan2((xTg - xSt),(yTg - ySt)) - rom.v0->getEstimatedValue() - rom.acst).rad(); 
}


TReal TContributionsGenerator::getZENDCalcMeas(const TTSTN& station, const TAdjustablePoint* targetAdjPoint, TReal targetHt){
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	TPositionVector targetPos = targetAdjPoint->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(targetAdjPoint->getFrameTreePosition(), fTree->begin()); 
	tgLor2RootTrafo.transform(targetPos);

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal distance3D = dist3D(stationPos.getX(),  stationPos.getY(),  stationPos.getZ()+station.instrumentHeightAdjustable->getEstimatedValue(), 
					   targetPos.getX(), targetPos.getY(), targetPos.getZ()+targetHt);
	if (distance3D < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (distance3D).");

	return (LGC::TAngle::acos(((targetPos.getZ()+targetHt - stationPos.getZ() - station.instrumentHeightAdjustable->getEstimatedValue())/distance3D).getValue())).rad();
}


TReal TContributionsGenerator::getDISTCalcMeas(const TTSTN& station, const TAdjustablePoint* targetAdjPoint, TReal targetHt, TReal distanceCorr){
	TPositionVector targetPos = targetAdjPoint->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(targetAdjPoint->getFrameTreePosition(), fTree->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

	return (dist3D(stationPos.getX(), stationPos.getY(), (stationPos.getZ() + station.instrumentHeightAdjustable->getEstimatedValue()), 
			targetPos.getX(),  targetPos.getY().getValue(), (targetPos.getZ() + targetHt)) - distanceCorr);
}


TReal TContributionsGenerator::getDHORCalcMeas(const TTSTN& station, const TLINE& dhor){
	TPositionVector stationPos = station.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(station.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	TPositionVector targetPos = dhor.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dhor.targetPos->getFrameTreePosition(), fTree->begin());  //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC && station.rot3D != true){
		transformPointsToMLASystem(station.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal D = dist(stationPos.getX().getValue(), stationPos.getY().getValue(),  targetPos.getX().getValue(), targetPos.getY().getValue());
	TReal cte = dhor.target.distCorrectionAdjustable->getEstimatedValue().getValue();
	return D - cte;
}

TReal	TContributionsGenerator::getECHOCalcMeas(const TECHOROM& echoROM, const TECHO& echo){
	TPositionVector stationPoint= echo.targetPos->getEstimatedValue();
	const TLOR2LOR& stationPTLor2RootTrafo = getLORTransformation(echo.targetPos->getFrameTreePosition(), fTree->begin()); 
	stationPTLor2RootTrafo.transform(stationPoint);

	/*Reference point is always defined in ROOT and is fixed, i.e. no transformation to ROOT required and no contribution required for its coordinates.*/
	TPositionVector referencePoint = echoROM.fMeasuredPlane->getReferencePoint()->getEstimatedValue();

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		transformPointsToMLASystem( echoROM.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, stationPoint);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal theta = echoROM.fMeasuredPlane->getThetaEstimatedValue().rad(); 
	TReal cEcVp = echo.target.distCorrectionValue; 
	TReal dRef = echoROM.fMeasuredPlane->getRefPtDistEstimatedValue().getValue(); 

	TReal calcMeas = -cosq(theta)*(stationPoint.getX() - referencePoint.getX() ).getValue() + sinq(theta)*(stationPoint.getY() - referencePoint.getY()).getValue() + dRef - cEcVp;

	return calcMeas;
}


TReal	TContributionsGenerator::getDLEVCalcMeas(const TLEVEL& levelInstr, const TDLEV& dlev){
	TPositionVector referencePoint = levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue();
	const TLOR2LOR& refPTLor2RootTrafo = getLORTransformation(levelInstr.fMeasuredPlane->getReferencePoint()->getFrameTreePosition(), fTree->begin()); 
	refPTLor2RootTrafo.transform(referencePoint);

	TPositionVector staffPosition = dlev.targetPos->getEstimatedValue();  // this Target / Levelling Staff / SCALE can be defined anywhere in the tree
	const TLOR2LOR& staffPTLor2RootTrafo = getLORTransformation(dlev.targetPos->getFrameTreePosition(), fTree->begin()); 
	staffPTLor2RootTrafo.transform(staffPosition);

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		transformPointsToMLASystem( levelInstr.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, staffPosition);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal collAngl = levelInstr.instrument.collAngleAdjustable->getEstimatedValue().rad(); //collimination angle in rads
	TReal cdz = dlev.target.distCorrectionValue; //distance of the target correction value
	TReal dRef = levelInstr.fMeasuredPlane->getRefPtDistEstimatedValue().getValue(); 

	TReal dTg = sqrtq(pow2q(staffPosition.getX().getValue() - referencePoint.getX().getValue()) + pow2q(staffPosition.getY().getValue() - referencePoint.getY().getValue())); 
	TReal calcMeas = referencePoint.getZ().getValue() - staffPosition.getZ().getValue() + dRef - cdz - dTg*tanq(collAngl);
	return calcMeas;
}

TReal	TContributionsGenerator::getDSPTCalcMeas(const TEDM& edmST, const TDSPT& dspt){
	TPositionVector targetPos = dspt.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = getLORTransformation(dspt.targetPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = edmST.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = getLORTransformation(edmST.instrumentPos->getFrameTreePosition(), fTree->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		transformPointsToMLASystem(edmST.instrumentPos->getName(), stationPos, targetPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

	TReal cst = dspt.target.distCorrectionAdjustable->getEstimatedValue().getValue();

	TReal D = dist3D(stationPos.getX().getValue(), stationPos.getY().getValue(), (stationPos.getZ().getValue() + edmST.instrument.instrHeight), 
				targetPos.getX().getValue(), targetPos.getY().getValue(), (targetPos.getZ().getValue() + dspt.target.targetHt));

	return D - cst;
}


TReal	TContributionsGenerator::getHorDistCalcMeas(const TAdjustablePoint* referencePoint, const TDLEV::TDHOR& dhor){
	TPositionVector refPointPos = referencePoint->getEstimatedValue();  // Reference point is the 'target'.
	const TLOR2LOR& refPTLor2RootTrafo = getLORTransformation(referencePoint->getFrameTreePosition(), fTree->begin()); 
	refPTLor2RootTrafo.transform(refPointPos);

	TPositionVector staffPos = dhor.targetPos->getEstimatedValue();   // Levelling staff is the 'station', can be defined anywhere in the tree.
	const TLOR2LOR& staffPTLor2RootTrafo = getLORTransformation( dhor.targetPos->getFrameTreePosition(), fTree->begin()); 
	staffPTLor2RootTrafo.transform(staffPos);

	if(fRefFrame != TLGCRefFrame::ERefs::kOLOC){
		transformPointsToMLASystem(dhor.targetPos->getName(), staffPos, refPointPos);
		fMLAused = true;
	}
	else
		fMLAused = false;

	return dist(staffPos.getX().getValue(), staffPos.getY().getValue(), refPointPos.getX().getValue(), refPointPos.getY().getValue());
}


TFreeVector TContributionsGenerator::getUVECCalcMeas(const TCAM& camera, const TUVEC& uvec){
	fMLAused = false;
	//Transformation from target into camera LOR
	const TLOR2LOR& tg2camTrafo = getLORTransformation(uvec.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition());
	TPositionVector targetPos = uvec.targetPos->getEstimatedValue(); // Target position
	tg2camTrafo.transform(targetPos);	//Transform from target LOR into camera LOR

	const TPositionVector& cameraPos =  camera.instrumentPos->getEstimatedValue();

	TFreeVector deltaStTg((targetPos.getX() - cameraPos.getX()).getValue(),
						(targetPos.getY() - cameraPos.getY()).getValue(),
						targetPos.getZ().getValue() - cameraPos.getZ().getValue(),
						TCoordSysFactory::k3DCartesian);
	
	// s - Distance
	TScalar sDist = deltaStTg.length();
	return TFreeVector((deltaStTg.getX()/sDist).getValue(), (deltaStTg.getY()/sDist).getValue(), (deltaStTg.getZ()/sDist).getValue(), TCoordSysFactory::k3DCartesian);
}


UVDCalcMeas TContributionsGenerator::getUVDCalcMeas(const TCAM& camera, const TUVD& uvd){
	fMLAused = false;
	//Transformation from target into camera LOR
	const TLOR2LOR& tg2camTrafo = getLORTransformation(uvd.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition());
	TPositionVector targetPos = uvd.targetPos->getEstimatedValue(); // Target position
	tg2camTrafo.transform(targetPos);	//Transform from target LOR into camera LOR

	const TPositionVector& cameraPos =  camera.instrumentPos->getEstimatedValue();

	TFreeVector deltaStTg((targetPos.getX() - cameraPos.getX()).getValue(),
						(targetPos.getY() - cameraPos.getY()).getValue(),
						targetPos.getZ().getValue() - cameraPos.getZ().getValue(),
						TCoordSysFactory::k3DCartesian);
	
	// s - Distance
	TScalar sDist = deltaStTg.length();
	TFreeVector vectCalcMeas((deltaStTg.getX()/sDist).getValue(), (deltaStTg.getY()/sDist).getValue(), (deltaStTg.getZ()/sDist).getValue(), TCoordSysFactory::k3DCartesian);
	UVDCalcMeas calMeas = {vectCalcMeas,sDist.getValue()};
	return calMeas;
}



//The ECTH implementation is not yet finished
#if 0
/*ECTH*/
ECTHContrib	 TContributionsGenerator::getECTHContrib(const TTSTN& station, const TTSTN::TROM& rom, const TECTH& ecth){
	/*
		CONTRIBUTION IS CALCULATED EITHER IN ROOT OR IN MLA OF THE STATION (Total station)
		There is the point where the Total station resides, which is defining the plane. And then there are stations with SCALE instrument,
		which are measuring this plane
	*/
	/*Contribnutions to be returned*/
	TReal calcMeas, distCorrection, variance;
	TFreeVector coordContribTSTNPt(TCoordSysFactory::k3DCartesian);
	TFreeVector scaleStationPtContrib(TCoordSysFactory::k3DCartesian);
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
	TReal varM = pow2q(dist.target.sigmaDist + dist.getDistance()/1000 * dist.target.ppmDist);
	TReal varInstHeight = pow2q(station.instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(dist.target.sigmaTargetHt);
	TReal varInstCent = pow2q(station.instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(dist.target.sigmaTargetCentering);
	variance = varM + pow2q((zTg - zSt + hTg - hInst)/D) * (varInstHeight +  varTgHeight) + ((pow2q(yTg - ySt) + pow2q(xSt - xTg))/pow2q(D)) * (varInstCent + varTgCent);

	DistMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, distCorrection, variance};
	return contrib;
}
#endif