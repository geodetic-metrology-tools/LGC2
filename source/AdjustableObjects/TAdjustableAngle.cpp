#include "TAdjustableAngle.h"
#include "Global.h"

TAdjustableAngle::TAdjustableAngle(const LGC::TAngle& angleValue, bool isFixed, const std::string& name):
fName(name),
isAngleFixed(isFixed),
fProvisionalValue(angleValue),
fCorrection(LGC::TAngle::EUnits::kRadians, 0.0),
fEstimatedValue(angleValue),
fEstimatedPrecision(LGC::TAngle::EUnits::kRadians,0.0),
uidx(-1)
{}

TAdjustableAngle TAdjustableAngle::createUninitialized(const std::string& name){
	TAdjustableAngle as(LGC::TAngle(LGC::TAngle::EUnits::kRadians ,NO_VALf), true, name);
	return as;
}

void TAdjustableAngle::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed scalar.");
	uidx = idx;
}

void TAdjustableAngle::setCorrection(int idx, TReal value) {
	if (uidx == idx){
		fCorrection.set(LGC::TAngle::kRadians, value);
		fEstimatedValue.set(LGC::TAngle::kRadians, fEstimatedValue.rad() + value);
		std::cout<< "V0: "<<fEstimatedValue.gon()<<std::endl;
	}
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}

void TAdjustableAngle::setEstimatedPrecision(int idx, TReal ep) {
	if (uidx == idx)
		fEstimatedPrecision.set(LGC::TAngle::kRadians, ep);
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}