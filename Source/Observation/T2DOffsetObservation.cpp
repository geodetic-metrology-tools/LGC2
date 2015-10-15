//T2DOffsetObservation.cpp

#include "T2DOffsetObservation.h"

#include "TDistMeasStation.h"
#include "T2DOffsetROM.h"
#include "T2DOffsetMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialLine.h"

//////////////////////////////////////////////////////////////
// Constructors and Destructors
//////////////////////////////////////////////////////////////

////////////////////////
// Default constructor
////////////////////////
T2DOffsetObservation::T2DOffsetObservation()
{
	fStationedPoint = 0;
	fTargetLine = 0;
}

///////////////////////////
// Constructor
///////////////////////////
T2DOffsetObservation::T2DOffsetObservation(DistStIterator station, Off2DROMIterator rom, Off2DMeasIterator meas,
											   TSpatialPoint* st, TSpatialLine* tg) :
fStation(station), fROM(rom), fMeasurement(meas), fStationedPoint(st), 
fTargetLine(tg) {
}

/*
//////////////////////////////////////////////////
// Copy constructor: not implemented
//////////////////////////////////////////////////
T2DOffsetObservation::T2DOffsetObservation(const T2DOffsetObservation& source) {
}
*/
//////////////////////////
// Destructor
/////////////////////////
T2DOffsetObservation::~T2DOffsetObservation()
{
	if (fStationedPoint != 0) delete fStationedPoint;
	if (fTargetLine != 0) delete fTargetLine;
}

/*
////////////////////////////////////////////////
// Copy assignement operator: not implemented 
////////////////////////////////////////////////
T2DOffsetObservation& T2DOffsetObservation::operator =(T2DOffsetObservation& right) {
}
*/