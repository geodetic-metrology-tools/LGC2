//
// TAMeasurement.cpp : implementation file
// abstract base class holding the behaviour common to all
// targeted measurements, mainly the active/inactive management

#include "TAMeasurement.h"

//////////////////////
// default constructor
//////////////////////
TAMeasurement::TAMeasurement()
{

	fIdentifier = 0;
	fHasAGeodeId = false;
	fComment = "";
	fHeaderComment = "";

}


///////////////////
// copy constructor
///////////////////
TAMeasurement::TAMeasurement(const TAMeasurement& source)
{

fIdentifier = source.fIdentifier;
fHasAGeodeId = false;
fComment = source.fComment;
fHeaderComment = source.fHeaderComment;

}

/////////////
// destructor
/////////////
TAMeasurement::~TAMeasurement(){

}




/////////////////////////////////////////
// sets the identifier of the measurement
/////////////////////////////////////////
void TAMeasurement::setId(int id)
{ 
	fIdentifier = id;
	fHasAGeodeId = true;
	return;
}

		



