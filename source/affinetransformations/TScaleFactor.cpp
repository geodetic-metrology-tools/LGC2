// TScaleFactor.cpp
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
#include  "TScaleFactor.h"
#include  "TPositionVector.h"
#include  "TFreeVector.h"
#include  "TRotationMatrix.h"
////////////////////////////////////////////////////////////////


//ClassImp(TScaleFactor)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TScaleFactor::TScaleFactor()
{	// default constructor
	fScaleFactor.setValue(LITERAL(1.0));
	this->setStatus( TVNumericValue::kNull );
}


TScaleFactor::TScaleFactor( TReal f )
{	// constructor taking factor
	fScaleFactor.setValue(f);
	this->setStatus( TVNumericValue::kKnown );
}

TScaleFactor::TScaleFactor( TScalar f )
{	// constructor taking factor
	fScaleFactor = f;
	this->setStatus( TVNumericValue::kKnown );
}


TScaleFactor::TScaleFactor( const  TScaleFactor & original )
{	// copy constructor
	*this = original;

}


TScaleFactor::~TScaleFactor()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TScaleFactor&  TScaleFactor::operator=(const TScaleFactor & right)
{	// Copy Assignment operator

	if (this != &right)
	{
		fScaleFactor = right.getScaleFactor();
		setStatus( right.getStatus() );
	}
	return *this;
}

//! Multiplication by another scaling transformation
TScaleFactor TScaleFactor::operator*( const TScaleFactor & right )
{
	TScaleFactor result( this->getScaleFactor() * right.getScaleFactor() );
	result.setStatus( this->testStatus(right) );
	return result;
}


TScaleFactor*  TScaleFactor::clone() const
{// Return a pointer to a clone of this transformation
	return new TScaleFactor( *this );
}

//////////////////////////////////////////////////////////////////////
// Transforme
//////////////////////////////////////////////////////////////////////

bool  TScaleFactor::transform(TPositionVector& pv) const
{/// Return a transformed position vector
	bool trans = false;

	if (isNull()==false)
	{
		pv *= this->getScaleFactor();
		trans = true;
	}
	return trans;
}

	
bool  TScaleFactor::transform(TFreeVector& fv) const
{/// Return a transformed free vector
	bool trans = false;
	if (isNull()==false)
	{
		fv *= this->getScaleFactor();
		trans = true;
	}
	return trans;
}

	
bool TScaleFactor::transform(TRotationMatrix& rm) const
{/// Return a transformed rotation matrix
	ignoring(rm);
	bool trans = false;
	if (isNull()==false)
	{
		trans = true;
	}
	return trans;
}


TPositionVector &  TScaleFactor::operator() ( TPositionVector & right ) const
{// apply this transformation to a position vector
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		right = right * this->getScaleFactor();
	}
	return right;
}


TFreeVector &  TScaleFactor::operator() ( TFreeVector & right ) const
{// apply this transformation to a free vector
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		right = right * this->getScaleFactor();
	}
	return right;
}


TRotationMatrix &  TScaleFactor::operator() ( TRotationMatrix & right ) const
{// apply this transformation to a Rotation Matrix
	if ( this->isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	return right;
}


TScaleFactor *  TScaleFactor::inverse() const
{// Return the inverse transformation
	TScaleFactor * inverse = new TScaleFactor( LITERAL(1.0) / this->getScaleFactor().getValue() );
	return inverse;
}


void TScaleFactor::invert()
{/// Change this transformation in the inverse
	fScaleFactor.setValue( LITERAL(1.0) / fScaleFactor.getValue() );
	return;
}


