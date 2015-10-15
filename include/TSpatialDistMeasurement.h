// TSpatialDistMeasurement
/*!
	Class for a spatial distance measurement

	Patterns:
 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_SPATIAL_DIST_MEASUREMENT
#define SU_SPATIAL_DIST_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TAPointMeasurement.h"
#include "TheodoliteTarget.h"
#include "TLength.h"
#include "TDistConstants.h"
/////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a spatial distance measurement
template<typename T>
class	TSpatialDistMeasurement : public TAPointMeasurement{

public :
	
	TSpatialDistMeasurement(int obsID, const TSpatialPoint* trgt, const T* t, const TLength* obsValue,
		const TLength* sigma, const TLength* ppmE, const TLength* th, const TLength* ths, const TLength* tcs) : TAPointMeasurement(obsID, trgt)
	{
		observedValue = obsValue;
		fSigmaAPriori = sigma;
		target = t;
		targetHeight = th;
		targetHeightSigma = ths;
		targetCenteringSigma = tcs;
		ppm = ppmE;
	}

	~TSpatialDistMeasurement()
	{
		delete observedValue;
		delete fSigmaAPriori;
		delete targetHeight;
		delete targetHeightSigma;
		delete targetCenteringSigma;
		delete ppm;
	}

	const TLength* getTargetCenteringSigma() const { return targetCenteringSigma; }

	const TLength* getObservedValue() const { return observedValue; }

	const TLength* getSigma() const { return fSigmaAPriori; }

	const TLength* getPPM() const { return ppm; }

	const T* getTarget() const { return target; }

	const TLength* getTargetHeight() const { return targetHeight; }

	const TLength* getTargetHeightSigma() const { return targetHeightSigma; }

private:

	const TLength* observedValue; /*!< measured distance */
	const TLength* fSigmaAPriori; /*!< estimated error on the distance measurement*/
	const TLength* ppm;

	const T* target;
	const TLength* targetCenteringSigma;
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
