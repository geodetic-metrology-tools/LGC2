// TDistConstants.h
/*!
	Class for constant distances to be added on distance measurements 
	like prisms constants.

	Patterns:

	Copyright 2000 CERN EST/SU. All rights reserved.
*/

#ifndef SU_DISTCONSTANTS
#define SU_DISTCONSTANTS

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif 


#include <string>
using namespace std;



#include "TLength.h"
#include "TAPointMeasurement.h"

/*! \ingroup spatialmeasurements
	@{*/

//!Class for constant distances
class TDistConstants
{

public:

	/*!@name Constructors / Destructor */
	//@{
	/*! Default constructor */
	TDistConstants();
	/*! Constructor
	\param in number of the instrument used for the constant's determination
	\param pn number of the prism used for the constant's determination 
	\param c constant's value 
	\param s constant's status for the following calculations */
	TDistConstants(TLength c, TAMeasurement::ECalcStatus s);
	/*! Copy constructor */
	TDistConstants(const TDistConstants& );
	/*! Destructor */
	~TDistConstants();
	//@}

	/*!@name Overloaded operators */
	//@{
	/*!Copy assignement operator */
	virtual  TDistConstants&  operator =( const TDistConstants& );	
	/*! Equality operator */
	virtual  bool  operator ==( const TDistConstants & ) const; //compare equal 
	/*! Less than operator */
	virtual  bool  operator <( const TDistConstants & ) const; //compare less than 
	//@}

	/*!@name Access methods */
	//@{
	/*! Returns string used to identified the constant */
	virtual string						getConstantName() const;
	/*! Returns the constant's value */
	virtual TLength						getValue() const;
	/*! Returns the constant's status */
	virtual TAMeasurement::ECalcStatus		getStatus() const;
	//@}

	/*!@name Settings methods*/
	//@{
	/*! Sets the constant's value 
	\param c the constant's value is an instance of TLength */
	virtual void						setValue(TLength c);
	/*! Sets the constant's status
	\param s the constant's status is an enum of TANumericValue from the module SpatialObjects */
	virtual void						setStatus(TAMeasurement::ECalcStatus s);
	/*! Sets the constant's name
	\param n the constant's name is a string */
	virtual void						setConstantName(string n);
	//@}

private:

	TLength							fValue; /*!< constant's value as an instance of TLength */
	TAMeasurement::ECalcStatus			fStatus; /*!< constant's status as an enum of TANumericValue (SpatialObjects) */
	string							fConstId; /*!< name of the constant */

};


/*@}*/



/*!@name Typedefs*/
//@{
//! Type of the container used to store points
typedef list<TDistConstants> DistConstContainer;
//! Type of an iterator pointing to an element of the container
typedef DistConstContainer::iterator DistConstantIterator;
//! Type of a const iterator pointing to an element of the container
typedef DistConstContainer::const_iterator DistConstantConstIter;
//@}
#endif
