////////////////////////////////////////////////////////////////////
// GyroscopeStationConverter.h
/*!
Write Gyroscope Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_EDM_GYRO_STATION_CONVERTER
#define SU_EDM_GYRO_STATION_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLGCProject.h"
#include "TAngleObsConverter.h"
#include "ObservationROM.h"

//Class definition
class  GyroscopeStationConverter : public TAngleObsConverter
{
	
public:

	GyroscopeStationConverter(TAStreamFormatter& stream);

	bool readObservations(int& lineNumber, TLGCProject* project);

	THorAngleMeasurement* readObservation(const Gyroscope* gyro, TLGCProject* project, int& i);
	
	void	writeResults(LSGyroOrieConstIter	obsIt);
	void	writeResultsHeader();
	void	writeMesData(LSGyroOrieConstIter	obsIt);

};

/*@}*/

#endif // SU_EDM_GYRO_STATION_CONVERTER
