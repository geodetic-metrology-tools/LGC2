// 
// TUnknownParameter.cpp : implementation file
//
// Class for an unknown parameter as defined for survey purposes
// Used for the V0, Instrument heights, ...
// almost a structure
//

#include "TUnknownParameter.h"

//////////////////////////
// no argument constructor
//////////////////////////
TUnknownParameter::TUnknownParameter(){

	fStatus = kUnknown;
	fIndex = 0;
	fProvisionalValue = 0.0;
	fEstimatedValue = 0.0;
	fEstimatedPrecision = 0.0;
}

///////////////////
// copy constructor
///////////////////
TUnknownParameter::TUnknownParameter(const TUnknownParameter& source){

	fStatus = source.fStatus;
	fIndex = source.fIndex;
	fProvisionalValue = source.fProvisionalValue;
	fEstimatedValue = source.fEstimatedValue;
	fEstimatedPrecision = source.fEstimatedPrecision;
}

/////////////
// destructor
/////////////
TUnknownParameter::~TUnknownParameter(){

}


/////////////////////////////
// gets the provisional value
/////////////////////////////
double TUnknownParameter::getProvisionalValue() const{

	return fProvisionalValue;
}


////////////////////////////
// gets the estimated value
////////////////////////////
double TUnknownParameter::getEstimatedValue() const{

	return fEstimatedValue;
}


///////////////////////////////
// gets the estimated precision
///////////////////////////////
double TUnknownParameter::getEstimatedPrecision() const{

	return fEstimatedPrecision;
}


///////////////////////////
// gets the unknown's index
///////////////////////////
MatrixIndex TUnknownParameter::getIndex() const{

	return fIndex;
}

/////////////////////////////
// gets the unknown's status
/////////////////////////////
TUnknownParameter::EUnknownStatus TUnknownParameter::getStatus() const{

	return fStatus;
}


/////////////////////////////
// sets the provisional value
/////////////////////////////
void TUnknownParameter::setProvisionalValue(double pv){

	fProvisionalValue = pv;
}


/////////////////////////////
// sets the estimated value
/////////////////////////////
void TUnknownParameter::setEstimatedValue(double ev){

	fEstimatedValue = ev;
}


///////////////////////////////
// sets the estimated precision
///////////////////////////////
void TUnknownParameter::setEstimatedPrecision(double ep){

	fEstimatedPrecision = ep;
}


/////////////////////////////
// sets the unknown's index
/////////////////////////////
MatrixIndex TUnknownParameter::setIndex(MatrixIndex i){

	fIndex = i;
	return ++i;
}


/////////////////////////////
// sets the unknown's status
/////////////////////////////
void TUnknownParameter::setStatus(EUnknownStatus us){

	fStatus = us;
}


