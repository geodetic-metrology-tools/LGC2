//  TMLA2GCTransformation.cpp
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//
#include  "TRotation.h"
#include  "TReflection.h"
#include  "TTranslation.h"
#include  "TGC2MLATransformation.h"
#include  "TMLA2GCTransformation.h"

#include  "TReferenceEllipsoid.h"
////////////////////////////////////////////////////////////////


//ClassImp(TMLA2GCTransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TMLA2GCTransformation::TMLA2GCTransformation()
: fFrom(0), fTo(0), fTransform(0)
{	// default constructor
}


TMLA2GCTransformation::TMLA2GCTransformation( TAModifiedLocalAstronomicalRF* from, TAGeoidModel* IMLARFGeoid )
: fFrom( from ), fGeoid( IMLARFGeoid ), fTransform(0)
{	// constructor taking pointers to the source and destination reference frames
	fTo = from->getGeodeticRF();
	// if both reference frame pointers are not null pointers initialise the transformation
	if( 0 != fFrom  &&  0 != fTo && 0!= fGeoid )
		initialise();
}


TMLA2GCTransformation::TMLA2GCTransformation( const  TMLA2GCTransformation& original )
: fFrom(0), fTo(0), fTransform(0)
{	// copy constructor
	*this = original;
}


TMLA2GCTransformation::~TMLA2GCTransformation()
{//destructor
	if( 0 != fTransform ) 
	{
		delete fTransform;
	}
}




//////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////
TMLA2GCTransformation&  TMLA2GCTransformation::operator=(const TMLA2GCTransformation& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		setSourceFrame( right.getMLARF() );
		setDestinationFrame( right.getGCRF() );
		setGeoid( right.getGeoid() );
	}
	return *this;
}



TMLA2GCTransformation*  TMLA2GCTransformation::clone() const
{// Return a pointer to a clone of this reference frame
	return new TMLA2GCTransformation( *this );
}



TARefFrameTransformation*  TMLA2GCTransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TGC2MLATransformation( getMLARF(), getGeoid() );
}


void TMLA2GCTransformation::setSourceFrame( TAModifiedLocalAstronomicalRF* LA)
{
	fFrom = LA; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo  &&  0 != fGeoid )
		initialise();

	return; 
}


void TMLA2GCTransformation::setDestinationFrame( TGeodeticRefFrame* LG )
{
	fTo = LG; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo  &&  0 != fGeoid )
		initialise();

	return; 
}


void TMLA2GCTransformation::setGeoid( TAGeoidModel* geoid)
{// Set the geoid
	fGeoid = geoid;

	// if all references set then initialise
	if( 0 != fFrom  &&  0 != fTo  &&  0 != fGeoid )
		initialise();

	return; 
}


bool  TMLA2GCTransformation::transform( TPositionVector& pv ) const
{// transform a position vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(pv);

	return result;
}


bool  TMLA2GCTransformation::transform( TFreeVector& fv ) const
{// transform a free vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(fv);

	return result;
}



bool  TMLA2GCTransformation::transform( TRotationMatrix& rmx ) const
{// transform a Rotation Matrix
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(rmx);

	return result;
}


void  TMLA2GCTransformation::initialise()
{// Initialise the transformation using the parameters of the two reference frames

	// establish the transformation from the instrument modified local
	// astronomical to the geodetic cartesian system

	// get the origin of the local reference frame in the geodetic cartesian system
	// and a pointer to it
	TSpatialPosition origin = getMLARF()->getOrigin();
	TSpatialPosition* pOrigin = &origin;

	// get the false origin for the IMLARF as a Free Vector 
	TFreeVector falseOrigin = getMLARF()->getFalseOrigin();
	
	// establish the components of the transformation

	// rotations
	TAngle pi, piBy2;
	pi = TAngle::pi();
	piBy2 = TAngle::piBy2();

	TReferenceEllipsoid* ell = fGeoid->getDefRefEll();
	
	TZAxisRotation  r1( (pi - getGCRF()->getGeodeticCoords(pOrigin, ell).getLambdaEllipsoid()).getRadiansValue() );
	TYAxisRotation  r2( (piBy2 - getGCRF()->getGeodeticCoords(pOrigin, ell).getPhiEllipsoid()).getRadiansValue() );
	TZAxisRotation  r3( piBy2.getRadiansValue() );


	TXAxisRotation  r4( fFrom->getEta().getRadiansValue() );
	TYAxisRotation  r5( -(fFrom->getXsi().getRadiansValue()) );
	TZAxisRotation  r6( fFrom->getDAlpha().getRadiansValue() );


	TRotation r7( getMLARF()->getOrientation().getElements(TCoordSysFactory::k3DCartesian) );

	// reflections
	TReflection  p( TReflection::kXEqualY );

	// translations
	TTranslation  t1( (falseOrigin.getX()), (falseOrigin.getY()), (falseOrigin.getZ()) );
	t1.invert();
	TTranslation  t2( origin.getCoordinates(TCoordSysFactory::k3DCartesian).getX(),
		origin.getCoordinates(TCoordSysFactory::k3DCartesian).getY(),
		origin.getCoordinates(TCoordSysFactory::k3DCartesian).getZ() );

	// establish the rotation
	// the rotation is defined in terms of left handed reference frames
	// so a reflection is needed before and after the rotation
	if(fTransform != 0)
	{
		delete fTransform;
	}
	fTransform = new TCompositeAffTransform( t2( (r1 * r2 * r3 )( p( (r6 * r5 * r4)( p( r7( t1 ) ) ) ) ) ) );

	return;
}



//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
