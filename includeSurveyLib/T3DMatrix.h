// T3DMatrix.h
/*!
	Class for a general 3x3 TReal matrix

	Patterns:

 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_T3DMatrix
#define SU_T3DMatrix


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//
//class  **classname**;
#include "TACoordSysMatrix.h"
#include "TFreeVector.h"
#include "TPositionVector.h"
#include "TCoordSysFactory.h"
//using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts
	@{*/

//! Class for a general 3x3 TReal matrix
class  T3DMatrix : public TACoordSysMatrix//: public TObject  
{
public:
	/*!@name Constructors and Destructors */
	//@{
		//! Constructor 
		T3DMatrix( TCoordSysFactory::ECoordSys );

		//! Copy Constructor 
		T3DMatrix(const  T3DMatrix&);

		//! Destructor
		virtual  ~T3DMatrix();
	//@}

/*!\name operator functions*/
	//@{
	//!Copy assignement operator
	virtual T3DMatrix&	operator=(const T3DMatrix&);

	/*!Initializes all the matrice's elements to a common value
	\param comVal the common value of all the matrice's elements*/
	virtual void	operator=(const TReal&);
	
	/*!\return the sum of this matrix and a second one*/
	T3DMatrix		operator+(const T3DMatrix&) const;

	/*!replaces this matrix by its sum with a second one
	\return the sum matrix*/
	T3DMatrix&	operator+=(const T3DMatrix&);

	/*!\return the difference of this matrix and a second one*/
	T3DMatrix		operator-(const T3DMatrix&) const;

	/*!replaces this matrix by its difference with a second one
	\return the sum matrix*/
	T3DMatrix&	operator-=(const T3DMatrix&);

	/*!\return the product of this matrix and a second one*/
	virtual T3DMatrix		operator*(const T3DMatrix&) const;

	/*!replaces this matrix by its product with a second one
	\return the sum matrix*/
	virtual T3DMatrix&	operator*=(const T3DMatrix&);
	
	/*!\return the product of this matrix by a position vector*/
	virtual TPositionVector		operator*(const TPositionVector& right) const;

	/*!\return the product of this matrix by a free vector*/
	virtual TFreeVector		operator*(const TFreeVector& right) const;

	/*!\return the product of this matrix by a scalar*/
	virtual T3DMatrix	operator*(const TReal&);

	/*!\return the product of this matrix by TDouble*/
	virtual T3DMatrix	operator*(const TDouble&);
	//@}

	/*!\name member functions*/
	//@{
	/*!\return a matrix equal to the transposed of this one*/
	virtual T3DMatrix			transposed() const;

	//!clear a matrix
	void	clear();
	
	/*!\return the inverse of this matrix*/
	virtual T3DMatrix			inverse();
	
	/*!Initializes all the matrice's diagonal elements to a common value.
	The non-diagonal elements are set to zero.
	\param comVal the common value of all the matrice's diagonal elements*/
	virtual bool	initDiag( const TReal&);
	
	/*!inverse and rewrite this
	\return true if possible*/
	bool	invert();
	
	//!set a element into a 3D matrix
	bool setElt(const int, const int, const TReal);

	//!get a element of a 3D matrix
	TReal getElt(const int, const int) const;

	
	//@}

private:
		//! Default Constructor 
		T3DMatrix();	

	//ClassDef(T3DMatrix, 1)
};
/*@}*/

#endif // SU_T3DMatrix
