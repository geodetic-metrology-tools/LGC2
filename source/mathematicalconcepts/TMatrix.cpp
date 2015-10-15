// TMatrix.cpp
//
/** Class for a matrix of doubles.
Designed to be easiliy usable with matrix functions of the NagC math library */
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


#include "TDouble.h"
#include "TMatrix.h"
#include "TColumnVector.h"
#include "TSparseMatrix.h"

#include <TMatrixImpl.h>

#include <iostream>
using namespace std;
////////////////////////////////////////////////////////////////


//ClassImp(TMatrix)


//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

TMatrix::TMatrix()
: fImpl(new TMatrixImpl)
{//default constructor
	setStatus( TANumericValue::kNull );
}

// TODO: Check who is using that
TMatrix::TMatrix(const TDouble & value)
: fImpl(new TMatrixImpl(1,1,value.getValue()))
{//Constructor return the matrice (1,1) with (1,1)=value ..... 
	setStatus( TANumericValue::kKnown );
}


TMatrix::TMatrix(int nRows, int nCols)
: fImpl(new TMatrixImpl(nRows, nCols, 0.0))
{//Constructor setting the dimensions of the matrix
    setStatus( TANumericValue::kKnown );	
	//{
	//	fNbRows = 1;
	//	fNbCols = 1;
	//	fMatrix = new double [fNbRows * fNbCols];
	//	(*this) = LITERAL(0.0);
	//	fError = "Invalid dimension" + '\n';
	//	setStatus( TANumericValue::kNull );
	//}
}	
		

TMatrix::TMatrix(const TMatrix & source)
: TANumericValue(source)
,  fImpl(new TMatrixImpl(*(source.fImpl)))
,  fError(source.fError)
{//!Copy constructor
	//copy the status
	//setStatus( source.getStatus() );
}

void TMatrix::swap(TMatrix & other) throw()
{
	TANumericValue::swap(other);
    using std::swap;
    swap(fImpl, other.fImpl);
    swap(fError, other.fError);
}

TMatrix::~TMatrix()
{//!Destructor
    delete fImpl;
}

//////////////////////////////////////////////////////////////////////
///operator Functions
//////////////////////////////////////////////////////////////////////

// the copy assignment operator takes its argument by value, 
// eliminating the need to explicitly create a copy of the other object.
TMatrix & TMatrix::operator=(TMatrix other)
{//Copy assignement operator	
	this->swap(other);
    return *this;
}


void TMatrix::operator=(const double& value)
{//inits all the matrix coefficients to a common value
	fImpl->setConstant(value);
}

// TODO: Dimension checks?
TMatrix & TMatrix::operator+=(const TMatrix & right)
{//replaces this matrix by its sum with a second one
    setStatus(this->testStatus(right));
    if(numRows()!=right.numRows() || numCols()!=right.numCols()) {
        setStatus(kNull);
        fError += "Cannot add matrices of a different sizes";
    }
    if(getStatus()!=kNull)
    {
        fImpl->operator+=(*right.fImpl);
    }
    return *this;
}

// TODO: Could be a nonmember function
TMatrix TMatrix::operator+(const TMatrix & right) const
{//returns the sum of this matrix and a second one
	TMatrix result(*this);
    return result += right;
}

// TODO: Dimension checks?
TMatrix & TMatrix::operator-=(const TMatrix& right)
{//replaces this matrix by its difference with a second one
    setStatus(this->testStatus(right));
    if(numRows()!=right.numRows() || numCols()!=right.numCols()) {
        setStatus(kNull);
        fError += "Cannot calculate a difference between the matrices of a different sizes";
    }
    if(getStatus()!=kNull)
    {
        fImpl->operator-=(*right.fImpl);
    }
    return *this;
}


// TODO: Could be a nonmember function
TMatrix TMatrix::operator-(const TMatrix& right) const
{//returns the difference of this matrix and a second one
	TMatrix result(*this);
    return result -= right;
}

TMatrix & TMatrix::operator*=(const TMatrix & right)
{//replaces this matrix by its product with a second one
    setStatus(this->testStatus(right));
    if(getStatus()!=kNull && numCols() == right.numRows())
    {
        fImpl->operator*=(*right.fImpl);
    }
    return *this;
}

TMatrix TMatrix::operator*(const TMatrix & right) const
{//returns the product of this matrix and a second one
	TMatrix result(*this);
    return result *= right;
}

TColumnVector TMatrix::operator*(const TColumnVector& right) const
{//returns the product of this matrix by a column vector
    TColumnVector result(numRows());
	result.setStatus(this->testStatus(right));
    if (result.getStatus()!=kNull && numCols() == right.dimension())
	{
        TMatrixImpl tmp(numCols(),1,0.0);
        for(int i=0; i!=numCols(); ++i)
            tmp(i,0) = right(i);

        TMatrixImpl res = *this->fImpl * tmp;
        for(int i=0; i!=numRows(); ++i)
            result(i) = res(i,0);
	}
	return result;
}

TMatrix & TMatrix::operator*=(double value)
{
    if(getStatus()!=kNull)
    {
        fImpl->operator*=(value);
    }
    return *this;
}

TMatrix TMatrix::operator*(double value)
{//multiplies the Matrix by a scalar
    TMatrix result(*this);
    return result *= value;
}


TMatrix & TMatrix::operator*=(const TDouble & right)
{
    setStatus(this->testStatus(right));
    if(getStatus()!=kNull)
    {
        fImpl->operator*=(right.getValue());
    }
    return *this;
}

TMatrix TMatrix::operator*(const TDouble & right)
{//multiplies the Matrix by a TDouble
	TMatrix result(*this);
    return result *= right;
}


////////////////////////////////////
///Member Functions
////////////////////////////////////

TMatrix TMatrix::transposed() const
{//return the tranposed matrix of this one
    TMatrix result(*this);
    result.fImpl->transpose();
    return result;
}


TColumnVector TMatrix::eqnSolve(const TColumnVector& B)
{//returns the solution of the system this*X = B
    TColumnVector result(numRows());
	result.setStatus(this->testStatus(B));
    if (result.getStatus()!=kNull && numCols() == B.dimension())
	{
        TMatrixImpl tmp(numCols(),1,0.0);
        for(int i=0; i!=numCols(); ++i)
            tmp(i,0) = B(i);

        TMatrixImpl res = this->fImpl->solve(tmp);
        for(int i=0; i!=numCols(); ++i)
            result(i) = res(i,0);
    }
    return result;
}

TColumnVector TMatrix::ldltSolve(const TColumnVector& B)
{//returns the solution of the system this*X = B, requires P(S)D matrix
    TColumnVector result(numRows());
	result.setStatus(this->testStatus(B));
    if (result.getStatus()!=kNull && numCols() == B.dimension())
	{
        TMatrixImpl tmp(numCols(),1,0.0);
        for(int i=0; i!=numCols(); ++i)
            tmp(i,0) = B(i);

        TMatrixImpl res = this->fImpl->solveLdlt(tmp);
		if (res.rows() == 0)
		{
			result.setStatus(TVNumericValue::kNull);
			return result;
		}
        for(int i=0; i!=numCols(); ++i)
            result(i) = res(i,0);
    }
    return result;
}

// TODO: Is this used in the intermediate calculations?
// Maybe it would be better to make it a constructor.
bool TMatrix::initDiag(double value)
{//inits all the matrix diagonal coefficients to a common value
    if(fImpl->rows() == fImpl->cols())
	{
        fImpl->initDiag(value);
		return true;
	}
	else
    {
        // TODO: Are we sure that we don't want rectangular matrices as well?
        return false;
    }
}

// TODO: Is this used in the intermediate calculations?
// Maybe it would be better to use only constructors
void TMatrix::setDimensions(int nRows, int nCols)
{//Sets the dimensions of the matrix
    fImpl->resize(nRows, nCols);
	//{
	//	fNbRows = 1;
	//	fNbCols = 1;
	//	fMatrix = new double [fNbRows * fNbCols];
	//	(*this) = LITERAL(0.0);
	//	fError = "Invalid dimension" + '\n';
	//	setStatus( TANumericValue::kNull );
	//}
}


int TMatrix::numRows() const
{//returns the number of rows
	return fImpl->rows();
}


int TMatrix::numCols() const
{//returns the number of Columns
	return fImpl->cols();
}


bool TMatrix::invert()
{//Overwrites this Matrix by its inverse
//checks if the Matrix can be inverted
	if(getStatus() != TVNumericValue::kNull && numRows() == numCols())
	{
	    return fImpl->invert();
	}
	else
	{
		return false;
	}
}


	

TMatrix TMatrix::inverse()
{//Returns the inverse of this matrix (the latter isn't affected)

	TMatrix result(*this);
	bool inverted = result.invert();
    if(!inverted)
        result.setStatus(TANumericValue::kNull);
    return result;
}


void TMatrix::clear()
{//return a matrice (i,j)=0
    fImpl->setConstant(0.0);
}

double & TMatrix::operator()(int row, int col)
{//returns a reference to a matrix coefficient
    return fImpl->operator()(row,col);
}


double TMatrix::operator()(int row, int col) const
{//returns a copy of a matrix coefficient (const version of the previous method
	return fImpl->operator()(row,col);
}
