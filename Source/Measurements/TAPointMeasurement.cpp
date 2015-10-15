//
// TAPointMeasurement.cpp : implementation file
// abstract base class holding the behaviour common to all
// targeted measurements, mainly the active/inactive management

#include "TAPointMeasurement.h"

//////////////////////
// default constructor
//////////////////////
TAPointMeasurement::TAPointMeasurement():
TAMeasurement(), fTargetPtName(""){

	
}

//////////////////////////////////////////////
// constructor setting the target point's name
//////////////////////////////////////////////
TAPointMeasurement::TAPointMeasurement(TSpatialPointName tgName):
TAMeasurement(), fTargetPtName(tgName){

}



///////////////////
// copy constructor
///////////////////
TAPointMeasurement::TAPointMeasurement(const TAPointMeasurement& source):
TAMeasurement(source), fTargetPtName(source.fTargetPtName) {

}

/////////////
// destructor
/////////////
TAPointMeasurement::~TAPointMeasurement(){

}


/////////////
// < operator
/////////////
bool TAPointMeasurement::operator<(const TAPointMeasurement& right) const{

	if (fTargetPtName < (right.fTargetPtName))
		return true;
	else
		return false;
}

//////////////
// == operator
//////////////
bool TAPointMeasurement::operator==(const TAPointMeasurement& right) const{

	if (fTargetPtName == (right.fTargetPtName))
		return true;
	else
		return false;
}

//////////////////////////////////////////
// returns an iterator on the target point
//////////////////////////////////////////
TSpatialPointName TAPointMeasurement::getTargetPoint() const{

	return fTargetPtName;
}



