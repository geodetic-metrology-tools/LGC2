//
// TAPointMeasurement.h : header file
// abstract base class holding the behaviour common to all
// targeted measurements
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_PT_MEAS
#define SU_PT_MEAS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
using namespace std;

#include "TAMeasurement.h"
#include "TSpatialPoint.h"

//! Abstract Base Class. Defines the methods common to all survey measurements having a target point
class TAPointMeasurement : public TAMeasurement 
{
public:

	const TSpatialPoint* getTargetPoint() const { return targetPoint; }	

	virtual bool operator ==(const TAPointMeasurement& right) const
	{
		return *targetPoint == *right.getTargetPoint();
	}

	virtual ~TAPointMeasurement() { }

protected:

	TAPointMeasurement(int obsID, const TSpatialPoint* tg) : TAMeasurement(obsID) { targetPoint = tg; }

	const TSpatialPoint* targetPoint;

};
#endif
