// 
// TUnknownAngle.cpp : implementation file
//
// Class for an unknown angle as defined for survey purposes
//

#include "TUnknownAngle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TUnknownAngle::TUnknownAngle(){
	fStatus = TANumericValue::kUnknown;
	fIndex = 0;
	fProvisionalValue = TAngle(0.0);
	fEstimatedValue = TAngle(0.0);
	fEstimatedPrecision = TAngle(0.0);
}


TUnknownAngle::~TUnknownAngle(){

}


/////////////////////////////
// gets the provisional value
/////////////////////////////
TAngle TUnknownAngle::getProvisionalValue() const{

	return fProvisionalValue;
}


////////////////////////////
// gets the estimated value
////////////////////////////
TAngle TUnknownAngle::getEstimatedValue() const{

	return fEstimatedValue;
}


///////////////////////////////
// gets the estimated precision
///////////////////////////////
TAngle TUnknownAngle::getEstimatedPrecision() const{

	return fEstimatedPrecision;
}


///////////////////////////
// gets the unknown's index
///////////////////////////
MatrixIndex TUnknownAngle::getIndex() const{

	return fIndex;
}

/////////////////////////////
// gets the unknown's status
/////////////////////////////
TUnknownAngle::EUnknownStatus TUnknownAngle::getStatus() const{

	return fStatus;
}


/////////////////////////////
// sets the provisional value
/////////////////////////////
void TUnknownAngle::setProvisionalValue(TAngle pv){

	fProvisionalValue = pv;
}

///////////////////////////////////////////////
// sets the correction to the provisional value
///////////////////////////////////////////////
void TUnknownAngle::setCorrection(TAngle da){

	fCorrection = da;
	setEstimatedValue();
}

/////////////////////////////
// sets the estimated value
/////////////////////////////
void TUnknownAngle::setEstimatedValue(){

	fEstimatedValue = fProvisionalValue + fCorrection;
}


///////////////////////////////
// sets the estimated precision
///////////////////////////////
void TUnknownAngle::setEstimatedPrecision(TAngle ep){

	fEstimatedPrecision = ep;
}


/////////////////////////////
// sets the unknown's index
/////////////////////////////
MatrixIndex TUnknownAngle::setIndex(MatrixIndex i){

	fIndex = i;
	return ++i;
}


/////////////////////////////
// sets the unknown's status
/////////////////////////////
void TUnknownAngle::setStatus(EUnknownStatus us){

	fStatus = us;
}

/////////////////////////////////
// checks if the Angle isn't unknown
/////////////////////////////////
bool TUnknownAngle::known() const{

	if (fStatus != TANumericValue::kUnknown)
		return true;
	else return false;
}

/////////////////////////////////////////////////
// makes this object ready for the next iteration
/////////////////////////////////////////////////
void TUnknownAngle::prepareNextIteration(){

	fProvisionalValue = fEstimatedValue;
}


