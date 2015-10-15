// 
// THorAngleMeasurement.cpp : implementation file
//
// Class for a survey measurement as defined for survey purposes
//
#include "THorAngleROM.h"
//#include "TSpatialPointName.h"
//#include "TWorkingStations.h"

#include "THorAngleMeasurement.h"




//////////////////////
// default constructor
//////////////////////
THorAngleMeasurement::THorAngleMeasurement(): TAPointMeasurement(),
fMeasuredAngle(LITERAL(0.0)), fSigmaAPriori(0){

}

////////////////////////////////////////////////////////////////////////
// constructor setting the measured point and the measured angle's value
////////////////////////////////////////////////////////////////////////
THorAngleMeasurement::THorAngleMeasurement(TSpatialPointName iterTg, TAngle obsAngle, TAngle sigma)
: TAPointMeasurement(iterTg), fMeasuredAngle(obsAngle), fSigmaAPriori(sigma)
{
	
}



/////////////
// destructor
/////////////
THorAngleMeasurement::~THorAngleMeasurement()
{
	

}

////////////////////////////
// copy assignement operator
////////////////////////////
THorAngleMeasurement& THorAngleMeasurement::operator=(const THorAngleMeasurement& source){

	if (this != &source)
	{
		fMeasuredAngle = source.getAngleValue();
		fSigmaAPriori = source.getSigma();
		fTargetPtName = source.fTargetPtName;

		fHeaderComment = source.fHeaderComment;
		fComment = source.fComment;
		fIdentifier = source.fIdentifier;
	}
	return (*this);

}

////////////////////////////
// copy assignement operator
////////////////////////////
bool THorAngleMeasurement::operator==(const THorAngleMeasurement& right) const {

	bool isEqualTo = false;
	if ((fTargetPtName == (right.fTargetPtName)) &&
		(fabsq(fMeasuredAngle.getRadiansValue() - right.fMeasuredAngle.getRadiansValue()) < powq(LITERAL(1.0),-LITERAL(10.0))))
		isEqualTo = true;
	return isEqualTo;
}

/////////////////////////////////////////////
// returns a non-const pointer to this object
/////////////////////////////////////////////
THorAngleMeasurement* THorAngleMeasurement::getPointer() const{

	return const_cast<THorAngleMeasurement*>(this);
}


///////////////////////////////////////////////////////
// returns the string representing the measurement kind
///////////////////////////////////////////////////////
string THorAngleMeasurement::getMeasKind() const{

	string s("HORANG");
	return s;
}


///////////////////////////////////////
// returns a copy of the measured angle
///////////////////////////////////////
TAngle THorAngleMeasurement::getAngleValue() const{

	return fMeasuredAngle;
}

///////////////////////////////////////
// returns a copy of the a priori sigma
///////////////////////////////////////
TAngle THorAngleMeasurement::getSigma() const{

	return fSigmaAPriori;
}

/////////////////////////////////////
// sets the sigma if it is still zero
/////////////////////////////////////
void THorAngleMeasurement::ifNotDoneSetSigma(TAngle sigma){

	if (fSigmaAPriori.getRadiansValue() == LITERAL(0.0))
		fSigmaAPriori = sigma;
}

