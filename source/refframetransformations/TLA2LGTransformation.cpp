//  TLA2LGTransformation.cpp
//
/** The transformation from a local astronomical reference frame to a
    local geodetic reference frame.
	
    The deflection of the vertical, and the laplace correction at the
	origin of the reference frames define the transformation.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////


//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TRotation.h"
#include  "TReflection.h"
#include  "TTranslation.h"
#include  "TLG2LATransformation.h"
#include  "TLA2LGTransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TLA2LGTransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLA2LGTransformation::TLA2LGTransformation()
: fFrom(0), fTo(0), fTransform(0)
{	// default constructor
}


TLA2LGTransformation::TLA2LGTransformation( TGraphLocalAstronomicalRF* from )
: fFrom( from ), fTransform(0)
{	// constructor taking pointers to the source and destination reference frames
	fTo = from->getLocalGeodeticRF();

	// if both reference frame pointers are not null pointers initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();
}


TLA2LGTransformation::TLA2LGTransformation( const  TLA2LGTransformation& original )
: fFrom(0), fTo(0), fTransform(0)
{	// copy constructor
	*this = original;
}


TLA2LGTransformation::~TLA2LGTransformation()
{//destructor
	if( 0 != fTransform ) 
	{
		delete fTransform;
	}
}


//////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////
TLA2LGTransformation&  TLA2LGTransformation::operator=(const TLA2LGTransformation& right)
{	// Copy Assignment operator
	if (this != &right)
	{
		setSourceFrame( right.getLARF() );
		setDestinationFrame( right.getLGRF() );
	}
	return *this;
}


TLA2LGTransformation*  TLA2LGTransformation::clone() const
{// Return a pointer to a clone of this reference frame
	return new TLA2LGTransformation( *this );
}


TARefFrameTransformation*  TLA2LGTransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TLG2LATransformation( getLARF() );
}


void TLA2LGTransformation::setSourceFrame( TGraphLocalAstronomicalRF* LA)
{
	fFrom = LA; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();

	return; 
}


void TLA2LGTransformation::setDestinationFrame( TModifiedLocalGeodeticRF* LG )
{
	fTo = LG; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();

	return; 
}


bool  TLA2LGTransformation::transform( TPositionVector& pv ) const
{// transform a position vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(pv);

	return result;
}


bool  TLA2LGTransformation::transform( TFreeVector& fv ) const
{// transform a free vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(fv);

	return result;
}


bool  TLA2LGTransformation::transform( TRotationMatrix& rmx ) const
{// transform a Rotation Matrix
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(rmx);

	return result;
}


void  TLA2LGTransformation::initialise()
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

	return;
}


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
