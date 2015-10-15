// TReflection.cpp
//
/** 3D Reflection, affine transformation, six cases are distinguished:
Reflection by a plane x=0, y=0, z=0 or Reflection by the plane x=y, x=z or y=z*/
//
// Patterns:
//
// 
// Copyright 2000-10 CERN SU, M.Jones. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TReflection.h"
#include  "TPositionVector.h"
#include  "TFreeVector.h"
#include  "TRotationMatrix.h"
////////////////////////////////////////////////////////////////


//ClassImp(TReflection)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TReflection::TReflection() : fReflectionMatrix(TCoordSysFactory::k3DCartesian)
{	// default constructor
	fReflectionType = kNull;
	setStatus(TVNumericValue::kNull);
}

TReflection::TReflection(const TReflection::EType type) : fReflectionMatrix(TCoordSysFactory::k3DCartesian)
{
	fReflectionType = type;
	setStatus(TVNumericValue::kKnown);
	fillMatrix(type);
}

TReflection::TReflection( const TReflection& original ) : fReflectionMatrix(TCoordSysFactory::k3DCartesian)
{	// copy constructor
	*this = original;
}


TReflection::~TReflection()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TReflection&  TReflection::operator=( const TReflection& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		fReflectionType = right.getType();
		fillMatrix(fReflectionType);
		
		setStatus(right.getStatus());
	}
	return *this;
}


TReflection*  TReflection::clone() const
{// Return a pointer to a clone of this transformation
	return new TReflection( *this );
}


bool TReflection::transform(TPositionVector& rightV) const
{/// Transform a TPositionVector
	// P*V1

	bool trans = false;

	if (isNull()==false)
	{
		rightV = fReflectionMatrix * rightV;
		trans = true;
	}		
	return trans;
}

		
bool TReflection::transform(TFreeVector & rightV) const
{/// Transform a TFreeVector
	// V2=P*V1
	bool trans = false;
	
	if (isNull()==false)
	{
		rightV = fReflectionMatrix * rightV;
		trans = true;
	}		
	return trans;
}

		
bool TReflection::transform(TRotationMatrix& rm) const
{/// Transform a TRotationMatrix
	// TODO: why is the other object unused here?
	ignoring(rm);
	bool trans = false;
	if (isNull() == false)
		trans = true;

	return trans;
}


TPositionVector &  TReflection::operator() ( TPositionVector & right ) const
{// apply this transformation to a position vector
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		right = fReflectionMatrix * right;
	}
	return right;
}


TFreeVector &  TReflection::operator() ( TFreeVector & right ) const
{// apply this transformation to a free vector
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		right = fReflectionMatrix * right;
	}
	return right;
}



TRotationMatrix &  TReflection::operator() ( TRotationMatrix & right ) const
{// apply this transformation to a Rotation Matrix
	if ( this->isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	return right;
}


TReflection * TReflection::inverse() const
{// Return a pointer to the inverse of this transformation
	// the inverse of a reflection is the same reflection
	TReflection * inverse = this->clone();
	return inverse;
}


void TReflection::invert()
{// Invert the transformation, replaces the current transformation parameters
	// the inverse of a reflection is the same reflection
	return;
}


//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

void TReflection::fillMatrix( TReflection::EType type )
{
	// create identity
	fReflectionMatrix.initDiag(1);
	

	switch (type){

	case kNull:break;

	case kXEqual0: fReflectionMatrix.setElt(0,0,-LITERAL(1.0));break;

	case kYEqual0: fReflectionMatrix.setElt(1,1,-LITERAL(1.0));break;

	case kZEqual0: fReflectionMatrix.setElt(2,2,-LITERAL(1.0));break;

	case kXEqualY: fReflectionMatrix.setElt(0,0,LITERAL(0.0));
				fReflectionMatrix.setElt(1,1,LITERAL(0.0));
				fReflectionMatrix.setElt(0,1,LITERAL(1.0));
				fReflectionMatrix.setElt(1,0,LITERAL(1.0));break;

	case kXEqualZ: fReflectionMatrix.setElt(0,0,LITERAL(0.0));
				fReflectionMatrix.setElt(2,2,LITERAL(0.0));
				fReflectionMatrix.setElt(0,2,LITERAL(1.0));
				fReflectionMatrix.setElt(2,0,LITERAL(1.0));break;

	case kYEqualZ: fReflectionMatrix.setElt(1,1,LITERAL(0.0));
				fReflectionMatrix.setElt(2,2,LITERAL(0.0));
				fReflectionMatrix.setElt(1,2,LITERAL(1.0));
				fReflectionMatrix.setElt(2,1,LITERAL(1.0));break;

	default: break;
	}
	
return;
}

//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
