#include "TCGRF2LGTransformation.h"
#include<Eigen/Dense>
#include "GeodeticConstants.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCGRF2LGTransformation::TCGRF2LGTransformation(TPositionVector& ILGorigin, bool isSphere)
	: fILGOrigin(ILGorigin),fInitialised(true)
{	
	if (isSphere)
		initialiseSphere();
	else
		initialiseEllipsoid();
}

TCGRF2LGTransformation::TCGRF2LGTransformation()
	: fILGOrigin(TCoordSysFactory::k3DCartesian),fInitialised(false)
{	
}

TCGRF2LGTransformation::~TCGRF2LGTransformation()
{//destructor
}

void  TCGRF2LGTransformation::initialise(TPositionVector& ILGorigin, bool isEllipsoid){
	fILGOrigin = ILGorigin;
	if (!isEllipsoid)
		initialiseSphere();
	else
		initialiseEllipsoid();
}


//////////////////////////////////////////////////////////////////////
//TRANSFORMATIONS
//////////////////////////////////////////////////////////////////////

bool  TCGRF2LGTransformation::transform( TPositionVector& pv ) const
{// transform a position vector
	return fInitialised ? fTransform.transform(pv) : fInitialised;
}

bool  TCGRF2LGTransformation::transform( TFreeVector& fv ) const
{// transform a free vector
	return fInitialised ? fTransform.transform(fv) : fInitialised;
}
#if 0
bool  TCGRF2LGTransformation::transform( TRotationMatrix& rmx ) const
{// transform a rotation matrix
	return fInitialised ? fTransform.transform(rmx) : fInitialised;
}
#endif

bool  TCGRF2LGTransformation::transformInverse( TPositionVector& pv ) const
{// inverse transformation of a position vector
	return fInitialised ? fTransform.getInversedTransformation().transform(pv) : fInitialised;
}

bool  TCGRF2LGTransformation::transformInverse( TFreeVector& fv ) const
{// inverse transformation of a free vector
	return fInitialised ? fTransform.getInversedTransformation().transform(fv) : fInitialised;
}
#if 0
bool  TCGRF2LGTransformation::transformInverse( TRotationMatrix& rmx ) const
{// inverse transformation of a rotation matrix
	return fInitialised ? fTransform.getInversedTransformation().transform(rmx) : fInitialised;
}
#endif
//////////////////////////////////////////////////////////////////////
//PRIVATE METHODS
//////////////////////////////////////////////////////////////////////

void  TCGRF2LGTransformation::initialiseEllipsoid()
{
   TReal fOriginX = fILGOrigin.getX().getMetresValue();
   TReal fOriginY = fILGOrigin.getY().getMetresValue();
   TReal fOriginZ = fILGOrigin.getZ().getMetresValue();

	TReal squareSemiMajor = SemiMajorAxisGRS80*SemiMajorAxisGRS80;
	TReal squareSemiMinor = SemiMinorAxisGRS80*SemiMinorAxisGRS80;

	TReal XYdistance = sqrt(fOriginX*fOriginX+fOriginY*fOriginY);

	//Transform from geodetic cartesian coordinates to a geodetic ellipsoidal coordinates using non-iterative algorithm
	TReal upsilon = atan((fOriginZ*SemiMajorAxisGRS80)/(XYdistance*SemiMinorAxisGRS80));
	TReal lambdaLGO = atan(fOriginY/fOriginX);
	TReal eDashSquared = (squareSemiMajor-squareSemiMinor)/(squareSemiMinor);
	TReal phiLGO = atan((fOriginZ+eDashSquared*SemiMinorAxisGRS80*pow((sin(upsilon)),3))/(XYdistance - eccentrGRS80*SemiMajorAxisGRS80*pow((cos(upsilon)),3)));

	TReal sinLambdaL = sin(lambdaLGO);
	TReal cosLambdaL = cos(lambdaLGO);
	TReal sinPhiL = sin(phiLGO);
	TReal cosPhiL = cos(phiLGO);

	//Filling the transformation matrix
	fTransform.setMatrixIJPosition(0,0, -sinLambdaL);
	fTransform.setMatrixIJPosition(0,1,  cosLambdaL);
	fTransform.setMatrixIJPosition(0,2, 0.0);

	fTransform.setMatrixIJPosition(1,0, -cosLambdaL*sinPhiL);
	fTransform.setMatrixIJPosition(1,1,  - sinLambdaL*sinPhiL);
	fTransform.setMatrixIJPosition(1,2, cosPhiL);

	fTransform.setMatrixIJPosition(2,0,  cosLambdaL*cosPhiL);
	fTransform.setMatrixIJPosition(2,1,  cosPhiL*sinLambdaL);
	fTransform.setMatrixIJPosition(2,2, sinPhiL);

	TReal tA1 = -(fTransform.getMmatrixIJPosition(0,0)*(fOriginX) + fTransform.getMmatrixIJPosition(0,1)*(fOriginY)); 
	TReal tA2 = -(fTransform.getMmatrixIJPosition(1,0)*(fOriginX) + fTransform.getMmatrixIJPosition(1,1)*(fOriginY) + fTransform.getMmatrixIJPosition(1,2)*(fOriginZ));
	TReal tA3 = -(fTransform.getMmatrixIJPosition(2,0)*(fOriginX) + fTransform.getMmatrixIJPosition(2,1)*(fOriginY) + fTransform.getMmatrixIJPosition(2,2)*(fOriginZ));

	fTransform.setMatrixIJPosition(0,3,tA1);
	fTransform.setMatrixIJPosition(1,3,tA2);
	fTransform.setMatrixIJPosition(2,3,tA3);
}

void  TCGRF2LGTransformation::initialiseSphere()
{
   TReal fOriginX = fILGOrigin.getX().getMetresValue();
   TReal fOriginY = fILGOrigin.getY().getMetresValue();
   TReal fOriginZ = fILGOrigin.getZ().getMetresValue();

	TReal XYdistance = sqrt(fOriginX*fOriginX+fOriginY*fOriginY);

	//Transform from geodetic cartesian coordinates to a geodetic ellipsoidal coordinates using non-iterative algorithm
	TReal lambdaLGO = atan(fOriginY/fOriginX);
	TReal phiLGO = atan(fOriginZ/XYdistance);

	TReal sinLambdaL = sin(lambdaLGO);
	TReal cosLambdaL = cos(lambdaLGO);
	TReal sinPhiL = sin(phiLGO);
	TReal cosPhiL = cos(phiLGO);

	//Filling the transformation matrix
	fTransform.setMatrixIJPosition(0,0, -sinLambdaL);
	fTransform.setMatrixIJPosition(0,1,  cosLambdaL);
	fTransform.setMatrixIJPosition(0,2, 0.0);

	fTransform.setMatrixIJPosition(1,0, -cosLambdaL*sinPhiL);
	fTransform.setMatrixIJPosition(1,1,  - sinLambdaL*sinPhiL);
	fTransform.setMatrixIJPosition(1,2, cosPhiL);

	fTransform.setMatrixIJPosition(2,0,  cosLambdaL*cosPhiL);
	fTransform.setMatrixIJPosition(2,1,  cosPhiL*sinLambdaL);
	fTransform.setMatrixIJPosition(2,2, sinPhiL);

	TReal tA1 = -(fTransform.getMmatrixIJPosition(0,0)*(fOriginX) + fTransform.getMmatrixIJPosition(0,1)*(fOriginY)); 
	TReal tA2 = -(fTransform.getMmatrixIJPosition(1,0)*(fOriginX) + fTransform.getMmatrixIJPosition(1,1)*(fOriginY) + fTransform.getMmatrixIJPosition(1,2)*(fOriginZ));
	TReal tA3 = -(fTransform.getMmatrixIJPosition(2,0)*(fOriginX) + fTransform.getMmatrixIJPosition(2,1)*(fOriginY) + fTransform.getMmatrixIJPosition(2,2)*(fOriginZ));

	fTransform.setMatrixIJPosition(0,3,tA1);
	fTransform.setMatrixIJPosition(1,3,tA2);
	fTransform.setMatrixIJPosition(2,3,tA3);
}