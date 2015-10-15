// TRotation.cpp
//
/** Class for rotations of TPositionVector and TFreeVector. 
It authorizes the inverse rotation and the multiplication 
by an other rotation or vector. Sub classes for rotations around each axis*/
//
// Patterns:
//
// 
// Copyright 2000-2010 CERN SU, M.Jones. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TRotation.h"
#include  "TPositionVector.h"
#include  "TFreeVector.h"
#include  "TRotationMatrix.h"
////////////////////////////////////////////////////////////////


//ClassImp(TRotation)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TRotation::TRotation()
{	// default constructor

	this->setStatus( TVNumericValue::kNull );
	fRotationMatrix.identity();

}

TRotation::TRotation( const TRotationMatrix matrix)
{//Constructor taking the rotation matrix
	fRotationMatrix = matrix;
	this->setStatus( TVNumericValue::kKnown );
}





TRotation::TRotation(TRotationMatrix::ERotationType kR, TReal omega, TReal phi, TReal kappa)
: fRotationMatrix(kR, omega, phi, kappa)
{//Constructor taking the radians values of the angles in the specified order
	this->setStatus( TVNumericValue::kKnown );
}




TRotation::TRotation( const  TRotation& original )
{	// copy constructor
	*this = original;
}


TRotation::~TRotation()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TRotation&  TRotation::operator=(const TRotation& right)
{	// Copy Assignment operator

	if (this != &right)
	{		
		fRotationMatrix = right.getRotationMatrix();
		setStatus(right.getStatus());
	}
	return *this;
}


//! Multiplication by another scaling transformation
TRotation TRotation::operator*( const TRotation & right )
{
	TRotation result( this->getRotationMatrix() * right.getRotationMatrix() );
	result.setStatus( this->testStatus(right) );
	return result;
}


TReal TRotation::operator()(int i, int j) const
{// return the ri, cj element with 0<=i,j<=2
	return fRotationMatrix.getElt(i,j);
}

void TRotation::setAllRotations(TRotationMatrix::ERotationType kR, TReal om, TReal p, TReal k)
{// set the 3 rotations taking angles values in the order specified by the enumerator
	fRotationMatrix.setAllRotations(kR,om,p,k);
}


Angles TRotation::getAngles(TRotationMatrix::ERotationType kR)
{// calculate the angles from the matrix with the specified rotation order
	Angles xyz;
	xyz=fRotationMatrix.getAngles(kR);

	return xyz;
}


TRotation *  TRotation::clone() const
{// Return a pointer to a clone of this transformation
	return new TRotation( *this );
}


//////////////////////////////////////////////////////////////////////
// Transformation methods
//////////////////////////////////////////////////////////////////////

bool TRotation::transform(TPositionVector& pv) const
{/// Transform a position vector

	bool trans = false;

	if (isNull()==false)
	{
		pv = this->getRotationMatrix() * pv;
		trans = true;
	}
	return trans;
}
		
		
bool TRotation::transform(TFreeVector& fv) const
{/// Transform a free vector
	bool trans = false;

	if (isNull()==false)
	{
		fv = this->getRotationMatrix() * fv;
		trans = true;
	}
	return trans;
}


bool  TRotation::transform(TRotationMatrix& rm) const 
{// Transform a rotation matrix

	bool trans = false;

	if (isNull()==false)
	{
		rm = this->getRotationMatrix() * rm;
		trans = true;
	}
	return trans;
}

	
TPositionVector &  TRotation::operator() ( TPositionVector & right ) const
{// apply this transformation to a position vector
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		right = fRotationMatrix * right;
	}
	return right;
}


TFreeVector &  TRotation::operator() ( TFreeVector & right ) const
{// apply this transformation to a free vector
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		right = fRotationMatrix * right;
	}
	return right;
}


TRotationMatrix &  TRotation::operator() ( TRotationMatrix & right ) const
{// apply this transformation to a Rotation Matrix
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		right = fRotationMatrix * right;
	}
	return right;
}


TRotation * TRotation::inverse() const
{//Return a pointer to the inverse of this transformation
	TRotation * copy = this->clone();
	copy->invert();
	return copy;
}
	
	
void TRotation::invert()
{// Invert the transformation, replaces the current transformation parameters
	fRotationMatrix.invert();
	return;
}


//////////////////////////////////////////////////////////////////////
// Private Member Functions
//////////////////////////////////////////////////////////////////////
void TRotation::fillRotationMatrix(TRotationMatrix::ERotationType kR, TReal om, TReal p, TReal k)
{
	fRotationMatrix.setAllRotations(kR,om,p,k);
	return;
}


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
