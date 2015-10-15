// TWorkingPointNamePair.h
//
/*! Constainer for pairs of points names. 
	Used to store the pair of points declared with option EREL
*/
// Patterns:
//
// 
// Copyright 2000-2003 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////

#ifndef SU_WORK_PT_PAIR
#define SU_WORK_PT_PAIR

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TPointNamePair.h"

class TWorkingPointNamePairs {

public:
	/*!@name constructor/destructor */
	//@{
		/*!Default constructor */
		TWorkingPointNamePairs();
		/*! Destructor */
		~TWorkingPointNamePairs();
	//@}

	/*! Equivalence operator */
	bool operator==(const TWorkingPointNamePairs& right) const;

	/*!@name Methods borrowed from the lists properties, named after them */
	//@{
		/*! Returns an iterator to the first element in the list */
		PtNamePairIter		begin() {return fWorkingPtNamePairs.begin();}

		/*! Returns a const iterator to the first element in the list */
		PtNamePairConstIter	begin() const {return fWorkingPtNamePairs.begin();}

		/*! Returns an iterator pointing after the last element in the list */
		PtNamePairIter		end() {return fWorkingPtNamePairs.end();}

		/*! Returns a const iterator pointing after the last element in the list */
		PtNamePairConstIter	end() const {return fWorkingPtNamePairs.end();}

		/*! Adds a new relative error at the end of the list, if it is not done yet (tests first if the object is
		already in the list or not)
		\returns an iterator to the pair of points names wether it was already inserted or just being inserted */
		PtNamePairIter		push_back(const TPointNamePair& pnp);
		/*! Erases the selected pair of points names and deletes its corresponding pointer from the list */
		bool	erase(TPointNamePair& pnp);
		/*! Returns the size of the list*/
		int		size() const {return fWorkingPtNamePairs.size();}
	//@}

		/*! Gets the warning message */
		string	getWarning() const {return fWarning;}

		/*! Sets the warning message */
		void	setWarning(const string msg) { fWarning = msg; return;}


private:

	PtNamePairsContainer	fWorkingPtNamePairs;
	int						fLastElNbr;
	bool					fListError;
	string					fWarning;
};

#endif //SU_WORK_PT_PAIR
