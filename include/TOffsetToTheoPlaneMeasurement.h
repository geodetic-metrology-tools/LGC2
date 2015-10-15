// TOffsetToTheoPlaneMeasurement
/*!
	Class for a horizontal distance between a point and a theodolite's
	sighting line
	
	Patterns:
  
	Copyright 2003, CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_ECTH_MEASUREMENT
#define SU_ECTH_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



/////////////////////////////////////////////////////
// Forward declarations
//
#include "TAPointMeasurement.h"
#include "Scale.h"
#include "TLength.h"
#include "TAngle.h"
/////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/ 

//! Class for a horizontal distance measurement
class	TOffsetToTheoPlaneMeasurement : public TAPointMeasurement{

public:

	TOffsetToTheoPlaneMeasurement(int obsID, const TSpatialPoint* trgt, const Scale* s, const TAngle* angl, const TLength* obs,
		const TLength* sigma, const TLength* ppmE, const TLength* scs) : TAPointMeasurement(obsID, trgt)
	{
		observedAngle = angl;
		observedValue = obs;
		fSigmaAPriori = sigma;
		scale = s;
		scaleCenteringSigma = scs;
		ppm = ppmE;
	}

	~TOffsetToTheoPlaneMeasurement()
	{
		delete observedValue;
		delete fSigmaAPriori;
		delete scaleCenteringSigma;
		delete ppm;
	}

	const TAngle* getObservedAngle() const { return observedAngle; }

	const TLength* getScaleCenteringSigma() const { return scaleCenteringSigma; }

	const TLength* getObservedValue() const { return observedValue; }

	const TLength* getSigma() const { return fSigmaAPriori; }

	const TLength* getPPM() const { return ppm; }

	const Scale* getScale() const { return scale; }

private:

	const TAngle* observedAngle;
	const TLength* observedValue; /*!< measured distance */
	const TLength* fSigmaAPriori; /*!< estimated error on the angle measurement*/
	const TLength* ppm; /*!< estimated error on the angle measurement*/

	const Scale* scale;
	const TLength* scaleCenteringSigma;


};

#endif

