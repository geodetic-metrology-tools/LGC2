// TTranslation.cpp
//
/** A Scaling Transformation */
//
// Patterns:
//
// 
// Copyright 2010 CERN SU, M.Jones. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TTranslation.h"
#include  "TPositionVector.h"
#include  "TFreeVector.h"
#include  "TRotationMatrix.h"
////////////////////////////////////////////////////////////////


//ClassImp(TTranslation)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TTranslation::TTranslation() : fTranslationVector(TCoordSysFactory::k3DCartesian)
{	// default constructor
	this->setStatus( TVNumericValue::kNull );
}




TTranslation::TTranslation(const TScalar Tx, const TScalar Ty, const TScalar Tz)
 : fTranslationVector(TCoordSysFactory::k3DCartesian)
{
	fTranslationVector.setX(Tx);
	fTranslationVector.setY(Ty);
	fTranslationVector.setZ(Tz);
	fTranslationVector.setStatus(kKnown);
	setStatus( TVNumericValue::kKnown );
}


TTranslation::TTranslation(const TFreeVector vector) : fTranslationVector(TCoordSysFactory::k3DCartesian)
{
	fTranslationVector = vector;
	setStatus( TVNumericValue::kKnown );
}


TTranslation::TTranslation( const  TTranslation& original )
 : fTranslationVector(TCoordSysFactory::k3DCartesian)
{	// copy constructor

	*this = original;
}


TTranslation::~TTranslation()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TTranslation&  TTranslation::operator=(const TTranslation & right)
{	// Copy Assignment operator

	if (this != &right)
	{		
		fTranslationVector = right.fTranslationVector;			
		setStatus(right.getStatus());
	}
	return *this;
}


TTranslation TTranslation::operator+(const TTranslation& right)
{
	TTranslation result( fTranslationVector + right.fTranslationVector );
	result.setStatus( this->testStatus(right) );
	return result;
}


TTranslation*  TTranslation::clone() const
{// Return a pointer to a clone of this transformation
	return new TTranslation( *this );
}


//////////////////////////////////////////////////////////////////////
// Transformations
//////////////////////////////////////////////////////////////////////

bool TTranslation::transform(TPositionVector& pv)const
{// TTranslate a vector of position

	if (isNull()==false)
	{
		pv += fTranslationVector;
		return true;
	}
	else
	{
		return false;
	}
}


bool TTranslation::transform(TFreeVector& fv)const
{// Transform a free vector
	ignoring(fv);
	if (isNull() == false)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool TTranslation::transform(TRotationMatrix& rm)const
{// Transform a rotation matrix
	ignoring(rm);
	if (isNull() == false)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/* apply this transformation to a position vector */
TPositionVector &  TTranslation::operator() ( TPositionVector & right ) const
{
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		right += this->getVector();
	}
	return right;
}


/* apply this transformation to a free vector */
TFreeVector &  TTranslation::operator() ( TFreeVector & right ) const
{
	if ( this->isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	return right;
}


/* apply this transformation to a Rotation Matrix */
TRotationMatrix &  TTranslation::operator() ( TRotationMatrix & right ) const
{
	if ( this->isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	return right;
}


TTranslation * TTranslation::inverse() const
{// Return a pointer to the inverse of this transformation
	TTranslation * copy = this->clone();
	copy->invert();
	return copy;
}


void TTranslation::invert()
{// Invert the transformation, replaces the current transformation parameters
	TFreeVector defaut(0,0,0, TCoordSysFactory::k3DCartesian);
	fTranslationVector.setStatus(kKnown);
	fTranslationVector= defaut-fTranslationVector;
	
	return;
}
