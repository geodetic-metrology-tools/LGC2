//
// ObservationROM.h : header file
/*! 

    Patterns:
  
    Copyright 2003 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////


#ifndef SU_OBSERVATION_ROM
#define SU_OBSERVATION_ROM

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TSpatialDistMeasurement.h"
#include "TOffsetToLineOrPlaneMeasurement.h"
#include "THorAngleMeasurement.h"
#include "THorizontalDistMeas.h"
#include "TSpatialDistMeasurement.h"
#include "TZenithDistMeasurement.h"
#include "PolarMeasurement.h"
#include "TVerticalDistMeasurement.h"
#include "TheodoliteTarget.h"
#include "EDMTarget.h"

template <typename Observation>
class ObservationROM
{ 

public:

	~ObservationROM()
	{
		list<const Observation*>::iterator iter = measurements.begin();

		while (iter != measurements.end())
		{
			delete *iter;
			iter++;
		}
	}

	const list<const Observation*>& getMeasurements() const { return measurements; }

	void addMeasurement(const Observation* meas) { measurements.push_back(meas); }

private:

	list<const Observation*> measurements;

};

typedef ObservationROM<TSpatialDistMeasurement<EDMTarget> > EDMSpatialDistanceROM;
typedef ObservationROM<TOffsetToLineOrPlaneMeasurement> OffsetToLineOrPlaneROM;
typedef ObservationROM<THorAngleMeasurement> TGyroOrientationROM;
typedef ObservationROM<THorAngleMeasurement> HorizontalAngleROM;
typedef ObservationROM<THorizontalDistMeas<TheodoliteTarget> > HorizontalDistanceROM;
typedef ObservationROM<TSpatialDistMeasurement<TheodoliteTarget> > SpatialDistanceROM;
typedef ObservationROM<TZenithDistMeasurement> ZenithDistanceROM;
typedef ObservationROM<PolarMeasurement> PolarROM;

typedef list<TGyroOrientationROM> GyroOrieROMContainer;
typedef GyroOrieROMContainer::iterator GyroOrieROMIterator;
typedef GyroOrieROMContainer::const_iterator GyroOrieROMConstIter;

#endif

