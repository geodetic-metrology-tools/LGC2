// TGraphLocalAstronomicalRF.cpp
//
/**
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////







#include  "TGraphLocalAstronomicalRF.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TGraphLocalAstronomicalRF::TGraphLocalAstronomicalRF( const string& name, 
													 const TAngle eta, const TAngle xsi,
													 const TAngle dAlpha,
													 TModifiedLocalGeodeticRF* locGeodSys)
    : TAModifiedLocalAstronomicalRF(name), fLocalGeodeticSys(locGeodSys),
	fOrigin(locGeodSys->getGeodeticRF()), fOrientationMatrix(locGeodSys)
{
	// origin
	fOrigin = locGeodSys->getOrigin();

	// false origin
	TFreeVector vector(0,0,0, TCoordSysFactory::k3DCartesian);
	setFalseOrigin( vector );

	// orientation matrix
	TRotationMatrix id;
	id.identity();
	TSpatialOrientation orientation(locGeodSys);
	orientation.setElements(id);
	fOrientationMatrix = orientation;

	// geodetic reference frame
	//fLocalGeodeticSys = locGeodSys;

	// geoid parameters
	setEta( eta );
	setXsi( xsi );
	setDAlpha( dAlpha );


}


TGraphLocalAstronomicalRF::~TGraphLocalAstronomicalRF()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// return the associated local geodetic reference frame
///////////////////////////////////////////////////////
TModifiedLocalGeodeticRF* TGraphLocalAstronomicalRF::getLocalGeodeticRF() const
{
	return fLocalGeodeticSys;
}

//////////////////////////////////////
// return the geodetic reference frame
//////////////////////////////////////
TGeodeticRefFrame* TGraphLocalAstronomicalRF::getGeodeticRF() const
{
	return fLocalGeodeticSys->getGeodeticRF();
}

