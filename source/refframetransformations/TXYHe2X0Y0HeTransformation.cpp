//  TXYHe2X0Y0HeTransformation.cpp
//
/** The direct transformation from a modified local geodetic reference frame to a
    geodetic reference frame.
	
    The coordinates of the origin, and the laplace correction at the
	origin of the reference frames define the transformation.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//
//#include  "TVReferenceFrame.h"
//#include  "TRotation.h"
#include  "TReferenceEllipsoid.h"
#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TXYHe2X0Y0HeTransformation.h"
#include  "TX0Y0He2XYHeTransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TX0Y0He2XYHeTransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TXYHe2X0Y0HeTransformation::TXYHe2X0Y0HeTransformation()
: fFrom(0), fTo(0), fEllipsoid(0)
{	// default constructor
}


TXYHe2X0Y0HeTransformation::TXYHe2X0Y0HeTransformation( TX0Y0HeProjection* to )
: fTo( to )
{	// constructor taking pointers to the source and destination reference frames
	fFrom = to->getXYHe();
	fEllipsoid = to->getEllipsoid();
}


TXYHe2X0Y0HeTransformation::TXYHe2X0Y0HeTransformation( const  TXYHe2X0Y0HeTransformation& original )
: fFrom(0), fTo(0), fEllipsoid(0)
{	// copy constructor
	*this = original;
}


TXYHe2X0Y0HeTransformation::~TXYHe2X0Y0HeTransformation()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TXYHe2X0Y0HeTransformation&  TXYHe2X0Y0HeTransformation::operator=(const TXYHe2X0Y0HeTransformation& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		setSourceFrame( right.getXYHe() );
		setDestinationFrame( right.getX0Y0He() );
		setEllipsoid( right.getEllipsoid() );
	}
	return *this;
}


TXYHe2X0Y0HeTransformation*  TXYHe2X0Y0HeTransformation::clone() const
{// Return a pointer to a clone of this reference frame
		return new TXYHe2X0Y0HeTransformation( *this );
}


TARefFrameTransformation*  TXYHe2X0Y0HeTransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TX0Y0He2XYHeTransformation( fTo );
}


bool  TXYHe2X0Y0HeTransformation::transform(TPositionVector& pv) const
{// Transformation of a position vector using the parameters of the two reference frames
	TReal Dx, dx, Dy, dy, he, falseX, falseY;

	// distance from P0 in XY-plane
	dx = pv.getX().getValue() - fFrom->getMLARefFrame()->getFalseOrigin().getX().getValue();
	dy = pv.getY().getValue() - fFrom->getMLARefFrame()->getFalseOrigin().getY().getValue();
	he = pv.getH().getValue();

	// bearing from P0 in the XY-plane
	TAngle beta = TAngle::aTan2(dx,dy);

	// azimuth from P0
	TAngle alpha = fFrom->getMLARefFrame()->getOrientation().getElements(TCoordSysFactory::k3DCartesian).getAngles(TRotationMatrix::kRzyx).kappa;
	TAngle azimuth = alpha + beta;
	
	// radius of the ellipsoid at azimuth and at phiP0
	TSpatialPosition falseOrigin(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
	TPositionVector vector(fFrom->getMLARefFrame()->getFalseOrigin().getX().getValue(),
		fFrom->getMLARefFrame()->getFalseOrigin().getY().getValue(),
		fFrom->getMLARefFrame()->getFalseOrigin().getZ().getValue(),
		TCoordSysFactory::k3DCartesian);
	falseOrigin.setCoordinates(vector);
	
	TAngle phiP0;
	phiP0 = fFrom->getMLARefFrame()->getOrigin().getCoordinates(TCoordSysFactory::kGeodetic).getPhiEllipsoid(/*fEllipsoid, &falseOrigin*/);
	
	TReal nuP0 = fEllipsoid->getNu(phiP0.getRadiansValue());

	TReal rhoalpha = nuP0 / (1 + fEllipsoid->getEPrimeSquared()
		*powq( (phiP0.cosine()) * (azimuth.cosine()) ,2 ) );
	
	// scale factor
	TReal k = rhoalpha / (rhoalpha + he);
	
	//Deltas to be added on X0 and Y0 coordinates, using the scale factor
 	Dx = k * dx;
	Dy = k * dy;
	
	falseX = fFrom->getMLARefFrame()->getFalseOrigin().getX().getValue();
	falseY = fFrom->getMLARefFrame()->getFalseOrigin().getY().getValue();
	
	// transform tpositionvector
	TScalar newX(falseX + Dx), newY(falseY + Dy);
	pv.setX(newX);
	pv.setY(newY);

	return true;
}


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
