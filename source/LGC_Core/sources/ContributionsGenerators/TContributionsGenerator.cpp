#include <LGCAdjustablePoint.h>
#include <TContributionsGenerator.h>
#include "TDist.h" 
#include "TTreeEntry.h"
#include "TXYH2CCS.h"


//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////
TContributionsGenerator::TContributionsGenerator(TPointTransformer& fPointTransfoFunc) :fPointTransfo(fPointTransfoFunc)
{}

//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- TSTN measurements
//////////////////////////////////////////////////////////////////////

/*
	All the TSTN contributions are calculated either in ROOT node of the TREE of local frames or in the modified local astronomical
	system of the instrument (station).

	In all cases the STATION and TARGET points can be defined anywhere in the TREE of local frames.
*/

//Spatial distance contributions
DistMeasContrib	TContributionsGenerator::getSpatialDistanceContrib(std::shared_ptr<TTSTN> station, const TLINE& dist){
//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(dist.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true){
		fPointTransfo.transformPointsToMLASystem(station->instrumentPos->getName(), stationPos, targetPos);
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
	TReal hInst = station->instrumentHeightAdjustable->getEstimatedValue();

	TReal cst;
	if(!dist.target.distCorrectionAdjustable->isFixed())
		cst = dist.target.distCorrectionAdjustable->getEstimatedValue();
	else
		cst = dist.target.distCorrectionValue;


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
	addTransformationsContributions(stLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c); // Contribution to a TARGET point
	// Target transformation contribution
	addTransformationsContributions(tgLor2RootTrafo, dist.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	TReal distCorrContrib =  - 1.0;

	// Variance calculation
	TReal varM = pow2q(dist.target.sigmaDist + dist.getDistance()/1000 * dist.target.ppmDist);
	TReal varInstHeight = pow2q(station->instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(dist.target.sigmaTargetHt);
	TReal varInstCent = pow2q(station->instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(dist.target.sigmaTargetCentering);
	TReal variance = varM + pow2q((zTg - zSt + hTg - hInst)/D) * (varInstHeight +  varTgHeight) + ((pow2q(yTg - ySt) + pow2q(xSt - xTg))/pow2q(D)) * (varInstCent + varTgCent);

	DistMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, distCorrContrib, variance};
	return contrib;
}

DistMeasContribFrame	TContributionsGenerator::getSpatialDistanceContribInFrame(std::shared_ptr<TTSTN> station, const TLINE& dist) {
	fPointTransfo.setMLA(false); // TSTN in Frame measurements never in MLA
	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();

	const TLOR2LOR& tg2stTrafo = fPointTransfo.getLORTransformation(dist.targetPos->getFrameTreePosition(), station->instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	TPositionVector targetPos = dist.targetPos->getEstimatedValue();
	tg2stTrafo.transform(targetPos);


	// Prepare coefficients (a,b,c) for the points and the transformations contributions
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();
	TReal hTg = dist.target.targetHt;

	TReal cst;
	if (!dist.target.distCorrectionAdjustable->isFixed())
		cst = dist.target.distCorrectionAdjustable->getEstimatedValue();
	else
		cst = dist.target.distCorrectionValue;
	TReal D = dist3D(xSt, ySt, zSt, xTg, yTg, (zTg + hTg));

	if (D < nullLimit)
		throw std::logic_error("TContributionGenerator::getSpatialDistanceContrib: Division by zero because observation points have identical coordinates.");

	TReal a, b, c;   //station's contributions coefficients (negative values of these give coefficients of the TARGET)		 
	a = (xSt - xTg) / D;  // xSt coefficient
	b = (ySt - yTg) / D;  //ySt coefficient
	c = (zSt - zTg - hTg) / D; //zSt coefficient


							   //coordinate contributions is calculated in a LOR system of the station and, therefore, the station's contribution is this
	TFreeVector coordContribStation(a, b, c, TCoordSysFactory::ECoordSys::k3DCartesian); // Contribution to a STATION point
	TFreeVector coordContribTarget = getPointContributions(tg2stTrafo, -a, -b, -c); // Contribution to a TARGET point


																					//transformation contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions; // Vector with target's transformations contributions
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = tg2stTrafo.getTransformationChain();
	TFreeVector scaleDeriv(TCoordSysFactory::k3DCartesian);
	TFreeVector omegaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector phiDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector kappaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector t1Derivative = (TCoordSysFactory::k3DCartesian);
	TFreeVector t2Derivative = (TCoordSysFactory::k3DCartesian);
	TFreeVector t3Derivative(TCoordSysFactory::k3DCartesian);

	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
	for (auto it(trafoChain.begin()); it != trafoChain.end(); ++it) {
		std::string transformationName = it->adjTrafo->getName();

		//Contributions for rotations : Omega, Phi and Kappa
		omegaDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 0);
		phiDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 1);
		kappaDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 2);
		//Contributions for translation: X, Y and Z coordinate
		t1Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 0);
		t2Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 1);
		t3Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 2);
		//Contributions for the scale
		scaleDeriv = tg2stTrafo.partialDerivativesScale(transformationName, targetPos);

		TransformationContrib thirdEqContrib = {
			TFreeVector(-a * omegaDerivative.getX().getMetresValue() - b * omegaDerivative.getY().getMetresValue() - c* omegaDerivative.getZ().getMetresValue()
			, -a * phiDerivative.getX().getMetresValue() - b * phiDerivative.getY().getMetresValue() - c* phiDerivative.getZ().getMetresValue()
				, -a * kappaDerivative.getX().getMetresValue() - b * kappaDerivative.getY().getMetresValue() - c* kappaDerivative.getZ().getMetresValue()
				, TCoordSysFactory::k3DCartesian),
			TFreeVector(-a * t1Derivative.getX().getMetresValue() - b * t1Derivative.getY().getMetresValue() - c* t1Derivative.getZ().getMetresValue()
				, -a * t2Derivative.getX().getMetresValue() - b * t2Derivative.getY().getMetresValue() - c* t2Derivative.getZ().getMetresValue()
				, -a * t3Derivative.getX().getMetresValue() - b * t3Derivative.getY().getMetresValue() - c* t3Derivative.getZ().getMetresValue()
				, TCoordSysFactory::k3DCartesian),
			-a * scaleDeriv.getX().getMetresValue() - b * scaleDeriv.getY().getMetresValue() - c* scaleDeriv.getZ().getMetresValue() };



		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib>(*it->adjTrafo, thirdEqContrib));
	}

	//Calculate the theorical measurement
	TReal calcMeas = D - cst;

	// Variance calculation
	TReal varM = pow2q(dist.target.sigmaDist + dist.getDistance() / 1000 * dist.target.ppmDist);
	TReal varTgHeight = pow2q(dist.target.sigmaTargetHt);
	TReal varInstCent = pow2q(station->instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(dist.target.sigmaTargetCentering);
	TReal variance = varM + pow2q((zTg - zSt + hTg) / D) * varTgHeight + ((pow2q(yTg - ySt) + pow2q(xSt - xTg)) / pow2q(D)) * (varInstCent + varTgCent);

	//Fill the contribution structure
	DistMeasContribFrame  contrib = { calcMeas, coordContribStation, coordContribTarget, targetTransfContributions, 0.0, -1.0, variance };
	return contrib;
}

// Horizontal angle contributions
AnglMeasContrib	TContributionsGenerator::getHorAnglContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TANGL& angl){
//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = angl.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(angl.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true){
		fPointTransfo.transformPointsToMLASystem(station->instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal xSt = stationPos.getX().getMetresValue();
   TReal ySt = stationPos.getY().getMetresValue();

   TReal xTg = targetPos.getX().getMetresValue();
   TReal yTg = targetPos.getY().getMetresValue();

	//Calculated measurement value
   TAngle calcMeas = TAngle::aTan2((xTg - xSt), (yTg - ySt)) - rom->v0->getEstimatedValue() - rom->acst;  //ACST is the constant orientation of the instrument

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
	addTransformationsContributions(stLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, angl.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	// Variance calculation
	TReal variance = pow2q(angl.target.sigmaAngl.getRadiansValue()) + (1.0/pow2q(dist2)) * (pow2q(station->instrument.sigmaInstrCentering) + pow2q(angl.target.sigmaTargetCentering));

	AnglMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

AnglMeasContribFrame	TContributionsGenerator::getHorAnglContribInFrame(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TANGL& angl) {
	fPointTransfo.setMLA(false); // TSTN in Frame measurements never in MLA
	const TLOR2LOR& tg2stTrafo = fPointTransfo.getLORTransformation(angl.targetPos->getFrameTreePosition(), station->instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
	TPositionVector targetPos = angl.targetPos->getEstimatedValue();
	tg2stTrafo.transform(targetPos);
	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();

	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal dist2 = pow2q(dist(xSt, ySt, xTg, yTg));
	if (dist2 < nullLimit)
		throw std::logic_error("TContributionGenerator::getHorAnglContrib: Division by zero because observation points have identical coordinates.");


	//Calculated measurement value
	TAngle calcMeas = TAngle::aTan2((xTg - xSt), (yTg - ySt)) - rom->v0->getEstimatedValue() - rom->acst;  //ACST is the constant orientation of the instrument

	TReal a, b, c; //station's contributions coefficients (negative values of these give the target coefficients)		
	a = (-LITERAL(1.0) * (yTg - ySt)) / dist2; //xSt coefficient
	b = (xTg - xSt) / dist2; //ySt coefficient
	c = 0.0; //zSt coefficient

			 //point contribution 
	TFreeVector coordContribStation(a, b, c, TCoordSysFactory::ECoordSys::k3DCartesian); // Contribution to a STATION point
	TFreeVector coordContribTarget = getPointContributions(tg2stTrafo, -a, -b, -c); // Contribution to a TARGET point

																					//transformation contribution
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions; // Vector with target's transformations contributions
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = tg2stTrafo.getTransformationChain();
	std::string transformationName;
	TFreeVector scaleDeriv(TCoordSysFactory::k3DCartesian);
	TFreeVector omegaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector phiDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector kappaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector t1Derivative = (TCoordSysFactory::k3DCartesian);
	TFreeVector t2Derivative = (TCoordSysFactory::k3DCartesian);
	TFreeVector t3Derivative(TCoordSysFactory::k3DCartesian);

	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
	for (auto it(trafoChain.begin()); it != trafoChain.end(); ++it) {
		transformationName = it->adjTrafo->getName();
		omegaDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 0);
		phiDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 1);
		kappaDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 2);
		t1Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 0);
		t2Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 1);
		t3Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 2);
		scaleDeriv = tg2stTrafo.partialDerivativesScale(transformationName, targetPos);

		TransformationContrib thirdEqContrib = { TFreeVector(-a*omegaDerivative.getX().getMetresValue() - b*omegaDerivative.getY().getMetresValue() - c*omegaDerivative.getZ().getMetresValue(),
			-a*phiDerivative.getX().getMetresValue() - b*phiDerivative.getY().getMetresValue() - c*phiDerivative.getZ().getMetresValue(),
			-a*kappaDerivative.getX().getMetresValue() - b*kappaDerivative.getY().getMetresValue() - c*kappaDerivative.getZ().getMetresValue(), TCoordSysFactory::k3DCartesian),
			TFreeVector(-a*t1Derivative.getX().getMetresValue() - b*t1Derivative.getY().getMetresValue() - c*t1Derivative.getZ().getMetresValue(),
				-a*t2Derivative.getX().getMetresValue() - b*t2Derivative.getY().getMetresValue() - c*t2Derivative.getZ().getMetresValue(),
				-a*t3Derivative.getX().getMetresValue() - b*t3Derivative.getY().getMetresValue() - c*t3Derivative.getZ().getMetresValue(), TCoordSysFactory::k3DCartesian),
			-a*scaleDeriv.getX().getMetresValue() - b*scaleDeriv.getY().getMetresValue() - c*scaleDeriv.getZ().getMetresValue() };


		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib>(*it->adjTrafo, thirdEqContrib));
	}

	// Variance calculation
	TReal variance = pow2q(angl.target.sigmaAngl.getRadiansValue()) + (1.0 / pow2q(dist2)) * (pow2q(station->instrument.sigmaInstrCentering) + pow2q(angl.target.sigmaTargetCentering));

	AnglMeasContribFrame  contrib = { calcMeas, coordContribStation, coordContribTarget, targetTransfContributions, 0.0, 0.0, variance };
	return contrib;
}

//Zenith distance (vertical angle) contributions
AnglMeasContrib	TContributionsGenerator::getZenDistContrib(std::shared_ptr<TTSTN> station, const TZEND& zend){
//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = zend.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(zend.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true){
		fPointTransfo.transformPointsToMLASystem(station->instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

// Prepare coefficients (a,b,c) for the points and the transformations contributions
	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
   TReal xSt = stationPos.getX().getMetresValue();
   TReal ySt = stationPos.getY().getMetresValue();
   TReal zSt = stationPos.getZ().getMetresValue();

   TReal xTg = targetPos.getX().getMetresValue();
   TReal yTg = targetPos.getY().getMetresValue();
   TReal zTg = targetPos.getZ().getMetresValue();

	TReal hTg = zend.target.targetHt;
	TReal hInst = station->instrumentHeightAdjustable->getEstimatedValue();

	TReal dx = xTg-xSt;
	TReal dy = yTg-ySt;
	TReal dz = zTg-zSt-hInst+hTg;

	TReal distance3D = dist3D(xSt, ySt, zSt+hInst, xTg, yTg, zTg+hTg);
	if (distance3D < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (distance3D).");

	TAngle calcMeas = TAngle::aCos((zTg - zSt - hInst + hTg)/distance3D);
	
	//We are taking the currently calculated value not the measured one (zend.getAngle().rad()), do not know what is better to take
	TReal sinPhi = sinq(calcMeas.getRadiansValue());

	if (sinPhi < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (sinV=0).");

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
	addTransformationsContributions(stLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, zend.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	TReal variance = pow2q(zend.target.sigmaZenD.getRadiansValue()) + (((pow2q(dx) + pow2q(dy))*pow2q(dz))/(powq(distance3D,6)*pow2q(sinPhi))) * 
					(pow2q(station->instrument.sigmaInstrCentering) + pow2q(zend.target.sigmaTargetCentering)) +
					 pow2q(-c) * (pow2q(station->instrument.sigmaInstrHeight) + pow2q(zend.target.sigmaTargetHt));

	AnglMeasContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance};
	return contrib;
}

AnglMeasContribFrame	TContributionsGenerator::getZenDistContribInFrame(std::shared_ptr<TTSTN> station, const TZEND& zend) {
	fPointTransfo.setMLA(false); // TSTN in Frame measurements never in MLA
	const TLOR2LOR& tg2stTrafo = fPointTransfo.getLORTransformation(zend.targetPos->getFrameTreePosition(), station->instrumentPos->getFrameTreePosition());
	TPositionVector targetPos = zend.targetPos->getEstimatedValue();
	tg2stTrafo.transform(targetPos);

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	TReal xSt = stationPos.getX().getMetresValue();
	TReal ySt = stationPos.getY().getMetresValue();
	TReal zSt = stationPos.getZ().getMetresValue();

	TReal xTg = targetPos.getX().getMetresValue();
	TReal yTg = targetPos.getY().getMetresValue();
	TReal zTg = targetPos.getZ().getMetresValue();
	TReal hTg = zend.target.targetHt;

	TReal dx = xTg - xSt;
	TReal dy = yTg - ySt;
	TReal dz = zTg - zSt + hTg;

	TReal distance3D = dist3D(xSt, ySt, zSt, xTg, yTg, zTg + hTg);
	if (distance3D < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (distance3D).");


	// Prepare coefficients (a,b,c) for the points and the transformations contributions
	TAngle calcMeas = TAngle::aCos((zTg - zSt + hTg) / distance3D);
	//We are taking the currently calculated value not the measured one (zend.getAngle().rad()), do not know what is better to take
	TReal sinPhi = sinq(calcMeas.getRadiansValue());

	if (sinPhi < nullLimit)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistContrib: Division by zero because observation points have identical coordinates (sinV=0).");

	TReal a, b, c; //station's contributions coefficients (negative values of these give target's coefficients)	
	a = (-1.0 * dz * dx) / (powq(distance3D, 3) * sinPhi);//xSt coefficient
	b = (-1.0 * dz * dy) / (powq(distance3D, 3) * sinPhi);//ySt coefficient
	c = (1.0 / (distance3D * sinPhi)) - powq(dz, 2) / (powq(distance3D, 3) * sinPhi);//zSt coefficient


																					 //point contribution
	TFreeVector coordContribStation(a, b, c, TCoordSysFactory::ECoordSys::k3DCartesian); // Contribution to a STATION point
	TFreeVector coordContribTarget = getPointContributions(tg2stTrafo, -a, -b, -c); // Contribution to a TARGET point

																					//transformation contribution
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions; // Vector with target's transformations contributions
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = tg2stTrafo.getTransformationChain();
	std::string transformationName;
	TFreeVector scaleDeriv(TCoordSysFactory::k3DCartesian);
	TFreeVector omegaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector phiDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector kappaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector t1Derivative = (TCoordSysFactory::k3DCartesian);
	TFreeVector t2Derivative = (TCoordSysFactory::k3DCartesian);
	TFreeVector t3Derivative(TCoordSysFactory::k3DCartesian);
	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
	for (auto it(trafoChain.begin()); it != trafoChain.end(); ++it) {
		transformationName = it->adjTrafo->getName();

		omegaDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 0);
		phiDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 1);
		kappaDerivative = tg2stTrafo.partialDerivativesAngle(transformationName, targetPos, 2);
		t1Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 0);
		t2Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 1);
		t3Derivative = tg2stTrafo.partialDerivativesTranslation(transformationName, targetPos, 2);
		scaleDeriv = tg2stTrafo.partialDerivativesScale(transformationName, targetPos);

		TransformationContrib thirdEqContrib = { TFreeVector(-a*omegaDerivative.getX().getMetresValue() - b*omegaDerivative.getY().getMetresValue() - c*omegaDerivative.getZ().getMetresValue(),
			-a*phiDerivative.getX().getMetresValue() - b*phiDerivative.getY().getMetresValue() - c*phiDerivative.getZ().getMetresValue(),
			-a*kappaDerivative.getX().getMetresValue() - b*kappaDerivative.getY().getMetresValue() - c*kappaDerivative.getZ().getMetresValue(), TCoordSysFactory::k3DCartesian),
			TFreeVector(-a*t1Derivative.getX().getMetresValue() - b*t1Derivative.getY().getMetresValue() - c*t1Derivative.getZ().getMetresValue(),
				-a*t2Derivative.getX().getMetresValue() - b*t2Derivative.getY().getMetresValue() - c*t2Derivative.getZ().getMetresValue(),
				-a*t3Derivative.getX().getMetresValue() - b*t3Derivative.getY().getMetresValue() - c*t3Derivative.getZ().getMetresValue(), TCoordSysFactory::k3DCartesian),
			-a*scaleDeriv.getX().getMetresValue() - b*scaleDeriv.getY().getMetresValue() - c*scaleDeriv.getZ().getMetresValue() };


		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib>(*it->adjTrafo, thirdEqContrib));
	}

	//Calculate and return the contributions
	TReal variance = pow2q(zend.target.sigmaZenD.getRadiansValue()) + (((pow2q(dx) + pow2q(dy))*pow2q(dz)) / (powq(distance3D, 6)*pow2q(sinPhi))) *
		(pow2q(station->instrument.sigmaInstrCentering) + pow2q(zend.target.sigmaTargetCentering)) + pow2q(-c) *pow2q(zend.target.sigmaTargetHt);

	AnglMeasContribFrame  contrib = { calcMeas, coordContribStation, coordContribTarget, targetTransfContributions, 0.0, 0.0, variance };
	return contrib;
}

//PLR3D Contribution
PLR3DContrib	TContributionsGenerator::getPolar3DContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TPLR3D& plr3D){
//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = plr3D.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(plr3D.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true){
		fPointTransfo.transformPointsToMLASystem(station->instrumentPos->getName(), stationPos, targetPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal Rx = 0.0; //Rotation around x-axis, default is no rotation
	TReal Ry = 0.0; //Rotation around y-axis, default is no rotation

	if(station->rot3D){ //If station can rotate freely get the rotation values
		if(station->rotX == nullptr || station->rotY == nullptr)
			throw std::runtime_error("TContributionGenerator::getPolar3DContrib station can rotate freely, but rotation angles are not defined.");
		Rx = station->rotX->getEstimatedValue().getRadiansValue();
		Ry = station->rotY->getEstimatedValue().getRadiansValue();
	}

	TReal sinV0 = (rom->v0->getEstimatedValue() - rom->acst).sine();
	TReal cosV0 = (rom->v0->getEstimatedValue() - rom->acst).cosine();

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
	const TPositionVector& stPointInLOR = station->instrumentPos->getEstimatedValue();
	addTransformationsContributions3D(stLor2RootTrafo, stPointInLOR, line1AMat,  line2AMat,  line3AMat, stationTransfContributions);

	//Fill target transformation contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> targetTransfContributions; // Vector for target transformations contributions
	const TPositionVector& tgPointInLOR = plr3D.targetPos->getEstimatedValue();
	addTransformationsContributions3D(tgLor2RootTrafo, tgPointInLOR, line1AMat,  line2AMat,  line3AMat, targetTransfContributions);


	TReal sDistPlusCs = plr3D.getDistance() + plr3D.target.distCorrectionAdjustable->getEstimatedValue();
	TReal sinTheta = plr3D.getAngle(kANGL).sine();
	TReal cosTheta = plr3D.getAngle(kANGL).cosine();

	TReal sinPhi = plr3D.getAngle(kZEND).sine();
	TReal cosPhi = plr3D.getAngle(kZEND).cosine();

   TReal dX = targetPos.getX().getMetresValue() - stationPos.getX().getMetresValue();
   TReal dY = targetPos.getY().getMetresValue() - stationPos.getY().getMetresValue();
   TReal dZ = targetPos.getZ().getMetresValue() + plr3D.target.targetHt - stationPos.getZ().getMetresValue() - station->instrumentHeightAdjustable->getEstimatedValue();

   TReal dist2 = pow2q(dist(stationPos.getX().getMetresValue(), stationPos.getY().getMetresValue(), targetPos.getX().getMetresValue(), targetPos.getY().getMetresValue()));
	TReal distance3D = sqrt(pow2q(dX) + pow2q(dY)+pow2q(dZ));
	TReal c = (1.0 / (distance3D * sinPhi)) - powq(dZ,2)/(powq(distance3D,3) * sinPhi);

	//Contribution to be returned
	PLR3DContrib contrib = {coordContribStation, coordContribTarget, stationTransfContributions,  targetTransfContributions, 
   {line1AMat.getZ().getMetresValue(), line2AMat.getZ().getMetresValue(), line3AMat.getZ().getMetresValue()},//Instrument height contribution
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

	if(station->rot3D){
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
	contrib.fObsVariance[0] = pow2q(plr3D.target.sigmaAngl.getRadiansValue()) + (1.0/pow2q(dist2)) * (pow2q(station->instrument.sigmaInstrCentering) + pow2q(plr3D.target.sigmaTargetCentering));
	// ZEND
	contrib.fObsVariance[1] = pow2q(plr3D.target.sigmaZenD.getRadiansValue()) + (((pow2q(dX) + pow2q(dY))*pow2q(dZ))/(powq(distance3D,6)*pow2q(sinPhi))) * 
					(pow2q(station->instrument.sigmaInstrCentering) + pow2q(plr3D.target.sigmaTargetCentering)) +
					 pow2q(-c) * (pow2q(station->instrument.sigmaInstrHeight) + pow2q(plr3D.target.sigmaTargetHt));
	// DIST
	TReal varM = pow2q(plr3D.target.sigmaDist + plr3D.getDistance()/1000 *plr3D.target.ppmDist);
	TReal varInstHeight = pow2q(station->instrument.sigmaInstrHeight);
	TReal varTgHeight = pow2q(plr3D.target.sigmaTargetHt);
	TReal varInstCent = pow2q(station->instrument.sigmaInstrCentering);
	TReal varTgCent = pow2q(plr3D.target.sigmaTargetCentering);
	contrib.fObsVariance[2] = varM + pow2q((dZ)/distance3D) * (varInstHeight +  varTgHeight) + ((pow2q(dY) + pow2q(dX))/pow2q(distance3D)) * (varInstCent + varTgCent);

	return contrib;
}

//Horizontal distance contributions, measurement made by TSTN
HorDistContrib	TContributionsGenerator::getHorDistContrib(std::shared_ptr<TTSTN> station, const TLINE& dhor){
	TReal calcMeas;
	TFreeVector coordContribStation(TCoordSysFactory::k3DCartesian);
	TFreeVector coordContribTarget(TCoordSysFactory::k3DCartesian);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;

//Transform TARGET and STATION from their LOCAL FRAME either to ROOT or to MLA of the station
	TPositionVector targetPos = dhor.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(dhor.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = station->instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true){
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
		throw std::logic_error("TLGCObsLSContributionGenerator::getHorDistDsgnMxContributions: Division by zero because observation points have identical coordinates.");

	TReal a,b,c;  //station's contributions coefficients (negative values of these give target's coefficients)			
	a = - (xTg - xSt)/D;  // xSt coefficient
	b = - (yTg - ySt)/D;  //ySt coefficient
	c = 0.0; //zSt coefficient

	//Station can be defined anywhere, get point contributions and transformations contributions
	coordContribStation = getPointContributions(stLor2RootTrafo, a, b, c);
	addTransformationsContributions(stLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	coordContribTarget = getPointContributions(tgLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(tgLor2RootTrafo, dhor.targetPos->getEstimatedValue(), -a, -b, -c, targetTransfContributions);

	calcMeas = D - cte;
	TReal distCorrCont = - 1.0;

	// Variance
	TReal varM = pow2q(dhor.target.sigmaDist + dhor.getDistance()/1000*dhor.target.ppmDist);
	TReal variance = varM + (pow2q(station->instrument.sigmaInstrCentering) + pow2q(dhor.target.sigmaTargetCentering));

	HorDistContrib  contrib = {calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, distCorrCont, variance};
	return contrib;
}

//ECTH Contribution
ECTHContrib	 TContributionsGenerator::getECTHContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TECTH& ecth){
	///////////////////Transform TARGET and STATION from their's LOR either to ROOT or to MLA of the station///////////////////////////////
	TPositionVector targetPos = station->instrumentPos->getEstimatedValue(); //position of the scale. Point to measure
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = ecth.targetPos->getEstimatedValue(); //position of the TSTN
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(ecth.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Target lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);


	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true){
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
	TReal a, b, c;   //station's contributions coefficients (negative values of these give target's coefficients)
	a = -cos(theta + Vo);  // xSt coefficient
	b = sin(theta + Vo);  //ySt coefficient
	c = 0.0;  //zSt coefficient

	TReal calcMeas = a*(xSt - xTg)+b*(ySt - yTg) - ecth.target.distCorrectionValue.getMetresValue();

	TReal V0Contrib = b*(xSt - xTg) - a*(ySt - yTg); //contribution for the V0 parameter
	TReal distCorrection = -1.0;  //Not use for the moment, because it is not adjustable.

	//Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, ecth.targetPos->getEstimatedValue(), -a, -b, -c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, a, b, c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, targetTransfContributions);

	// Variance calculation
	TReal varM = pow2q(ecth.target.sigmaD + ecth.getDistance() / 1000 * ecth.target.ppmD);
	TReal variance = varM + (pow2q(cos(theta + Vo)) + pow2q(sin(theta + Vo)))*pow2q(ecth.target.sigmaInstrCentering);

	ECTHContrib	contrib = { calcMeas, coordContribStation, coordContribTarget, V0Contrib, distCorrection, stationTransfContributions, targetTransfContributions, variance };
	return contrib;
}

//ECDIR contribution
ECTHContrib	 TContributionsGenerator::getECDIRContrib(std::shared_ptr<TTSTN> station, std::shared_ptr<TTSTN::TROM> rom, const TECDIR& ecdir)
{
	///////////////////Transform TARGET and STATION from their's LOR to ROOT///////////////////////////////
	TPositionVector targetPos = station->instrumentPos->getEstimatedValue(); //position of the scale. Point to measure
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(station->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = ecdir.targetPos->getEstimatedValue(); //position of the TSTN
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(ecdir.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Target lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);


	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && station->rot3D != true){
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
	//line direction at the TSTN position
	TFreeVector l(sin(theta + Vo) * sin(phi), cos(theta + Vo) * sin(phi), cos(phi), TCoordSysFactory::ECoordSys::k3DCartesian);


	//Calcul par le produit scalaire (u^l)²+(u.l)²=|v|²|l|²
	TReal d, pScal;
	d = sqrt(pow2(xSt - xTg) + pow2(ySt - yTg) + pow2(zSt - zTg));  // distance St-Tg
	pScal = l[0] * (xSt - xTg) + l[1] * (ySt - yTg) + l[2] * (zSt - zTg);  //produit scalaire

	TReal calcMeas = sqrt(pow2(d) - pow2(pScal)) - ecdir.target.distCorrectionValue.getMetresValue();

	//contributions
	TReal a, b, c, V0Contrib;
	if (calcMeas > nullLimit)
	{
		a = ((xSt - xTg) - l[0] * pScal) / calcMeas;
		b = ((ySt - yTg) - l[1] * pScal) / calcMeas;
		c = ((zSt - zTg) - l[2] * pScal) / calcMeas;
		V0Contrib = -1 * (l[1] * (xSt - xTg) - l[0] * (ySt - yTg)) / calcMeas; /**/
	}
	else
		throw std::logic_error("TContributionGenerator::getECDIRContrib: Division by zero because the point is on the line");


	TReal distCorrection = -1.0;  //Not use for the moment, because it is not adjustable.

	//Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, -a, -b, -c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, ecdir.targetPos->getEstimatedValue(), -a, -b, -c, stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, a, b, c);
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, station->instrumentPos->getEstimatedValue(), a, b, c, targetTransfContributions);

	// Variance calculation
	TReal varM = pow2q(ecdir.target.sigmaD + ecdir.getDistance() / 1000 * ecdir.target.ppmD);
	TReal variance = varM + (pow2q(cos(theta + Vo)*sin(phi)) + pow2q(sin(theta + Vo)*sin(phi)))*pow2q(ecdir.target.sigmaInstrCentering);

	ECTHContrib	contrib = { calcMeas, coordContribStation, coordContribTarget, V0Contrib, distCorrection, stationTransfContributions, targetTransfContributions, variance };
	return contrib;
}
//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- individual measurements
//////////////////////////////////////////////////////////////////////

// Spatial distance made by an EDM
DistMeasContrib	TContributionsGenerator::getDSPTContrib(const TEDM& edmST, const TDSPT& dspt){
	TPositionVector targetPos = dspt.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(dspt.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Target lor" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);

	TPositionVector stationPos = edmST.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(edmST.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
	stLor2RootTrafo.transform(stationPos);

	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
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
		throw std::logic_error("TContributionGenerator::getDSPTContrib: Division by zero because observation points have identical coordinates.");

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
HorDistContribLEVEL	TContributionsGenerator::getHorDistContrib(const LGCAdjustablePoint* referencePoint, const TDLEV::TDHOR& dhor){
	TFreeVector staffContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector referencePTContrib(TCoordSysFactory::k3DCartesian);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> staffTransfContributions;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> referencePTTransfContributions;
	TReal calcMeas;

	TPositionVector refPointPos = referencePoint->getEstimatedValue();  // Reference point
	const TLOR2LOR& refPTLor2RootTrafo = fPointTransfo.getLORTransformation(referencePoint->getFrameTreePosition(), fPointTransfo.getTree()->begin()); 
	refPTLor2RootTrafo.transform(refPointPos);

	TPositionVector staffPos = dhor.targetPos->getEstimatedValue();   // Levelling staff is the 'target'
	const TLOR2LOR& staffPTLor2RootTrafo = fPointTransfo.getLORTransformation( dhor.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); 
	refPTLor2RootTrafo.transform(staffPos);

	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo.transformPointsToMLASystem(dhor.targetPos->getName(), staffPos, refPointPos);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal xSt = staffPos.getX().getMetresValue();
	TReal ySt = staffPos.getY().getMetresValue();

	TReal xTg = refPointPos.getX().getMetresValue();
	TReal yTg = refPointPos.getY().getMetresValue();

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
	TReal collAngl = levelInstr.instrument.collAngleAdjustable->getEstimatedValue().getRadiansValue(); //collimination angle in rads
	TReal cdz = dlev.target.distCorrectionValue; //distance correction value
	TReal dRef = levelInstr.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(); //Distance of the reference point from the plane

	TPositionVector referencePoint = levelInstr.fMeasuredPlane->getReferencePoint()->getEstimatedValue();
	const TLOR2LOR& refPTLor2RootTrafo = fPointTransfo.getLORTransformation(levelInstr.fMeasuredPlane->getReferencePoint()->getFrameTreePosition(), fPointTransfo.getTree()->begin()); 
	refPTLor2RootTrafo.transform(referencePoint);

	TPositionVector staffPosition = dlev.targetPos->getEstimatedValue();  // this Target / Levelling Staff / SCALE assumed to be in ROOT!!!!!
	const TLOR2LOR& staffPTLor2RootTrafo = fPointTransfo.getLORTransformation(dlev.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); 
	staffPTLor2RootTrafo.transform(staffPosition);

	// If not OLOC => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo.transformPointsToMLASystem(levelInstr.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, staffPosition);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal dTg = sqrtq(pow2q(staffPosition.getX().getMetresValue() - referencePoint.getX().getMetresValue()) + pow2q(staffPosition.getY().getMetresValue() - referencePoint.getY().getMetresValue())); 
	TReal calcMeas = referencePoint.getZ().getMetresValue() - staffPosition.getZ().getMetresValue() + dRef - cdz - dTg*tanq(collAngl);

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
	TReal theta = echoROM.fMeasuredPlane->getThetaEstimatedValue().getRadiansValue(); // Theta angle of the plane
	TReal cEcVp = echo.target.distCorrectionValue; //distance of the target correction value
	TReal dRef = echoROM.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue();  // distance of the reference point from the plane

	TPositionVector stationPoint= echo.targetPos->getEstimatedValue();

	const TLOR2LOR& stationPTLor2RootTrafo = fPointTransfo.getLORTransformation(echo.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); 
	stationPTLor2RootTrafo.transform(stationPoint);

	/*Reference point is always defined in ROOT and is fixed (it is implicitly defined),
	i.e. no transformation to ROOT required and no contribution required for its coordinates.*/
	TPositionVector referencePoint = echoROM.fMeasuredPlane->getReferencePoint()->getEstimatedValue();

	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo.transformPointsToMLASystem(echoROM.fMeasuredPlane->getReferencePoint()->getName(), referencePoint, stationPoint);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal calcMeas = -cosq(theta)*(stationPoint.getX() - referencePoint.getX() ).getMetresValue() + sinq(theta)*(stationPoint.getY() - referencePoint.getY()).getMetresValue() + dRef - cEcVp;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector stationContrib = getPointContributions(stationPTLor2RootTrafo,  -cosq(theta), sinq(theta), 0.0);
	addTransformationsContributions(stationPTLor2RootTrafo, echo.targetPos->getEstimatedValue(),  -cosq(theta), sinq(theta), 0.0, stationTransfContributions);
	
	TReal thetaContrib = sinq(theta)*(stationPoint.getX() - referencePoint.getX()).getMetresValue() + cosq(theta)*(stationPoint.getY() - referencePoint.getY()).getMetresValue();

	TReal refPtDistContrib = 1.0;
	TReal obsVariance = pow2q(echo.target.sigmaD + echo.getDistance()/1000*echo.target.ppmD) + pow2q(echo.target.sigmaInstrCentering);
	
	ECHOContrib echoContrib = {calcMeas, stationContrib, thetaContrib, refPtDistContrib, stationTransfContributions, obsVariance};
	return echoContrib;
}

//ECVE contribution
ScaleMeasContrib TContributionsGenerator::getECVEContrib(const TECVEROM& ecveROM, const TECVE& ecve){
	TReal cEcVp = ecve.target.distCorrectionValue; //distance of the target correction value
	TPositionVector stationPoint = ecve.targetPos->getEstimatedValue();

	const TLOR2LOR& stationPTLor2RootTrafo = fPointTransfo.getLORTransformation(ecve.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	stationPTLor2RootTrafo.transform(stationPoint);

	/*Reference point is always defined in ROOT and is fixed (it is implicitly defined),
	i.e. no transformation to ROOT required and no contribution required for its coordinates.*/
	TPositionVector linePoint = ecveROM.fMeasuredLine->getLinePoint()->getEstimatedValue();
	const TLOR2LOR& linePTLor2RootTrafo = fPointTransfo.getLORTransformation(ecveROM.fMeasuredLine->getLinePoint()->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	linePTLor2RootTrafo.transform(linePoint);

	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
		fPointTransfo.transformPointsToMLASystem(ecveROM.fMeasuredLine->getName(), linePoint, stationPoint);
		fPointTransfo.setMLA(true);
	}
	else
		fPointTransfo.setMLA(false);

	TReal D = sqrt(pow2(linePoint.getX() - stationPoint.getX()) + pow2(linePoint.getY() - stationPoint.getY()));
	TReal calcMeas = D - cEcVp;
	
	//coefficient's contribution for the station
	TReal a = -(linePoint.getX() - stationPoint.getX())/D;
	TReal b = -(linePoint.getY() - stationPoint.getY()) / D;
	TReal c = 0.0;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector stationContrib = getPointContributions(stationPTLor2RootTrafo, a, b, c);
	addTransformationsContributions(stationPTLor2RootTrafo, ecve.targetPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> pointLineTransfContributions;
	TFreeVector pointLineContrib = getPointContributions(linePTLor2RootTrafo, -a, -b, -c);
	addTransformationsContributions(linePTLor2RootTrafo, ecve.targetPos->getEstimatedValue(), -a, -b, -c, pointLineTransfContributions);
	
	// TReal linePointContrib = -1.0;

	TReal obsVariance = pow2q(ecve.target.sigmaD + ecve.getDistance() / 1000 * ecve.target.ppmD) + pow2q(ecve.target.sigmaInstrCentering);
	
	ScaleMeasContrib ecspContrib = { calcMeas, stationContrib, pointLineContrib, stationTransfContributions, pointLineTransfContributions, obsVariance };
	return ecspContrib;
}

//ECSP Contribution
ECSPContrib	 TContributionsGenerator::getECSPContrib(const TECSPROM& ecspROM, const TECSP& ecsp){
	TPositionVector stationPoint = ecsp.targetPos->getEstimatedValue();
	const TLOR2LOR& stationPTLor2RootTrafo = fPointTransfo.getLORTransformation(ecsp.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	stationPTLor2RootTrafo.transform(stationPoint);

	TPositionVector linePoint1 = ecspROM.p1->getEstimatedValue();
	const TLOR2LOR& linePTLor2RootTrafo1 = fPointTransfo.getLORTransformation(ecspROM.p1->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	linePTLor2RootTrafo1.transform(linePoint1);

	TPositionVector linePoint2 = ecspROM.p2->getEstimatedValue();
	const TLOR2LOR& linePTLor2RootTrafo2 = fPointTransfo.getLORTransformation(ecspROM.p2->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	linePTLor2RootTrafo2.transform(linePoint2);

	// contributions can be calculated simply in both the CCS and a local 
	// reference frame since no instrument or target heights are involved
	// and the measurement is independent of the local vertical
	// Therefore there are no transformations necessary
	//PARAMETERS IN CARTESIAN REFERENCE SYSTEM

	/////////////////////Prepare coefficients (a,b,c) and calculate observation value (calcMeas)////////////////////////////////////////////

	//Calcul par le produit scalaire (u^l)²+(u.l)²=|v|²|l|²
	TReal dis, pScal, D;
	dis = dist3D(stationPoint.getX(), stationPoint.getY(), stationPoint.getZ(), linePoint1.getX(), linePoint1.getY(), linePoint1.getZ());  // distance P1 - stn
	D = dist3D(linePoint1.getX(), linePoint1.getY(), linePoint1.getZ(), linePoint2.getX(), linePoint2.getY(), linePoint2.getZ());// distance P1-P2
	pScal = (linePoint2.getX() - linePoint1.getX()).getMetresValue() * (stationPoint.getX() - linePoint1.getX()).getMetresValue()
		+ (linePoint2.getY() - linePoint1.getY()).getMetresValue() * (stationPoint.getY() - linePoint1.getY()).getMetresValue()
		+ (linePoint2.getZ() - linePoint1.getZ()).getMetresValue() * (stationPoint.getZ() - linePoint1.getZ()).getMetresValue();  //produit scalaire

	//contributions
	TReal a, b, c, d, e, f, g,h,i;
	TReal div = sqrt( dis*dis*D*D - pow2(pScal));
	if (div > nullLimit)
	{
		//stn
		a = 1.0/div * (D*(stationPoint.getX() - linePoint1.getX()) - pScal/D *(linePoint2.getX() - linePoint1.getX()));
		b = 1.0/div * (D*(stationPoint.getY() - linePoint1.getY()) - pScal/D *(linePoint2.getY() - linePoint1.getY()));
		c = 1.0/div * (D*(stationPoint.getZ() - linePoint1.getZ()) - pScal/D *(linePoint2.getZ() - linePoint1.getZ()));
		//p2
		d = 1.0 / div * (-pScal / D *(stationPoint.getX() - linePoint1.getX()) + pow2(pScal) / pow(D, 3) * (linePoint2.getX() - linePoint1.getX()));
		e = 1.0 / div * (-pScal / D *(stationPoint.getY() - linePoint1.getY()) + pow2(pScal) / pow(D, 3) * (linePoint2.getY() - linePoint1.getY()));
		f = 1.0 / div * (-pScal / D *(stationPoint.getZ() - linePoint1.getZ()) + pow2(pScal) / pow(D, 3) * (linePoint2.getZ() - linePoint1.getZ()));
		//p1
		g = 1.0 / div * (-D*(stationPoint.getX() - linePoint1.getX()) + pScal / D *(linePoint2.getX() - linePoint1.getX() + stationPoint.getX() - linePoint1.getX()) - pow2(pScal) / pow(D, 3) * (linePoint2.getX() - linePoint1.getX()));
		h = 1.0 / div * (-D*(stationPoint.getY() - linePoint1.getY()) + pScal / D *(linePoint2.getY() - linePoint1.getY() + stationPoint.getY() - linePoint1.getY()) - pow2(pScal) / pow(D, 3) * (linePoint2.getY() - linePoint1.getY()));
		i = 1.0 / div * (-D*(stationPoint.getZ() - linePoint1.getZ()) + pScal / D *(linePoint2.getZ() - linePoint1.getZ() + stationPoint.getZ() - linePoint1.getZ()) - pow2(pScal) / pow(D, 3) * (linePoint2.getZ() - linePoint1.getZ()));
	}
	else
		throw std::logic_error("TContributionGenerator::getECSPContrib: Division by zero.");

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	TFreeVector stationContrib = getPointContributions(stationPTLor2RootTrafo, a, b, c);
	addTransformationsContributions(stationPTLor2RootTrafo, ecsp.targetPos->getEstimatedValue(), a, b, c, stationTransfContributions);

	//first point on the line
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> pointLineTransfContributions1;
	TFreeVector pointLineContrib1 = getPointContributions(linePTLor2RootTrafo1, g, h, i);
	addTransformationsContributions(linePTLor2RootTrafo1, ecsp.targetPos->getEstimatedValue(), g, h, i, pointLineTransfContributions1);
	//second point on the line
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> pointLineTransfContributions2;
	TFreeVector pointLineContrib2 = getPointContributions(linePTLor2RootTrafo2, d, e, f);
	addTransformationsContributions(linePTLor2RootTrafo2, ecsp.targetPos->getEstimatedValue(), d, e, f, pointLineTransfContributions2);

	// Variance calculation
	TReal varM = pow2q(ecsp.target.sigmaD + ecsp.getDistance() / 1000 * ecsp.target.ppmD);
	TReal obsVariance = varM + (pow2q((linePoint2.getX() - linePoint1.getX()) / D) + pow2q((linePoint2.getY() - linePoint1.getY()) / D))*pow2q(ecsp.target.sigmaInstrCentering);
	
	TReal calcmeas = div / D - ecsp.target.distCorrectionValue;
	ECSPContrib ecspContrib = { calcmeas , stationContrib, pointLineContrib1, pointLineContrib2, stationTransfContributions, pointLineTransfContributions1, pointLineTransfContributions2, obsVariance };
	return ecspContrib;
	

}

//DVER contributions
DVERContrib	TContributionsGenerator::getDVERContrib(const TDVER& dver){
	fPointTransfo.setMLA(false);
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

	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(dver.station->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Station's position frame
	stLor2RootTrafo.transform(station); //Transform to ROOT(CCS)

	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(dver.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Station's position frame
	tgLor2RootTrafo.transform(target); //Transform to ROOT(CCS)


	TPositionVector stationCCS = station;
	TPositionVector targetCCS = target;

	TReal  dh = 0.0;

	if(fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){  /*Needs to be calculated in CGRF.*/
		fPointTransfo.set2MLATransformation(station);
		fPointTransfo.transformMLA2CGRF(stationC); // transform to CGRF
		
		fPointTransfo.set2MLATransformation(target);
		fPointTransfo.transformMLA2CGRF(targetC);  // transform to CGRF

		if(fPointTransfo.getRefFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS){
			TXYH2CCS::CCS2XYHs(station);
			TXYH2CCS::CCS2XYHs(target);
		}
		else if(fPointTransfo.getRefFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine){
			TXYH2CCS::CCS2XYHg2000Machine(station);
			TXYH2CCS::CCS2XYHg2000Machine(target);
		}
		else if (fPointTransfo.getRefFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine){
			TXYH2CCS::CCS2XYHg1985Machine(station);
			TXYH2CCS::CCS2XYHg1985Machine(target);
		}
		/*IMPORTANT ENABLE CGRF, APPLIES IN POINT AND TRANSFORMATION CONTRIBUTIONS*/
		fPointTransfo.setCGRF(true);
		//Calculating the distance meas
		dh = target.getH().getMetresValue() - station.getH().getMetresValue() - dver.getDistanceCorrection();

	}
	else{ /*OLOC = Calculated as XYZ, simple case*/
		fPointTransfo.setCGRF(false);
		dh = target.getZ().getMetresValue() - station.getZ().getMetresValue() - dver.getDistanceCorrection();
		
	}
	
	TFreeVector stationContrib = getPointContributions(stLor2RootTrafo,stationC.getX().getMetresValue(),stationC.getY().getMetresValue(),stationC.getZ().getMetresValue());
	addTransformationsContributions(stLor2RootTrafo,stationLOR,0,0,-1,stationTransfContributions);
	
	TFreeVector targetContrib = getPointContributions(tgLor2RootTrafo, targetC.getX().getMetresValue(),targetC.getY().getMetresValue(),targetC.getZ().getMetresValue());
	addTransformationsContributions(tgLor2RootTrafo,targetLOR, 0, 0, 1, targetTransfContributions);

	//Reset the CGRF status (can have other effect on the code)
	fPointTransfo.setCGRF(false);

	DVERContrib dverC =  { dh, stationContrib, targetContrib, stationTransfContributions, targetTransfContributions, pow2q( dver.getObservedStDev())};
	
	return dverC;
}

//ORIE contributions
AnglMeasContrib	TContributionsGenerator::getOrieContrib(const TORIEROM& orieROM, const TORIE& orie){
	//Transform TARGET and STATION in a LOCAL ASTRONOMICAL FRAME
	TPositionVector targetPos = orie.targetPos->getEstimatedValue();
	const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(orie.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "TARGET FRAME" to "ROOT"
	tgLor2RootTrafo.transform(targetPos);
	
	TPositionVector stationPos = orieROM.instrumentPos->getEstimatedValue();
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(orieROM.instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	stLor2RootTrafo.transform(stationPos);
	
	// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame){
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

	//Calculated measurement value
	TAngle calcMeas = TAngle::aTan2((xTg - xSt), (yTg - ySt)) - orieROM.fConstantAngle;

	TReal dist2 = pow2q(dist(xSt, ySt, xTg, yTg));
	if (dist2 < nullLimit)
		throw std::logic_error("TContributionGenerator::getOrieContrib: Division by zero because observation points have identical coordinates.");

	TReal a, b, c; //station's contributions coefficients (negative values of these give the target coefficients)		
	a = (-LITERAL(1.0) * (yTg - ySt)) / dist2; //xSt coefficient
	b = (xTg - xSt) / dist2; //ySt coefficient
	c = 0.0; //zSt coefficient

	TReal v0Contrib = 0.0; //no V0 parameter for a gyro theodolithe
	TReal hiContrib = 0.0; // no contribution for the instrument height

	TFreeVector abc(a, b, c, TCoordSysFactory::k3DCartesian);

	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		fPointTransfo.getLA2MLA().transform(abc);
	

	//Station can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribStation = getPointContributions(stLor2RootTrafo, abc.getX().getMetresValue(), abc.getY().getMetresValue(), abc.getZ().getMetresValue());
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> stationTransfContributions;
	addTransformationsContributions(stLor2RootTrafo, orieROM.instrumentPos->getEstimatedValue(), abc.getX().getMetresValue(), abc.getY().getMetresValue(), abc.getZ().getMetresValue(), stationTransfContributions);

	//Target can be defined anywhere, get point contributions and transformations contributions
	TFreeVector coordContribTarget = getPointContributions(tgLor2RootTrafo, -abc.getX().getMetresValue(), -abc.getY().getMetresValue(), -abc.getZ().getMetresValue());
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> targetTransfContributions;
	addTransformationsContributions(tgLor2RootTrafo, orie.targetPos->getEstimatedValue(), -abc.getX().getMetresValue(), -abc.getY().getMetresValue(), -abc.getZ().getMetresValue(), targetTransfContributions);

	// Variance calculation
	TReal variance = pow2q(orie.target.sigmaAngl.getRadiansValue()) + (1.0 / pow2q(dist2)) * (pow2q(orieROM.instrument.sigmaInstrCentering) + pow2q(orie.target.sigmaTargetCentering));

	AnglMeasContrib  contrib = { calcMeas, coordContribStation, coordContribTarget, stationTransfContributions, targetTransfContributions, hiContrib, v0Contrib, variance };
	return contrib;

	
}

//PDOR contribution
PtOrientationContrib	TContributionsGenerator::getPDORContrib(const TPdorObs& pdorObs)
{
	//The fixed point is defined in root
	TPositionVector fixedPt = pdorObs.calaPt->getEstimatedValue();

	TPositionVector oriPt = pdorObs.orientationPt->getEstimatedValue();
	const TLOR2LOR& oriPtLor2RootTrafo = fPointTransfo.getLORTransformation(pdorObs.orientationPt->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transform station to ROOT 
	oriPtLor2RootTrafo.transform(oriPt);

	TReal xFix = fixedPt.getX().getMetresValue();
	TReal yFix = fixedPt.getY().getMetresValue();

	TReal xRef = oriPt.getX().getMetresValue();
	TReal yRef = oriPt.getY().getMetresValue();

	TReal D = dist(xFix, yFix, xRef, yRef);
	if (D < nullLimit)
		throw std::logic_error("TContributionsGenerator::getPDORContrib: Division by zero because observation points have identical coordinates.");

	// GKA (26/09/2019) : change of definition to a bearing, changes also made in TLSInputMatricesFiller
	TReal calcmeas;
	if (pdorObs.isBearingDefined())
		calcmeas = (TAngle::aTan2((xRef - xFix), (yRef - yFix))) - pdorObs.getBearing().getRadiansValue();
	else
	{
		TPositionVector oriPtPro = pdorObs.orientationPt->getProvisionalValue();
		TReal xRefPro = oriPtPro.getX().getMetresValue();
		TReal yRefPro = oriPtPro.getY().getMetresValue();
		calcmeas = (TAngle::aTan2((xRef - xFix), (yRef - yFix))) - (TAngle::aTan2((xRefPro - xFix), (yRefPro - yFix)));
	}

	//CALCULATION OF THE CONTRIBUTION IN LOCAL INSTRUMENT SYSTEM	
	TReal a = (yRef - yFix) / powq(D, 2);//xFix coefficient
	TReal b = -(xRef - xFix) / powq(D, 2);//yFix coefficient
	TReal c = 0.0;


	//Target can be defined anywhere, get point contributions and transformations contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fRefPtTransformContrib;
	TFreeVector oriPointContrib = getPointContributions(oriPtLor2RootTrafo, a, b, c);
	addTransformationsContributions(oriPtLor2RootTrafo, oriPt, a, b, c, fRefPtTransformContrib);

	
	return{ oriPointContrib, fRefPtTransformContrib, calcmeas };

}

//RADI contribution
PtOrientationContrib	TContributionsGenerator::getRADIContrib(const TRADI& radi)
{
	//Constraint made in CCS, later could be extended to subframe
	fPointTransfo.setMLA (false);
	
	TPositionVector prov = radi.station->getProvisionalValue();
	TPositionVector estimated = radi.station->getEstimatedValue();
	const TLOR2LOR& Lor2RootTrafo = fPointTransfo.getLORTransformation(radi.station->getFrameTreePosition(), fPointTransfo.getTree()->begin()); // Transform to ROOT 
	Lor2RootTrafo.transform(estimated);
	Lor2RootTrafo.transform(prov);

	TReal xp = prov.getX().getMetresValue();
	TReal yp = prov.getY().getMetresValue();

	TReal xe = estimated.getX().getMetresValue();
	TReal ye = estimated.getY().getMetresValue();

	TAngle bear = radi.getAngleCnstr();


	//gets calc value and sigma
	TLength calcmeas = TLength(-LITERAL(1.0) * sinq(bear) * (ye - yp) + cosq(bear) * (xe - xp));

	//calculated contibutions in a local system
	TReal a = -LITERAL(1.0) * cosq(bear);// xPt coefficient
	TReal b = sinq(bear);//yPt coefficient
	TReal c = LITERAL(0.0);//zPt coefficient


	//Point can be defined anywhere, get point contributions and transformations contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> festimatedPtTransformContrib;
	TFreeVector estimatedPointContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian);
	addTransformationsContributions(Lor2RootTrafo, estimated, a, b, c, festimatedPtTransformContrib);

	return{ estimatedPointContrib, festimatedPtTransformContrib, calcmeas };
}

//OBSXYZ contribution
OBSXYZContrib  TContributionsGenerator::getOBSXYZContrib(const TOBSXYZ& OBSXYZ)
{
	TPositionVector estimated = OBSXYZ.station->getEstimatedValue();  //tgt
	TPositionVector prov = OBSXYZ.initialValue;  //stn
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(OBSXYZ.positionInTree, OBSXYZ.station->getFrameTreePosition()); // Transform to frame in which point is defined 
	stLor2RootTrafo.transform(prov);

	//ets calc value
	TReal dx = (estimated.getX().getMetresValue() - prov.getX().getMetresValue());
	TReal dy = (estimated.getY().getMetresValue() - prov.getY().getMetresValue());
	TReal dz = (estimated.getZ().getMetresValue() - prov.getZ().getMetresValue());

	//Contributions for the coordinates
	Point3DContrib coordContribStation = { TFreeVector(1.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian), TFreeVector(0.0, 1.0, 0.0, TCoordSysFactory::k3DCartesian), TFreeVector(0.0, 0.0, 1.0, TCoordSysFactory::k3DCartesian) };

	//Fill transformation contributions
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> TransfContributions;
	addUVDTgTransfContributionsCamera(stLor2RootTrafo, OBSXYZ.initialValue, TransfContributions);

	return{ coordContribStation, TransfContributions, { dx, dy, dz } };
}

//INCLY contribution
INCLYContrib  TContributionsGenerator::getINCLYContrib(const TINCLYROM& inclST, const TINCLY& incly)
{
	fPointTransfo.setMLA(false);

	//Transform the point of meeasure to the Root frame
	TPositionVector stationPos = incly.targetPos->getEstimatedValue(); //get the position of the station
	const TLOR2LOR& ptLor2RootTrafo = fPointTransfo.getLORTransformation(incly.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	ptLor2RootTrafo.transform(stationPos);

	//Creating the Local Vertical vector (no change if OLOC)
	TFreeVector stationV(0,0,1, TCoordSysFactory::k3DCartesian);

	//Express te local vertical if MLA is used
	if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame) {
		fPointTransfo.set2MLATransformation(stationPos);
		fPointTransfo.transformMLA2CGRF(stationV);
		fPointTransfo.transformCGRF2CCS(stationV);
	}
	
	//Transform the local vertical in the station LOR
	const TLOR2LOR& vert2stTrafo = fPointTransfo.getLORTransformation(fPointTransfo.getTree()->begin(), inclST.positionInTree); //Trafo from from CCS LOR to station's LOR
	vert2stTrafo.transform(stationV);

	//Compute the calcMeas, watchout for the sign of the correction, with - it is the definition of the ref angle
	TReal XSt = stationV.getX().getMetresValue();
	TReal ZSt = stationV.getZ().getMetresValue();
	TAngle calcMeas =  TAngle::aTan2(-XSt, ZSt) - incly.target.angleCorrectionValue - incly.target.refAngleCorrectionValue;
	
	//Add the transformation contribution
	
	//Transform the projected vector from the station frame to the rootframe
	const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(inclST.positionInTree, fPointTransfo.getTree()->begin()); //Transformation from "STATION FRAME" to "ROOT"
	TPositionVector ProjLocalV(XSt, 0, ZSt, TCoordSysFactory::ECoordSys::k3DCartesian);
	stLor2RootTrafo.transform(ProjLocalV);

	//Other method possible: use Partial derivative for vectors TLOR2LOR::partialDerivativesAngle (TO develop) with:
	//TFreeVector ProjLocalV(XSt, 0, ZSt, TCoordSysFactory::ECoordSys::k3DCartesian);
	
	//Compute the variance of the observation
	TReal obsVariance = pow2q(incly.target.sigmaAngl.getRadiansValue()) + pow2q(incly.target.sigmaCorrectionValue.getRadiansValue()) + pow2q(incly.target.refSigmaCorrectionValue.getRadiansValue());
	
	// CalcMeas, transformationContributions, variance
	return { calcMeas, addINCLContributions(vert2stTrafo, ProjLocalV, XSt, ZSt) , obsVariance };
}

//ECWS contribution
ECWSContrib	TContributionsGenerator::getECWSContrib(const TECWSROM& ecwsROM, const TECWS& ecws) {
	
	//Get the measured distance to the water surface
	TReal dWS = ecwsROM.fMeasuredWS->getMetresValue(); // Distance from the reference point to the WS
	
	//Get the observed WS 1-sigma precision
	TReal obsWSSigma = ecwsROM.fSigmaWS->getMMetresValue();

	TPositionVector snrPoint = ecws.targetPos->getEstimatedValue();

	//Staton point defined at root frame
	const TLOR2LOR& snrPTLor2RootTrafo = fPointTransfo.getLORTransformation(ecws.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin());
	snrPTLor2RootTrafo.transform(snrPoint);

	//Obs equation
	TReal calcMeas = snrPoint.getZ().getMetresValue() - dWS;


	ECWSContrib ecwsContrib = { calcMeas, dWS, obsWSSigma};
	return ecwsContrib;

}

//////////////////////////////////////////////////////////////////////
// CONTRIBUTIONS CALCULATION -- CAMERA measurements (UVEC/UVD)
//////////////////////////////////////////////////////////////////////
UVECContrib	TContributionsGenerator::getUVECContrib(const TCAM& camera, const TUVEC& uvec){
	fPointTransfo.setMLA (false);  // TCAM measurements are never in MLA

	const TLOR2LOR& tg2stTrafo = fPointTransfo.getLORTransformation(uvec.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); //Trafo from from target's LOR to station's LOR
	TPositionVector targetPos = uvec.targetPos->getEstimatedValue();
	tg2stTrafo.transform(targetPos);	

	const TFreeVector& unitVec = uvec.getVectorValue(); // observed vector (X and Y), Z is not observed
	TReal i = unitVec.getX().getMetresValue();
	TReal j = unitVec.getY().getMetresValue();
	TReal k = unitVec.getZ().getMetresValue();

	const TPositionVector& instrEstimValue = camera.instrumentPos->getEstimatedValue();
	TReal dx = targetPos.getX().getMetresValue() - instrEstimValue.getX().getMetresValue();
	TReal dy = targetPos.getY().getMetresValue() - instrEstimValue.getY().getMetresValue();
	TReal dz = targetPos.getZ().getMetresValue() - instrEstimValue.getZ().getMetresValue();

	TFreeVector stFirstEqContrib(-k/dz, 
						  0.0, 
						  k*(dx/pow2q(dz)), TCoordSysFactory::k3DCartesian);

	TFreeVector stSecondEqContrib(0, 
						  -k/dz, 
						  k*(dy/pow2q(dz)), TCoordSysFactory::k3DCartesian);


	TFreeVector tgFirstEqContrib(k*(-(dx/pow2q(dz))*tg2stTrafo.partDerivWRespToX0().getZ().getMetresValue() + (1/dz)*tg2stTrafo.partDerivWRespToX0().getX().getMetresValue()), 
						  k*(-(dx/pow2q(dz))*tg2stTrafo.partDerivWRespToY0().getZ().getMetresValue() + (1/dz)*tg2stTrafo.partDerivWRespToY0().getX().getMetresValue()), 
						  k*(-(dx/pow2q(dz))*tg2stTrafo.partDerivWRespToZ0().getZ().getMetresValue() + (1/dz)*tg2stTrafo.partDerivWRespToZ0().getX().getMetresValue()), TCoordSysFactory::k3DCartesian);

	TFreeVector tgSecondEqContrib(k*(-(dy/pow2q(dz))*tg2stTrafo.partDerivWRespToX0().getZ().getMetresValue() + (1/dz)*tg2stTrafo.partDerivWRespToX0().getY().getMetresValue()), 
						  k*(-(dy/pow2q(dz))*tg2stTrafo.partDerivWRespToY0().getZ().getMetresValue() + (1/dz)*tg2stTrafo.partDerivWRespToY0().getY().getMetresValue()), 
						  k*(-(dy/pow2q(dz))*tg2stTrafo.partDerivWRespToZ0().getZ().getMetresValue() + (1/dz)*tg2stTrafo.partDerivWRespToZ0().getY().getMetresValue()), TCoordSysFactory::k3DCartesian);


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
			TFreeVector(k*(-(1/pow2q(dz))*omegaPD.getZ().getMetresValue()*dx + (1/dz)*omegaPD.getX().getMetresValue()), 
						  k*(-(1/pow2q(dz))*phiPD.getZ().getMetresValue()*dx + (1/dz)*phiPD.getX().getMetresValue()), 
						  k*(-(1/pow2q(dz))*kappaPD.getZ().getMetresValue()*dx + (1/dz)*kappaPD.getX().getMetresValue()), TCoordSysFactory::k3DCartesian),
			TFreeVector(k*((-1/pow2q(dz))*trans1PD.getZ().getMetresValue()*dx + (1/dz)*trans1PD.getX().getMetresValue()), 
						  k*(-(1/pow2q(dz))*trans2PD.getZ().getMetresValue()*dx + (1/dz)*trans2PD.getX().getMetresValue()), 
						  k*(-(1/pow2q(dz))*trans3PD.getZ().getMetresValue()*dx + (1/dz)*trans3PD.getX().getMetresValue()), TCoordSysFactory::k3DCartesian),
				k*(-(1/pow2q(dz))*scalePD.getZ().getMetresValue()*dx + (1/dz)*scalePD.getX().getMetresValue())
		};

		TransformationContrib secondEqContrib = {
			TFreeVector(k*(-(1/pow2q(dz))*omegaPD.getZ().getMetresValue()*dy + (1/dz)*omegaPD.getY().getMetresValue()), 
						  k*(-(1/pow2q(dz))*phiPD.getZ().getMetresValue()*dy + (1/dz)*phiPD.getY().getMetresValue()), 
						 k*(-(1/pow2q(dz))*kappaPD.getZ().getMetresValue()*dy + (1/dz)*kappaPD.getY().getMetresValue()), TCoordSysFactory::k3DCartesian),
			TFreeVector(k*(-(1/pow2q(dz))*trans1PD.getZ().getMetresValue()*dy + (1/dz)*trans1PD.getY().getMetresValue()), 
						  k*(-(1/pow2q(dz))*trans2PD.getZ().getMetresValue()*dy + (1/dz)*trans2PD.getY().getMetresValue()), 
						  k*(-(1/pow2q(dz))*trans3PD.getZ().getMetresValue()*dy + (1/dz)*trans3PD.getY().getMetresValue()), TCoordSysFactory::k3DCartesian),
						k*(-(1/pow2q(dz))*scalePD.getZ().getMetresValue()*dy + (1/dz)*scalePD.getY().getMetresValue())
		};

		TransformationContrib2D trafoContrib = {firstEqContrib, secondEqContrib};

		targetTransfContributions.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib2D> (*it->adjTrafo, trafoContrib));
	}	
//End of filling transformation contributions

	UVECContrib contrib = {stFirstEqContrib, stSecondEqContrib, tgFirstEqContrib, tgSecondEqContrib, targetTransfContributions, 
						   {-1.0, 0.0}, // X contribution (i) for a first and second equation
				           {0.0, -1.0}, // Y contribution (j) for a first and second equation
						   {-i+(k/dz)*dx, -j+(k/dz)*dy}, //Misclosure vector for a first and second equation
						   { pow2q(uvec.target.sigmaX) + pow2q(k / (dz)*(uvec.target.sigmaTargetCentering + camera.instrument.sigmaInstrCentering)), pow2q(uvec.target.sigmaY) + pow2q(k / (dz)*(uvec.target.sigmaTargetCentering + camera.instrument.sigmaInstrCentering)) } }; //Obs variances
	return contrib;
}

UVDContrib	TContributionsGenerator::getUVDContrib(const TCAM& camera, const TUVD& uvd){
	fPointTransfo.setMLA (false); // TCAM measurements never in MLA
	TPositionVector targetPos = uvd.targetPos->getEstimatedValue();
	const TLOR2LOR& tg2stTrafo = fPointTransfo.getLORTransformation(uvd.targetPos->getFrameTreePosition(), camera.instrumentPos->getFrameTreePosition()); // Transformation to LOR of the Camera
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
	TReal i = uvd.getVectorValue().getX().getMetresValue(); // X component
	TReal j = uvd.getVectorValue().getY().getMetresValue(); // Y component
	TReal k = uvd.getVectorValue().getZ().getMetresValue(); // Z component

	TReal dx = targetPos.getX().getMetresValue() - camera.instrumentPos->getEstimatedValue().getX().getMetresValue();
	TReal dy = targetPos.getY().getMetresValue() - camera.instrumentPos->getEstimatedValue().getY().getMetresValue();
	TReal dz = targetPos.getZ().getMetresValue() - camera.instrumentPos->getEstimatedValue().getZ().getMetresValue();

	UVDContrib contrib = {coordContribStation, coordContribTarget, targetTransfContributions, 
		{sDist,0.0, 0.0}, // i
		{0.0,sDist, 0.0}, //j
		{i, j, k}, // dist contrib
		{sDist*i - dx, sDist*j - dy, sDist*k - dz}, //misclosure vector
		{ pow2q(uvd.target.sigmaX) + pow2q(uvd.target.sigmaTargetCentering) + pow2(camera.instrument.sigmaInstrCentering), pow2q(uvd.target.sigmaY) + pow2q(uvd.target.sigmaTargetCentering) + pow2(camera.instrument.sigmaInstrCentering), pow2q(uvd.target.sigmaDist) + pow2q(uvd.target.sigmaTargetCentering) + pow2(camera.instrument.sigmaInstrCentering) } };   //variances
	
	return contrib;
}


///////////////////////////////////////////////////////////////////////////
// PRIVATE / SUPPORTING METHODS
///////////////////////////////////////////////////////////////////////////
TFreeVector TContributionsGenerator::getPointContributions(const TLOR2LOR& lorTrafo, TReal a, TReal b, TReal c){
	TFreeVector derX0 = lorTrafo.partDerivWRespToX0();
	TFreeVector derY0 = lorTrafo.partDerivWRespToY0();
	TFreeVector derZ0 = lorTrafo.partDerivWRespToZ0();

	if(fPointTransfo.getMLAused()){ //Transform partial derivatives into ILA if necessary
		fPointTransfo.transform2MLA(derX0);
		fPointTransfo.transform2MLA(derY0);
		fPointTransfo.transform2MLA(derZ0);
	}
	else if (fPointTransfo.getCGRFused()){
		fPointTransfo.getCCS2CGRF().transform(derX0);
		fPointTransfo.getCCS2CGRF().transform(derY0);
		fPointTransfo.getCCS2CGRF().transform(derZ0);
	}

	TReal xContrib = a * derX0.getX().getMetresValue() + b * derX0.getY().getMetresValue() + c* derX0.getZ().getMetresValue();
	TReal yContrib = a * derY0.getX().getMetresValue() + b * derY0.getY().getMetresValue() + c* derY0.getZ().getMetresValue();
	TReal zContrib = a * derZ0.getX().getMetresValue() + b * derZ0.getY().getMetresValue() + c* derZ0.getZ().getMetresValue();
	
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

			if(fPointTransfo.getMLAused()){
				fPointTransfo.transform2MLA(omegaPD);
				fPointTransfo.transform2MLA(phiPD);
				fPointTransfo.transform2MLA(kappaPD);

				fPointTransfo.transform2MLA(trans1PD);
				fPointTransfo.transform2MLA(trans2PD);
				fPointTransfo.transform2MLA(trans3PD);

				fPointTransfo.transform2MLA(scalePD);
			}
			else if (fPointTransfo.getCGRFused()){
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

		if(fPointTransfo.getMLAused()){ //If MLA used, then transform contributions
			fPointTransfo.transform2MLA(omegaDerivative);
			fPointTransfo.transform2MLA(phiDerivative);
			fPointTransfo.transform2MLA(kappaDerivative);

			fPointTransfo.transform2MLA(t1Derivative);
			fPointTransfo.transform2MLA(t2Derivative);
			fPointTransfo.transform2MLA(t3Derivative);

			fPointTransfo.transform2MLA(scaleDeriv);
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

	if(fPointTransfo.getMLAused()){
		fPointTransfo.transform2MLA(partDerWRespToX0);
		fPointTransfo.transform2MLA(partDerWRespToY0);
		fPointTransfo.transform2MLA(partDerWRespToZ0);
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
			transformationName = it->adjTrafo->getName();

			//Contributions for rotations : Omega, Phi and Kappa
			omegaDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 0);
			phiDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 1);
			kappaDerivative = transformations.partialDerivativesAngle(transformationName, pointPos, 2);

			//Contributions for translation: X, Y and Z coordinate
			t1Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 0);
			t2Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 1); 
			t3Derivative = transformations.partialDerivativesTranslation(transformationName, pointPos, 2);

			scaleDeriv = transformations.partialDerivativesScale(transformationName, pointPos);


			TransformationContrib firstEqContrib = {TFreeVector(-omegaDerivative.getX().getMetresValue(),-phiDerivative.getX().getMetresValue(), -kappaDerivative.getX().getMetresValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- t1Derivative.getX().getMetresValue(), -t2Derivative.getX().getMetresValue(), -t3Derivative.getX().getMetresValue(),TCoordSysFactory::k3DCartesian),
													- scaleDeriv.getX().getMetresValue()};

			TransformationContrib secondEqContrib = {TFreeVector(-  omegaDerivative.getY().getMetresValue(), -  phiDerivative.getY().getMetresValue(), - kappaDerivative.getY().getMetresValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- t1Derivative.getY().getMetresValue(), - t2Derivative.getY().getMetresValue(), - t3Derivative.getY().getMetresValue(),TCoordSysFactory::k3DCartesian),
													-  scaleDeriv.getY().getMetresValue()};

			TransformationContrib thirdEqContrib = {TFreeVector(- omegaDerivative.getZ().getMetresValue(), - phiDerivative.getZ().getMetresValue(), -  kappaDerivative.getZ().getMetresValue(),TCoordSysFactory::k3DCartesian),
													TFreeVector(- t1Derivative.getZ().getMetresValue(), - t2Derivative.getZ().getMetresValue(), -  t3Derivative.getZ().getMetresValue(),TCoordSysFactory::k3DCartesian),
													- scaleDeriv.getZ().getMetresValue()};

			TransformationContrib3D trContrib = {firstEqContrib, secondEqContrib, thirdEqContrib};

			transfContrib.push_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib3D> (*it->adjTrafo, trContrib));
	}
}

decltype(INCLYContrib::fStTransformContrib) TContributionsGenerator::addINCLContributions(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, TReal numerator, TReal denominator) {
	const std::vector<TLOR2LOR::TransformAndParams>& trafoChain = lorTrafo.getTransformationChain();
	
	std::string transformationName;
	TFreeVector scalePD(TCoordSysFactory::k3DCartesian);
	TFreeVector omegaPD(TCoordSysFactory::k3DCartesian);
	TFreeVector phiPD(TCoordSysFactory::k3DCartesian);
	TFreeVector kappaPD(TCoordSysFactory::k3DCartesian);

	TReal omegaContrib, phiContrib, kappaContrib, scaleContrib;
	
	decltype(INCLYContrib::fStTransformContrib) transfContrib;

	// Iterate through the transformations, calculate contributions and store them in the vector of pairs 'transfContrib'
	for (const auto& it : trafoChain) {
		transformationName = it.adjTrafo->getName();
		omegaPD = lorTrafo.partialDerivativesAngle(transformationName, pointPos, 0);
		phiPD = lorTrafo.partialDerivativesAngle(transformationName, pointPos, 1);
		kappaPD = lorTrafo.partialDerivativesAngle(transformationName, pointPos, 2);
		scalePD = lorTrafo.partialDerivativesScale(transformationName, pointPos);

		if ((pow2q(numerator) + pow2q(denominator)) < nullLimit)
			throw std::logic_error("TContributionGenerator::getINCLYContrib: Division by zero because observation points have identical coordinates.");

		omegaContrib = -1.0 * ((omegaPD.getX().getMetresValue()) * denominator - numerator * (omegaPD.getZ().getMetresValue())) / (pow2q(numerator) + pow2q(denominator));
		phiContrib = -1.0 * ((phiPD.getX().getMetresValue()) * denominator - numerator * (phiPD.getZ().getMetresValue())) / (pow2q(numerator) + pow2q(denominator));
		kappaContrib = -1.0 * ((kappaPD.getX().getMetresValue()) * denominator - numerator * (kappaPD.getZ().getMetresValue())) / (pow2q(numerator) + pow2q(denominator));
		scaleContrib = -1.0 * ((scalePD.getX().getMetresValue()) * denominator - numerator * (scalePD.getZ().getMetresValue())) / (pow2q(numerator) + pow2q(denominator));

		TransformationContrib trContrib = { TFreeVector(omegaContrib, phiContrib, kappaContrib, TCoordSysFactory::k3DCartesian), TFreeVector(0,0,0,TCoordSysFactory::k3DCartesian),scaleContrib };
		transfContrib.emplace_back(std::pair<TAdjustableHelmertTransformation, TransformationContrib>(*it.adjTrafo, trContrib));
	}

	return transfContrib;
}

