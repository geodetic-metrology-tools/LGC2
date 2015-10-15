#include <stdexcept>
#include "TAdjustablePlane.h"
#include "TAdjustablePoint.h"

TAdjustablePlane::TAdjustablePlane(const TAdjustablePoint* referencePoint, const TScalar& refPointDistance, const LGC::TAngle& theta, const LGC::TAngle& phi, bool thetaFixed, bool phiFixed, const std::string& name)
	:
	fName(name),
	fReferencePoint(referencePoint),

	fProvValRefPtDist(refPointDistance),
	fCorrectionRefPtDist(0.0),
	fEstValRefPointDist(refPointDistance),
	fEstPrecisionRefPtDist(0.0),

	fProvValTheta(theta),
	fCorrectionTheta(LGC::TAngle::EUnits::kRadians, 0.0),
	fEstValTheta(theta),
	fEstPrecisionTheta(LGC::TAngle::EUnits::kRadians,0.0),

	fProvValPhi(phi),
	fCorrectionPhi(LGC::TAngle::EUnits::kRadians, 0.0),
	fEstValPhi(phi),
	fEstPrecisionPhi(LGC::TAngle::EUnits::kRadians,0.0),


	fRefPtDistFixed(false),
	fThetaFixed(thetaFixed),
	fPhiFixed(phiFixed),

	uidx_Theta(-1),
	uidx_Phi(-1),
	uidx_rpDistance(-1),

	fInitialized(true)
{}

TAdjustablePlane TAdjustablePlane::createUninitialized(const std::string& name){
	TAdjustablePlane ap(0, TScalar(NO_VALf), LGC::TAngle(LGC::TAngle::EUnits::kRadians ,NO_VALf), LGC::TAngle(LGC::TAngle::EUnits::kRadians ,NO_VALf), true, true, name);
	ap.fInitialized = false;
	return ap;
}

void TAdjustablePlane::initialize(const TAdjustablePoint* referencePoint, const TScalar& refPointDistance, const LGC::TAngle& theta, const LGC::TAngle& phi, bool thetaFixed, bool phiFixed){
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

void TAdjustablePlane::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed plane.");
	
	if (!fThetaFixed)
			uidx_Theta = idx++;

	if (!fPhiFixed)
			uidx_Phi = idx++;

	//Ref. Pt distance is always variable
	uidx_rpDistance = idx++;
}

void TAdjustablePlane::setCorrection(int idx, TReal value){
	if (uidx_rpDistance == idx){
		fCorrectionRefPtDist = value;
		fEstValRefPointDist += value;
	}
	else if  (uidx_Theta == idx){
		fCorrectionTheta.set(LGC::TAngle::kRadians, value);
		fEstValTheta.set(LGC::TAngle::kRadians, fEstValTheta.rad() + value);
	}
	else if  (uidx_Phi == idx){
		fCorrectionPhi.set(LGC::TAngle::kRadians, value);
		fEstValPhi.set(LGC::TAngle::kRadians, fEstValPhi.rad() + value);
	}
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}

void	TAdjustablePlane::setEstimatedPrecision(int idx, TReal value){
	if (uidx_rpDistance == idx)
		fEstPrecisionRefPtDist.setValue(value);
	else if (uidx_Theta == idx)
		fEstPrecisionTheta.set(LGC::TAngle::kRadians, value);
	else if (uidx_Phi == idx)
		fEstValPhi.set(LGC::TAngle::kRadians, value);
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}

void TAdjustablePlane::reInitialise(){
	fEstValRefPointDist = fProvValRefPtDist;
	fCorrectionRefPtDist.setValue(0.0);
	fEstPrecisionRefPtDist.setValue(0.0);

	fEstValTheta = fProvValTheta;
	fCorrectionTheta.set(LGC::TAngle::EUnits::kRadians,0.0);
	fEstPrecisionTheta.set(LGC::TAngle::EUnits::kRadians,0.0);

	fEstValPhi = fProvValPhi;
	fCorrectionPhi.set(LGC::TAngle::EUnits::kRadians,0.0);
	fEstPrecisionPhi.set(LGC::TAngle::EUnits::kRadians,0.0);
}