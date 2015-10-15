#include "TAdjustablePlane.h"

TAdjustablePlane::TAdjustablePlane(const TAdjustablePoint* referencePoint, const TScalar& refPointDistance, const TFreeVector& planeNormalVect, const std::bitset<3>& planeNormalVectLock, const std::string& name)
	:
	fName(name),
	fReferencePoint(referencePoint),
	fProvValRefPtDist(refPointDistance),
	fCorrectionRefPtDist(LITERAL(0.0)),
	fEstValRefPointDist(refPointDistance),
	fEstPrecisionRefPtDist(LITERAL(0.0)),

	fProvValPlaneNormalVector(planeNormalVect),
	fCorrectionPlaneNormalVector(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
	fEstValPlaneNormalVector(planeNormalVect),
	fEstPrecisionPlaneNormalVector(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
	fCovariancePlaneNormalVector(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
	fixedStateRpDistance(false),
	fixedStatePlaneVector(planeNormalVectLock),
	fInitialized(true)
{
	uidx_planeVector[0] = -1;
	uidx_planeVector[1] = -1;
	uidx_planeVector[2] = -1;
	uidx_rpDistance		= -1;	
}

TAdjustablePlane TAdjustablePlane::createUninitialized(const std::string& name){
	TAdjustablePlane ap(nullptr, TScalar(NO_VALf), TFreeVector(NO_VALf,  NO_VALf,  NO_VALf,TCoordSysFactory::ECoordSys::k3DCartesian), std::bitset<3>(std::string("111")), name);
	ap.fInitialized = false;
	return ap;
}

void TAdjustablePlane::initialize(const TAdjustablePoint* referencePoint, const TScalar& refPointDistance, const TFreeVector& planeNormalVect, const std::bitset<3>& planeNormalVectLock){
	fReferencePoint = referencePoint;
	fProvValRefPtDist = refPointDistance;
	fEstValRefPointDist = refPointDistance;
	fProvValPlaneNormalVector = planeNormalVect;
	fEstValPlaneNormalVector = planeNormalVect;
	fixedStatePlaneVector = planeNormalVectLock;
	fInitialized = true;
}

void TAdjustablePlane::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed plane.");
	
	for (int i = 0; i < 3; i++)
		if (!fixedStatePlaneVector[i])
			uidx_planeVector[i] = idx++;
	//Distance is in any case variable
		uidx_rpDistance = idx++;
}

void TAdjustablePlane::setCorrection(int idx, TReal value){
	if (uidx_rpDistance == idx){
		fCorrectionRefPtDist = value;
		fEstValRefPointDist += value;
		return;
	}
	for (int i = 0; i < 3; i++){
		if (uidx_planeVector[i] == idx){
			if (i == 0 ){
				fCorrectionPlaneNormalVector.setX(value); 
				fEstValPlaneNormalVector.setX(fEstValPlaneNormalVector.getX() + value);
				return;}
			else if(i == 1){
				fCorrectionPlaneNormalVector.setY(value); 
				fEstValPlaneNormalVector.setY(fEstValPlaneNormalVector.getY() + value);
				return;}
			else{
				fCorrectionPlaneNormalVector.setZ(value); 
				fEstValPlaneNormalVector.setZ(fEstValPlaneNormalVector.getZ()+value);
				return;}
		}	
	}
	throw std::logic_error("Invalid unknown index in parameter access.");
}

void	TAdjustablePlane::setEstimatedPrecision(int idx, TReal value){
	if (uidx_rpDistance == idx){
		fEstPrecisionRefPtDist.setValue(value);
		return;
	}

	for (int i = 0; i < 3; i++)
		if (uidx_planeVector[i] == idx) {
			if (i == 0 ){
				fEstPrecisionPlaneNormalVector.setX(value);}
			else if(i == 1){
				fEstPrecisionPlaneNormalVector.setY(value);}
			else{
				fEstPrecisionPlaneNormalVector.setZ(value);}
			return;
		}
	throw std::logic_error("Invalid unknown index in parameter access.");
}

void	TAdjustablePlane::setXYNormalVectorCovariance(TReal value){
	if (!fixedStatePlaneVector[0] && !fixedStatePlaneVector[1])
		fCovariancePlaneNormalVector.setX(value);
	else
		throw std::logic_error("TAdjustablePlane::setYZNormalVectorCovariance, normal vector must be variable in both X and Y.");
}

void	TAdjustablePlane::setYZNormalVectorCovariance(TReal value){
	if (!fixedStatePlaneVector[1] && !fixedStatePlaneVector[2])
		fCovariancePlaneNormalVector.setY(value);
	else
		throw std::logic_error("TAdjustablePlane::setYZNormalVectorCovariance, normal vector must be variable in both Y and Z.");
}

void	TAdjustablePlane::setXZNormalVectorCovariance(TReal value){
	if (!fixedStatePlaneVector[0] && !fixedStatePlaneVector[2])
		fCovariancePlaneNormalVector.setZ(value);
	else
		throw std::logic_error("TAdjustablePlane::setXZNormalVectorCovariance, normal vector must be variable in both X and Z.");
}