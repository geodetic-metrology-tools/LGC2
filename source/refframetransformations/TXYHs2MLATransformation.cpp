//  TXYHs2MLATransformation.cpp
//
/** The direct transformation from a modified local astronomical reference frame to a
    geodetic reference frame.
	
    The coordinates of the origin, and the laplace correction at the
	origin of the reference frames define the transformation.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////


//For ROOT//////////////////////////////////////////////////////
//
//#include  "TVReferenceFrame.h"
#include  "TRotation.h"
#include  "TReferenceEllipsoid.h"
#include  "TXYHs2MLATransformation.h"
#include  "TMLA2XYHsTransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TXYHs2MLATransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TXYHs2MLATransformation::TXYHs2MLATransformation()
: fFrom(0), fTo(0), fEllipsoid(0)
{	// default constructor
}


TXYHs2MLATransformation::TXYHs2MLATransformation( TXYHeProjection* from )
: fFrom( from )
{	// constructor taking pointers to the source and destination reference frames
	fTo = from->getMLARefFrame();
	fEllipsoid = from->getEllipsoid();
}


TXYHs2MLATransformation::TXYHs2MLATransformation( const  TXYHs2MLATransformation& original )
: fFrom(0), fTo(0), fEllipsoid(0)
{	// copy constructor
	*this = original;
}


TXYHs2MLATransformation::~TXYHs2MLATransformation()
{
}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TXYHs2MLATransformation&  TXYHs2MLATransformation::operator=(const TXYHs2MLATransformation& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		setSourceFrame( right.getXYHs() );
		setDestinationFrame( right.getMLARF() );
		setEllipsoid( right.getEllipsoid() );
	}
	return *this;
}


TXYHs2MLATransformation*  TXYHs2MLATransformation::clone() const
{// Return a pointer to a clone of this reference frame
		return new TXYHs2MLATransformation( *this );
}


TARefFrameTransformation*  TXYHs2MLATransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TMLA2XYHsTransformation( fFrom );
}


void TXYHs2MLATransformation::setSourceFrame( TXYHeProjection* XYHs )
{
	fFrom = XYHs; 
	setDestinationFrame(XYHs->getMLARefFrame());
	setEllipsoid(XYHs->getEllipsoid());
	
	return; 
}



bool  TXYHs2MLATransformation::transform(TPositionVector& pv) const
{// Transformation of a position vector using the parameters of the two reference frames
	TReal dx, dy, hs, d;
	TReal d0, omega;
	// radius of the sphere
	TReal R = LITERAL(6371000.0);
	// transform TPositionVector
	TReal Z;

	// distance from P0 in XY-plane
	dx = pv.getX().getValue() - fTo->getFalseOrigin().getX().getValue();
	dy = pv.getY().getValue() - fTo->getFalseOrigin().getY().getValue();
	hs = pv.getH().getValue();
	d=sqrtq( (powq(dx,2)) + (powq(dy,2)) );

	//Z = sqrtq( (powq((R+hs),2) - (powq(d,2))) )-  R + LITERAL(2000.00079);
	d0 = d * R / (R + hs);
	omega = asinq(d0 / R);
	Z = LITERAL(2000.00079) + (hs * cosq(omega)) - (d0 * tanq(omega/LITERAL(2.0)));

	TScalar newZ (Z);

	// change the coordinate system of the position vector
	pv.setCoordSys(TCoordSysFactory::k3DCartesian);

	// change the H coordinate of the position vector into the third coordinate Z
	pv.setZ( newZ ); 

	return true;
}


//////////////////////////////////////////////////////////////////////
//End
//////////////////////////////////////////////////////////////////////
