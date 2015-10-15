// TColumnVector.cpp
//
/** Class for a vector of doubles.
Designed to be easiliy usable with vector functions of the NagC math library */
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include "TFreeVector.h"
#include  "TColumnVector.h"
////////////////////////////////////////////////////////////////


//ClassImp(TColumnVector)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////


TColumnVector::TColumnVector()
{//!Default constructor	
	fVector = 0;
	fNbRows = 0;
	setStatus( TANumericValue::kNull );
}


TColumnVector::TColumnVector(int nRows):
fNbRows(nRows)
{//!Constructor setting the dimensions of the vector
	fVector = new TReal [fNbRows];
	(*this) = TReal(LITERAL(0.0));
	setStatus( TANumericValue::kKnown );
}
		


TColumnVector::TColumnVector( const  TColumnVector& source ):
fNbRows(source.fNbRows)
{//!Copy constructor

	//!dimensioning of the vector
	fVector = new TReal [fNbRows];

	//!copy the vector coefficients
	for (int i = 0; i<fNbRows; i++)
		fVector[i] = source.fVector[i];

	//!copy the status
	setStatus( source.getStatus() );

}



TColumnVector::~TColumnVector()
{//!Destructor
	delete[] fVector;
}



//////////////////////////////////////////////////////////////////////
///operator
//////////////////////////////////////////////////////////////////////
TColumnVector&  TColumnVector::operator=(const TColumnVector& right)
{//!Copy assignement operator
	if (this != &right)
	{
		if (dimension() <= right.dimension() && dimension()!=0)
		{
			for (int i=0; i<fNbRows; i++)
				(const_cast<TColumnVector*>(this))->operator()(i) = right(i);
			setStatus (right.getStatus());
		}
		else
		{
			this->setStatus(kNull);
			this->setDimension(0);
		}
	}

	return *this;
}


TColumnVector TColumnVector::operator +(const TColumnVector& right) const
{//!returns the sum of this vector and a second one
	TColumnVector resultat;
	resultat.setStatus(kNull);
	EStatus status=this->testStatus(right);
	if (status!=kNull && dimension() == right.dimension())
	{
		TColumnVector resul (dimension());
		for (int i=0; i<fNbRows; i++)
		{
			(resul)(i) = (*this)(i) + right(i);
		}
		resultat.setDimension(dimension());
		resultat = resul;
		resultat.setStatus(status);
	}
	return resultat;
}


TColumnVector& TColumnVector::operator +=(const TColumnVector& right)
{//!replaces this vector by its sum with a second one
	(*this) = (*this) + (right);
	return (*this);
}



TColumnVector TColumnVector::operator -(const TColumnVector& right) const
{//!returns the difference of this vector and a second one

	TColumnVector resultat;
	resultat.setStatus(kNull);
	EStatus status=this->testStatus(right);
	if (status!=kNull && dimension() == right.dimension())
	{
		TColumnVector resul (dimension());
		for (int i=0; i<fNbRows; i++)
		{
			(resul)(i) = (*this)(i) - right(i);
		}
		resultat.setDimension(dimension());
		resultat = resul;
		resultat.setStatus(status);
	}
	return resultat;	
}


TColumnVector& TColumnVector::operator -=(const TColumnVector& right)
{//!replaces this vector by its difference with a second one
	(*this) = (*this) - (right);
	return (*this);
}


TColumnVector TColumnVector::operator*(const TReal k)
{//!multiplies the vector by a scalar
	TColumnVector result (dimension());
	for (int i=0; i<fNbRows; i++)
		(result)(i) = (*this)(i)*k;
	return result;
}

TColumnVector TColumnVector::operator*(const TDouble k)
{//!multiplies the vector by a TDouble
	TColumnVector resultat;
	EStatus status=this->testStatus(k);
	if (status!=kNull )
	{
		resultat.setDimension(dimension());
		TReal d=k.getValue();
		for (int i=0; i<fNbRows; i++)
			(resultat)(i) = (*this)(i)*d;
	}
	resultat.setStatus(status);
	return resultat;
}


TDouble TColumnVector::operator *(const TColumnVector& right) const
{//!returns the scalar product of this vector and a second one
	TDouble resultat;
	resultat.setStatus(kNull);
	EStatus status=this->testStatus(right);
	if (status!=kNull && dimension() == right.dimension())
	{
		TReal result = LITERAL(0.0);
		for (int i=0; i<dimension(); i++)
			result =result + (*this)(i) * right(i);
		resultat.setValue(result);
		resultat.setStatus(status);	
	}
	return resultat;
}


void TColumnVector::operator=(const TReal comVal)
{//!inits all the vector coefficients to a common value
	for (int i = 0; i<fNbRows; i++)
		fVector[i] = comVal;
}


TReal& TColumnVector::operator()(const int row)
{//!returns a reference to a vector coefficient
	return fVector[row];
}


TReal TColumnVector::operator()(const int row) const
{//!returns a copy of a vector coefficient (const version of the previous method)
	return fVector[row];
}


///////////////////////////////////////////////////////////////////
///member functions
///////////////////////////////////////////////////////////////////

int TColumnVector::dimension() const
{//!Get dimension
	return fNbRows;
}


void TColumnVector::setDimension(const int nRows)
{//!Sets the dimensions of the vector

	//!destroy the previous values
	if (fVector != 0)
	{
		delete[] fVector;
	}

	//!creates the new vector
	fNbRows = nRows;
	fVector = new TReal [fNbRows];
}


TMatrix TColumnVector::transposed()const
{//!return the tranposed row vector as a matrix
	TMatrix resultat (1,dimension());
	if((this->isNull())==false)
	{	for (int i=0; i<dimension(); i++)
		{
			(resultat)(0,i) = (*this)(i);
		}
	}
	resultat.setStatus(getStatus());
	return resultat;
}


/////////////////////////////////////////////////////
///Private Function
/////////////////////////////////////////////////////

TReal* TColumnVector::getFirstEltAdr() const
{//!returns the adress of the vector's first element
	return fVector;
}


