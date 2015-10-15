//
// TGyroOrientationROM.h : header file
/*! Class for a orientation round of measurements

    Patterns:
  
    Copyright 2003 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////


#ifndef SU_GYROORIENTATION_ROM
#define SU_GYROORIENTATION_ROM

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

//////////////////////////
// Forward declarations
//
class TTheodoliteStation;

#include <list>
#include <string>
#include <iostream>
using namespace std;

#include "TAUnreferencedROM.h"

#include "THorAngleMeasurement.h"
#include "TAngle.h"

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a round of horizontal angle measurements
class TGyroOrientationROM : public TAUnreferencedROM{ 

public:

	/*!@name Constructors / Destructor*/
	//@{
		/*!Constructor*/
		TGyroOrientationROM();
		//!Destructor
		virtual ~TGyroOrientationROM();
	//@}
	
	/*!Copy assignement operator (shallow copy)*/
	TGyroOrientationROM&		operator=(const TGyroOrientationROM& source);
	

	/*!@name Horizontal angle measurement related*/
	//@{
		/*! Adds a horizontal angle measurement to this ROM
		\param ha a pointer to the horizontal angle to be inserted.
		 NB : a copy of the meas. pointed to will be made*/
		virtual THorAngleMeasurement*	addGyroOrientation(THorAngleMeasurement* ha);

		/*!check if the pointed to angle measurement is already stored by this ROM
		\return a boolean*/
		virtual bool							notInContainer(THorAngleMeasurement *) const;
	//@}

	/*!@access methods*/
	//@{
		/*!Gives acces to a hor. angle measurement specified by its target point
		\param iterTg an iterator pointing to the target TSpatialPoint of the wanted measurement
		\return an iterator pointing to the hor. angle measurement made to the specified point*/
		virtual HorAngMeasIterator				getGyroOrieMeas(TSpatialPointName iterTg);

		/*!Gives acces to a hor. angel measurement specified by its target point
		\param iterTg an iterator pointing to the target TSpatialPoint of the wanted measurement
		\return a const iterator pointing to the hor. angle measurement made to the specified point*/
		virtual HorAngMeasConstIter				getGyroOrieMeas(TSpatialPointName iterTg) const;

		/*!\return an iterator to the first hor. angle measurement of this ROM's container*/
		virtual HorAngMeasIterator				getGyroOrieMeasBeginIterator();

		/*!\return a const iterator to the first hor. angle measurement of this ROM's container*/
		virtual HorAngMeasConstIter				getGyroOrieMeasBeginIterator() const;

		/*!\return an iterator to one past the last hor. angle measurement of this ROM's container*/
		virtual HorAngMeasIterator				getGyroOrieMeasEndIterator();

		/*!\return a const iterator to one past the last hor. angle measurement of this ROM's container*/
		virtual HorAngMeasConstIter				getGyroOrieMeasEndIterator() const;
	//@}

	/*!@returns a non-const pointer to this object*/
	virtual TGyroOrientationROM*					getPointer() const;

private:

	HorAngleContainer				fMeasuredAngles; /*!< list of hor. angle measurements */

};

/*@}*/


	/*!@name Typedefs*/
	//@{
	/*! Defines the container storing the horizontal angles ROMs as a type */
	typedef list<TGyroOrientationROM > GyroOrieROMContainer;
	/*! Defines an iterator to a TGyroOrieROM as a type */
	typedef GyroOrieROMContainer::iterator GyroOrieROMIterator;
	/*! Defines a const iterator to a TGyroOrieROM as a type */
	typedef GyroOrieROMContainer::const_iterator GyroOrieROMConstIter;
	//@}

#endif

