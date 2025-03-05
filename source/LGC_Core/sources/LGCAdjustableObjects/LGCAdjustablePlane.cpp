#include <stdexcept>

#include <LGCAdjustablePlane.h>

#include "LGCAdjustablePoint.h"

LGCAdjustablePlane::LGCAdjustablePlane(const LGCAdjustablePoint *referencePoint,
	const TLength &refPointDistance,
	const TAngle &theta,
	const TAngle &phi,
	bool thetaFixed,
	bool phiFixed,
	bool fRefPtDistFixed,
	const std::string &name) :
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

	fRefPtDistFixed(fRefPtDistFixed),
	fThetaFixed(thetaFixed),
	fPhiFixed(phiFixed),

	uidx_Theta(-1),
	uidx_Phi(-1),
	uidx_rpDistance(-1),

	fInitialized(true)
{
}

LGCAdjustablePlane LGCAdjustablePlane::createUninitialized(const std::string &name)
{
	LGCAdjustablePlane ap(0, TLength(NO_VALf), TAngle(NO_VALf, TAngle::EUnits::kRadians), TAngle(NO_VALf, TAngle::EUnits::kRadians), true, true, false, name);
	ap.fInitialized = false;
	return ap;
}

void LGCAdjustablePlane::initialize(const LGCAdjustablePoint *referencePoint, const TLength &refPointDistance, const TAngle &theta, const TAngle &phi, bool thetaFixed, bool phiFixed)
{
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

const Eigen::VectorXd LGCAdjustablePlane::getEstParamVector()
{
	Eigen::VectorXd vector(3);
	vector << fEstValTheta.getRadiansValue(), fEstValPhi.getRadiansValue(), fEstValRefPointDist;

	return vector;
}

const std::vector<int> LGCAdjustablePlane::getRelativeUnknIndices() const
{
	std::vector<int> relIndices;
	
	if (!fThetaFixed)
		relIndices.push_back(0);
	if (!fPhiFixed)
		relIndices.push_back(1);
	if (!fRefPtDistFixed)
		relIndices.push_back(2);

	return relIndices;
}

void LGCAdjustablePlane::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed plane.");

	if (!fThetaFixed)
		uidx_Theta = idx++;

	if (!fPhiFixed)
		uidx_Phi = idx++;

	if (!fRefPtDistFixed)
		uidx_rpDistance = idx++;
}

int LGCAdjustablePlane::getFirstUidx() const
{
	if (!fThetaFixed)
		return uidx_Theta;
	else if (!fPhiFixed)
		return uidx_Phi;
	else if (!fRefPtDistFixed)
		return uidx_rpDistance;

	throw std::logic_error("Trying to get first unknown index from fixed plane.");
}

int LGCAdjustablePlane::getLastUidx() const
{
	if (!fThetaFixed | !fPhiFixed | !fRefPtDistFixed)
		return getFirstUidx() + int(!fThetaFixed) + int(!fPhiFixed) + int(!fRefPtDistFixed) - 1;

	throw std::logic_error("Trying to get last unknown index from fixed plane.");
}

void LGCAdjustablePlane::setCorrection(int idx, TReal value)
{
	if (uidx_rpDistance == idx)
	{
		fCorrectionRefPtDist = TLength(value);
		fEstValRefPointDist += TLength(value);
	}
	else if (uidx_Theta == idx)
	{
		fCorrectionTheta.setRadiansValue(value);
		fEstValTheta.setRadiansValue(fEstValTheta.getRadiansValue() + value);
	}
	else if (uidx_Phi == idx)
	{
		fCorrectionPhi.setRadiansValue(value);
		fEstValPhi.setRadiansValue(fEstValPhi.getRadiansValue() + value);
	}
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}
void LGCAdjustablePlane::setEstVal(int idx, TReal value)
{
	if (uidx_rpDistance == idx)
	{
		fEstValRefPointDist = TLength(value);
	}
	else if (uidx_Theta == idx)
	{
		fEstValTheta.setRadiansValue(value);
	}
	else if (uidx_Phi == idx)
	{
		fEstValPhi.setRadiansValue(value);
	}
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}

void LGCAdjustablePlane::setEstimatedPrecision(int idx, TReal value)
{
	if (uidx_rpDistance == idx)
		fEstPrecisionRefPtDist = TLength(value);
	else if (uidx_Theta == idx)
		fEstPrecisionTheta.setRadiansValue(value);
	else if (uidx_Phi == idx)
		fEstValPhi.setRadiansValue(value);
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}

void LGCAdjustablePlane::reInitialise()
{
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

#if USE_SERIALIZER
void LGCAdjustablePlane::serialize(ObjectSerializer &obj) const
{
	TVAdjustableObject::serialize(obj);
	obj.addProperty("fCorrectionPhi", fCorrectionPhi.getRadiansValue());
	obj.addProperty("fCorrectionRefPtDist", fCorrectionRefPtDist.getMetresValue());
	obj.addProperty("fCorrectionTheta", fCorrectionTheta.getRadiansValue());
	obj.addProperty("fEstPrecisionPhi", fEstPrecisionPhi.getRadiansValue());
	obj.addProperty("fEstPrecisionRefPtDist", fEstPrecisionRefPtDist.getMetresValue());

	obj.addProperty("fEstPrecisionTheta", fEstPrecisionTheta.getRadiansValue());
	obj.addProperty("fEstValPhi", fEstValPhi.getRadiansValue());
	obj.addProperty("fEstValRefPointDist", fEstValRefPointDist.getMetresValue());
	obj.addProperty("fEstValTheta", fEstValTheta.getRadiansValue());
	obj.addProperty("fInitialized", fInitialized);

	obj.addProperty("fName", fName);
	obj.addProperty("fPhiFixed", fPhiFixed);
	obj.addProperty("fProvValPhi", fProvValPhi.getRadiansValue());
	obj.addProperty("fProvValRefPtDist", fProvValRefPtDist.getMetresValue());
	obj.addProperty("fProvValTheta", fProvValTheta.getRadiansValue());

	obj.addProperty("fReferencePoint", fReferencePoint);
	obj.addProperty("fRefPtDistFixed", fRefPtDistFixed);
	obj.addProperty("fThetaFixed", fThetaFixed);
	obj.addProperty("uidx_Phi", uidx_Phi);
	obj.addProperty("uidx_rpDistance", uidx_rpDistance);

	obj.addProperty("uidx_Theta", uidx_Theta);
}
#endif // USE_SERIALIZER
