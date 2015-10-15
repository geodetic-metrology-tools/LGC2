//  TXYHe2MLATransformation.cpp
//
/** The direct transformation from a modified local astronomical reference frame to a
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
#include  "TRotation.h"
#include  "TReferenceEllipsoid.h"
#include  "TMLA2XYHeTransformation.h"
#include  "TXYHe2MLATransformation.h"
#include  "GeodeticConstants.h"
////////////////////////////////////////////////////////////////


//ClassImp(TXYHe2MLATransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TXYHe2MLATransformation::TXYHe2MLATransformation()
: fFrom(0), fTo(0), fEllipsoid(0)
{	// default constructor
}


TXYHe2MLATransformation::TXYHe2MLATransformation( TXYHeProjection* from )
: fFrom( from )
{	// constructor taking pointers to the source and destination reference frames
	fTo = from->getMLARefFrame();
	fEllipsoid = from->getEllipsoid();
}


TXYHe2MLATransformation::TXYHe2MLATransformation( const  TXYHe2MLATransformation& original )
: fFrom(0), fTo(0), fEllipsoid(0)
{	// copy constructor
	*this = original;
}


TXYHe2MLATransformation::~TXYHe2MLATransformation()
{
}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TXYHe2MLATransformation&  TXYHe2MLATransformation::operator=(const TXYHe2MLATransformation& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		setSourceFrame( right.getXYHe() );
		setDestinationFrame( right.getMLARF() );
		setEllipsoid( right.getEllipsoid() );
	}
	return *this;
}


TXYHe2MLATransformation*  TXYHe2MLATransformation::clone() const
{// Return a pointer to a clone of this reference frame
	return new TXYHe2MLATransformation( *this );
}


TARefFrameTransformation*  TXYHe2MLATransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TMLA2XYHeTransformation( fFrom );
}

void TXYHe2MLATransformation::setSourceFrame( TXYHeProjection* xyhe )
{
	fFrom = xyhe; 
	setDestinationFrame(xyhe->getMLARefFrame());
	setEllipsoid(xyhe->getEllipsoid());
	return; 
}


bool  TXYHe2MLATransformation::transform(TPositionVector& pv) const
{// Transformation of a position vector using the parameters of the two reference frames	
	TReal dx, dy, he, d, d0, Dzh;

	// distance from P0 in XY-plane
	dx = pv.getX().getValue() - fTo->getFalseOrigin().getX().getValue();
	dy = pv.getY().getValue() - fTo->getFalseOrigin().getY().getValue();
	he = pv.getH().getValue();
	d=sqrtq( (powq(dx,2)) + (powq(dy,2)) );

	// bearing from P0 in the XY-plane
	TAngle beta = TAngle::aTan2(dx,dy);

	// azimuth from P0
	TAngle alpha = fTo->getOrientation().getElements(TCoordSysFactory::k3DCartesian).getAngles(TRotationMatrix::kRzyx).kappa;
	TAngle azimuth = alpha + beta;

	// radius of the ellipsoid at azimuth and at phiP0
	TSpatialPosition falseOrigin(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
	TPositionVector vector(fTo->getFalseOrigin().getX().getValue(),
		fTo->getFalseOrigin().getY().getValue(),
		fTo->getFalseOrigin().getZ().getValue(),
		TCoordSysFactory::k3DCartesian);
	falseOrigin.setCoordinates(vector);
	
	TAngle phiP0;
	phiP0 = fTo->getOrigin().getCoordinates(TCoordSysFactory::kGeodetic).getPhiEllipsoid(/*fEllipsoid, &falseOrigin*/);
	TReal nuP0 = fEllipsoid->getNu(phiP0.getRadiansValue());

	TReal rhoalpha = nuP0 / (1 + fEllipsoid->getEPrimeSquared()
		*powq( (phiP0.cosine()) * (azimuth.cosine()) ,2 ) );

	// angle between point and z-axis
	TAngle omega = TAngle::aSin( d / (rhoalpha + he) );

	// scale factor
	TReal k = rhoalpha  / (rhoalpha + he);

	d0 = k*d;

	// transform TPositionVector
	//Again "fTo->getOrigin().getCoordinates(TCoordSysFactory::kGeodetic).getH(/*TGraph::getGraph()->getEllipsoid(TGraph::kGRS80)*/).getValue()" does not return correct value
	Dzh = fTo->getFalseOrigin().getZ().getValue() - HP0;
//	Dzh = fTo->getFalseOrigin().getZ().getValue() - fTo->getOrigin().getCoordinates(TCoordSysFactory::kGeodetic).getH(/*TGraph::getGraph()->getEllipsoid(TGraph::kGRS80)*/).getValue();

	TAngle omegaBy2 = omega * LITERAL(0.5);

	TScalar newZ(Dzh + he * omega.cosine()
		- d0 * (omegaBy2.tangent()));

	// change the H coordinate of the position vector into the third coordinate Z
//	pv.setH( newZ ); 

	// change the coordinate system of the position vector
	pv.setCoordSys(TCoordSysFactory::k3DCartesian);
	pv.setZ( newZ ); 
	return true;
}


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
