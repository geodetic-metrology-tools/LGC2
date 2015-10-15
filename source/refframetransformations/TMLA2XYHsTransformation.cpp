//  TMLA2XYHsTransformation.h
//
/** The direct transformation from a modified local astronomical reference frame to a
    geodetic reference frame.
	
    The cooerdinates of the origin, and the laplace correction at the
	origin of the reference frames define the transformation.


    Patterns:
  
   
    Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//
#include  "TRotation.h"
#include  "TReferenceEllipsoid.h"
#include  "TMLA2XYHsTransformation.h"
#include  "TXYHs2MLATransformation.h"
#include  "TCoordSysFactory.h"
////////////////////////////////////////////////////////////////


//ClassImp(TMLA2XYHsTransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMLA2XYHsTransformation::TMLA2XYHsTransformation()
: fFrom(0), fTo(0)
{	// default constructor
}


TMLA2XYHsTransformation::TMLA2XYHsTransformation( TXYHeProjection* to )
: fTo( to )
{	// constructor taking pointers to the source and destination reference frames
	fFrom = to->getMLARefFrame();
	fEllipsoid = to->getEllipsoid();
}


TMLA2XYHsTransformation::TMLA2XYHsTransformation( const  TMLA2XYHsTransformation& original )
: fFrom(0), fTo(0)
{	// copy constructor
	*this = original;
}


TMLA2XYHsTransformation::~TMLA2XYHsTransformation()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TMLA2XYHsTransformation&  TMLA2XYHsTransformation::operator=(const TMLA2XYHsTransformation& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		setSourceFrame( right.getMLARF() );
		setDestinationFrame( right.getXYHs() );
		setEllipsoid( right.getEllipsoid() );
	}
	return *this;
}


TMLA2XYHsTransformation*  TMLA2XYHsTransformation::clone() const
{// Return a pointer to a clone of this reference frame
	return new TMLA2XYHsTransformation( *this );
}


TARefFrameTransformation*  TMLA2XYHsTransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TXYHs2MLATransformation( fTo );
}


bool  TMLA2XYHsTransformation::transform(TPositionVector& pv) const
{// Transformation of a position vector using the parameters of the two reference frames
	
	TReal dx, dy, dz, d;
	TReal d0, omega;
	// radius of the sphere
	TReal R = 6371000;	
	// transform TPositionVector
	TReal H;

	//distance from P0 in XY-plane
	dx = pv.getX().getValue() - fFrom->getFalseOrigin().getX().getValue();
	dy = pv.getY().getValue() - fFrom->getFalseOrigin().getY().getValue();
	//Following line does not work properly, "fTo->getMLARefFrame()->getOrigin().getCoordinates(TCoordSysFactory::kGeodetic).getH(/*TGraph::getGraph()->getEllipsoid(TGraph::kGRS80)*/).getValue()" 
	//does not return what is expected
	dz = pv.getZ().getValue() - fFrom->getFalseOrigin().getZ().getValue()
		+ fTo->getMLARefFrame()->getOrigin().getCoordinates(TCoordSysFactory::kGeodetic).getH(/*TGraph::getGraph()->getEllipsoid(TGraph::kGRS80)*/).getValue();
	d=sqrtq( (powq(dx,2)) + (powq(dy,2)) );

	//H = sqrtq( (powq((R+dz),2) + (powq(d,2))) )-  R;
	omega = atanq(d / (R+dz));
	d0 = d * R * cosq(omega) / (R + dz);
	H = ( dz + (d0 * tanq(omega/LITERAL(2.0))) ) / cosq(omega);

	TScalar newH (H);
	
	// Change the coordinate system of the position vector
	pv.setCoordSys(TCoordSysFactory::k2DPlusH);   
	
	// Change the third dimension Z of the position vector into H
	pv.setH(newH);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
//End
//////////////////////////////////////////////////////////////////////
