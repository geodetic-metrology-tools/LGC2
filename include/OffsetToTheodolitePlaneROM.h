//
// OffsetToTheodolitePlaneROM.h : header file
/*!

    Patterns:
  
    Copyright 2002 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////


#ifndef SU_OTHEOPLANE_ROM
#define SU_OTHEOPLANE_ROM

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include <list>
using namespace std;

#include "TOffsetToTheoPlaneMeasurement.h"

class OffsetToTheodolitePlaneROM
{ 
public:

	OffsetToTheodolitePlaneROM(const TAngle* angle)
	{
		measuredAngle = angle;
	}

	~OffsetToTheodolitePlaneROM()
	{
		list<const TOffsetToTheoPlaneMeasurement*>::iterator iter = offsetToTheodolitePlaneMeasurements.begin();

		while (iter != offsetToTheodolitePlaneMeasurements.end())
		{
			delete *iter;
			iter++;
		}

		delete measuredAngle;
	}

	const list<const TOffsetToTheoPlaneMeasurement*>& getMeasurements() const { return offsetToTheodolitePlaneMeasurements; }

	void addMeasurement(const TOffsetToTheoPlaneMeasurement* meas) { offsetToTheodolitePlaneMeasurements.push_back(meas); }

private:

	const TAngle* measuredAngle;
	list<const TOffsetToTheoPlaneMeasurement*> offsetToTheodolitePlaneMeasurements;

};

#endif
