// LevelMeasurement.h
//
/*! Class for a vertical distance measurement 

	Patterns:
 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_LEVEL_MEASUREMENT
#define SU_LEVEL_MEASUREMENT


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
using namespace std;
//
#include "TLength.h"
#include  "TAMeasurement.h"
#include  "TDistConstants.h"
#include  "Staff.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a vertical distance measurement
class  LevelMeasurement : public TAPointMeasurement  
{
public:

	LevelMeasurement(int obsID, const TSpatialPoint* target,
		const TLength* obsDist, const TLength* sigma, const TLength* ppmE,
		const TLength* horDist, const TLength* hdSigma, bool hasDistanceMeasurement, const TLength* tgH, const TLength* tgHS,
		const Staff* st) : TAPointMeasurement(obsID, target)
	{
        observedValue = obsDist;
        sigma = sigma;
        ppm = ppmE;
		hasDistanceMeas = hasDistanceMeasurement;
        horizontalDistanceValue = horDist;
        hodizontalDistanceSigma = hdSigma;
        targetHeight = tgH;
        targetHeightSigma = tgHS;
		staff = st;
	}

	~LevelMeasurement()
	{
        delete observedValue;
        delete sigma;
        delete ppm;
        delete horizontalDistanceValue;
        delete hodizontalDistanceSigma;        
        delete targetHeight;
        delete targetHeightSigma;
	}

	const TLength* getObservedValue() const { return observedValue; }
	const TLength* getSigma() const { return sigma; }
	const TLength* getPPM() const { return ppm; }
	const TLength* getHorizontalDistanceValue() const { return horizontalDistanceValue; }
	const TLength* getHorizontalDistanceSigma() const { return hodizontalDistanceSigma; }

	bool hasDistanceMeasurement() const { return hasDistanceMeas; }

	const TLength* getTargetHeight() const { return targetHeight; }
	const TLength* getTargetHeightSigma() const { return targetHeightSigma; }

	const Staff* getStaff() const { return staff; }

private:

	const TLength* observedValue; /*!< vertical measurement */
	const TLength* sigma; /*!< error on the vertical measurement */
	const TLength* ppm; /*!< error on the vertical measurement */

	bool hasDistanceMeas;
	const TLength* horizontalDistanceValue;
	const TLength* hodizontalDistanceSigma;
	
	const TLength* targetHeight;
	const TLength* targetHeightSigma;

	const Staff* staff;
};


/**@name Typedefs*/
//@{
// Type of the container used to store the vertical dist measurements
typedef list< TVerticalDistMeasurement > VertDistContainer;
// Type of the iterator pointing to an element of the vertical dists container
typedef VertDistContainer::iterator VertDistMeasIterator;
// Type of the const iterator pointing to an element of the vertical dists container
typedef VertDistContainer::const_iterator VertDistMeasConstIter;
//@}


#endif // SU_LEVEL_MEASUREMENT


