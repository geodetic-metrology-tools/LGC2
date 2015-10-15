#include "TDistConstants.h"


TDistConstants::TDistConstants():fValue(LITERAL(0.0)){

	fConstId = "";
	fStatus = TAMeasurement::kFixed;
}


TDistConstants::TDistConstants(TLength c, TAMeasurement::ECalcStatus s){

	fValue = c;
	fStatus = s;
}

TDistConstants::TDistConstants(const TDistConstants& cte)
{
	*this = cte;
}

TDistConstants::~TDistConstants(){

}



//////////////////////////////////////////////////////////////////////
// copy assignement operator
//////////////////////////////////////////////////////////////////////
TDistConstants&  TDistConstants::operator=(const TDistConstants& tdc)
{
	if (this != &tdc)
	{
		fValue = tdc.getValue();
		fStatus = tdc.getStatus();
		fConstId = tdc.getConstantName();
	}
	return *this;
}


bool TDistConstants::operator <(const TDistConstants& tdc) const
{
	if( this->getConstantName() < tdc.getConstantName())
		return true;
	else return false;
}


bool TDistConstants::operator ==(const TDistConstants& tdc) const
{
	if( (this->getConstantName() == tdc.getConstantName()) &&
		(this->getValue() == tdc.getValue()) && (this->getStatus() == tdc.getStatus()))
		return true;
	else return false;
}







TLength	TDistConstants::getValue() const{

	return fValue;
}


TAMeasurement::ECalcStatus	TDistConstants::getStatus() const{

	return fStatus;
}


void TDistConstants::setValue(TLength c){

	fValue = c;
}


void TDistConstants::setStatus(TAMeasurement::ECalcStatus s){

	fStatus = s;
}

void TDistConstants::setConstantName(string name){
	fConstId = name;
}



string TDistConstants::getConstantName() const{

	return fConstId;
}




