// 
// THorizontalDistROM.h : Header File
/*!
	Class for a round of horizontal distance measurements

	Patterns:
  
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_HORDIST_ROM
#define SU_HORDIST_ROM

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
#include "THorizontalDistMeas.h"

//////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a round of horizontal distance measurements
class THorizontalDistROM : public TAUnreferencedROM{ 

public:

	/*!@name Constructors/Destructor */
	//@{
	THorizontalDistROM();
	/*! Copy constructor */
	THorizontalDistROM(const THorizontalDistROM& source);
	/*! Destructor */
	virtual ~THorizontalDistROM();
	//@}

	/*!Copy assignment operator */
	THorizontalDistROM&		operator=(const THorizontalDistROM& source);
	//DANGER : shallow copy (see NetworkRoundOfMeas)

	/*!@name Horizontal distance measurement related*/
	//@{
	/*! Adds a horizontal dist meas to this ROM
	\param hd  a pointer to the hor. dist. to be inserted NB: a copy of the meas. pointed at will be made */
	virtual THorizontalDistMeas*	addHorDist(THorizontalDistMeas* hd);
	/*! return a boolean indicating if the pointer to the dist meas. is already stored by this ROM*/
	virtual bool					notInContainer(THorizontalDistMeas *) const;
	/* Gives access to the hor. dist. meas. made to the specified point 
	\param iterTg an iterator pointing at the target TSpatialPoint of the wanted meas.
	\return an iterator pointing to the hor. dist. meas. made to the specified point */
	virtual HorDistMeasIterator		getDistMeas(/*TWorkingPoints::PointIterator*/TSpatialPointName iterTg);
	/* Gives access to the hor. dist. meas. made to the specified point 
	\param iterTg an iterator pointing at the target TSpatialPoint of the wanted meas.
	\return an const iterator pointing to the hor. dist. meas. made to the specified point */
	virtual HorDistMeasConstIter	getDistMeas(/*TWorkingPoints::PointIterator*/TSpatialPointName iterTg) const;
	/*! return the number of hor. dist. in the ROM  */
//	virtual int						getDistCount() const;
	/*! return an iterator to the first hor. dist. meas. of this ROM's Container */
	virtual HorDistMeasIterator		getDistMeasBeginIterator();
	/*! return a const iterator to the first hor. dist. meas. of this ROM's Container */
	virtual HorDistMeasConstIter	getDistMeasBeginIterator() const;
	/*! return an iterator to the last hor. dist. meas. of this ROM's Container */
	virtual HorDistMeasIterator		getDistMeasEndIterator();
	/*! return an const iterator to the last hor. dist. meas. of this ROM's Container */
	virtual HorDistMeasConstIter	getDistMeasEndIterator() const;
	//@}
	
private:

	HorDistContainer					fMeasuredDists; /*!< list of horizontal distance measurements */
	
};


/*@}*/


	/*!@name Typedefs*/
	//@{
	/*!Type of the container storing the horizontal distance ROMs */
	typedef list< THorizontalDistROM > HorDistROMContainer;
	/*!Type of the iterators pointing to horizontal distance ROMs */
	typedef HorDistROMContainer::iterator HorDistROMIterator;
	/*!Type of the constant iterators pointing to horizontal distance ROMs */
	typedef HorDistROMContainer::const_iterator HorDistROMConstIter;
	//@}


#endif













/////////////////////////////////////////////////////////////
//end
/////////////////////////////////////////////////////////////

//#include "UEOIndices.h"
//#include "TUnknownParameter.h"



/* Constructor */
/*	@param parent a reference to the TDistMeasStation from which the ROM has been made*/
//	explicit THorizontalDistROM(TDistMeasStation& parent);
	
/* inherited from TVParentMeasTreeNode */
//	virtual void							activateReadyChildren();


//	virtual void					sendEnoughTotalEqns();
	
	/*inherited from TVChildMeasTreeNode*/
//	virtual void					parentEnoughTotalEqns();

	/* tells the children meas. that this ROM has been disactivated */
//	virtual void					sendDisactivatedToChildren();
	/* tells the children meas. that this ROM has been activated */
//	virtual void					sendActivatedToChildren();

	/* tells the parent DM station that this ROM has been disactivated */
//	virtual void					sendDisactivatedToParent();
	/* tells the parent DM station that this ROM has been activated */
//	virtual void					sendActivatedToParent();

//	TDistMeasStation*					fParentStation;
