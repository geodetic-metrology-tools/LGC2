// PolarMeasurement
/*!
	Class for a polar 3D measurement

	Patterns:
 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_POLAR_MEASUREMENT
#define SU_POLAR_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TAPointMeasurement.h"
#include "TheodoliteTarget.h"
#include "TLength.h"
#include "TAngle.h"
#include "TDistConstants.h"
#include "THorAngleMeasurement.h"
#include "TZenithDistMeasurement.h"
#include "TSpatialDistMeasurement.h"

//! Class for a polar measurement
class	PolarMeasurement : public TAPointMeasurement{

public :
	
	PolarMeasurement(int obsID, const TSpatialPoint* sp, const TLength* tcs, const TAngle* oa, const TAngle* ozd,
		const TLength* osd, const TAngle* as, const TAngle* zds, const TLength* sds, const TLength* p,
		const TheodoliteTarget* t, const TLength* th, const TLength* ths)
		: TAPointMeasurement(obsID, sp)
	{
        targetCenteringSigma = tcs;
        observedAngle = oa;
        observedZenithDistance = ozd;
        observedSpatialDistance = osd;
        angleSigma = as;
        zenithDistanceSigma = zds;
        spatialDistanceSigma = sds;
        ppm = p;
        target = t;
        targetHeight = th;
        targetHeightSigma = ths;
	}

	~PolarMeasurement()
	{
		delete targetCenteringSigma;
		delete observedAngle;
		delete observedZenithDistance;
		delete observedSpatialDistance;
		delete angleSigma;
		delete zenithDistanceSigma;
		delete ppm;
		delete targetHeight;
		delete targetHeightSigma;
	}

	const TLength* getTargetCenteringSigma() const { return targetCenteringSigma; }

	const TAngle* getAngleValue() const { return observedAngle; }

	const TAngle* getZenithDistanceValue() const { return observedZenithDistance; }

	const TLength* getSpatialDistanceValue() const { return observedSpatialDistance; }

	const TAngle* getAngleSigma() const { return angleSigma; }

	const TAngle* getZenithDistanceSigma() const { return zenithDistanceSigma; }

	const TLength* getSpatialDistanceSigma() const { return spatialDistanceSigma; }

	const TLength* getPPM() const { return ppm; }

	const TheodoliteTarget* getTarget() const { return target; }

	const TLength* getTargetHeight() const { return targetHeight; }

	const TLength* getTargetHeightSigma() const { return targetHeightSigma; }

private:

	const TLength* targetCenteringSigma;
	const TAngle* observedAngle;
	const TAngle* observedZenithDistance;
	const TLength* observedSpatialDistance;
	const TAngle* angleSigma;
	const TAngle* zenithDistanceSigma;
	const TLength* spatialDistanceSigma;
	const TLength* ppm;
	const TheodoliteTarget* target;
	const TLength* targetHeight;
	const TLength* targetHeightSigma;

};
	
#endif
















//class TWorkingStations;
//#include "UEOIndices.h"



	/* Constructor (comments to be updated) */
/*	\param parent a reference to the TSpatialDistROM from which this measurement is part
	\param iterTg an iterator pointing at the target TSpatialPoint
	\param obsDist a TLength object representing the measured distance
	\param sigma a TLength object representing the measurement's a priori precision
	\param height a TLength object representing the prism height
	TSpatialDistMeasurement(TSpatialDistROM& parent, TWorkingPoints::PointIterator iterTg, TLength obsDist, TLength sigma, TLength height, PrismNb pn);
*/	
	/* add this measurement to the relevant station (to be updated) */
/*	\param wp a pointer to the working points object
	\param ws a pointer to the working stations object */
//	virtual bool							addToStation(TWorkingPoints* wp, TWorkingStations* ws);
	
	/*! tells the parent ROM this measurement has been disactivated */
//	virtual void							sendDisactivatedToParent();

	/*! tells the parent ROM this measurement has been activated */
//	virtual void							sendActivatedToParent();
//	TSpatialDistROM*				fParentROM;






















//	TSpatialPointName				fStationPtName; /*!< name of the stationed point */
