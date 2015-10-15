//	TDouble.h 
/*!
The TDouble class represents a TReal numerical value in TReal precision.

 
Patterns:

 
Copyright 1999-2007, Mark Jones, CERN TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////

#ifndef SU_DOUBLE
#define SU_DOUBLE

#if _MSC_VER >= 1000
#pragma once
#endif //! _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////

//!Includes Files

class T3DMatrix;
class TFreeVector;

#include "TANumericValue.h"
#include "TAngle.h"
#include "TLength.h"
#include "TMatrix.h"
#include "TColumnVector.h"

//!Type Definition
typedef	TReal	DoubleValue;	// the value for TReal
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts
	@{*/

//! Class for a TReal
class	TDouble : public  TANumericValue //: public TObject
{

public:

			
	/*!\name Constructors and Destructors*/
	//@{
	//! Default Constructor
	TDouble();
	//! constructor taking a given TReal value
	TDouble(TReal DoubleValue);
	//! copy constructor
	TDouble(const TDouble&);
	//! Destructor
	virtual	~TDouble();
	//@}

	/*!\name Public Basic Methods*/
	//@{
	
	/*!set a value to a TDouble Object
	\param DoubleValue, value of TDouble in metres 
	*/
	void setValue(DoubleValue);
	
	//!give the value of a TDouble Object
	DoubleValue	getValue() const;
	
	/*! Equality operator, comparaison between two TDouble objects
	\param TDouble&, TDouble object
	\return true if the two TDouble objects are egals, else return False
	*/
	virtual bool operator==(const TDouble&) const;

	/*!Less than operator, comparaison between two TDouble objects
	\param TDouble&, TLength object
	\return true if the value of the first object is less than the value of
	the second object, else return false
	*/
	virtual bool operator<(const TDouble&) const;

	//!add two TDouble objects
	TDouble operator+(const TDouble &);
	
	//!substract two TDouble objects
	TDouble operator-(const TDouble &);
	
	//!multiply a TDouble object by an other
	TDouble operator*(const TDouble &);
	
	//!divide a Tdouble object by an other
	TDouble operator/(const TDouble &);

	//!assign a TDouble object to an other
	TDouble& operator=(const TDouble &);
	
	//!add two TDouble objects and rewrite this
	TDouble& operator+=(const TDouble &);
	
	//!substract two TDouble objects and rewrite this
	TDouble& operator-=(const TDouble &);
	
	//!multiply a TDouble object by an other TDouble object and rewrite this
	TDouble& operator*=(const TDouble &);
	
	//!divide a TDouble object by an other TDouble object and rewrite this
	TDouble& operator/=(const TDouble &);
	//@}

	/*!\name Public Special Methods*/
	//@{
	//!multiply a TDouble object by a TAngle object
	TAngle operator* (const TAngle);

	//!multiply a TDouble object by a Tlength object
	TLength operator* (const TLength);

	//!multiply a TDouble object by a TMatrix object
	TMatrix operator* (const TMatrix);

	//!multiply a TDouble object by a T3DMatrix object
	T3DMatrix operator* (const T3DMatrix);
	
	//!multiply a TDouble object by a TColumnVector object
	TColumnVector operator*(const TColumnVector);

	//!multiply a TDouble object by a TFreeVector object
	TFreeVector operator*(const TFreeVector);
	//@}

private:

	/*!\name Protected Attributes*/
	//@{
	
	DoubleValue			fValue; /*!< default = LITERAL(0.0) */
	//@}


	//ClassDef(TDouble, 1)
};
/*@}*/




#endif // SU_DOUBLE
