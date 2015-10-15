//
// THorAngleROM.h : header file
/*! Class for a horizontal angle round of measurements

    Patterns:
  
    Copyright 2002 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////


#ifndef SU_HORANGLE_ROM
#define SU_HORANGLE_ROM

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
class THorAngleROM : public TAUnreferencedROM{ 

public:

	/*!@name Constructors / Destructor*/
	//@{
	/*!Constructor
	@param parent a reference to the TTheodoliteStation from which this ROM has been made*/
	explicit THorAngleROM();
	//!Copy constructor
	THorAngleROM(const THorAngleROM& source);
	//!Destructor
	virtual ~THorAngleROM();
	//@}
	
	/*!Copy assignement operator (shallow copy)*/
	THorAngleROM&		operator=(const THorAngleROM& source);
	
	/*!@name V0 related methods */
	//@{
	/*!@return a TAngle object representing the ROM's V0*/
	virtual TAngle							getV0() const;
	/*!@return the status of the ROM's V0 (fixed or variable)*/
	virtual TAMeasurement::ECalcStatus		getV0Status() const;
	/*! Sets the variable/unknown/fixed status of the rom's V0
	@param v0Status one of the status defined in the EUnknownStatus of the TUnknownAngle class*/
	void									setV0Status(TAMeasurement::ECalcStatus);
	/*!Sets the provisional value of the V0 */
	void									setV0ProvValue(TAngle provValue);
	//@}

	void									setRefAngle(THorAngleMeasurement *);

	/*!@name Horizontal angle measurement related*/
	//@{
	/*! Adds a hor. angle measurement to this ROM
	@param ha a pointer to the hor. angle to be inserted. NB : a copy of the meas. pointed to will be made*/
	virtual THorAngleMeasurement *			addHorAngle(THorAngleMeasurement *ha);
	/*!@return a boolean indicating if the pointed to angle measurement is already stored by this ROM*/
	virtual bool							notInContainer(THorAngleMeasurement *) const;

	/*! Gives acces to a hor. angel measurement specified by its target point
	@param iterTg an iterator pointing to the target TSpatialPoint of the wanted measurement
	@return an iterator pointing to the hor. angle measurement made to the specified point*/
	virtual HorAngMeasIterator				getHorAngMeas(TSpatialPointName iterTg);
	/*! Gives acces to a hor. angel measurement specified by its target point
	@param iterTg an iterator pointing to the target TSpatialPoint of the wanted measurement
	@return a const iterator pointing to the hor. angle measurement made to the specified point*/
	virtual HorAngMeasConstIter				getHorAngMeas(TSpatialPointName iterTg) const;
	/*!@return an iterator to the first hor. angle measurement of this ROM's container*/
	virtual HorAngMeasIterator				getHorAngMeasBeginIterator();
	/*!@return a const iterator to the first hor. angle measurement of this ROM's container*/
	virtual HorAngMeasConstIter				getHorAngMeasBeginIterator() const;
	/*!@return an iterator to one past the last hor. angle measurement of this ROM's container*/
	virtual HorAngMeasIterator				getHorAngMeasEndIterator();
	/*!@return a const iterator to one past the last hor. angle measurement of this ROM's container*/
	virtual HorAngMeasConstIter				getHorAngMeasEndIterator() const;
	virtual int								size() const;
	//@}

	/*!@return an iterator to the reference hor. angle measurement of the ROM*/
	virtual HorAngMeasIterator				getRefMeas();
	/*!@return a const iterator to the reference hor. angle measurement of the ROM*/
	virtual HorAngMeasConstIter				getRefMeas() const;
	/*!@return the value of the ROM's reference measurement*/
	virtual TAngle							getRefMeasValue() const;
	/*!@returns a non-const pointer to this object*/
	virtual THorAngleROM*					getPointer() const;


	

protected:


private:

	HorAngMeasIterator				fRefMeasAngle;	/*!< iterator pointing to the first element of the hor. angles container */
	HorAngleContainer				fMeasuredAngles; /*!< list of hor. angle measurements */
	TAngle							fV0;	/*!< V0 estimated value */
	TAMeasurement::ECalcStatus		fV0Status; /*!< V0 status */
};

/*@}*/


	/*!@name Typedefs*/
	//@{
	/*! Defines the container storing the horizontal angles ROMs as a type */
	typedef list<THorAngleROM > HorAngROMContainer;
	/*! Defines an iterator to a THorAngROM as a type */
	typedef HorAngROMContainer::iterator HorAngROMIterator;
	/*! Defines a const iterator to a THorAngROM as a type */
	typedef HorAngROMContainer::const_iterator HorAngROMConstIter;
	//@}

#endif









//////////////////////////////////////////////////////////////////////
//end
//////////////////////////////////////////////////////////////////////
	
//#include "UEOIndices.h"


//inherited from TVParentMeasTreeNode
//virtual void							activateReadyChildren();



	//virtual THorAngleROM*					getPointer() const;





	//virtual void							sendEnoughTotalEqns();
	/*!@name Activation/Disactivation related*/
	//@{
	//!Tells the children measurements that this ROM has been disactivated
	//virtual void							sendDisactivatedToChildren();
	//!Tells the children measurements that this ROM has been activated
	//virtual void							sendActivatedToChildren();

	//!Tells the parent theodolite station that this ROM has been disactivated
	//virtual void							sendDisactivatedToParent();
	//!Tells the parent theodolite station that this ROM has been activated
	//virtual void							sendActivatedToParent();
	//@}

	//virtual void							parentEnoughTotalEqns();



//	TTheodoliteStation*				fParentStation;
//	TSpatialPointName				fReferencePoint;
