// TGeodeticRefFrame.cpp
//
/** A Geodetic Ellisoidal Reference Frame. 
    Deals with requests for Spatial Position coordinates
    appropriate to a Geodetic reference frame.

  
    Patterns:
    A concrete STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//forward declarations
#include	"TReferenceEllipsoid.h"

#include	"TSpatialPosition.h"
#include	"TPositionStatus.h"
#include	"TSpatialPositionError.h"

#include	"TSpatialVector.h"
#include	"TSpatialVectorStatus.h"
#include	"TSpatialVectorError.h"

#include  "TGeodeticRefFrame.h"
////////////////////////////////////////////////////////////////


//ClassImp(TGeodeticRefFrame)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
const TReal TGeodeticRefFrame::precisionH = LITERAL(0.0000001); //precision calcul h
const TReal TGeodeticRefFrame::precisionPhi = LITERAL(0.00000000000001); //precision calcul phi


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TGeodeticRefFrame::TGeodeticRefFrame( const string& name ) 
	: TA3DEuclideanRefFrame( name )
{	// constructor taking the name of the reference frame
}



TGeodeticRefFrame::TGeodeticRefFrame( const string& name, TReferenceEllipsoid* ell ) 
	: TA3DEuclideanRefFrame( name )
{	// constructor taking the name of the reference frame
	fEllipsoid = ell;
}

// **Deliberately not implemented**
//TGeodeticRefFrame::TGeodeticRefFrame( const  TGeodeticRefFrame& original )
//{	// copy constructor
//}


TGeodeticRefFrame::~TGeodeticRefFrame()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TPositionVector TGeodeticRefFrame::getCoordinates(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys coordsys) const
{
	if(coordsys == TCoordSysFactory::k3DCartesian) {
		return getPositionVector(sp);
	}
	else if(coordsys == TCoordSysFactory::kGeodetic) 
	{
		//fEllipsoid = ;
		return getGeodeticCoords(getPositionVector(sp),
			fEllipsoid/*TRefSystemFactory::getRefSystemFactory()->getEllipsoid(TRefSystemFactory::kGRS80)*/); //compiles but doesn't work
	}
	else if(coordsys == TCoordSysFactory::kGeodeticSphere)
	{
		
		return getGeodeticCoords(getPositionVector(sp),
			fEllipsoid/*TRefSystemFactory::getRefSystemFactory()->getEllipsoid(TRefSystemFactory::kSphere)*/);
	}
	else
	{
		TPositionVector pv(TCoordSysFactory::k3DCartesian);
		return pv;
	}
}



bool TGeodeticRefFrame::setCoordinates(TSpatialPosition* sp, const TPositionVector& pv)
{
	if(pv.getCoordSys() == TCoordSysFactory::k3DCartesian)
	{
		return setPositionVector(sp,pv);
	}
	else if (pv.getCoordSys() == TCoordSysFactory::kGeodetic)
	{
		return setPositionVector(sp, getCartesianCoords(pv, fEllipsoid));
	}
	else
	{
		return false;
	}
}


TPositionVector   TGeodeticRefFrame::getGeodeticCoords( const TSpatialPosition* pos, const TReferenceEllipsoid* ellipsoid )  const
{
	return getGeodeticCoords( getPositionVector(pos), ellipsoid );
}

TGeodeticRefFrame* TGeodeticRefFrame::instance() {
	static TGeodeticRefFrame *grf = new TGeodeticRefFrame("NULL");
	return grf;
}

/////////////////////////////////////////////////////////////////////////////////////////
//private fonctions
/////////////////////////////////////////////////////////////////////////////////////////

TPositionVector   TGeodeticRefFrame::getGeodeticCoords( const TPositionVector pv, const TReferenceEllipsoid* ellipsoid )  const
{
	// get the Cartesian coordinates of the position
	TReal  x = pv.getX().getValue();
	TReal  y = pv.getY().getValue();
	TReal  z = pv.getZ().getValue();
	//TReal  x = getX1( posn );
	//TReal  y = getX2( posn );
	//TReal  z = getX3( posn );

	//get a copy of the parameters of the ellipsoid
	TReal  a = ellipsoid->getA();
	TReal  eSquared = ellipsoid->getESquared();

	TReal p;
	TReal nu;
	AngleValue phi, tempphi, lambda;
	LengthValue h, temph;
	TPositionVector  geodpv(TCoordSysFactory::kGeodetic);
//	TPositionVector*  pgeodpv;
//	pgeodpv=&geodpv;

	if( x==0 && y==0)
	{
		TAngle phirad, lrad;
		TScalar hm;
		phirad = TAngle::piBy2();;
		lrad.setRadiansValue(LITERAL(0.0));
		hm.setValue(z-a);
		geodpv.setPhiEllipsoid(phirad);
		geodpv.setLambdaEllipsoid(lrad);
		geodpv.setH(hm);
		geodpv.setStatus(pv.getStatus());
	}
	else
	{	
		p = sqrtq(x*x + y*y);

		//calcul lambda
		lambda = 2 * atanq ( y / (x + p) );
		
		//calcul iteratif phi et h
		//initialisation
		h = 0;
		temph = 1;
		phi = atanq ( z / (p * ( LITERAL(1.0) - eSquared )) );
		tempphi = phi+1;

		//boucle
		while ( ( fabsq( phi-tempphi ) > TGeodeticRefFrame::precisionPhi ) ||  ( fabsq( temph-h ) > TGeodeticRefFrame::precisionH ))
		{
			nu = ellipsoid->getNu( phi );
			temph = h;
			h = ( p / cosq(phi) ) - nu;
			tempphi = phi;
			phi = atanq ( z / (p * ( LITERAL(1.0) - (eSquared * nu / (nu+h)) )) );
		}
		
		TAngle phirad, lrad;
		TScalar hm;
		phirad.setRadiansValue(phi);
		lrad.setRadiansValue(lambda);
		hm.setValue(h);
		geodpv.setPhiEllipsoid(phirad);
		geodpv.setLambdaEllipsoid(lrad);
		geodpv.setH(hm);
		geodpv.setStatus(pv.getStatus());
	}
//	pv.setCoordSys(TGraph::kGeodetic); //to be implemented in TGraph and in TACoordSysVector
	return geodpv;
}


TPositionVector  TGeodeticRefFrame::getCartesianCoords( TPositionVector posvec, const TReferenceEllipsoid* ellipsoid )
{
	// get coordinates of posvec as TReal
	TReal phi = posvec.getPhiEllipsoid().getRadiansValue();
	TReal lambda = posvec.getLambdaEllipsoid().getRadiansValue();
	TReal he = posvec.getH().getValue();

	//get a copy of the parameters of the ellipsoid
	TReal a = ellipsoid->getA();
	TReal b = ellipsoid->getB();
	TReal nu = ellipsoid->getNu( phi );

	//retablissement XYZ
	TReal x((nu+he)*cosq(phi)*cosq(lambda)),
		y((nu+he)*cosq(phi)*sinq(lambda)),
		z( (nu*b*b/(a*a) + he)*sinq(phi));
	TPositionVector pv(x,y,z,TCoordSysFactory::k3DCartesian);

	return pv;
}


// **Deliberately not implemented**
//TGeodeticRefFrame&  TGeodeticRefFrame::operator=(const TGeodeticRefFrame& right)
//{	// Copy Assignment operator
//
//	if (this != &right)
//	{
//	}
//	return *this;
//}


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////

