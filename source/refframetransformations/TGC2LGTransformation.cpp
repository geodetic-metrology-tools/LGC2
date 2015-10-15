//  TGC2LGTransformation.cpp
//
/** The transformation from a local astronomical reference frame to a
    local geodetic reference frame.
	
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
#include  "TTranslation.h"
#include  "TLG2GCTransformation.h"
#include  "TGC2LGTransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TGC2LGTransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TGC2LGTransformation::TGC2LGTransformation()
: fFrom(0), fTo(0), fTransform(0)
{	// default constructor
}


TGC2LGTransformation::TGC2LGTransformation( TModifiedLocalGeodeticRF* to )
: fTo( to )
{	// constructor taking pointers to the source and destination reference frames
	fTransform = 0;

	fFrom = to->getGeodeticRF();
	
	// if both reference frame pointers are not null pointers initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
	{
		initialise();
	}
}


TGC2LGTransformation::TGC2LGTransformation( const  TGC2LGTransformation& original )
: fFrom(0), fTo(0), fTransform(0)
{// copy constructor
	*this = original;
}


TGC2LGTransformation::~TGC2LGTransformation()
{//destructor
	if( 0 != fTransform )
	{
		delete fTransform;
	}
}


//////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////
TGC2LGTransformation&  TGC2LGTransformation::operator=(const TGC2LGTransformation& right)
{	// Copy Assignment operator
	if (this != &right)
	{
		setSourceFrame( right.getGCRF() );
		setDestinationFrame( right.getLGRF() );
	}
	return *this;
}


TGC2LGTransformation*  TGC2LGTransformation::clone() const
{// Return a pointer to a clone of this reference frame
	return new TGC2LGTransformation( *this );
}


TARefFrameTransformation*  TGC2LGTransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TLG2GCTransformation( getLGRF() );
}


void TGC2LGTransformation::setSourceFrame( TGeodeticRefFrame* GC)
{
	fFrom = GC; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();

	return; 
}


void TGC2LGTransformation::setDestinationFrame( TModifiedLocalGeodeticRF* LG )
{
	fTo = LG; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();

	return; 
}


bool TGC2LGTransformation::transform( TPositionVector& pv ) const
{// transform a position vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(pv);

	return result;
}


bool  TGC2LGTransformation::transform( TFreeVector& fv ) const
{// transform a free vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(fv);

	return result;
}


bool  TGC2LGTransformation::transform( TRotationMatrix& rmx ) const
{// transform a Rotation Matrix
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(rmx);

	return result;
}


void  TGC2LGTransformation::initialise()
{// Initialise the transformation using the parameters of the two reference frames

	// establish the transformation from the geodetic to the local
	// geodetic system
	
	// get the origin of the local geodetic reference frame (rotation matrix)
	TSpatialPosition  origin = getLGRF()->getOrigin();

	// establish the components of the transformation
	// rotations
	TZAxisRotation  r1( TAngle::pi().getRadiansValue() - 
		origin.getCoordinates(TCoordSysFactory::kGeodetic).getLambdaEllipsoid(/*getLGRF()->getReferenceEllipsoid()*/).getRadiansValue() );
	TYAxisRotation  r2( TAngle::piBy2().getRadiansValue() - 
		origin.getCoordinates(TCoordSysFactory::kGeodetic).getPhiEllipsoid(/*getLGRF()->getReferenceEllipsoid()*/).getRadiansValue() );
	TZAxisRotation  r3( TAngle::piBy2().getRadiansValue() );

	// translation
	TTranslation t(origin.getCoordinates(TCoordSysFactory::k3DCartesian).getX(),
		origin.getCoordinates(TCoordSysFactory::k3DCartesian).getY(),
		origin.getCoordinates(TCoordSysFactory::k3DCartesian).getZ());

	// get the false origin
	TFreeVector  falseOrigin = getLGRF()->getFalseOrigin();
	
	// establish the components of the transformation
	TRotation  r( getLGRF()->getOrientation().getElements(TCoordSysFactory::k3DCartesian) );
	TTranslation  t2( falseOrigin.getX(), falseOrigin.getY(),
		falseOrigin.getZ() );
	
	
	// establish the transformation
	if(fTransform != 0)
	{
		delete fTransform;
	}
	fTransform = new TCompositeAffTransform( t( (r1 * r2 * r3)( r( t2 ) ) ) );
	fTransform->invert();//invert ne fait pas appelle a new

	return;
}


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
