#include  "TCCS2CGRFTransformation.h"
#include <Global.h>
#include "GeodeticConstants.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TCCS2CGRFTransformation::TCCS2CGRFTransformation(bool isSphere) : fIsSphere(isSphere)
{	
	if (!fIsSphere)
		initialiseEllipsoid();
	else
		initialiseSphere();
}

TCCS2CGRFTransformation::~TCCS2CGRFTransformation()
{//destructor
}

void TCCS2CGRFTransformation::reInitialize (bool isSphere){
	fIsSphere = isSphere;
	if (!fIsSphere)
		initialiseEllipsoid();
	else
		initialiseSphere();
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


//////////////////////////////////////////////////////////////////////
//Private methods
//////////////////////////////////////////////////////////////////////

void  TCCS2CGRFTransformation::initialiseEllipsoid()
{
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

void  TCCS2CGRFTransformation::initialiseSphere()
{
	//translation pour situer le centre de la sphere
	TReal txs (LITERAL(4381882.331989)), tys (LITERAL(461505.530464)), tzs (LITERAL(4598944.364158));
	fTransform.setMatrixIJPosition(0,3,txs);
	fTransform.setMatrixIJPosition(1,3,tys);
	fTransform.setMatrixIJPosition(2,3,tzs);

	//rotation pour diriger les axes du CGRFs parallele a ceux du CGRF
	TReal rx, ry, rz;
	rx=-LITERAL(42.726243230216)* M_PI/200;
	ry=-LITERAL(25.285434244947)* M_PI/200;
	rz=-LITERAL(77.864346765085)* M_PI/200;
	TReal xsin =  sin(rx);
	TReal xcos =  cos(rx);
	TReal ysin =  sin(ry);
	TReal ycos =  cos(ry);
	TReal zsin =  sin(rz);
	TReal zcos =  cos(rz);

	//filling transformation matrix
	fTransform.setMatrixIJPosition(0,0, zcos*ycos);
	fTransform.setMatrixIJPosition(0,1, zsin*xcos+zcos*ysin*xsin);
	fTransform.setMatrixIJPosition(0,2, zsin*xsin-zcos*ysin*xcos);

	fTransform.setMatrixIJPosition(1,0, -zsin*ycos);
	fTransform.setMatrixIJPosition(1,1,  zcos*xcos-zsin*ysin*xsin);
	fTransform.setMatrixIJPosition(1,2, zcos*xsin+zsin*ysin*xcos);

	fTransform.setMatrixIJPosition(2,0,  ysin);
	fTransform.setMatrixIJPosition(2,1,  -ycos*xsin);
	fTransform.setMatrixIJPosition(2,2, ycos*xcos);

}