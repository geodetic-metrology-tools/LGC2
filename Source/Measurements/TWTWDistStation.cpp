// TWTWDistStation.cpp
//
/** Class for a wire to wire distance measurement station */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
//#include "TWorkingStations.h"

#include  "TWTWDistStation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TWTWDistStation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
//////////////
// constructor
//////////////

TWTWDistStation::TWTWDistStation(TWorkingStations* ws)
{ 

	TWTWDistStation* wtws = ws->addWTWStation(this);

}
*/

///////////////////////
// Default constructor
///////////////////////
TWTWDistStation::TWTWDistStation() {
}


///////////////////
// copy constructor
///////////////////
TWTWDistStation::TWTWDistStation( const  TWTWDistStation& source ):
TAFreeInstrumentStation(source),
fWTWDistMeas(*(source.getWTWDistMeas()))
{	

	
}


/////////////
// destructor
/////////////
TWTWDistStation::~TWTWDistStation()
{

}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
///////////////////////////
// Copy Assignment operator
///////////////////////////
TWTWDistStation&  TWTWDistStation::operator=(const TWTWDistStation& source)
{	

	if (this != &source){

		fInstrumentNumber = source.fInstrumentNumber;
		fSetup = source.fSetup;
		fWTWDistMeas = *(source.getWTWDistMeas());
	}
	return *this;
}

///////////////////////
// Temporary
//////////////////////
bool TWTWDistStation::operator ==(const TWTWDistStation& source)
{	
	this->setSetup(source.getSetup());
	return true;
}

/////////////////////////////////////////////////////////////////
// returns an iterator pointing to the station's only measurement
/////////////////////////////////////////////////////////////////
WTWDistMeasIterator TWTWDistStation::getWTWDistMeas() {

	return &fWTWDistMeas;
}

//////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the station's only measurement
//////////////////////////////////////////////////////////////////////
WTWDistMeasConstIter TWTWDistStation::getWTWDistMeas() const{

	return const_cast<TWireToWireDistMeas*>(&fWTWDistMeas);
}


/////////////////////////////////
// sets the station's measurement
/////////////////////////////////
TWireToWireDistMeas* TWTWDistStation::setWTWDistMeas(TWireToWireDistMeas* wtwMeas){

	fWTWDistMeas = *wtwMeas;
	return &fWTWDistMeas;
}
/*
/////////////////////////////////////////////////////////
// tells the measurement that the station is disactivated
/////////////////////////////////////////////////////////
void TWTWDistStation::sendDisactivatedToChildren() {

	const TAObjectMeasurement* caom = &fWTWDistMeas;
	TAObjectMeasurement* aom = const_cast< TAObjectMeasurement* >(caom);
	aom->parentDisactivated();
}

/////////////////////////////////////////////////////////
// tells the ROM children that the station is Activated
/////////////////////////////////////////////////////////
void TWTWDistStation::sendActivatedToChildren() {

	const TAObjectMeasurement* caom = &fWTWDistMeas;
	TAObjectMeasurement* aom = const_cast< TAObjectMeasurement* >(caom);
	aom->parentActivated();
}
*/