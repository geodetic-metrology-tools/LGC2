#include "TILG2ILATransformation.h"
#include<Eigen/Dense>
#include "GeodeticConstants.h"
#include "TRefSystemFactory.h"
#include "TGraphLocalAstronomicalRF.h"
#include "TCernGridGeoid.h"
#include  "TCCS2CGRFTransformation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TILG2ILATransformation::TILG2ILATransformation(TPositionVector& origin, TRefSystemFactory::EGeoid geoidModel )
	: fOrigin(origin), fGeoidModel(geoidModel), fInitialised(true)
{	
	initialise();
}

TILG2ILATransformation::TILG2ILATransformation()
	: fOrigin(TCoordSysFactory::k3DCartesian),fGeoidModel(TRefSystemFactory::EGeoid::kNoGeoid), fInitialised(false)
{
}

TILG2ILATransformation::~TILG2ILATransformation()
{//destructor
}


void  TILG2ILATransformation::initialise(TPositionVector& origin, TRefSystemFactory::EGeoid geoidModel){
	fOrigin = origin;
	fGeoidModel = geoidModel;
	initialise();
}


//////////////////////////////////////////////////////////////////////
//TRANSFORMATIONS
//////////////////////////////////////////////////////////////////////
bool  TILG2ILATransformation::transform( TPositionVector& pv ) const
{// transform a position vector
	return fInitialised ? fTransform.transform(pv) : fInitialised;
}


bool  TILG2ILATransformation::transform( TFreeVector& fv ) const
{// transform a free vector
	return fInitialised ? fTransform.transform(fv) : fInitialised;
}

#if 0
bool  TILG2ILATransformation::transform( TRotationMatrix& rmx ) const
{// transform a rotation matrix
	return fInitialised ? fTransform.transform(rmx) : fInitialised;
}
#endif

bool  TILG2ILATransformation::transformInverse( TPositionVector& pv ) const
{// transform a position vector
	return fInitialised ? fTransform.getInversedTransformation().transform(pv) : fInitialised;
}


bool  TILG2ILATransformation::transformInverse( TFreeVector& fv ) const
{// inverse transformation of a free vector
	return fInitialised ? fTransform.getInversedTransformation().transform(fv) : fInitialised;
}

#if 0
bool  TILG2ILATransformation::transformInverse( TRotationMatrix& rmx ) const
{// inverse transformation of a rotation matrix
	return fInitialised ? fTransform.getInversedTransformation().transform(rmx) : fInitialised;
}
#endif

//////////////////////////////////////////////////////////////////////
//PRIVATE METHODS
//////////////////////////////////////////////////////////////////////
void TILG2ILATransformation::initialise()
{
	/*Ask reference frame factory for a CCS2CGRF transformation.*/
	auto pCCS2CGRF (TRefSystemFactory::getRefSystemFactory()->getCCS2CGRFTransformation());
	//Transform copy of origin given in CCS into CGRF (geodetic cartisian)
	TPositionVector fOrigin2 = fOrigin;
	pCCS2CGRF->transform(fOrigin2);

	TReal fOriginX = fOrigin2.getX().getValue();
	TReal fOriginY = fOrigin2.getY().getValue();
	TReal fOriginZ = fOrigin2.getZ().getValue();
	TReal squareSemiMajor = SemiMajorAxisGRS80*SemiMajorAxisGRS80;
	TReal squareSemiMinor = SemiMinorAxisGRS80*SemiMinorAxisGRS80;

	TReal XYdistance = sqrt(fOriginX*fOriginX+fOriginY*fOriginY);
	//Transform from geodetic cartesian coordinates to a geodetic ellipsoidal coordinates using non-iterative algorithm
	TReal upsilon = atan((fOriginZ*SemiMajorAxisGRS80)/(XYdistance*SemiMinorAxisGRS80));
	TReal eDashSquared = (squareSemiMajor-squareSemiMinor)/(squareSemiMinor);
	TReal phiLGO = atan((fOriginZ+eDashSquared*SemiMinorAxisGRS80*pow((sin(upsilon)),3))/(XYdistance - eccentricity*SemiMajorAxisGRS80*pow((cos(upsilon)),3)));

	//Get appropriate geoid model and get Xi, Eta and DAlpha values
	TAGeoidModel* geoidModel = TRefSystemFactory::getRefSystemFactory()->getGeoid(fGeoidModel);
	TReal Xi = geoidModel->getXi(TSpatialPosition(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::ERefFrame::kCCS),fOrigin)).getRadiansValue();
	TReal Eta = geoidModel->getEta(TSpatialPosition(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::ERefFrame::kCCS),fOrigin)).getRadiansValue();
	TReal DAlpha = Eta * tanq(phiLGO);
	
	//Filling the transformation matrix
	fTransform.setMatrixIJPosition(0,1, DAlpha);
	fTransform.setMatrixIJPosition(0,2, -Eta);

	fTransform.setMatrixIJPosition(1,0, -DAlpha);
	fTransform.setMatrixIJPosition(1,2, -Xi);

	fTransform.setMatrixIJPosition(2,0, Eta);
	fTransform.setMatrixIJPosition(2,1, Xi);
}
	