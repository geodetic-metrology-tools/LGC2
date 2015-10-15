// 
// TUnknownLength.cpp : implementation file
//
// Class for an unknown length as defined for survey purposes
//

#include "TUnknownLength.h"

//////////////////////////
// no argument constructor
//////////////////////////
TUnknownLength::TUnknownLength(){

	fStatus = TANumericValue::kUnknown;
	fIndex = 0;
}

///////////////////
// copy constructor
///////////////////
TUnknownLength::TUnknownLength(const TUnknownLength& source){

	fStatus = source.fStatus;
	fIndex = source.fIndex;
	fProvisionalValue = source.fProvisionalValue;
	fEstimatedValue = source.fEstimatedValue;
	fEstimatedPrecision = source.fEstimatedPrecision;
}

/////////////
// destructor
/////////////
TUnknownLength::~TUnknownLength(){

}


/////////////////////////////
// gets the provisional value
/////////////////////////////
TLength TUnknownLength::getProvisionalValue() const{

	return fProvisionalValue;
}


////////////////////////////
// gets the estimated value
////////////////////////////
TLength TUnknownLength::getEstimatedValue() const{

	return fEstimatedValue;
}


///////////////////////////////
// gets the estimated precision
///////////////////////////////
TLength TUnknownLength::getEstimatedPrecision() const{

	return fEstimatedPrecision;
}


///////////////////////////
// gets the unknown's index
///////////////////////////
MatrixIndex TUnknownLength::getIndex() const{

	return fIndex;
}

/////////////////////////////
// gets the unknown's status
/////////////////////////////
TUnknownLength::EUnknownStatus TUnknownLength::getStatus() const{

	return fStatus;
}

//////////////////////////////////////////////
// tells if the length's status isn't kUnknown
//////////////////////////////////////////////
bool TUnknownLength::known() const{

	if (fStatus != TANumericValue::kUnknown && fStatus != TANumericValue::kNull)
		return true;
	else
		return false;
}

/////////////////////////////
// sets the provisional value
/////////////////////////////
void TUnknownLength::setProvisionalValue(TLength pv){

	fProvisionalValue = pv;
}

///////////////////////////////////////////////
// sets the correction to the provisional value
///////////////////////////////////////////////
void TUnknownLength::setCorrection(TLength dd){

	fCorrection = dd;
	setEstimatedValue();
}


/////////////////////////////
// sets the estimated value
/////////////////////////////
void TUnknownLength::setEstimatedValue(){

	fEstimatedValue = fProvisionalValue + fCorrection;
}


///////////////////////////////
// sets the estimated precision
///////////////////////////////
void TUnknownLength::setEstimatedPrecision(TLength ep){

	fEstimatedPrecision = ep;
}


/////////////////////////////
// sets the unknown's index
/////////////////////////////
MatrixIndex TUnknownLength::setIndex(MatrixIndex i){

	fIndex = i;
	return ++i;
}


/////////////////////////////
// sets the unknown's status
/////////////////////////////
void TUnknownLength::setStatus(EUnknownStatus us){

	fStatus = us;
}

/////////////////////////////////////////////////
// makes this object ready for the next iteration
/////////////////////////////////////////////////
void TUnknownLength::prepareNextIteration(){

	fProvisionalValue = fEstimatedValue;
}
