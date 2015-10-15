//TWTWObservation.cpp

#include "TWTWObservation.h"

#include "TWTWDistStation.h"
#include "TWireToWireDistMeas.h"
#include "TSpatialLine.h"

//////////////////////////////////////////////////////////////
// Constructors and Destructors
//////////////////////////////////////////////////////////////

////////////////////////
// Default constructor
////////////////////////
TWTWObservation::TWTWObservation() {
	fStation = 0;
	fMeasurement = 0;
	fTargetLine = 0;
}

///////////////////////////
// Constructor
///////////////////////////
TWTWObservation::TWTWObservation(TWTWDistStation* station, 
								TWireToWireDistMeas* meas, TSpatialLine* tg) :
fStation(station), fMeasurement(meas), fTargetLine(tg) {
}

/*
//////////////////////////////////////////////////
// Copy constructor: not implemented
//////////////////////////////////////////////////
TWTWObservation::TWTWObservation(const TWTWObservation& source) {
}
*/
//////////////////////////
// Destructor
/////////////////////////
TWTWObservation::~TWTWObservation() {
	if (fStation != 0) delete fStation;
	if (fMeasurement != 0) delete fMeasurement;
	if (fTargetLine != 0) delete fTargetLine;
}

/*
////////////////////////////////////////////////
// Copy assignement operator: not implemented 
////////////////////////////////////////////////
TWTWObservation& TWTWObservation::operator =(TWTWObservation& right) {
}
*/