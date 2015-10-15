//  TXYHg2XYHeTransformation.cpp
//
/** The direct transformation from a modified local geodetic reference frame to a
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
#include  "TAGeoidModel.h"
#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TXYHg2XYHeTransformation.h"
#include  "TXYHe2XYHgTransformation.h"
#include  "TLength.h"
////////////////////////////////////////////////////////////////


//ClassImp(TX0Y0He2XYHeTransformation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TXYHg2XYHeTransformation::TXYHg2XYHeTransformation()
: fFrom(0), fTo(0), fEllipsoid(0), fGeoid(0)
{	// default constructor
}


TXYHg2XYHeTransformation::TXYHg2XYHeTransformation( TXYHgProjection* from )
: fFrom( from )
{	// constructor taking pointers to the source and destination reference frames
	fTo = from->getXYHe();
	fGeoid = from->getGeoid();
	fEllipsoid = fTo->getEllipsoid();
}


TXYHg2XYHeTransformation::TXYHg2XYHeTransformation( const  TXYHg2XYHeTransformation& original )
: fFrom(0), fTo(0), fEllipsoid(0), fGeoid(0)
{	// copy constructor
	*this = original;
}


TXYHg2XYHeTransformation::~TXYHg2XYHeTransformation()
{
}




//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TXYHg2XYHeTransformation&  TXYHg2XYHeTransformation::operator=(const TXYHg2XYHeTransformation& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		setSourceFrame( right.getXYHg() );
		setDestinationFrame( right.getXYHe() );
		setEllipsoid( right.getEllipsoid() );
		setGeoid( right.getGeoid() );
	}
	return *this;
}


TXYHg2XYHeTransformation*  TXYHg2XYHeTransformation::clone() const
{// Return a pointer to a clone of this reference frame
	return new TXYHg2XYHeTransformation( *this );
}


TARefFrameTransformation*  TXYHg2XYHeTransformation::inverse() const
{// Return a pointer to the inverse of this transformtion
	return new TXYHe2XYHgTransformation( fFrom );
} 


void TXYHg2XYHeTransformation::setSourceFrame( TXYHgProjection* xyhg )
{
	fFrom = xyhg;
	setDestinationFrame(xyhg->getXYHe());
	setEllipsoid(xyhg->getXYHe()->getEllipsoid());
	setGeoid(xyhg->getGeoid());
	
	return; 
}


bool  TXYHg2XYHeTransformation::transform( TPositionVector& pv ) const
{// Transformation of a position vector using N (height above geoid)


	TScalar hg(pv.getH());

	TReal x, y, h;
	x = pv.getX().getValue();
	y = pv.getY().getValue();
	h = hg.getValue();

	// extraction of N using a tspatialposition of coordinates = pv, and reference frame = CCS
	TSpatialPosition trick(fTo->getMLARefFrame(), x, y, h, pv.getCoordSys()); 
	TScalar N = fGeoid->getN(trick).getMetresValue();

	pv.setH(hg+N);

	return true;
}


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
