#include "TAngleConstants.h"


TAngleConstants::TAngleConstants():fValue(LITERAL(0.0)){

	fConstId = "";
	fStatus = TAMeasurement::kFixed;
}


TAngleConstants::TAngleConstants(TAngle c, TAMeasurement::ECalcStatus s){

	fValue = c;
	fStatus = s;
}

TAngleConstants::TAngleConstants(const TAngleConstants& cte)
{
	*this = cte;
}


TAngleConstants::~TAngleConstants(){

}



//////////////////////////////////////////////////////////////////////
// copy assignement operator
//////////////////////////////////////////////////////////////////////
TAngleConstants&  TAngleConstants::operator=(const TAngleConstants& tdc)
{
	if (this != &tdc)
	{
		fValue = tdc.getValue();
		fStatus = tdc.getStatus();
		fConstId = tdc.getConstantName();
	}
	return *this;
}


bool TAngleConstants::operator <(const TAngleConstants& tdc) const
{
	if( this->getConstantName() < tdc.getConstantName())
		return true;
	else return false;
}


bool TAngleConstants::operator ==(const TAngleConstants& tdc) const
{
	if( (this->getConstantName() == tdc.getConstantName()) &&
		(this->getValue() == tdc.getValue()) && (this->getStatus() == tdc.getStatus()))
		return true;
	else return false;
}







TAngle	TAngleConstants::getValue() const{

	return fValue;
}


TAMeasurement::ECalcStatus	TAngleConstants::getStatus() const{

	return fStatus;
}


void TAngleConstants::setValue(TAngle c){

	fValue = c;
}


void TAngleConstants::setStatus(TAMeasurement::ECalcStatus s){

	fStatus = s;
}


void TAngleConstants::setConstantName(string name){
	fConstId = name;
}


string TAngleConstants::getConstantName() const{

	return fConstId;
}




