//TLSCalcOffsetToSpatialLineObservation.h
/*! 
	Class for a offset (ECSP) observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position and length parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_SPATIAL_LINE_OBS
#define LSCALC_OFFSET_SPATIAL_LINE_OBS


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
using namespace std;

#include "TLSCalcOffsetToVerticalLineObservation.h"
#include "lsalgo/TLSCalcPosVectorParam.h" 
#include "lsalgo/TLSCalcFreeVectorParam.h" 
#include "lsalgo/TLSCalcLengthParam.h"

/*!\ingroup lgc++
	@{*/

//! Class for a offset to spatial line observation to be used in the least square algorithms
class TLSCalcOffsetToSpatialLineObservation : public TLSCalcOffsetToVerticalLineObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{
		TLSCalcOffsetToSpatialLineObservation(const TOffsetToLineOrPlaneMeasurement* obs,
			LSPosVecIter target,
			LSPosVecIter origin,
			LSPosVecIter pointOnLine,
			LSFreeVecIter unit,
			LSLengthIter cte,
			LSPosVecConstIter sp) :	TLSCalcOffsetToVerticalLineObservation(obs, target, origin, pointOnLine, unit, cte)
		{
			secondPointOnLine = sp;
		}

		~TLSCalcOffsetToSpatialLineObservation()
		{
		}

		const LSPosVecConstIter& getSecondPointOnLine() const { return secondPointOnLine; }

private:

	LSPosVecConstIter secondPointOnLine;


};

/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcOffsetToSpatialLineObservation >	LSOffsetToSpaLineContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToSpaLineContainer::iterator			LSOffsetToSpaLineIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToSpaLineContainer::const_iterator		LSOffsetToSpaLineConstIter;

#endif //LSCALC_OFFSET_SPATIAL_LINE_OBS



