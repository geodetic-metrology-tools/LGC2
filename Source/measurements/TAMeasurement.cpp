//
// TAMeasurement.cpp : implementation file
// abstract base class holding the behaviour common to all
// targeted measurements, mainly the active/inactive management

#include "TAMeasurement.h"


/////////////////////////////////////////
// sets the identifier of the measurement
/////////////////////////////////////////
void TAMeasurement::setId(int id)
{ 
	fIdentifier = id;
	fHasAGeodeId = true;
	return;
}

		



