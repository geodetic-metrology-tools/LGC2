// TPointNamePair.h
//
/*! Pair of points names. 
	Used to store the pair of points declared with option EREL

	Patterns:

 
	Copyright 2000-2008 CERN TS/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_PT_PAIR
#define SU_PT_PAIR

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
using namespace std;

#include "TSpatialPointName.h"

//! class for a pair of points
class TPointNamePair {

public:
	/*!@name constructor/destructor */
	//@{
		/*! Default constructor */
		TPointNamePair();

		/*!Constructor
		\param pt1	spatial point name 1
		\param pt2	spatial point name 2 */
		TPointNamePair(const TSpatialPointName pt1, const TSpatialPointName pt2);

		/*! Copy constructor */
		TPointNamePair(const TPointNamePair& source);

		/*! Destructor */
		virtual ~TPointNamePair();
	//@}

	/*! copy assignment operator */
	TPointNamePair& operator=(const TPointNamePair& );

	/*! Equivalence operator */
	bool operator==(const TPointNamePair& right) const;

	/*!@name Access methods */
	//@{
	/*! returns point1's name as a spatial point name */
	TSpatialPointName	getPt1Name() const {return fPt1Name;}
	/*! returns point2's name as a spatial point name */
	TSpatialPointName	getPt2Name() const {return fPt2Name;}
	//@}

private:

	TSpatialPointName	fPt1Name; /*!< name of point 1 */
	TSpatialPointName	fPt2Name; /*!< name of point 2 */

};

/*! List of point name pairs */
typedef	list<TPointNamePair>	PtNamePairsContainer;
/*! iterator for the PtNamePairsContainer */
typedef PtNamePairsContainer::iterator	PtNamePairIter;
/*! Const iterator for the PtNamePairsContainer */
typedef PtNamePairsContainer::const_iterator PtNamePairConstIter;

#endif // SU_PT_PAIR
