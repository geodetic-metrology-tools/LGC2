//	TScalar.h 
/*!
The TScalar class represents a TReal numerical value in TReal precision.

 
Patterns:

 
Copyright 1999-2007, Mark Jones, CERN TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////

#ifndef SU_SCALAR
#define SU_SCALAR

#if _MSC_VER >= 1000
#pragma once
#endif //! _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////

//!Includes Files

class T3DMatrix;
class TFreeVector;
class TDouble;

#include "TANumericValue.h"
#include "TAngle.h"
#include "TLength.h"
#include "TMatrix.h"
#include "TColumnVector.h"

//!Type Definition
typedef	TReal	ScalarValue;	// the value for Scalar
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts
	@{*/

//! Class for a Scalar
class	TScalar : public  TANumericValue //: public TObject
{

public:

			
	/*!\name Constructors and Destructors*/
	//@{
	//! Default Constructor
	TScalar();
	//! constructor taking a given Scalar value
	TScalar(TReal value);
	//! constructor taking a given Scalar value
	TScalar(TDouble& value);
	//! copy constructor
	TScalar(const TScalar&);
	//! Destructor
	virtual	~TScalar();
	//@}

	/*!\name Public Basic Methods*/
	//@{
	
	/*!set a value to a TScalar Object
	\param ScalarValue, value of TScalar in metres 
	*/
	void setValue(const ScalarValue);
	
	//!give the value of a TScalar Object
	ScalarValue	getValue() const;
	
	/*! Equality operator, comparaison between two TScalar objects
	\param TScalar&, TScalar object
	\return true if the two TScalar objects are egals, else return False
	*/
	virtual bool operator==(const TScalar&) const;

	/*!Less than operator, comparaison between two TScalar objects
	\param TScalar&, TLength object
	\return true if the value of the first object is less than the value of
	the second object, else return false
	*/
	virtual bool operator<(const TScalar&) const;

	/*!Greater than operator, comparaison between two TScalar objects
	\param TScalar&, TLength object
	\return true if the value of the first object is greater than the value of
	the second object, else return false
	*/
	virtual bool operator>(const TScalar&) const;

	//!add two TScalar objects
	TScalar operator+(const TScalar &);
	
	//!substract two TScalar objects
	TScalar operator-(const TScalar &);
	
	//!multiply a TScalar object by an other
	TScalar operator*(const TScalar &);
	
	//!divide a TScalar object by an other
	TScalar operator/(const TScalar &);

	//!assign a TScalar object to an other
	TScalar& operator=(const TScalar &);
	
	//!add two TScalar objects and rewrite this
	TScalar& operator+=(const TScalar &);
	
	//!substract two TScalar objects and rewrite this
	TScalar& operator-=(const TScalar &);
	
	//!multiply a TScalar object by an other TScalar object and rewrite this
	TScalar& operator*=(const TScalar &);
	
	//!divide a TScalar object by an other TScalar object and rewrite this
	TScalar& operator/=(const TScalar &);
	//@}

	/*!\name Public Special Methods*/
	//@{
	//!multiply a TScalar object by a TAngle object
	TAngle operator* (const TAngle);

	//!multiply a TScalar object by a Tlength object
	TLength operator* (const TLength);

	//!multiply a TScalar object by a TMatrix object
	TMatrix operator* (const TMatrix);

	//!multiply a TScalar object by a T3DMatrix object
	T3DMatrix operator* (const T3DMatrix);
	
	//!multiply a TScalar object by a TColumnVector object
	TColumnVector operator*(const TColumnVector);

	//!multiply a TScalar object by a TFreeVector object
	TFreeVector operator*(const TFreeVector);

	//!multiply a TScalar object by a factor
	TScalar operator*(const TReal );

	//@}

private:

	/*!\name Protected Attributes*/
	//@{
	
	ScalarValue			fValue; /*!< default = LITERAL(0.0) */
	//@}


	//ClassDef(TScalar, 1)
};
/*@}*/




#endif // SU_Scalar
