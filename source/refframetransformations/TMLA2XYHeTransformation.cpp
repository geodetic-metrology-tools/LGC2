//  TMLA2XYHeTransformation.h
//
/** The direct transformation from a modified local astronomical reference frame to a
    geodetic reference frame.
	
    The cooerdinates of the origin, and the laplace correction at the
	origin of the reference frames define the transformation.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//
#include  "TRotation.h"
#include  "TReferenceEllipsoid.h"
#include  "TMLA2XYHeTransformation.h"
#include  "TXYHe2MLATransformation.h"
#include  "TCoordSysFactory.h"
////////////////////////////////////////////////////////////////


//ClassImp(TMLA2XYHeTransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMLA2XYHeTransformation::TMLA2XYHeTransformation()
: fFrom(0), fTo(0)
{	// default constructor
}


TMLA2XYHeTransformation::TMLA2XYHeTransformation( TXYHeProjection* to )
: fTo( to )
{	// constructor taking pointers to the source and destination reference frames
	fFrom = to->getMLARefFrame();
	fEllipsoid = to->getEllipsoid();
}


TMLA2XYHeTransformation::TMLA2XYHeTransformation( const  TMLA2XYHeTransformation& original )
: fFrom(0), fTo(0)
{	// copy constructor
	*this = original;
}


TMLA2XYHeTransformation::~TMLA2XYHeTransformation()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TMLA2XYHeTransformation&  TMLA2XYHeTransformation::operator=(const TMLA2XYHeTransformation& right)
{	// Copy Assignment operator
	if (this != &right)
	{
		setSourceFrame( right.getMLARF() );
		setDestinationFrame( right.getXYHe() );
		setEllipsoid( right.getEllipsoid() );
	}
	return *this;
}


TMLA2XYHeTransformation*  TMLA2XYHeTransformation::clone() const
{// Return a pointer to a clone of this reference frame
	return new TMLA2XYHeTransformation( *this );
}


TARefFrameTransformation*  TMLA2XYHeTransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TXYHe2MLATransformation( fTo );
}


bool  TMLA2XYHeTransformation::transform(TPositionVector& pv) const
{// Transformation of a position vector using the parameters of the two reference frames
	
	TReal dx, dy, dz, d, d0;

	// distance from P0 in XY-plane
	dx = pv.getX().getValue() - fFrom->getFalseOrigin().getX().getValue();
	dy = pv.getY().getValue() - fFrom->getFalseOrigin().getY().getValue();
	dz = pv.getZ().getValue() - fFrom->getFalseOrigin().getZ().getValue()
		+ fTo->getMLARefFrame()->getOrigin().getCoordinates(TCoordSysFactory::kGeodetic).getH(/*TGraph::getGraph()->getEllipsoid(TGraph::kGRS80)*/).getValue();
	d=sqrtq( (powq(dx,2)) + (powq(dy,2)) );
	// bearing from P0 in the XY-plane
	TAngle beta = TAngle::aTan2(dx,dy);

	// azimuth from P0
	TAngle alpha = fFrom->getOrientation().getElements(TCoordSysFactory::k3DCartesian).getAngles(TRotationMatrix::kRzyx).kappa;
	TAngle azimuth = alpha + beta;

	// radius of the ellipsoid at azimuth and at phiP0
	TSpatialPosition falseOrigin(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
	TScalar xp, yp, zp;
	xp.setValue(fFrom->getFalseOrigin().getX().getValue());
	yp.setValue(fFrom->getFalseOrigin().getY().getValue());
	zp.setValue(fFrom->getFalseOrigin().getZ().getValue());
	falseOrigin.getCoordinates(TCoordSysFactory::k3DCartesian).setX(xp);
	falseOrigin.getCoordinates(TCoordSysFactory::k3DCartesian).setY(yp);
	falseOrigin.getCoordinates(TCoordSysFactory::k3DCartesian).setZ(zp);
	
	TAngle phiP0;
	phiP0 = fFrom->getOrigin().getCoordinates(TCoordSysFactory::kGeodetic).getPhiEllipsoid(/*fEllipsoid, &falseOrigin*/);
	TReal nuP0 = fEllipsoid->getNu(phiP0.getRadiansValue());

	TReal rhoalpha = nuP0 / (1 + fEllipsoid->getEPrimeSquared()
		*powq( (phiP0.cosine()) * (azimuth.cosine()) ,2 ) );

	// angle between point and z-axis
	TAngle omega = TAngle::aTan( d / (rhoalpha + dz) );

	TReal k = ( rhoalpha * omega.cosine() ) / (rhoalpha + dz);

	// scale factor
	d0 = k*d;

	// transform TPositionVector
	TAngle omegaBy2 = omega * LITERAL(0.5);
	TScalar newH( (dz + d0 * omegaBy2.tangent()) / omega.cosine() );
	
	// Change the third dimension Z of the position vector into H
//	pv.setZ(newH);
	// Change the coordinate system of the position vector
	pv.setCoordSys(TCoordSysFactory::k2DPlusH);             
	pv.setH(newH);
	return true;
}



//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
