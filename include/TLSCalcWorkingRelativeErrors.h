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

class TLSCalcWorkingRelativeErrors {

public:

	/*!@name Constructor/Destructor */
	//@{
	/*! Default constructor */
	TLSCalcWorkingRelativeErrors();
	/*! Destructor */
	~TLSCalcWorkingRelativeErrors();
	//@}

	/*!@name Methods borrowed from the lists properties, named after them */
	//@{
	/*! Returns an iterator to the first element in the list */
	LSRelErrorIter		begin();
	/*! Returns a const iterator to the first element in the list */
	LSRelErrorConstIter	begin() const;
	/*! Returns an iterator pointing after the last element in the list */
	LSRelErrorIter		end();
	/*! Returns a const iterator pointing after the last element in the list */
	LSRelErrorConstIter	end() const;
	/*! Adds a new relative error at the end of the list, if it is not done yet (tests first if the object is
	already in the list or not)
	\returns an iterator to the relative error wether it was already inserted or just being inserted */
	LSRelErrorIter		push_back(const TLSCalcRelativeError& erel);
	/*! Erases the selected relative error and deletes its corresponding pointer from the list */
	bool	erase(TLSCalcRelativeError& erel);
	/*! Returns the size of the list*/
	int					size() const {return fRelErrorsList.size();}
	//@}

	/*! returns the warning message */
	string	getWarning() const {return fWarning;}
	/*! sets the warning message */
	void	setWarning(const string msg); 

private:

	LSRelErrorsContainer	fRelErrorsList; /*!< list of relative errors */
	string					fWarning; /*!< warning in case of an invalid point */
	bool					fListError; /*!< handling error attribute: false if no errors */
	size_t					fLastElNbr;	/*!< last number of elements in the list */
};

#endif //SU_LS_WORK_EREL