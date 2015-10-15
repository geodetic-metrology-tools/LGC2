// TANumericValue.h
//
/*! 
The TANumericValue class implements the interface for a
numerical value.
 
It is a abstract base class (ABC) that should be inherited 
by all numerical values. 

The methods declared in the TVNumericValue for accessing, 
changing, and assessing the status of a numerical value 
are defined.

Patterns:
 
 
Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_A_NUMERIC_VALUE
#define SU_A_NUMERIC_VALUE


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <iostream>
//using namespace std;
//
#include  "TVNumericValue.h"
//
////////////////////////////////////////////////////////////////



/*! \ingroup MathematicalConcepts
	@{*/

//!  The TANumericValue class implements the interface for a numerical value.
class  TANumericValue : virtual public TVNumericValue//: public TObject  
{
public:
	
	TANumericValue();
	void swap(TANumericValue & other) throw();

	virtual ~TANumericValue() { }


	/*!@name Member methods */
	//@{
		/*! Returns a boolean indicating if the numeric value is NULL */ 
		virtual  bool  isNull() const { return ( kNull == fStatus ); }

		/*! Returns the computational status of the numeric value */
		virtual  TVNumericValue::EStatus  getStatus() const  { return fStatus; }
		
		/*! Sets the computational status of the numeric value */
		virtual  void  setStatus( EStatus status )  { fStatus = status; return; }
	//@}


protected:

	/*!@name Protected Member Functions */
	//@{
		/*! Determines the status of a numerical value that should
            be assigned when an operation is performed on this 
	        numeric value and the given value. */
		virtual TVNumericValue::EStatus testStatus( const TVNumericValue& ) const;
		
		/*! Sets the status of a numeric value when a value has been set  */
		virtual  void  valueSet()  { fStatus = kKnown; return; }
	//@}

private:

	TVNumericValue::EStatus  fStatus;  /*!< Computational status of the numerical value */
	
	//ClassDef(TANumericValue, 1)
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_A_NUMERIC_VALUE
