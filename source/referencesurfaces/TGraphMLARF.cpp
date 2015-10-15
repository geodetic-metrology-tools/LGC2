// TGraphMLARF.cpp
//
/**
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////




#include  "TGraphMLARF.h"


////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TGraphMLARF::TGraphMLARF( const string& name,TFreeVector falseOrigin,
						 TGraphLocalAstronomicalRF* RF, const TAngle omega,
						 const TAngle phi, const TAngle kappa) 
	: TAModifiedLocalAstronomicalRF( name ), fLocAstroSys(RF),
	fOrigin(RF->getGeodeticRF()), fOrientationMatrix(RF->getGeodeticRF())
{
	// origin
	fOrigin = RF->getLocalGeodeticRF()->getOrigin();
	
	// false origin
	setFalseOrigin( falseOrigin );

	// orientation matrix
	TReal o(omega.getRadiansValue()),p(phi.getRadiansValue()),k(kappa.getRadiansValue());
	TRotationMatrix matrix(TRotationMatrix::kRzyx,o,p,k);
	fOrientationMatrix.setElements(matrix);
//	TSpatialOrientation matrix(,fLocAstroSys);
//	setOrientation( matrix );

}



TGraphMLARF::~TGraphMLARF()
{
}

//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
// return the associated local astronomical ref frame
/////////////////////////////////////////////////////
TGraphLocalAstronomicalRF* TGraphMLARF::getLocalAstronomicalRF() const
{
	return fLocAstroSys;
}


////////////////////////////////
// return the geodetic ref frame
////////////////////////////////
TGeodeticRefFrame* TGraphMLARF::getGeodeticRF() const
{
	return fLocAstroSys->getGeodeticRF();
}

//////////////////////////////////////
// return the local geodetic ref frame
//////////////////////////////////////
TModifiedLocalGeodeticRF* TGraphMLARF::getLocalGeodeticRF() const
{
	return fLocAstroSys->getLocalGeodeticRF();
}



////////////////////
// get the eta value
////////////////////
TAngle TGraphMLARF::getEta() const
{	// default function : return a null value
	return fLocAstroSys->getEta();
}

////////////////////
// get the xsi value
////////////////////
TAngle TGraphMLARF::getXsi() const
{	// default function : return a null value
	return fLocAstroSys->getXsi();
}

///////////////////////
// get the dAlpha value
///////////////////////
TAngle TGraphMLARF::getDAlpha() const
{	// default function : return a null value
	return fLocAstroSys->getDAlpha();
}


////////////////////
// set the eta value
////////////////////
void TGraphMLARF::setEta(const TAngle eta)
{
	fLocAstroSys->setEta(eta);
}

////////////////////
// set the xsi value
////////////////////
void TGraphMLARF::setXsi(const TAngle xsi)
{
	fLocAstroSys->setXsi(xsi);
}

///////////////////////
// set the dAlpha value
///////////////////////
void TGraphMLARF::setDAlpha(const TAngle dAlpha)
{
	fLocAstroSys->setDAlpha(dAlpha);
}


