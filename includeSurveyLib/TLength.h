// TLength.h
//
// Class for a length 
//
// include math operators +, -, *factor, =.
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////


#ifndef SU_LENGTH
#define SU_LENGTH

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////

//!Includes Files
#include    <iostream>
#include	<float.h>
#include    <math.h>
#include	<assert.h>
//

#include  "TANumericValue.h"
class TDouble;
//using namespace std;

//!Type Definition
typedef	TReal	LengthValue;	// the value for the coordinate
//
////////////////////////////////////////////////////////////////


//!Class Definition
/// \ingroup MathematicalConcepts
class	TLength  : public  TANumericValue //: public TObject
{

public:

	/*!\name Constants*/
	//@{
	//!what type of value the coordinate holding
	enum ENumberSign {kNegative=-1, kZero, kPositive};
	//! possible length units
	enum EUnits {kKilometres, kMetres, kMillimetres}; 
	//@}
	
	/*!\name Constructors and Destructors*/
	//@{
	//! Default Constructor
	TLength();
	//! constructor taking a given length value in metres
	explicit  TLength(LengthValue);
	//TLength(LengthValue);
	//! copy constructor
	TLength(const TLength&);
	//! Destructor
	virtual	~TLength();
	//@}

	/*!\name Public Methods*/
	//@{
	
	/*!set a value in metres
	\param LengthValue, value of Tlength in metres 
	*/
	void setMetresValue(LengthValue);

	/*!set a value in kilometres
	\param LengthValue, value of Tlength in kilometres 
	*/
	void setKMetresValue(LengthValue);

	/*!set a value in millimetres
	\param LengthValue, value of Tlength in millimetres 
	*/
	void setMMetresValue(LengthValue);
	
	//!give the value in metres
	LengthValue	getMetresValue() const;

	//!give the value in kilometres
	LengthValue	getKMetresValue() const;

	//!give the value in millimetres
	LengthValue	getMMetresValue() const;
	
	/*!Equality operator, comparaison between two TLength objects
	\param Tlength&, TLength object
	\return true if the two TLength objects are egals, else return False
	*/
	virtual bool operator==(const TLength&) const;

	/*!Less than operator, comparaison between two TLength objects
	\param Tlength&, TLength object
	\return true if the value of the first object is less than the value of
	the second object, else return false
	*/
	virtual bool operator<(const TLength&) const;

	//!add two TLength objects
	TLength operator+(const TLength &);
	
	//!substract two TLength objects
	TLength operator-(const TLength &);

	/*! div. a TLength by a TLength*/
	TDouble operator/(const TLength& );
	
	//!multiply a TLength object by a factor
	TLength operator*(const TReal );
	
	//!idem *
	TLength operator*(const TDouble &);

	//!idem *
	friend  TLength operator*(const TReal, const TLength & );

	//!assign a TLength object to an other
	TLength& operator=(const TLength &);
	
	//!add two TLength objects and rewrite this
	TLength& operator+=(const TLength &);
	
	//!substract two TLength objects and rewrite this
	TLength& operator-=(const TLength &);
	
	//!multiply a TLength object by a factor and rewrite this
	TLength& operator*=(const TReal );
	
	//!idem *=
	TLength& operator*=(const TDouble&);


	//!idem *=
	//friend  TLength& operator*=(const TReal, const TLength & );
	//@}


private:

	/*!\name Protected Methods*/
	//@{
	//!return the sign of a TReal number
	ENumberSign		sign(TReal	number) const;
	//@}


private:

	/*!\name Protected Attributes*/
	//@{
	//! default = LITERAL(0.0)
	LengthValue			fValue;
	//@}


	//ClassDef(TLength, 1)
};



//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////



inline void TLength::setMetresValue(const LengthValue value)
{	// set as a length measure of the given metres value
	fValue = value;
	valueSet();
	return;
}


inline void TLength::setKMetresValue(const LengthValue value)
{	// set as a length measure of the given kilometres value
	// scale the value input
	fValue = value * 1000;
	valueSet();
	return;
}


inline void TLength::setMMetresValue(const LengthValue value)
{	// set as a length measure of the given millimetres value
	// scale the value input
	fValue = value / 1000;
	valueSet();
	return;
}


inline LengthValue TLength::getMetresValue() const
{	// get the metres length value
	return fValue;
}


inline LengthValue	TLength::getKMetresValue() const
{	// get the km length value
	// return the converted length value
	return fValue / 1000;
}


inline LengthValue	TLength::getMMetresValue() const
{	// get the mm length value
	// return the converted length value
	return fValue * 1000;
}

#endif // SU_LENGTH
