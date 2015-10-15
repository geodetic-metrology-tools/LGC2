// TWTWDistStation.h
//
/*! Class for a wire to wire distance measurement station 

	Patterns:

	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TWTWDistStation
#define SU_TWTWDistStation


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//using namespace std;
//
#include "TWireToWireDistMeas.h"

#include  "TAFreeInstrumentStation.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a wire to wire distance measurement station
class  TWTWDistStation : public TAFreeInstrumentStation  
{
public:

	/**@name Constructors and Destructors */
	//@{
		/*!DefautConstructor */
		TWTWDistStation();
		/*! Copy Constructor */ 
		TWTWDistStation(const  TWTWDistStation&);
		/*! Destructor */
		virtual  ~TWTWDistStation();
	//@}

	/*! Copy Assignment Operator */
	TWTWDistStation& operator=( const TWTWDistStation& );
	/*! Equality operator : temporary */
	virtual bool operator==( const TWTWDistStation&);
	/*! Less than operator : temporary */
//	bool operator<(const TWTWDistStation&);

	/*!@name Member Functions */
	//@{
	/*!sets the station's only measurement */
	virtual TWireToWireDistMeas*			setWTWDistMeas(TWireToWireDistMeas* dist);
	/*! return an iterator to the station's only measurement */
	virtual WTWDistMeasIterator				getWTWDistMeas();
	/*! return a const iterator to the station's only measurement */
	virtual WTWDistMeasConstIter			getWTWDistMeas() const;
	//@}

private:

	TWireToWireDistMeas			fWTWDistMeas; /*!< wire to wire distance measurement attribute */

};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_TWTWDistStation
/*@}*/



/*!@name Typedefs*/
//@{
//!Type of the container used to store WTW stations
typedef list< TWTWDistStation > WTWStContainer;
//!Type of an iterator pointing to an element of the WTW stations container
typedef WTWStContainer::iterator WTWStIterator;
//!Type of a const iterator pointing to an element of the WTW stations container
typedef WTWStContainer::const_iterator WTWStConstIter;
//@}












//class TWorkingStations;

//		\param ws a pointer to the TWorkingStations object*/
//		TWTWDistStation(TWorkingStations* parent);


	///tells the child measurement that the station is disactivated
//	virtual void							sendDisactivatedToChildren();
	///tells the child measurement that the station is activated
//	virtual void							sendActivatedToChildren();
