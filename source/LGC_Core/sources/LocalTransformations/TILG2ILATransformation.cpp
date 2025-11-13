// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TILG2ILATransformation.h"
#include <Eigen/Dense>
#include "GeodeticConstants.h"
#include "TRefSystemFactory.h"
#include "TCernGridGeoid.h"
#include "TCCS2CGRFTransformation.h"
#include "TCGRF2LGTransformation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TILG2ILATransformation::TILG2ILATransformation(TPositionVector& origin, TRefSystemFactory::EGeoid geoidModel )
	: fOrigin(origin), originInCGRF(TCoordSysFactory::k3DCartesian),fGeoidModel(geoidModel), fInitialised(false)
{	
	bool isSphere = (fGeoidModel == TRefSystemFactory::EGeoid::kCGSphere);

	//transform the origin of the LA in LG
	auto pCCS2CGRF = TCCS2CGRFTransformation(isSphere);
	auto pCGRF2LG = TCGRF2LGTransformation(origin, isSphere);
	pCCS2CGRF.transform(fOrigin);
	originInCGRF = fOrigin;
	pCGRF2LG.transform(fOrigin); //origin in LG is (0 0 0)

	initialise();
}

TILG2ILATransformation::TILG2ILATransformation()
	: fOrigin(TCoordSysFactory::k3DCartesian), originInCGRF(TCoordSysFactory::k3DCartesian), fGeoidModel(TRefSystemFactory::EGeoid::kNoGeoid), fInitialised(false)
{
}

TILG2ILATransformation::~TILG2ILATransformation()
{//destructor
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

bool  TILG2ILATransformation::transformInverse( TPositionVector& pv ) const
{// transform a position vector
	return fInitialised ? fTransform.getInversedTransformation().transform(pv) : fInitialised;
}

bool  TILG2ILATransformation::transformInverse( TFreeVector& fv ) const
{// inverse transformation of a free vector
	return fInitialised ? fTransform.getInversedTransformation().transform(fv) : fInitialised;
}

//////////////////////////////////////////////////////////////////////
//PRIVATE METHODS
//////////////////////////////////////////////////////////////////////
void TILG2ILATransformation::initialise()
{

	//Get appropriate geoid model and get Xi, Eta and DAlpha values
	TAGeoidModel* geoidModel = TRefSystemFactory::getRefSystemFactory()->getGeoid(fGeoidModel);
	TReal Xi = geoidModel->getXi(TSpatialPosition(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::ERefFrame::kCGRF), originInCGRF)).getRadiansValue();
	TReal Eta = geoidModel->getEta(TSpatialPosition(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::ERefFrame::kCGRF), originInCGRF)).getRadiansValue();
	TReal DAlpha = geoidModel->getDAlpha(TSpatialPosition(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::ERefFrame::kCGRF), originInCGRF)).getRadiansValue();
	
	TReal cosXi = cos(-Xi);
	TReal sinXi = sin(-Xi);
	TReal cosEta = cos(Eta);
	TReal sinEta = sin(Eta);
	TReal cosDA = cos(DAlpha);
	TReal sinDA = sin(DAlpha);

	//Filling the transformation matrix  inverse of Pxy * Rz(dA) * Ry(-Xi) *Rx(eta)*Pxy
	fTransform.setMatrixIJPosition(0,0, cosDA*cosEta - sinDA*sinEta*sinXi);
	fTransform.setMatrixIJPosition(0,1, sinDA*cosEta + cosDA*sinEta*sinXi);
	fTransform.setMatrixIJPosition(0,2, -cosXi*sinEta);

	fTransform.setMatrixIJPosition(1, 0, -sinDA*cosXi);
	fTransform.setMatrixIJPosition(1, 1, cosDA*cosXi);
	fTransform.setMatrixIJPosition(1, 2, sinXi);

	fTransform.setMatrixIJPosition(2, 0, cosDA*sinEta+sinDA*sinXi*cosEta);
	fTransform.setMatrixIJPosition(2, 1, sinDA*sinEta - cosDA*sinXi*cosEta);
	fTransform.setMatrixIJPosition(2, 2, cosXi*cosEta);

	fInitialised = true;
}
	