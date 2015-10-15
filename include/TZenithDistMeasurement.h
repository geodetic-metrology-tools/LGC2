// TZenithDistMeasurement.h
/*!
	Class for a zenithal distance measurement

	Pattern:

	Copyright 2002 CERN EST/SU. All rights reserved.
*/

////////////////////////////////////////////////////////////////

#ifndef SU_ZENITH_DIST_MEASUREMENT
#define SU_ZENITH_DIST_MEASUREMENT


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000




#include  "TAPointMeasurement.h"
#include  "TAngle.h"
#include  "TLength.h"
#include  "TheodoliteTarget.h"

////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a zenithal distance measurement 
class  TZenithDistMeasurement : public TAPointMeasurement  
{
public:
	
	TZenithDistMeasurement(int obsID, const TSpatialPoint* trgt, const TheodoliteTarget* polarTarget, const TAngle* obsAngle,
		const TAngle* sigma, const TLength* th, const TLength* ths, const TLength* tcs) : TAPointMeasurement(obsID, trgt)
	{
		observedValue = obsAngle;
		fSigmaAPriori = sigma;
		target = polarTarget;
		targetHeight = th;
		targetHeightSigma = ths;
		targetCenteringSigma = tcs;
	}

	~TZenithDistMeasurement()
	{
		delete observedValue;
		delete fSigmaAPriori;
		delete targetHeight;
		delete targetHeightSigma;
		delete targetCenteringSigma;
	}

	const TLength* getTargetCenteringSigma() const { return targetCenteringSigma; }

	const TAngle* getObservedValue() const { return observedValue; }

	const TAngle* getSigma() const { return fSigmaAPriori; }

	const TheodoliteTarget* getTarget() const { return target; }

	const TLength* getTargetHeight() const { return targetHeight; }

	const TLength* getTargetHeightSigma() const { return targetHeightSigma; }

private:

	const TAngle* observedValue; /*!< measured angle */
	const TAngle* fSigmaAPriori; /*!< estimated error on the angle measurement*/

	const TheodoliteTarget* target;
	const TLength* targetCenteringSigma;
	const TLength* targetHeight;
	const TLength* targetHeightSigma;
};

#endif  //SU_ZENITH_DIST_MEASUREMENT























//class TWorkingStations;
//#include  "UEOIndices.h"

	/*!Constructor
	@param parent a reference to the TZenithDistROM from which this measurement is part
	@param iterTg an iterator pointing to the target TSpatialPoint
	@param obsAngle a TAngle object representing the measured angle
	@param sigma a TAngle object representing the measurement's a priori precision*/
//	TZenithDistMeasurement(TZenithDistROM& parent, TWorkingPoints::PointIterator iterTg, TAngle obsAngle, TAngle sigma);

	/*!adds this measurement to the relevant station
	@param	wp a pointer to the working points object
	@param	ws a pointer to the working stations object*/
//	virtual bool				addToStation(TWorkingPoints* wp, TWorkingStations* ws);

//	TZenithDistROM*						fParentROM;




//protected:

	//!Tells the parent ROM this measurement has been disactivated
//	virtual void							sendDisactivatedToParent();
	//!Tells the parent ROM this measurement has been activated
//	virtual void							sendActivatedToParent();




//	TSpatialPointName					fStPtName; /*< stationed point name */
