// 
// TCernParabolicGeoid
//
// Paraboloid defined to approximate the geoid in CERN's area, 
// with respect to the swiss mapping plane
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
//forward declarations

#include	"TGraph.h"
#include	"TSpatialPosition.h"
#include	"TAReferenceFrame.h"
#include	"TCernParabolicGeoid.h"

#include	"TLength.h"
#include	"TAngle.h"

////////////////////////////////////////////////////////////////

//ClassImp(TCernParabolicGeoid)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
const TReal TCernParabolicGeoid::scaleFactor = LITERAL(0.001);
const TReal TCernParabolicGeoid::scaleFactorM = LITERAL(0.01);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TCernParabolicGeoid::TCernParabolicGeoid() :fName(0), fDefRFPtr(0),
fCalcRFPtr(0), fDefEllPtr(0)
{
	fA = 0;
	fB = 0;
	fThs = 0;
	costhc = 0;
	cosazp = 0;
	sinthc = 0;
	sinazp = 0;

}


/* Constructor taking the geoid model name and the parameters of the 
 *	paraboloid as arguments.
 *	
 *	The paraboloid is of the form:
 *		
 *						a                 b
 *		  z = (xp*xp) * -   +   (yp*yp) * -
 *						2                 2
 */

/*TCernParabolicGeoid::TCernParabolicGeoid( const string& name, const TReal a,
										 const TReal b, const TReal ths)
		:  fName( name ), fA( a ), fB( b ), fThs( ths ),
		fDefinitionRF(TRefSurfServer::kCGRF), fDefinitionEllipsoid(TRefSurfServer::kGRS80), fCalculationRF(TRefSurfServer::kCCS)
{		
		// set the derived parameters of the paraboloid
		TReal thc, azp;
		TReal azxs = -LITERAL(1.12878) * TAngle::gonsToRadsFactor();
		TReal gsc = LITERAL(38.90742) * TAngle::gonsToRadsFactor();

		thc = fThs - gsc;
		azp = fThs + azxs;
		costhc = cosq(thc);
		sinthc = sinq(thc);
		cosazp = cosq(azp);
		sinazp = sinq(azp);


	
	
}
*/





TCernParabolicGeoid::TCernParabolicGeoid( const string& name, const TReal a,
										 const TReal b, const TReal ths,
										 TAReferenceFrame* def, TReferenceEllipsoid* ell,
										 TAReferenceFrame* calc)
		:  fName( name ), fA( a ), fB( b ), fThs( ths ),
		fDefRFPtr(def), fDefEllPtr(ell), fCalcRFPtr(calc)
{		
		// set the derived parameters of the paraboloid
		TReal thc, azp;
		TReal azxs = -LITERAL(1.12878) * TAngle::gonsToRadsFactor();
		TReal gsc = LITERAL(38.90742) * TAngle::gonsToRadsFactor();

	/*	//modif du 25/07/03 pour representer le niv5 utiliser dans LGC
		if(fThs* TAngle::radsToGonsFactor() == LITERAL(48.219))
		{
			thc = LITERAL(9.31158177001953) * TAngle::gonsToRadsFactor();
		//	azp = thc;
		}
		else
		{
			thc = fThs - gsc;
		}*/
		thc = fThs - gsc;
		azp = fThs + azxs;
		costhc = cosq(thc);
		sinthc = sinq(thc);
		cosazp = cosq(azp);
		sinazp = sinq(azp);
}


// **Deliberately not implemented**
//TCernParabolicGeoid::TCernParabolicGeoid( const  TCernParabolicGeoid& original )
//{	// copy constructor
//}


TCernParabolicGeoid::~TCernParabolicGeoid()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


// **Deliberately not implemented**
//TCernParabolicGeoid&  TCernParabolicGeoid::operator=(const TCernParabolicGeoid& right)
//{	// Copy Assignment operator
//
//	if (this != &right)
//	{
//	}
//	return *this;
//}


TLength	TCernParabolicGeoid::getN( const TSpatialPosition& position ) const
{//
	LengthValue x, y;
	TReal dx, dy, xp, yp;
	//TSpatialPosition position( point.getPosition( modelSystem ) );
	//GeoidValue fNValue;
	TReal falseOriginX(2000), falseOriginY(LITERAL(2097.79265));

	x = position.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue();
	y = position.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue();

	dx = (x-falseOriginX)*scaleFactor;
	dy = (y-falseOriginY)*scaleFactor;

	xp = dx*costhc-dy*sinthc;
	yp = dx*sinthc+dy*costhc;

	//TSpatialPosition position( point.getPosition( modelSystem ) );
	//xp = (position.getXCoord()).getMetresValue();
	//yp = (position.getYCoord()).getMetresValue();

	//Calculate N (separation between ellipsoid and geoid) in meters
	TLength fNValue;
	fNValue.setMetresValue( ((fA/2)*xp*xp + (fB/2)*yp*yp)*scaleFactorM );
	return fNValue;
}


TAngle	TCernParabolicGeoid::getXi( const TSpatialPosition& sp ) const
{//

	// deep copy of TSpatialPosition
	TSpatialPosition position(sp);

	// RF of TSpatialPosition must be the same as the geoid CalculationRF
	if (position.getRefFrame() != fCalcRFPtr)
	{
		position.transform(fCalcRFPtr);
	}



	LengthValue x, y;
	TReal dx, dy, xp, yp;
	TReal falseOriginX(2000), falseOriginY(LITERAL(2097.79265));
	//TSpatialPosition position( modelSystem );
	TAngle fXiValue;

	//position = point.getPosition( modelSystem );
	x = position.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue();
	y = position.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue();

	dx = (x-falseOriginX)*scaleFactor;
	dy = (y-falseOriginY)*scaleFactor;

	xp = dx*costhc-dy*sinthc;
	yp = dx*sinthc+dy*costhc;

// Calculate the vertical deflection in N-S direction  
	fXiValue.setRadiansValue((fA*sinazp*xp -fB*cosazp*yp)/100000 ); //* LITERAL(6.366);
	return fXiValue;

}

TAngle	TCernParabolicGeoid::getEta( const TSpatialPosition& sp ) const
{//

	// deep copy of TSpatialPosition
	TSpatialPosition position(sp);

	// RF of TSpatialPosition must be the same as the geoid CalculationRF
	if (position.getRefFrame() != fCalcRFPtr)
	{
		position.transform(fCalcRFPtr);
	}



	LengthValue x, y;
	TReal dx, dy, xp, yp;
	TReal falseOriginX(2000), falseOriginY(LITERAL(2097.79265));
	TAngle fEtaValue;

	x = position.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue();
	y = position.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue();

	dx = (x-falseOriginX)*scaleFactor;
	dy = (y-falseOriginY)*scaleFactor;

	xp = dx*costhc-dy*sinthc;
	yp = dx*sinthc+dy*costhc;

// Calculate the vertical deflection in E-O direction 
	fEtaValue.setRadiansValue((-fA*cosazp*xp -fB*sinazp*yp)/100000 ); // * LITERAL(6.366);
	return fEtaValue;

}



TAngle	TCernParabolicGeoid::getDAlpha( const TSpatialPosition& sp ) const
{

	// deep copy of TSpatialPosition
	TSpatialPosition position(sp);

	// RF of TSpatialPosition must be the same as the geoid DefinitionRF
	if (position.getRefFrame() != fDefRFPtr)
	{
		position.transform(fDefRFPtr);
	}


	TAngle latitude;
	AngleValue phi;
	TAngle eta, fDAlphaValue;
	
	latitude = position.getCoordinates(TCoordSysFactory::kGeodetic).getPhiEllipsoid();
	phi = latitude.getRadiansValue();

	// transformation in the calculation RF 
	position.transform(fCalcRFPtr);

	eta = getEta(position);
	fDAlphaValue = eta*tanq(phi);
	return fDAlphaValue;
}

TAngle	TCernParabolicGeoid::getDAlpha( const TSpatialPosition& sp, const TAngle& latitude ) const
{
	AngleValue phi;
	TAngle eta, fDAlphaValue;
	
	phi = latitude.getRadiansValue();

	// deep copy of TSpatialPosition
	TSpatialPosition position(sp);

	// RF of TSpatialPosition must be the same as the geoid CalculationRF
	if (position.getRefFrame() != fCalcRFPtr)
	{
		position.transform(fCalcRFPtr);
	}


	eta = getEta(position);
	fDAlphaValue = eta*tanq(phi);
	return fDAlphaValue;
}



/////////////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////////////
