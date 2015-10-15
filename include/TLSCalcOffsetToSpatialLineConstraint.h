//TLSCalcOffsetToSpatialLineConstraint.h
/*! 
	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_SPA_LINE_CSTR
#define LSCALC_OFFSET_SPA_LINE_CSTR


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#include <list>
using namespace std;

#include "TALSCalcLengthObservation.h" 
#include "lsalgo/TLSCalcPosVectorParam.h" 
#include "lsalgo/TLSCalcFreeVectorParam.h" 

/*!\ingroup lgc++
	@{*/

//! Class for a offset to vertical line observation to be used in the least square algorithms
class TLSCalcOffsetToSpatialLineConstraint : public TALSCalcLengthObservation
{
public:

	TLSCalcOffsetToSpatialLineConstraint(
		LSPosVecIter origin,
		LSFreeVecIter unit,
		LSPosVecIter pointOnLine);

	LSPosVecIter getOrigin() const { return fOrigin; }
	LSFreeVecIter getUnitVector() const { return fUnitVector; }
	LSPosVecIter getPointOnLine() const { return pointOnLine; }
		
	UEOIndices	setEOIndices(UEOIndices ueoi)
	{
		fIndices.EIndex = ueoi.EIndex;
		fIndices.OIndex = ueoi.OIndex;
		ueoi.EIndex += 2;
		ueoi.OIndex += 2;
		return ueoi;
	}

private:

	LSPosVecIter fOrigin;
	LSFreeVecIter fUnitVector;
	LSPosVecIter pointOnLine;
		
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcOffsetToSpatialLineConstraint >		LSOffsetToSpaLineCstrContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToSpaLineCstrContainer::iterator			LSOffsetToSpaLineCstrIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToSpaLineCstrContainer::const_iterator		LSOffsetToSpaLineCstrConstIter;
//@}

#endif //LSCALC_OFFSET_SPA_LINE_CSTR



