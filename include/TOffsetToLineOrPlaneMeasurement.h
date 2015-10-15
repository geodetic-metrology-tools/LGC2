// TOffsetToLineOrPlaneMeasurement
/*!
	Class for a distance between a point and a spatial line
	
	Patterns:
  
	Copyright 2003, CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_OFFSET_LINE_PLANE_MEASUREMENT
#define SU_OFFSET_LINE_PLANE_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



/////////////////////////////////////////////////////
// Forward declarations
//
#include "TAPointMeasurement.h"
#include "Scale.h"
/////////////////////////////////////////////////////


class	TOffsetToLineOrPlaneMeasurement : public TAPointMeasurement{

public :

		TOffsetToLineOrPlaneMeasurement(int obsID, const TSpatialPoint* trgt, const Scale* scl, const TLength* obs,
			const TLength* s, const TLength* ppmE, const TLength* ics) : TAPointMeasurement(obsID, trgt)
		{
			scale = scl;
            observedValue = obs;
            sigma = s;
            ppm = ppmE;
            instrumentCenteringSigma = ics;
		}
	
		~TOffsetToLineOrPlaneMeasurement()
		{
			delete observedValue;
			delete sigma;
			delete ppm;
			delete instrumentCenteringSigma;
		}

		const Scale* getScale() const { return scale; }

		const TLength* getObservedValue() const { return observedValue; }

		const TLength* getSigma() const { return sigma; }

		const TLength* getPPM() const { return ppm; }

		const TLength* getInstrumentCenteringSigma() const { return instrumentCenteringSigma; }

private:

	const TLength* observedValue;
	const TLength* sigma;
	const TLength* ppm;
	const TLength* instrumentCenteringSigma;

	const Scale* scale;

};


#endif

