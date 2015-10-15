//
// THorAngleMeasurement.h : header file
//
// Class for a survey measurement as defined for survey purposes
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_HOR_ANGLE_MEASUREMENT
#define SU_HOR_ANGLE_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TAPointMeasurement.h"
#include "TheodoliteTarget.h"
#include "TAngle.h"
#include "TLength.h"

//! Class for a horizontal angle measurement
class THorAngleMeasurement : public TAPointMeasurement
{

public:

	THorAngleMeasurement(int obsID, const TSpatialPoint* trgt, const TheodoliteTarget* polarTarget, const TAngle* obsAngle,
		const TAngle* sigma, const TLength* tcs) : TAPointMeasurement(obsID, trgt)
	{
		observedValue = obsAngle;
		fSigmaAPriori = sigma;
		target = polarTarget;
		targetCenteringSigma = tcs;
	}

	~THorAngleMeasurement()
	{
		delete observedValue;
		delete fSigmaAPriori;
		delete targetCenteringSigma;
	}

	const TLength* getTargetCenteringSigma() const { return targetCenteringSigma; }

	const TAngle* getObservedValue() const { return observedValue; }

	const TAngle* getSigma() const { return fSigmaAPriori; }

	const TheodoliteTarget* getTarget() const { return target; }

private:

	const TAngle* observedValue; /*!< measured angle */
	const TAngle* fSigmaAPriori; /*!< estimated error on the angle measurement*/

	const TheodoliteTarget* target;
	const TLength* targetCenteringSigma;

};

#endif









//class TWorkingStations;
//#include "UEOIndices.h"


/*!adds this measurement to the relevant station
	@param	wp a pointer to the working points object
	@param	ws a pointer to the working stations object*/
	//virtual bool				addToStation(TSpatialPointName wp, TWorkingStations* ws);


//protected: 

	//!Tells the parent ROM this measurement has been disactivated
	//virtual void							sendDisactivatedToParent();
	//!Tells the parent ROM this measurement has been activated
	//virtual void							sendActivatedToParent();



	//THorAngleROM*						fParentROM;





