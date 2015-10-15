//  TGC2MLATransformation.cpp
//
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//
//#include  "TVReferenceFrame.h"
#include  "TRotation.h"
#include  "TReflection.h"
#include  "TTranslation.h"
#include  "TMLA2GCTransformation.h"
#include  "TGC2MLATransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TGC2MLATransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TGC2MLATransformation::TGC2MLATransformation()
: fFrom(0), fTo(0), fTransform(0)
{	// default constructor
}


TGC2MLATransformation::TGC2MLATransformation( TAModifiedLocalAstronomicalRF* to,
											   TAGeoidModel* IMLARFGeoid)
: fTo( to ), fGeoid( IMLARFGeoid )
{	// constructor taking pointers to the source and destination reference frames
	fTransform = 0;

	fFrom = to->getGeodeticRF();
	
	// if both reference frame pointers are not null pointers initialise the transformation
	if( 0 != fFrom  &&  0 != fTo  &&  0 != fGeoid )
		fTransform=0;
		initialise();
}


TGC2MLATransformation::TGC2MLATransformation( const  TGC2MLATransformation& original )
: fFrom(0), fTo(0), fTransform(0)
{	// copy constructor
	*this = original;
}


TGC2MLATransformation::~TGC2MLATransformation()
{//destructor
	if( 0 != fTransform )
	{
		delete fTransform;
	}
}


//////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////
TGC2MLATransformation&  TGC2MLATransformation::operator=(const TGC2MLATransformation& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		setSourceFrame( right.getGCRF() );
		setDestinationFrame( right.getMLARF() );
		setGeoid( right.getGeoid() );
	}
	return *this;
}


TGC2MLATransformation*  TGC2MLATransformation::clone() const
{// Return a pointer to a clone of this reference frame
	return new TGC2MLATransformation( *this );
}


TARefFrameTransformation*  TGC2MLATransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TMLA2GCTransformation( getMLARF(), fGeoid );
}


void TGC2MLATransformation::setSourceFrame( TGeodeticRefFrame* LG )
{
	fFrom = LG; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo  &&  0 != fGeoid )
		initialise();

	return; 
}


void TGC2MLATransformation::setDestinationFrame( TAModifiedLocalAstronomicalRF* LA)
{
	fTo = LA; 
	
	// if both reference frames have been set then initialise the transformation
	if( 0 != fFrom  &&  0 != fTo  &&  0 != fGeoid )
		initialise();

	return; 
}


void TGC2MLATransformation::setGeoid( TAGeoidModel* geoid )
{// Set the geoid
	fGeoid = geoid;

	// if all references set initialise transformation
	if( 0 != fFrom  &&  0 != fTo  &&  0 != fGeoid )
		initialise();

	return; 
}


bool  TGC2MLATransformation::transform( TPositionVector& pv ) const
{// transform a position vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(pv);

	return result;
}


bool  TGC2MLATransformation::transform( TFreeVector& fv ) const
{// transform a free vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(fv);

	return result;
}


bool  TGC2MLATransformation::transform( TRotationMatrix& rmx ) const
{// transform a Rotation Matrix
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(rmx);

	return result;
}


void  TGC2MLATransformation::initialise()
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

	// between geodetic and local geodetic systems
	TZAxisRotation  r1( (pi - getGCRF()->getGeodeticCoords(pOrigin, ell).getLambdaEllipsoid()).getRadiansValue() );
	TYAxisRotation  r2( (piBy2 - getGCRF()->getGeodeticCoords(pOrigin, ell).getPhiEllipsoid()).getRadiansValue() );
	TZAxisRotation  r3( piBy2.getRadiansValue() );


	// between local geodetic and local astronomical systems
	TXAxisRotation  r4( fTo->getEta().getRadiansValue() );
	TYAxisRotation  r5( -(fTo->getXsi().getRadiansValue()) );
	TZAxisRotation  r6( fTo->getDAlpha().getRadiansValue() );


	// between local astronomical and modified local astronomical systems
	TRotation r7( getMLARF()->getOrientation().getElements(TCoordSysFactory::k3DCartesian) );

	// reflections
	TReflection  p( TReflection::kXEqualY );


	// translations
	TTranslation  t1( (falseOrigin.getX()), (falseOrigin.getY()),
		(falseOrigin.getZ()) );
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
	fTransform = new TCompositeAffTransform( t2( (r1 * r2 * r3)( p( (r6 * r5 * r4)( p( r7( t1 ) ) ) ) ) ) );
	fTransform->invert();//invert ne fait pas appelle a new

	return;
}


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
