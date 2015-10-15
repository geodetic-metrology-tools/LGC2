// THorizontalDistMeas
/*!
	Class for a horizontal distance measurement
	
	Patterns:
  
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_HOR_DIST_MEASUREMENT
#define SU_HOR_DIST_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TAPointMeasurement.h"
#include "TLength.h"

//! Class for a horizontal distance measurement
template <typename T>
class	THorizontalDistMeas : public TAPointMeasurement{

public :
	THorizontalDistMeas(int obsID, const TSpatialPoint* trgt, const T* t, const TLength* obs,
		const TLength* sigma, const TLength* ppmE, const TLength* tcs) : TAPointMeasurement(obsID, trgt)
	{
		observedValue = obs;
		fSigmaAPriori = sigma;
		target = t;
		targetCenteringSigma = tcs;
		ppm = ppmE;
	}

	~THorizontalDistMeas()
	{
		delete observedValue;
		delete fSigmaAPriori;
		delete targetCenteringSigma;
		delete ppm;
	}

	const TLength* getTargetCenteringSigma() const { return targetCenteringSigma; }

	const TLength* getObservedValue() const { return observedValue; }

	const TLength* getSigma() const { return fSigmaAPriori; }

	const TLength* getPPM() const { return ppm; }

	const T* getTarget() const { return target; }

private:

	const TLength* observedValue; /*!< measured distance */
	const TLength* fSigmaAPriori; /*!< estimated error on the angle measurement*/
	const TLength* ppm; /*!< estimated error on the angle measurement*/

	const T* target;
	const TLength* targetCenteringSigma;

};


#endif













//class TWorkingStations;
//#include "UEOIndices.h"


	/*! Constructor */
/*	\param parent a reference to the THorizontalDistROM from which this measurement is part
	\param iterTg an iterator pointing at the target TSpatialPoint
	\param obsDist a TLength object representing the measured distance
	\param sigma a TLength objct representing the measurement's a priori precision*/
//	THorizontalDistMeas(THorizontalDistROM& parent, TWorkingPoints::PointIterator iterTg, TLength obsDist, TLength sigma);

	/*! add this measurement to the relevant station */
/*	param wp a pointer to the working points object
	param ws a pointer to the working stations object*/
//	virtual bool							addToStation(TWorkingPoints* wp, TWorkingStations* ws);

	/*! tells the parent ROM this measurement has been disactivated */
//	virtual void							sendDisactivatedToParent();

	/*! tells the parent ROM this measurement has been activated */
//	virtual void							sendActivatedToParent();

//	THorizontalDistROM*				fParentROM;
