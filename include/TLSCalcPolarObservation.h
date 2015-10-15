//TLSCalcPolarObservation.h

/*! 
	Class for a horizontal angle observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				angle observations, position and angle parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_POLAR_OBS
#define LSCALC_POLAR_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
using namespace std;

#include "PolarMeasurement.h"
#include "lsalgo/TLSCalcOrientationParam.h"
#include "lsalgo/TLSCalcPosVectorParam.h" 

#include "TALSCalcObservation.h"
#include "TLSCalcHorAngleObservation.h"
#include "TLSCalcZenithDistObservation.h"
#include "TLSCalcSpatialDistObservation.h"
#include "TheodoliteTarget.h"


/*!\ingroup lgc++
	@{*/

//! Class for a polar observation to be used in the least square algorithms
class TLSCalcPolarObservation : public TALSCalcObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{

		TLSCalcPolarObservation(const PolarMeasurement* pMeas,LSOrientIter v0,
			LSPosVecIter st,LSPosVecIter tg,LSOrientIter cte, LSLengthIter hi,TLength hp,
			LSLengthIter cst, const TheodoliteStation* theoStation);

		TLSCalcPolarObservation(const TLSCalcPolarObservation&);

		/*! Destructor */
		~TLSCalcPolarObservation();
	//@}

		TLSCalcHorAngleObservation* getHorizontalAngleObservation() { return horizontalAngleObservation; }
		TLSCalcZenithDistObservation* getZenithDistanceObservation() { return zenithDistanceObservation; }
		TLSCalcSpatialDistObservation<TheodoliteTarget>* getSpatialDistanceObservation() { return spatialDistanceObservation; }

		const TLSCalcHorAngleObservation* getHorizontalAngleObservation() const { return horizontalAngleObservation; }
		const TLSCalcZenithDistObservation* getZenithDistanceObservation() const { return zenithDistanceObservation; }
		const TLSCalcSpatialDistObservation<TheodoliteTarget>* getSpatialDistanceObservation() const { return spatialDistanceObservation; }

		int getObservationID() const { return horizontalAngleObservation->getObservationID(); }
		
		UEOIndices	setEOIndices(UEOIndices ueoi)
		{
			fIndices.EIndex = ueoi.EIndex;
			fIndices.OIndex = ueoi.OIndex;
			ueoi.EIndex += 3;
			ueoi.OIndex += 3;
			return ueoi;
		}

		TLSCalcPolarObservation operator= (const TLSCalcPolarObservation&);

private:
	
	LSOrientIter V0;
	LSPosVecIter station;
	LSPosVecIter target;
	LSOrientIter constant;
	LSLengthIter instrumentHeight;
	TLength targetHeight;
	LSLengthIter lengthConstant;

	const TheodoliteStation* tStation;

	TLSCalcHorAngleObservation* horizontalAngleObservation;
	TLSCalcZenithDistObservation* zenithDistanceObservation;
	TLSCalcSpatialDistObservation<TheodoliteTarget>* spatialDistanceObservation;

	const THorAngleMeasurement* horizontalAngleMeasurement;
	const TZenithDistMeasurement* zenithDistanceMeasurement;
	const TSpatialDistMeasurement<TheodoliteTarget>* spatialDistanceMeasurement;

};

/*@}*/

/*!@name Constants definition */
//@{
	/*! List of ls calc horizontal angle observations  */
	typedef list< TLSCalcPolarObservation >		LSPolarContainer;
	/*! Iterator of the list of ls calc horizontal angle observations */
	typedef LSPolarContainer::iterator		LSPolarIter;
	/*! Constant iterator of the list of ls calc horizontal angle observations */
	typedef LSPolarContainer::const_iterator	LSPolarConstIter;
//@}


#endif //LSCALC_POLAR_OBS





