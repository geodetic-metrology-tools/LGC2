#include <stdexcept>
#include "TAdjustableLength.h"
#include "Global.h"

TAdjustableLength::TAdjustableLength(const TLength& angleValue, bool isFixed, const std::string& name) :
   fName(name),
   ifFixed(isFixed),
   fProvisionalValue(angleValue),
   fCorrection(0.0),
   fEstimatedValue(angleValue),
   fEstimatedPrecision(0.0),
   uidx(-1)
{}

TAdjustableLength TAdjustableLength::createUninitialized(const std::string& name)
{
   return TAdjustableLength(TLength(NO_VALf), true, name);
}

void TAdjustableLength::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to a fixed angle.");
	uidx = idx;
}

void TAdjustableLength::setCorrection(int idx, TReal value) {
	if (uidx == idx){
      fCorrection.setMetresValue(value);
      fEstimatedValue.setMetresValue(fEstimatedValue.getMetresValue() + value);
	}
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}

void TAdjustableLength::setEstimatedPrecision(int idx, TReal ep) {
	if (uidx == idx)
      fEstimatedPrecision.setMetresValue(ep);
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
}

void TAdjustableLength::reInitialise(){
   fEstimatedPrecision.setMetresValue(0.0);
   fCorrection.setMetresValue(0.0);
}