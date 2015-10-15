#include "TAdjustableScalar.h"
#include "Global.h"

TAdjustableScalar::TAdjustableScalar(const TScalar& scalValue, bool isFixed, const std::string& name):
fName(name),
isScalarFixed(isFixed),
fProvisionalValue(scalValue),
fCorrection(0.0),
fEstimatedValue(scalValue),
fEstimatedPrecision(0.0),
uidx(-1)
{}

TAdjustableScalar TAdjustableScalar::createUninitialized(const std::string& name){
	TAdjustableScalar as(TScalar(NO_VALf), true, name);
	return as;
}

void TAdjustableScalar::setFirstUidx(int idx)  {
	if (isScalarFixed)
		throw std::logic_error("Trying to assign unknown index to fixed scalar.");
	uidx = idx;
}

void TAdjustableScalar::setCorrection(int idx, TReal value) {
	if (uidx == idx){
		fCorrection = value;
		fEstimatedValue += value;
		return;
	}
	throw std::logic_error("Invalid unknown index in parameter access.");
}

void	TAdjustableScalar::setEstimatedPrecision(int idx, TReal ep) {
	if (uidx == idx)
		fEstimatedPrecision = ep;
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}