//TLSCalcOffsetToVerticalPlaneConstraint.h
/*! 
	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_VER_PLANE_CSTR
#define LSCALC_OFFSET_VER_PLANE_CSTR


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#include <list>
using namespace std;

#include "TALSCalcLengthObservation.h" 
#include "lsalgo/TLSCalcFreeVectorParam.h" 
#include "lsalgo/TLSCalcPosVectorParam.h" 

/*!\ingroup lgc++
	@{*/

//! Class for a offset to vertical line observation to be used in the least square algorithms
class TLSCalcOffsetToVerticalPlaneConstraint : public TALSCalcLengthObservation
{
public:

	TLSCalcOffsetToVerticalPlaneConstraint(LSFreeVecIter unit, LSPosVecIter or) { fUnitVector = unit; origin = or; }

	LSFreeVecIter getUnitVector() const { return fUnitVector; }
	LSPosVecIter getOrigin() const { return origin; }
	
	UEOIndices	setEOIndices(UEOIndices ueoi)
	{
		fIndices.EIndex = ueoi.EIndex;
		fIndices.OIndex = ueoi.OIndex;
		ueoi.EIndex += 2;
		ueoi.OIndex += 2;
		return ueoi;
	}

private:

	LSFreeVecIter fUnitVector;
	LSPosVecIter origin;
		
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcOffsetToVerticalPlaneConstraint >		LSOffsetToVerPlaneCstrContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerPlaneCstrContainer::iterator			LSOffsetToVerPlaneCstrIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerPlaneCstrContainer::const_iterator		LSOffsetToVerPlaneCstrConstIter;
//@}

#endif //LSCALC_OFFSET_VER_PLANE_CSTR



