// TVerticalDistMeasurement.h
//
/*! Class for a vertical distance measurement 

	Patterns:
 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TVerticalDistMeasurement
#define SU_TVerticalDistMeasurement


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
class  TVerticalDistMeasurement : public TAPointMeasurement  
{
public:

	TVerticalDistMeasurement(int obsID, const TSpatialPoint* tg1, const TSpatialPoint* tg2,
		const TLength* obsDist, const TLength* fsSigma, const TLength* fsPpmE, const TLength* sndSigma, const TLength* sndPpmE,
		const TLength* fsTgH, const TLength* fsTgHS, const TLength* sndTgH, const TLength* sndTgHS,
		const Staff* fS, const Staff* sS);

	~TVerticalDistMeasurement();

	const TLength* getObservedValue() const { return observedValue; }
	const TLength* getFirstStaffSigma() const { return firstStaffSigma; }
	const TLength* getFirstStaffPPM() const { return firstStaffPpm; }
	const TLength* getSecondStaffSigma() const { return secondStaffSigma; }
	const TLength* getSecondStaffPPM() const { return secondStaffPpm; }

	const TLength* getFirstTargetHeight() const { return firstTargetHeight; }
	const TLength* getFirstTargetHeightSigma() const { return firstTargetHeightSigma; }

	const TLength* getSecondTargetHeight() const { return secondTargetHeight; }
	const TLength* getSecondTargetHeightSigma() const { return secondTargetHeightSigma; }

	const Staff* getFirstStaff() const { return firstStaff; }
	const Staff* getSecondStaff() const { return secondStaff; }

	const TSpatialPoint* getSecondTargetPoint() const { return secondTargetPoint; }

private:

	const TSpatialPoint* secondTargetPoint;
	const TLength* observedValue; /*!< vertical measurement */

	const TLength* firstStaffSigma; /*!< error on the vertical measurement */
	const TLength* firstStaffPpm;
	const TLength* secondStaffSigma; /*!< error on the vertical measurement */
	const TLength* secondStaffPpm;
	
	const TLength* firstTargetHeight;
	const TLength* firstTargetHeightSigma;
	
	const TLength* secondTargetHeight;
	const TLength* secondTargetHeightSigma;

	const Staff* firstStaff;
	const Staff* secondStaff;
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


#endif // SU_TVerticalDistMeasurement


