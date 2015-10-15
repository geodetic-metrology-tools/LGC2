//  TLG2LATransformation.cpp
//
/** The transformation from a local geodetic reference frame to a 
    local astronomical reference frame.
	
    The deflection of the vertical, and the laplace correction at the
	origin of the reference frames define the transformation.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TRotation.h"
#include  "TSpatialPosition.h"
#include  "TReflection.h"
#include "TTranslation.h"
#include  "THelmertTransformation.h"
#include  "TLA2LGTransformation.h"
#include  "TLG2LATransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TLG2LATransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLG2LATransformation::TLG2LATransformation()
: fFrom(0), fTo(0), fTransform(0)
{	// default constructor
}


TLG2LATransformation::TLG2LATransformation( TGraphLocalAstronomicalRF* to )
: fTo( to ), fTransform(0)
{	// constructor taking pointers to the source and destination reference frames
	fFrom = to->getLocalGeodeticRF();
	
	// if both reference frame pointers are not null pointers initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();
}


TLG2LATransformation::TLG2LATransformation( const  TLG2LATransformation& original )
: fFrom(0), fTo(0), fTransform(0)
{	// copy constructor
	*this = original;
}


TLG2LATransformation::~TLG2LATransformation()
{//delete
	if( 0 != fTransform ) 
	{
		delete fTransform;
	}
}



//////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////
TLG2LATransformation&  TLG2LATransformation::operator=(const TLG2LATransformation& right)
{	// Copy Assignment operator
	if (this != &right)
	{
		setSourceFrame( right.getLGRF() );
		setDestinationFrame( right.getLARF() );
	}
	return *this;
}


TLG2LATransformation*  TLG2LATransformation::clone() const
{// Return a pointer to a clone of this reference frame
  return new TLG2LATransformation( *this );
}


TARefFrameTransformation*  TLG2LATransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TLA2LGTransformation( getLARF() );
}


void TLG2LATransformation::setSourceFrame( TModifiedLocalGeodeticRF* LG)
{
	fFrom = LG; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();

	return; 
}


void TLG2LATransformation::setDestinationFrame( TGraphLocalAstronomicalRF* LA )
{
	fTo = LA; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();

	return; 
}


bool  TLG2LATransformation::transform( TPositionVector& pv ) const
{// transform a position vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(pv);

	return result;
}


bool  TLG2LATransformation::transform( TFreeVector& fv ) const
{// transform a free vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(fv);

	return result;
}


bool  TLG2LATransformation::transform( TRotationMatrix& rmx ) const
{// transform a Rotation Matrix
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(rmx);

	return result;
}


void  TLG2LATransformation::initialise()
{// Initialise the transformation using the parameters of the two reference frames

	// establish the transformation from the local geodetic to the local
	// astronomical system
	
	// get the origin in the reference frame of the local astronomical system geoid
	// and the geoid values at the origin
	TSpatialPosition  origin = getLGRF()->getOrigin();



	// establish the components of the transformation
	TXAxisRotation  r1( getLARF()->getEta().getRadiansValue() );
	TYAxisRotation  r2( -getLARF()->getXsi().getRadiansValue() );
	TZAxisRotation  r3( getLARF()->getDAlpha().getRadiansValue() );
	TReflection  p( TReflection::kXEqualY );

	TRotation r4(getLARF()->getOrientation().getElements(TCoordSysFactory::k3DCartesian));
	TTranslation t(getLARF()->getFalseOrigin().getX(),
		getLARF()->getFalseOrigin().getY(),
		getLARF()->getFalseOrigin().getZ());
	
	// establish the rotation
	// the rotation is defined in terms of left handed reference frames
	// so a reflection is needed before and after the rotation
	if(fTransform != 0)
	{
		delete fTransform;
	}
	fTransform = new TCompositeAffTransform( p( (r3 * r2 * r1)( p( r4( t ) ) ) ) );
	fTransform->invert();//n utilise pas new

	return;
}




//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////

