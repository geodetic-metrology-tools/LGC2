//  TLG2GCTransformation.cpp
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
#include  "TGC2LGTransformation.h"
#include  "TLG2GCTransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TLG2GCTransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TLG2GCTransformation::TLG2GCTransformation()
: fFrom(0), fTo(0), fTransform(0)
{	// default constructor
}


TLG2GCTransformation::TLG2GCTransformation( TModifiedLocalGeodeticRF* from )
: fFrom( from ), fTransform(0)
{	// constructor taking pointers to the source and destination reference frames
	fTo = from->getGeodeticRF();
	
	// if both reference frame pointers are not null pointers initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();
}


TLG2GCTransformation::TLG2GCTransformation( const  TLG2GCTransformation& original )
: fFrom(0), fTo(0), fTransform(0)
{	// copy constructor
	*this = original;
}


TLG2GCTransformation::~TLG2GCTransformation()
{//destructor
	if( 0 != fTransform )
	{
		delete fTransform;
	}
}



//////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////
TLG2GCTransformation&  TLG2GCTransformation::operator=(const TLG2GCTransformation& right)
{	// Copy Assignment operator
	if (this != &right)
	{
		setSourceFrame( right.getLGRF() );
		setDestinationFrame( right.getGCRF() );
	}
	return *this;
}


TLG2GCTransformation*  TLG2GCTransformation::clone() const
{// Return a pointer to a clone of this reference frame
	return new TLG2GCTransformation( *this );
}


TARefFrameTransformation*  TLG2GCTransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TGC2LGTransformation( getLGRF() );
}


void TLG2GCTransformation::setSourceFrame( TModifiedLocalGeodeticRF* LG)
{
	fFrom = LG; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();

	return; 
}


void TLG2GCTransformation::setDestinationFrame( TGeodeticRefFrame* GC )
{
	fTo = GC; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo )
		initialise();

	return; 
}


bool  TLG2GCTransformation::transform( TPositionVector& pv ) const
{// transform a position vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(pv);

	return result;
}


bool  TLG2GCTransformation::transform( TFreeVector& fv ) const
{// transform a free vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(fv);

	return result;
}


bool  TLG2GCTransformation::transform( TRotationMatrix& rmx ) const
{// transform a Rotation Matrix
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(rmx);

	return result;
}


void  TLG2GCTransformation::initialise()
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
	fTransform = new TCompositeAffTransform( t( (r1 * r2 * r3 * r)( t2 ) ) );
	
	return;
}



//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
