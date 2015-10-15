// TAAffineTransformation.cpp
//
/** Abstract Base Class for affine transformation. Deals with the status */
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
#include  "TSpatialPosition.h"
#include  "TSpatialVector.h"
#include  "TSpatialOrientation.h"
#include  "TCompositeAffTransform.h"

#include  "TAAffineTransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TAAffineTransformation)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



TAAffineTransformation::TAAffineTransformation()
{
}

	
TAAffineTransformation::~TAAffineTransformation()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Composition
// Create a composite transformation by applying this transformation 
// to an affine transformation
//////////////////////////////////////////////////////////////////////
TCompositeAffTransform & TAAffineTransformation::operator()( TCompositeAffTransform & right ) const
{
	right.prepend( *this );
	return right;
}


TCompositeAffTransform TAAffineTransformation::operator()( const TAAffineTransformation & right ) const
{
	TCompositeAffTransform result( *this );
	return result( right );
}


bool  TAAffineTransformation::transform( TSpatialPosition& spos) const
{// transform a spatial position
	TPositionVector vect = spos.getCoordinates(TCoordSysFactory::k3DCartesian);
	bool transformed = transform(vect);
	if (transformed == true)
	{
		spos.setCoordinates(vect);
	}
	return transformed;

}


bool  TAAffineTransformation::transform( TSpatialVector& svec) const
{// transform a spatial vector
	TFreeVector vect = svec.getElements(TCoordSysFactory::k3DCartesian);
	bool transformed = transform(vect);
	if (transformed == true)
	{
		svec.setElements(vect);
	}
	return transformed;
}


bool  TAAffineTransformation::transform( TSpatialOrientation& sori) const
{// transform a spatial orientation
	TRotationMatrix matrix = sori.getElements(TCoordSysFactory::k3DCartesian);
	bool transformed = transform(matrix);
	if (transformed == true)
	{
		sori.setElements(matrix);
	}
	return transformed;
}


/*bool  TAAffineTransformation::transform( TPositionVector& pos) const
{// transform a position vector : default function
	return false;
}


bool  TAAffineTransformation::transform( TFreeVector& free) const
{// transform a free vector : default function
	return false;
}


bool  TAAffineTransformation::transform( TRotationMatrix& rotation) const
{// transform a rotation matrix : default function
	return false;
}*/


TSpatialPosition &  TAAffineTransformation::operator() ( TSpatialPosition & spos ) const
{// apply this transformation to a spatial position

	if( !spos.isNull() && !this->isNull() )
	{
		TPositionVector vect = spos.getCoordinates(TCoordSysFactory::k3DCartesian);
		this->operator ()(vect);
		spos.setCoordinates(vect);
	}
	else
	{
		spos.setStatus( TVNumericValue::kNull );
	}
	return spos;
}


TSpatialVector &  TAAffineTransformation::operator() ( TSpatialVector & svec) const
{// apply this transformation to a spatial vector
	if( !svec.isNull() && !this->isNull() )
	{
		TFreeVector vect = svec.getElements(TCoordSysFactory::k3DCartesian);
		this->operator ()(vect);
		svec.setElements(vect);
	}
	else
	{
		svec.setStatus( TVNumericValue::kNull );
	}
	return svec;
}


TSpatialOrientation &  TAAffineTransformation::operator() ( TSpatialOrientation & sori) const
{// apply this transformation to a spatial orientation

	if( !sori.isNull() && !this->isNull() )
	{
		TRotationMatrix matrix = sori.getElements(TCoordSysFactory::k3DCartesian);
		this->operator ()(matrix);
		sori.setElements(matrix);
	}
	else
	{
		sori.setStatus( TVNumericValue::kNull );
	}
	return sori;
}
