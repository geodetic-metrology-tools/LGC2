// TVerticalDistROM.h
//
/** Class for a round of vertical distance measurements */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_TVerticalDistROM
#define SU_TVerticalDistROM


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <list>
#include <iostream>
using namespace std;
//
//class  TLevelStation;
#include "TVerticalDistMeasurement.h"
#include  "TARoundOfMeas.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////


/*!\ingroup spatialmeasurements
	@{*/

//! Class for a round of vertical distance measurements
class  TVerticalDistROM : public TARoundOfMeas  
{
public:

	/**@name Constructors and Destructors */
	//@{
	/*! Default Constructor */
	TVerticalDistROM();
	/**Constructor 
	@param parent a reference to the level station from which this round of meas was made
	@param refPt an iterator pointing to the reference point*/
//	TVerticalDistROM(TLevelStation& parent, TWorkingPoints::PointIterator refPt);
	/*! Constructor */
	explicit TVerticalDistROM(TSpatialPointName);
	/*! Copy Constructor */
	TVerticalDistROM(const  TVerticalDistROM&);
	/*! Destructor */
	virtual  ~TVerticalDistROM();
	//@}


	/// Copy Assignment Operator 
	TVerticalDistROM& operator=( const TVerticalDistROM& );
	
	/**@name Vertical distance measurement related*/
	//@{
	/** Adds a vertical distance measurement to this ROM
	@param vd a pointer to the vert. distance to be inserted. NB : a copy of the pointed to meas. will be made*/
	virtual TVerticalDistMeasurement *			addVertDist(TVerticalDistMeasurement *ha);
	/**@return a boolean indicating if the pointed to vertical dist measurement is already stored by this ROM*/
	virtual bool								notInContainer(TVerticalDistMeasurement *) const;

	/** Gives acces to a vertical dist. measurement specified by its target point
	@param iterTg an iterator pointing to the target TSpatialPoint of the wanted measurement
	@return an iterator pointing to the verticl dist measurement made to the specified point*/
	virtual VertDistMeasIterator				getVertDistMeas(/*TWorkingPoints::PointIterator*/TSpatialPointName iterTg);
	/** Gives acces to a vertical dist. measurement specified by its target point
	@param iterTg an iterator pointing to the target TSpatialPoint of the wanted measurement
	@return a const iterator pointing to the verticl dist measurement made to the specified point*/
	virtual VertDistMeasConstIter				getVertDistMeas(/*TWorkingPoints::PointIterator*/ TSpatialPointName iterTg) const;
	/**@return an iterator to the first vert. dist. measurement of this ROM's container*/
	virtual VertDistMeasIterator				getVertDistMeasBeginIterator();
	/**@return a const iterator to the first vert. dist. measurement of this ROM's container*/
	virtual VertDistMeasConstIter				getVertDistMeasBeginIterator() const;
	/**@return an iterator to one past the last vert. dist. measurement of this ROM's container*/
	virtual VertDistMeasIterator				getVertDistMeasEndIterator();
	/**@return a const iterator to one past the last vert. dist. measurement of this ROM's container*/
	virtual VertDistMeasConstIter				getVertDistMeasEndIterator() const;


	/*!@return the identifier of the measurement*/
	virtual int					getId() const { return fIdentifier;}
	/*!sets the identifier of the measurement
	@param id an int*/
	virtual void				setId(int id) { fIdentifier = id; return;}



	/**@name Activation/Disactivation related*/
	//@{
	///Tells the children measurements that this ROM has been disactivated
//	virtual void							sendDisactivatedToChildren();
	///Tells the children measurements that this ROM has been activated
//	virtual void							sendActivatedToChildren();

	///Tells the parent level station that this ROM has been disactivated
//	virtual void							sendDisactivatedToParent();
	///Tells the parent level station that this ROM has been activated
//	virtual void							sendActivatedToParent();
	//@}

//	virtual void			sendEnoughTotalEqns();
//	virtual void			childReadyToActivate(int, int, int);
//	virtual bool			enoughTotalEquationns() const;



private:

//	TLevelStation*				fParentStation;
	VertDistContainer			fMeasuredDists;

	int							fIdentifier; /*< identifier from geode */
};

	/*!@name Typedefs*/
	//@{
	/*! Type of the container used to store the vertical dist roms */
	typedef list< TVerticalDistROM > VertDistROMContainer;
	/*! Type of the iterator pointing to an element of the vertical ROMs container */
	typedef VertDistROMContainer::iterator VertDistROMIterator;
	/*! Type of the const iterator pointing to an element of the vertical ROMs container */
	typedef VertDistROMContainer::const_iterator VertDistROMConstIter;
	//@}


#endif // SU_TVerticalDistROM

/*@}*/