// TAngleConstants.h
/*!
	Class for constant angles to be added on angle measurements 
	like collimation errors.

	Patterns:

	Copyright 2002 CERN EST/SU. All rights reserved.
*/

#ifndef SU_ANGCONSTANTS
#define SU_ANGCONSTANTS

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif 


#include <string>
//using namespace std;



#include "TAngle.h"
#include "TAPointMeasurement.h"

/*! \ingroup spatialmeasurements
	@{*/

//!Class for constant distances
class TAngleConstants
{

public:

	/*!@name Constructors / Destructor */
	//@{
	/*! Default constructor */
	TAngleConstants();
	/*! Constructor
	\param in number of the instrument used for the constant's determination
	\param pn number of the prism used for the constant's determination 
	\param c constant's value 
	\param s constant's status for the following calculations */
	TAngleConstants(TAngle c, TAMeasurement::ECalcStatus s);
	/*! Copy constructor */
	TAngleConstants(const TAngleConstants& );
	/*! Destructor */
	~TAngleConstants();
	//@}

	/*!@name Overloaded operators */
	//@{
	/*!Copy assignement operator */
	virtual  TAngleConstants&  operator =( const TAngleConstants& );	
	/*! Equality operator */
	virtual  bool  operator ==( const TAngleConstants & ) const; //compare equal 
	/*! Less than operator */
	virtual  bool  operator <( const TAngleConstants & ) const; //compare less than 
	//@}

	/*!@name Access methods */
	//@{
	/*! Returns string used to identified the constant */
	virtual string						getConstantName() const;
	/*! Returns the constant's value */
	virtual TAngle						getValue() const;
	/*! Returns the constant's status */
	virtual TAMeasurement::ECalcStatus		getStatus() const;
	//@}

	/*!@name Settings methods*/
	//@{
	/*! Sets the constant's value 
	\param c the constant's value is an instance of TAngle */
	virtual void						setValue(TAngle c);
	/*! Sets the constant's status
	\param s the constant's status is an enum of TANumericValue from the module SpatialObjects */
	virtual void						setStatus(TAMeasurement::ECalcStatus s);
	/*! Sets the constant's name
	\param n the constant's name is a string */
	virtual void						setConstantName(string n);//@}

private:

	TAngle							fValue; /*!< constant's value as an instance of TAngle */
	TAMeasurement::ECalcStatus			fStatus; /*!< constant's status as an enum of TANumericValue (SpatialObjects) */
	string							fConstId; /*!< name of the constant */

};


/*@}*/



/*!@name Typedefs*/
//@{
//! Type of the container used to store points
typedef list<TAngleConstants> AngleConstContainer;
//! Type of an iterator pointing to an element of the container
typedef AngleConstContainer::iterator AngleConstantIterator;
//! Type of a const iterator pointing to an element of the container
typedef AngleConstContainer::const_iterator AngleConstantConstIter;
//@}
#endif
