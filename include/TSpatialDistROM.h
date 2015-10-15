// 
// TSpatialDistROM.h : Header File
//
/*!
	Class for a round of spatial distance measurements
	Patterns:
  
  
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_SPATIALDIST_ROM
#define SU_SPATIALDIST_ROM

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
class TDistMeasStation;

#include <list>
#include <string>
#include <iostream>
using namespace std;

#include "TAUnreferencedROM.h"
#include "TSpatialDistMeasurement.h"

//////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a round of spatial distance measurements
class TSpatialDistROM : public TAUnreferencedROM{ 

public:

	/*!@name Constructors/Destructor */
	//@{
	TSpatialDistROM();
	/*! Copy constructor */
	TSpatialDistROM(const TSpatialDistROM& source);
	/*! Destructor */
	virtual ~TSpatialDistROM();
	//@}

	//!Copy assignment operator */
	TSpatialDistROM&		operator=(const TSpatialDistROM& source);
	//DANGER : shallow copy (see NetworkRoundOfMeas)

	/*!@name Spatial distance measurement related*/
	//@{

	/*! Adds a spatial dist meas to this ROM
	\param hd  a pointer to the spatial dist. to be inserted NB: a copy of the meas. pointed at will be made */
	virtual TSpatialDistMeasurement*	addSpatialDist(TSpatialDistMeasurement* sd);

	/*! return a boolean indicating if the pointer to the dist meas. is already stored by this ROM*/
	virtual bool					notInContainer(TSpatialDistMeasurement *) const;

	/*! return the number of spatial dist. in the ROM  */
//	virtual int						getDistCount() const;

	/* Gives access to the spatial dist. meas. made to the specified point 
	\param iterTg an iterator pointing at the target TSpatialPoint of the wanted meas.
	\return a const iterator pointing to the spatial dist. meas. made to the specified point */ 
	virtual SpatialDistMeasConstIter	getDistMeas(/*TWorkingPoints::PointIterator iterTg*/ TSpatialPointName iterTg) const;

	/* Gives access to the spatial dist. meas. made to the specified point 
	\param iterTg an iterator pointing at the target TSpatialPoint of the wanted meas.
	\return an iterator pointing to the spatial dist. meas. made to the specified point */
	virtual SpatialDistMeasIterator		getDistMeas(/*TWorkingPoints::PointIterator*/ TSpatialPointName iterTg);

	/*! return a const iterator to the first spatial dist. meas. of this ROM's Container */
	virtual SpatialDistMeasIterator		getDistMeasBeginIterator();
	
	/*! return an iterator to the first spatial dist. meas. of this ROM's Container */
	virtual SpatialDistMeasConstIter	getDistMeasBeginIterator() const;
	
	/*! return an iterator to the last spatial dist. meas. of this ROM's Container */
	virtual SpatialDistMeasIterator		getDistMeasEndIterator();

	/*! return a const iterator to the last spatial dist. meas. of this ROM's Container */
	virtual SpatialDistMeasConstIter	getDistMeasEndIterator() const;

//	virtual void					sendEnoughTotalEqns();
	//@}

private:

	SpatialDistContainer				fMeasuredDists; /*!< list of spatial distance measurements */
	
};

/*@}*/


	/*!@name Typedefs*/
	//@{
	/*!Type of the container storing the spatial distance ROMs */
	typedef list< TSpatialDistROM > SpatialDistROMContainer;
	/*!Type of the iterators pointing to spatial distance ROMs */
	typedef SpatialDistROMContainer::iterator SpatialDistROMIterator;
	/*!Type of the constant iterators pointing to spatial distance ROMs */
	typedef SpatialDistROMContainer::const_iterator SpatialDistROMConstIter;
	//@}

#endif







///////////////////////////////////////////////////////////////////////
//end
///////////////////////////////////////////////////////////////////////
//#include "UEOIndices.h"
//#include "TUnknownParameter.h"

	/*! Constructor (to be updated) */
/*	\param parent a reference to the TDistMeasStation from which the ROM has been made*/
//	explicit TSpatialDistROM(TDistMeasStation& parent);
	
	// inherited from TVParentMeasTreeNode
//	virtual void							activateReadyChildren();


	//inherited from TVChildMeasTreeNode
//	virtual void					parentEnoughTotalEqns();
	
	/* tells the children meas. that this ROM has been disactivated */
//	virtual void					sendDisactivatedToChildren();
	/* tells the children meas. that this ROM has been activated */
//	virtual void					sendActivatedToChildren();

	/* tells the parent DM station that this ROM has been disactivated */
//	virtual void					sendDisactivatedToParent();
	/*! tells the parent DM station that this ROM has been activated */
//	virtual void					sendActivatedToParent();

//	TDistMeasStation*					fParentStation;
