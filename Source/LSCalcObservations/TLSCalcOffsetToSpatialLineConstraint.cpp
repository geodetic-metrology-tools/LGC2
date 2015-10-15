//TLSCalcOffsetToSpatialLineConstraint.cpp

#include "TLSCalcOffsetToSpatialLineConstraint.h"




TLSCalcOffsetToSpatialLineConstraint::TLSCalcOffsetToSpatialLineConstraint(
			LSPosVecIter origin,
			LSFreeVecIter unit,
			LSPosVecIter pointOnLine)
{	
	fOrigin = origin;
	fUnitVector = unit;
	this->pointOnLine = pointOnLine;
}
