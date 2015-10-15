////
//// OffsetToSpatialLineROM.h : header file
///*!
//
//    Patterns:
//  
//    Copyright 2002 CERN EST/SU. All rights reserved.
//*/
//////////////////////////////////////////////////////////////////////////


#ifndef SU_OSPATIAL_LINE_ROM_
#define SU_OSPATIAL_LINE_ROM_

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include <list>
//using namespace std;

#include "TOffsetToLineOrPlaneMeasurement.h"

class OffsetToSpatialLineROM
{ 
public:

	OffsetToSpatialLineROM(const TSpatialPoint* firstPointOnLine, const TSpatialPoint* secondPointOnLine) :
		firstPointOnLine(firstPointOnLine),
		secondPointOnLine(secondPointOnLine){}

	~OffsetToSpatialLineROM()
	{
		list<const TOffsetToLineOrPlaneMeasurement*>::iterator iter = offsetToSpatialLineMeasurements.begin();

		while (iter != offsetToSpatialLineMeasurements.end())
		{
			delete *iter;
			iter++;
		}
	}

	const TSpatialPoint* getFirstPointOnLine() const { return firstPointOnLine; }
	const TSpatialPoint* getSecondPointOnLine() const { return secondPointOnLine; }

	const list<const TOffsetToLineOrPlaneMeasurement*>& getMeasurements() const { return offsetToSpatialLineMeasurements; }

	void addMeasurement(const TOffsetToLineOrPlaneMeasurement* meas) { offsetToSpatialLineMeasurements.push_back(meas); }

private:

	const TSpatialPoint* firstPointOnLine;
	const TSpatialPoint* secondPointOnLine;
	list<const TOffsetToLineOrPlaneMeasurement*> offsetToSpatialLineMeasurements;

};

#endif
