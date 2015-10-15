// TColumnVector.h
//
/** Class for a vector of doubles.
Designed to be easiliy usable with matrix/vector functions of the NagC math library 
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TCOLUMN_VECTOR
#define SU_TCOLUMN_VECTOR


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////
//
#include  <list>
//using namespace std;
//
//class  **classname**;
class TDouble;
class TFreeVector;
#include  "TMatrix.h"
#include  "TANumericValue.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts
	@{*/

//! Class for a vector of doubles
class  TColumnVector : public  TANumericValue//: public TObject  
{
public:

	friend class TMatrix;


	/*!\name Constructors and Destructors */
	//@{
		//!Default Constructor 
		TColumnVector();

		//!Constructor
		/*!\param nRows the vector's number of rows*/
		TColumnVector(int nRows);


		//!Copy Constructor 
		TColumnVector(const  TColumnVector&);

		//!Destructor
		virtual  ~TColumnVector();
	//@}


	/*!\name operators */
	//@{

	//!Copy assignement operator
	virtual TColumnVector&	operator=(const TColumnVector&);
	
	//!return the sum of this vector and a second one
	virtual TColumnVector	operator+(const TColumnVector&) const;

	//!replaces this vector by its sum with a second one
	/*!\return the sum vector*/
	virtual TColumnVector&	operator+=(const TColumnVector&);

	//!@return the difference of this vector and a second one
	virtual TColumnVector	operator-(const TColumnVector&) const;

	//!replaces this vector by its difference with a second one
	/*!\return the sum vector*/
	virtual TColumnVector&	operator-=(const TColumnVector&);

	//!return the product of this vector by a scalar
	virtual TColumnVector	operator*(const TReal k);

	//!return the product of this vector by TDouble
	virtual TColumnVector	operator*(const TDouble k);

	//!return the scalar product of this vector and a second one
	virtual TDouble 	operator*(const TColumnVector&) const;

	//!* Initializes all the vector's elements to a common value
	/*!	\param comVal the common value of all the vector's elements*/
	virtual void	operator=(const TReal comVal);

	//!Vector element access operator (non-const version). Indices start at 1.
	/*!\param row the row of the desired element*/
	virtual TReal&		operator()(const int row);

	//!Vector element access operator (const version). Indices start at 1.
	/*!\param row the row of the desired element*/
	virtual TReal		operator()(const int row) const;

	
	//@}


	/*!\name member functions */
	//@{

	//!Get dimension
	virtual int dimension() const;

	//!Sets the dimension of the vector. Destroys previously stored values.
	/*!\param nRows the vectors's number of rows*/
	virtual void	setDimension(const int nRows);

	//!return a matrix (row vector) equal to the transposed of this vector
	virtual TMatrix	transposed() const;

	//@}


private:

	//!return the adress of the vector's first element
	virtual TReal *	getFirstEltAdr() const;

	TReal*			fVector;
	int				fNbRows;

};
/*@}*/

#endif // SU_TCOLUMN_VECTOR


