//
// OffsetToVerticalLineROM.h : header file
/*!

    Patterns:
  
    Copyright 2002 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////


#ifndef SU_OVERTICAL_LINE_ROM_
#define SU_OVERTICAL_LINE_ROM_

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include <list>
//using namespace std;

#include "TOffsetToLineOrPlaneMeasurement.h"

class OffsetToVerticalLineROM
{ 
public:

	OffsetToVerticalLineROM(const TSpatialPoint* plombKnownPoint)
	{
		this->plombKnownPoint = plombKnownPoint;
	}

	~OffsetToVerticalLineROM()
	{
		list<const TOffsetToLineOrPlaneMeasurement*>::iterator iter = offsetToVerticalLineMeasurements.begin();

		while (iter != offsetToVerticalLineMeasurements.end())
		{
			delete *iter;
			iter++;
		}
	}

	const TSpatialPoint* getPlombKnownPoint() const { return plombKnownPoint; }

	const list<const TOffsetToLineOrPlaneMeasurement*>& getMeasurements() const { return offsetToVerticalLineMeasurements; }

	void addMeasurement(const TOffsetToLineOrPlaneMeasurement* meas) { offsetToVerticalLineMeasurements.push_back(meas); }

private:

	const TSpatialPoint* plombKnownPoint;
	list<const TOffsetToLineOrPlaneMeasurement*> offsetToVerticalLineMeasurements;

};

#endif
