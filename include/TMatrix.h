// TMatrix.h
//
/*! Class for a matrix of doubles.
Designed to be easiliy usable with matrix functions of the NagC math library 
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TMatrix
#define SU_TMatrix


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <iostream.h>
//using namespace std;
//
class  TColumnVector;
class  TDouble;
#include	<list>

//using namespace std;
//
#include "TANumericValue.h"

class TMatrixImpl;

// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/



//! Class for a matrix of doubles
class  TMatrix : public  TANumericValue //: public TObject  
{
public:
	//constants

	/*!\name Constructors and Destructors */
	//@{
		//!Default Constructor 
		TMatrix();

		/*!Constructor
		\param TDouble the matrice (1,1) value*/
		TMatrix(const TDouble&);

		/*!Constructor
		\param nRows the matrice's number of rows
		\param nCols the matrice's number of columns*/
		TMatrix(int nRows, int nCols);


		//!Copy Constructor 
		TMatrix(const  TMatrix&);
        void swap(TMatrix & other) throw();

		//!Destructor
		virtual  ~TMatrix();
	//@}

	/*!\name operator functions*/
	//@{
		//!Copy assignement operator
		virtual TMatrix&	operator=(TMatrix);

		/*!Initializes all the matrice's elements to a common value
		\param comVal the common value of all the matrice's elements*/
		virtual void	operator=( const double&);
		
        /*!replaces this matrix by its sum with a second one
		\return the sum matrix*/
		virtual TMatrix&	operator+=(const TMatrix&);

		/*!\return the sum of this matrix and a second one*/
		virtual TMatrix		operator+(const TMatrix&) const;

		/*!replaces this matrix by its difference with a second one*/
		virtual TMatrix&	operator-=(const TMatrix&);

        /*!\return the difference of this matrix and a second one*/
		virtual TMatrix		operator-(const TMatrix&) const;

        /*!replaces this matrix by its product with a second one*/
		virtual TMatrix&	operator*=(const TMatrix&);

		/*!\return the product of this matrix and a second one*/
		virtual TMatrix		operator*(const TMatrix&) const;

		/*!\return the product of this matrix by a column vector*/
		virtual TColumnVector		operator*(const TColumnVector& right) const;


        virtual TMatrix	& operator*=(double);

		/*!\return the product of this matrix by a scalar*/
		virtual TMatrix	operator*(double);

        virtual TMatrix	& operator*=(const TDouble&);

		/*!\return the product of this matrix by TDouble*/
		virtual TMatrix	operator*(const TDouble&);
		
		/*!Matrix element access operator (non-const version). Indices start at 1.
		\param row the row of the desired element
		\param col the column of the desired element*/
		double & operator()(int row, int col);
		
		/*!Matrix element access operator (const version). Indices start at 1.
		\param row the row of the desired element
		\param col the column of the desired element*/
		double operator()(int row, int col) const;
	//@}

	/*!\name member functions*/
	//@{
		/*!\return a matrix equal to the transposed of this one*/
		virtual TMatrix			transposed() const;

		/*!\param B the right hand side vector of the system to solve
			\return the X solution vector of the system AX = B*/
		virtual TColumnVector		eqnSolve(const TColumnVector& B);
		
		/*!\param B the right hand side vector of the system to solve using LDLT
			\return the X solution vector of the system AX = B*/
		virtual TColumnVector		ldltSolve(const TColumnVector& B);

		/*!Initializes all the matrice's diagonal elements to a common value.
		The non-diagonal elements are set to zero.
		\param value the common value of all the matrice's diagonal elements*/
		virtual bool	initDiag(double);
		
		/*!Sets the dimensions of the matrix. Destroys previously stored values.
		\param nRows the matrice's number of rows
		\param nCols the matrice's number of columns*/
		virtual void		setDimensions(int nRows, int nCols);

		/*!\return the matrice's number of rows*/
		virtual int			numRows() const;

		/*!\return the matrice's number of columns*/
		virtual int			numCols() const;
		
		/*!Inverts this matrix (the matrix is overwritten by its inverse)*/
		virtual  bool		invert();

		/*!\return the inverse of this matrix*/
		virtual TMatrix		inverse();
			
		/*!clear a matrice
		\return a matrice (i,j)=0, status kNull*/
		void clear();
			
		/*! Returns the error message */
		std::string	getError() const { return fError; }
	//@}

private:
	TMatrixImpl * fImpl;
	std::string			fError; /*!< errors generated from nagc functions */
};

#endif // SU_TMatrix


