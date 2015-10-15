////////////////////////////////////////////////////////////////////
// TLSCalcWorkingRelativeErrors.h
/*!
Container for the relative errors to be determined as part of a LGC calculation

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/

#ifndef SU_LS_WORK_EREL
#define SU_LS_WORK_EREL

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TLSCalcRelativeError.h"
#include "TLSCalcWorkingObservation.h"

class TLSCalcWorkingRelativeErrors : public TLSCalcWorkingObservation<TLSCalcRelativeError> {

public:

	TLSCalcWorkingRelativeErrors();

	/*! returns the warning message */
	string	getWarning() const {return fWarning;}
	/*! sets the warning message */
	void	setWarning(const string msg); 

private:

	string					fWarning; /*!< warning in case of an invalid point */
};

#endif //SU_LS_WORK_EREL
