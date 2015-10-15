#include  "TCCS2CGRFTransformation.h"
#include "Global.h"
#include "GeodeticConstants.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TCCS2CGRFTransformation::TCCS2CGRFTransformation(bool isEllipsoid): fIsEllipsoid(isEllipsoid)
{	
	initialise();
}

TCCS2CGRFTransformation::~TCCS2CGRFTransformation()
{//destructor
}

void TCCS2CGRFTransformation::reInitialize (bool isEllipsoid){
	fIsEllipsoid = isEllipsoid;
	initialise();
}

//////////////////////////////////////////////////////////////////////
//Transfotmations
//////////////////////////////////////////////////////////////////////
bool  TCCS2CGRFTransformation::transform( TPositionVector& pv ) const
{// transform a position vector
	return fTransform.transform(pv);
}


bool  TCCS2CGRFTransformation::transform( TFreeVector& fv ) const
{// transform a free vector
	return fTransform.transform(fv);
}


TPositionVector  TCCS2CGRFTransformation::getTransformedPt( const TPositionVector& pv ) const
{// transform a position vector
	TPositionVector result = pv;
	fTransform.transform(result);
	return result;
}


#if 0
bool  TCCS2CGRFTransformation::transform( TRotationMatrix& rmx ) const
{// transform a Rotation Matrix
	return fTransform.transform(rmx);
}
#endif

bool  TCCS2CGRFTransformation::transformIverse( TPositionVector& pv ) const
{// inverse transformation of a position vector, i.e. CGRF -> CCS transformation
	return fTransform.getInversedTransformation().transform(pv);
}

TPositionVector  TCCS2CGRFTransformation::getTransformedPtInverse( const TPositionVector& pv ) const
{// transform a position vector
	TPositionVector result = pv;
	fTransform.getInversedTransformation().transform(result);
	return result;
}


bool  TCCS2CGRFTransformation::transformInverse( TFreeVector& fv ) const
{// inverse transformation of a transform a free vector, i.e. CGRF -> CCS transformation
	return fTransform.getInversedTransformation().transform(fv);
}

#if 0
bool  TCCS2CGRFTransformation::transformInverse( TRotationMatrix& rmx ) const
{// inverse transformation of a transform a Rotation Matrix, i.e. CGRF -> CCS transformation
	return fTransform.getInversedTransformation().transform(rmx);
}
#endif

//////////////////////////////////////////////////////////////////////
//Private methods
//////////////////////////////////////////////////////////////////////
void  TCCS2CGRFTransformation::initialise()
{
	if(fIsEllipsoid){ //ELLIPSOID
		//GRAD -> RAD conversion of the geodetic ellipsoidal coordinates of the CCS origin (P0) 
		TReal LambdaP0RAD = LambdaP0 * M_PI/200;
		TReal PhiP0RAD = PHIP0 * M_PI/200;
		//Negative azimuthY in RAD
		TReal AzRADNeg = - AzimuthCCSYaxis * M_PI/200;

		TReal sinLambda = sin(LambdaP0RAD);
		TReal cosLambda = cos(LambdaP0RAD);
		TReal sinPhi = sin(PhiP0RAD);
		TReal cosPhi = cos(PhiP0RAD);
		TReal sinAzimuth = sin(AzRADNeg);
		TReal cosAzimuth = cos(AzRADNeg);

		//prime vertical radius of curvature at phi 
		TReal radOfCurvature = SemiMajorAxisGRS80 / (sqrt(1 - eccentrGRS80*sinPhi*sinPhi));

		//geodetic cartesian coordinates of the P0
		TReal GeodCartisianP0X = (radOfCurvature + HP0)*cosPhi*cosLambda;
		TReal GeodCartisianP0Y = (radOfCurvature + HP0)*cosPhi*sinLambda;
		TReal GeodCartisianP0Z = (radOfCurvature * (1-eccentrGRS80)+HP0)*sinPhi;

		//filling transformation matrix
		fTransform.setMatrixIJPosition(0,0, - cosAzimuth*sinLambda - sinAzimuth*cosLambda*sinPhi);
		fTransform.setMatrixIJPosition(0,1,  - cosLambda*sinPhi*cosAzimuth + sinLambda*sinAzimuth);
		fTransform.setMatrixIJPosition(0,2, cosLambda*cosPhi);

		fTransform.setMatrixIJPosition(1,0, cosAzimuth*cosLambda - sinAzimuth*sinLambda*sinPhi);
		fTransform.setMatrixIJPosition(1,1,  - sinLambda*sinPhi*cosAzimuth - cosLambda*sinAzimuth);
		fTransform.setMatrixIJPosition(1,2, sinLambda*cosPhi);

		fTransform.setMatrixIJPosition(2,0,  sinAzimuth*cosPhi);
		fTransform.setMatrixIJPosition(2,1,  cosAzimuth*cosPhi);
		fTransform.setMatrixIJPosition(2,2, sinPhi);

		TReal tA1 = fTransform.getMmatrixIJPosition(0,0)*(-XP0) + fTransform.getMmatrixIJPosition(0,1)*(-YP0) + fTransform.getMmatrixIJPosition(0,2)*(-ZP0);
		TReal translation1 = tA1 + GeodCartisianP0X;

		TReal tA2 = fTransform.getMmatrixIJPosition(1,0)*(-XP0) + fTransform.getMmatrixIJPosition(1,1)*(-YP0) + fTransform.getMmatrixIJPosition(1,2)*(-ZP0);
		TReal translation2 = tA2 + GeodCartisianP0Y;

		TReal tA3 = fTransform.getMmatrixIJPosition(2,0)*(-XP0) + fTransform.getMmatrixIJPosition(2,1)*(-YP0) + fTransform.getMmatrixIJPosition(2,2)*(-ZP0);
		TReal translation3 = tA3 + GeodCartisianP0Z;

		fTransform.setMatrixIJPosition(0,3,translation1);
		fTransform.setMatrixIJPosition(1,3,translation2);
		fTransform.setMatrixIJPosition(2,3,translation3);
	}
	else{//SPHERE
		/*Original
		rx.setGonsValue(-LITERAL(42.726243230216));
		ry.setGonsValue(-LITERAL(25.285434244947));
		rz.setGonsValue(-LITERAL(77.864346765085));
		*/
        //XYZ order
#if 0
		TReal rx = /*TAngle::gonsToRadsFactor() * LITERAL(6.40452443525314 )*/0.100602034577638;
		TReal ry = /*TAngle::gonsToRadsFactor() * LITERAL(-48.2921155054988 )*/ -0.758570776491924;
		TReal rz = /*TAngle::gonsToRadsFactor() * LITERAL( -71.4971554998866) */ -1.12307469235505;
#endif
 //ZYX order
		TReal rx = -TAngle::gonsToRadsFactor() *LITERAL(42.726243230216);
		TReal ry = -TAngle::gonsToRadsFactor() *LITERAL(25.285434244947);
		TReal rz = -TAngle::gonsToRadsFactor() *LITERAL(77.864346765085);



		fTransform.setTransformation(4381882.331989, 461505.530464, 4598944.364158, rx, ry, rz, 1.0);
	}
}