// T2DOffsetROM.h

/*!
	Class for a round of bidimensional offset measurements

	Copyright 2000 CERN EST/SU. All rights reserved.

*/

//////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_2D_OFF_ROM
#define SU_2D_OFF_ROM


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000


// Forward declarations
//
#include  <list>
#include  <string>
#include  <iostream>
using namespace std;

class  TDistMeasStation;

#include  "TAUnreferencedROM.h"
#include  "T2DOffsetMeasurement.h"

///////////////////////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a round of bidimensional offset measurements
class  T2DOffsetROM : public TAUnreferencedROM  
{
public:

	/*!@name Constructors and Destructors */
	//@{
	/*! Default constructor */
	T2DOffsetROM();
	/*! Copy Constructor */
	T2DOffsetROM(const  T2DOffsetROM& source);
	/*! Destructor */
	virtual  ~T2DOffsetROM();
	//@}

	
	/*! Copy Assignment Operator */
	T2DOffsetROM& operator=( const T2DOffsetROM& source);


	/*!@name 2D Offset measurement related*/
	//@{
	/* Adds a 2D Offset to this ROM
	\param of a pointer to the 2D offset to be inserted. NB : a copy of the meas. pointed to will be made*/
	virtual T2DOffsetMeasurement*  add2DOffset(T2DOffsetMeasurement* of);
	/*! returns a boolean indicating if the pointed to 2D offset is already stored by this ROM*/
	virtual bool  notInContainer(T2DOffsetMeasurement *) const;
	/* Gives access to a 2D Offset measurement specified by its target point
	\param iterSpl an iterator pointing to the target TSpatialLine of wanted measurement
	\returns an iterator pointing to the 2D offset measurement made to the specified line*/
	virtual Off2DMeasIterator  get2DOffsetMeas(/*TWorkingTargetObjects::SpLineIterator*/ string iterSpl);
	/* Gives access to a 2D Offset measurement specified by its target point
	\param iterSpl an iterator pointing to the target TSpatialLine of wanted measurement
	\returns a const iterator pointing to the 2D offset measurement made to the specified line*/
	virtual Off2DMeasConstIter  get2DOffsetMeas(/*TWorkingTargetObjects::SpLineIterator*/ string iterSpl) const;
	/*! returns an iterator to the first 2D offset measurement of this ROM's container*/
	virtual Off2DMeasIterator  get2DOffsetMeasBeginIterator();
	/*! returns a const iterator to the first 2D offset measurement of this ROM's container*/
	virtual Off2DMeasConstIter  get2DOffsetMeasBeginIterator() const;
	/*! returns an iterator to one past the last 2D offset measurement of this ROM's container*/
	virtual Off2DMeasIterator  get2DOffsetMeasEndIterator();
	/*! returns a const iterator to one past the last 2D offset measurement of this ROM's container*/
	virtual Off2DMeasConstIter  get2DOffsetMeasEndIterator() const;
	/*! returns the number of offsets of this round*/
//	virtual int					getOffsetCount() const;
	//@}

protected:


private:
	
	Off2DContainer					fMeasuredOffsets; /*!< list of 2D offset measurements */
	
};



/*@}*/

	/*!@name Typedefs*/
	//@{
	/*!Type of the container storing the 2D offset ROMs */
	typedef list< T2DOffsetROM > Off2DROMContainer;
	/*!Type of the iterators pointing to 2D offset ROMs */
	typedef Off2DROMContainer::iterator Off2DROMIterator;
	/*!Type of the constant iterators pointing to 2D offset ROMs */
	typedef Off2DROMContainer::const_iterator Off2DROMConstIter;
	//@}


#endif







////////////////////////////////////////////////////////////////////
//end
////////////////////////////////////////////////////////////////////




//#include  "UEOIndices.h"


	//param parent a reference to the TDistMeasStation from which this ROM has been made*/
//	explicit T2DOffsetROM(TDistMeasStation& parent);
	
/*
	//Tells the children measurements that this ROM has been disactivated
	virtual void							sendDisactivatedToChildren();
	//Tells the children measurements that this ROM has been activated
	virtual void							sendActivatedToChildren();

	//Tells the parent dist. meas. station that this ROM has been disactivated
	virtual void							sendDisactivatedToParent();
	//Tells the parent dist. meas. station that this ROM has been activated
	virtual void							sendActivatedToParent();
	virtual void	parentEnoughTotalEqns();
	virtual void	sendEnoughTotalEqns();
	virtual void	activateReadyChildren();
*/

//	TDistMeasStation*				fParentStation;
