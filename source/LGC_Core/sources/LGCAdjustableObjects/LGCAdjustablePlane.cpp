#include <stdexcept>
#include <LGCAdjustablePlane.h>
#include "LGCAdjustablePoint.h"

LGCAdjustablePlane::LGCAdjustablePlane(const LGCAdjustablePoint* referencePoint, const TLength& refPointDistance, const TAngle& theta, const TAngle& phi, bool thetaFixed, bool phiFixed, const std::string& name)
	:
	fName(name),
	fReferencePoint(referencePoint),

	fProvValRefPtDist(refPointDistance),
	fCorrectionRefPtDist(0.0),
	fEstValRefPointDist(refPointDistance),
	fEstPrecisionRefPtDist(0.0),

	fProvValTheta(theta),
	fCorrectionTheta(0.0, TAngle::EUnits::kRadians),
	fEstValTheta(theta),
	fEstPrecisionTheta(0.0, TAngle::EUnits::kRadians),

	fProvValPhi(phi),
	fCorrectionPhi(0.0, TAngle::EUnits::kRadians),
	fEstValPhi(phi),
	fEstPrecisionPhi(0.0, TAngle::EUnits::kRadians),


	fRefPtDistFixed(false),
	fThetaFixed(thetaFixed),
	fPhiFixed(phiFixed),

	uidx_Theta(-1),
	uidx_Phi(-1),
	uidx_rpDistance(-1),

	fInitialized(true)
{}

LGCAdjustablePlane LGCAdjustablePlane::createUninitialized(const std::string& name){
   LGCAdjustablePlane ap(0, TLength(NO_VALf), TAngle(NO_VALf, TAngle::EUnits::kRadians), TAngle(NO_VALf, TAngle::EUnits::kRadians), true, true, name);
	ap.fInitialized = false;
	return ap;
}

void LGCAdjustablePlane::initialize(const LGCAdjustablePoint* referencePoint, const TLength& refPointDistance, const TAngle& theta, const TAngle& phi, bool thetaFixed, bool phiFixed){
	fReferencePoint = referencePoint;

	fProvValRefPtDist = refPointDistance;
	fEstValRefPointDist = refPointDistance;

	fProvValTheta = theta;
	fEstValTheta = theta;

	fProvValPhi = phi;
	fEstValPhi = phi;

	fThetaFixed = thetaFixed;
	fPhiFixed = phiFixed;

	fInitialized = true;
}

void LGCAdjustablePlane::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed plane.");
	
	if (!fThetaFixed)
			uidx_Theta = idx++;

	if (!fPhiFixed)
			uidx_Phi = idx++;

	//Ref. Pt distance is always variable
	uidx_rpDistance = idx++;
}

void LGCAdjustablePlane::setCorrection(int idx, TReal value){
	
   if (uidx_rpDistance == idx){
      fCorrectionRefPtDist = TLength(value);
      fEstValRefPointDist += TLength(value);
	}
	else if  (uidx_Theta == idx){
		fCorrectionTheta.setRadiansValue(value);
		fEstValTheta.setRadiansValue(fEstValTheta.getRadiansValue() + value);
	}
	else if  (uidx_Phi == idx){
		fCorrectionPhi.setRadiansValue(value);
		fEstValPhi.setRadiansValue(fEstValPhi.getRadiansValue() + value);
	}
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}

void	LGCAdjustablePlane::setEstimatedPrecision(int idx, TReal value){
	if (uidx_rpDistance == idx)
      fEstPrecisionRefPtDist = TLength(value);
	else if (uidx_Theta == idx)
		fEstPrecisionTheta.setRadiansValue(value);
	else if (uidx_Phi == idx)
		fEstValPhi.setRadiansValue(value);
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}

void LGCAdjustablePlane::reInitialise(){
	fEstValRefPointDist = fProvValRefPtDist;
   fCorrectionRefPtDist = TLength(0.0);
   fEstPrecisionRefPtDist = TLength(0.0);

	fEstValTheta = fProvValTheta;
	fCorrectionTheta.setRadiansValue(0.0);
	fEstPrecisionTheta.setRadiansValue(0.0);

	fEstValPhi = fProvValPhi;
	fCorrectionPhi.setRadiansValue(0.0);
	fEstPrecisionPhi.setRadiansValue(0.0);
}