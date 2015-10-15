//TWPSObservation.cpp

#include "TWPSObservation.h"

#include "TWPSStation.h"
#include "TWPSMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialLine.h"

//////////////////////////////////////////////////////////////
// Constructors and Destructors
//////////////////////////////////////////////////////////////

////////////////////////
// Default constructor
////////////////////////
TWPSObservation::TWPSObservation() {
	fStation = 0;
	fMeasurement = 0;
	fStationedPoint = 0;
	fTargetLine = 0;
}

///////////////////////////
// Constructor
///////////////////////////
TWPSObservation::TWPSObservation(TWPSStation* station, 
								TWPSMeasurement* meas, TSpatialPoint* st, TSpatialLine* tg) :
fStation(station), fMeasurement(meas), fStationedPoint(st), 
fTargetLine(tg) {
}

/*
//////////////////////////////////////////////////
// Copy constructor: not implemented
//////////////////////////////////////////////////
TWPSObservation::TWPSObservation(const TWPSObservation& source) {
}
*/
//////////////////////////
// Destructor
/////////////////////////
TWPSObservation::~TWPSObservation() {
	if (fStation != 0) delete fStation;
	if (fMeasurement != 0) delete fMeasurement;
	if (fStationedPoint != 0) delete fStationedPoint;
	if (fTargetLine != 0) delete fTargetLine;
}

/*
////////////////////////////////////////////////
// Copy assignement operator: not implemented 
////////////////////////////////////////////////
TWPSObservation& TWPSObservation::operator =(TWPSObservation& right) {
}
*/