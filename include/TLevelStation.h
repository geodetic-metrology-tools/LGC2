// TLevelStation.h
//
/*! 
	Class for a level instrument station 

	Patterns:

 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TLevelStation
#define SU_TLevelStation


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <list>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
//
#include "TAFreeInstrumentStation.h"
#include "TVerticalDistROM.h"
////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a level station
class  TLevelStation : public TAFreeInstrumentStation  
{
public:

	/*!@name Constructors and Destructors */
	//@{
	/*! Default constructor */
	TLevelStation();
	/*! Copy Constructor */
	TLevelStation(const  TLevelStation&);
	/*! Destructor */
	virtual  ~TLevelStation();
	//@}


	/*!@name Overloaded operators */
	//@{
	/*! Copy Assignment Operator */
	TLevelStation& operator=( const TLevelStation& );
	//@}

	/*!@name Vertical distance round of measurements related*/
	//@{
	/*! Adds a vertical dist ROM to the station
	\param vdROM a pointer to the vertical dist ROM to be inserted. NB : a copy of the ROM pointed to will be made*/
	virtual TVerticalDistROM*					addVertDistROM(TVerticalDistROM *vdROM);
	/*! Returns a boolean indicating if the pointed to vertical dist ROM is already stored by the station*/
	virtual bool							notInContainer(TVerticalDistROM *) const;
	/*! Returns an iterator pointing to the first vertical dist ROM of the station*/
	virtual VertDistROMIterator				getVertDistROMBeginIterator() ;
	/*! Returns a const iterator pointing to the first vertical dist ROM of the station*/
	virtual VertDistROMConstIter			getVertDistROMBeginIterator() const;
	/*! Returns an iterator pointing to one past the last vertical dist ROM of the station*/
	virtual VertDistROMIterator				getVertDistROMEndIterator() ;
	/*! Returns a const iterator pointing to one past the last vertical dist ROM of the station*/
	virtual VertDistROMConstIter			getVertDistROMEndIterator() const;
	/*! Returns an iterator pointing to the corresponding vertical dist ROM
	\param iterRef an iterator pointing to the reference point of the TVerticalDistROM object looked for */
	virtual VertDistROMIterator				getVertDistROM(/*TWorkingPoints::PointIterator*/ TSpatialPointName iterRef) ;
	/*! Returns a const iterator pointing to the corresponding vertical dist ROM
	\param iterRef an iterator pointing to the reference point of the TVerticalDistROM object looked for */
	virtual VertDistROMConstIter			getVertDistROM(/*TWorkingPoints::PointIterator*/ TSpatialPointName iterRef) const;
	//@}

private :

	int										fLastVertDistSeriesNbr; /*!< last number of vertical distance series */
	VertDistROMContainer					fMadeVDistROMs; /*!< list of vertical distance round of measurements */
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////
#endif // SU_TLevelStation

/*@}*/


/*!@name Typedefs*/
//@{
//!Type of the container used to store Level stations
typedef list< TLevelStation > LevelStContainer;
//!Type of an iterator pointing to an element of the level stations container
typedef LevelStContainer::iterator LevelStIterator;
//!Type of a const iterator pointing to an element of the level stations container
typedef LevelStContainer::const_iterator LevelStConstIter;
//@}







//class  TWorkingStations;
	/** Constructor
	@param parent a pointer to the TWorkingStations objects storing all the instrument stations*/
//	TLevelStation(TWorkingStations* parent);

	//@}

	//virtual void							sendEnoughTotalEqns() const;
	///Tells the children ROMs that this station has been disactivated
//	virtual void							sendDisactivatedToChildren();
	///Tells the children ROMs that this station has been activated
//	virtual void							sendActivatedToChildren();

