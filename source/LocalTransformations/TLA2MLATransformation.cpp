#include "TLA2MLATransformation.h"
#include <Eigen/Dense>
#include "GeodeticConstants.h"
#include "TRefSystemFactory.h"
#include "TCernGridGeoid.h"
#include "TCCS2CGRFTransformation.h"
#include "TCGRF2LGTransformation.h"
#include "TILG2ILATransformation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TLA2MLATransformation::TLA2MLATransformation(TPositionVector& origin, TRefSystemFactory::EGeoid geoidModel, TAngle gis, TAngle slope)
	: fOrigin(origin), fBearing(gis), fSlope(slope), fGeoidModel(geoidModel), fInitialised(true)
{
	initialise();
}

TLA2MLATransformation::TLA2MLATransformation()
	: fOrigin(TCoordSysFactory::k3DCartesian), fGeoidModel(TRefSystemFactory::EGeoid::kNoGeoid), fBearing(0.0), fSlope(0.0), fInitialised(false)
{
}

TLA2MLATransformation::~TLA2MLATransformation()
{//destructor
}


//////////////////////////////////////////////////////////////////////
//TRANSFORMATIONS
//////////////////////////////////////////////////////////////////////
bool  TLA2MLATransformation::transform(TPositionVector& pv) const
{// transform a position vector
	return fInitialised ? fTransform.transform(pv) : fInitialised;
}


bool  TLA2MLATransformation::transform(TFreeVector& fv) const
{// transform a free vector
	return fInitialised ? fTransform.transform(fv) : fInitialised;
}


bool  TLA2MLATransformation::transformInverse(TPositionVector& pv) const
{// transform a position vector
	return fInitialised ? fTransform.getInversedTransformation().transform(pv) : fInitialised;
}


bool  TLA2MLATransformation::transformInverse(TFreeVector& fv) const
{// inverse transformation of a free vector
	return fInitialised ? fTransform.getInversedTransformation().transform(fv) : fInitialised;
}


//////////////////////////////////////////////////////////////////////
//PRIVATE METHODS
//////////////////////////////////////////////////////////////////////
void TLA2MLATransformation::initialise()
{
	bool isSphere = (fGeoidModel == TRefSystemFactory::EGeoid::kCGSphere);

	/*Ask reference frame factory for a CCS2CGRF, CGRF2LG and LG2LA transformation.*/
	auto pCCS2CGRF = TCCS2CGRFTransformation(!isSphere);
	auto pCGRF2LG = TCGRF2LGTransformation(fOrigin, isSphere);
	auto pLG2LA = TILG2ILATransformation(fOrigin, fGeoidModel);


	//Transform a unit vector from CCS to LA
	TPositionVector unitY(fSlope.cosine()*fBearing.sine(),
		fSlope.cosine()*fBearing.cosine(),
		fSlope.sine(),
		TCoordSysFactory::k3DCartesian);
	pCCS2CGRF.transform(unitY);
	pCGRF2LG.transform(unitY);
	pLG2LA.transform(unitY);

	//
	TAngle Az(atan2(unitY.getY(), unitY.getX()));
	fTransform.setRotationTransformation(0.0, 0.0, -Az);

}
