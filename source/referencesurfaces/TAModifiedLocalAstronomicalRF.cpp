// TAModifiedLocalAstronomicalRF.cpp
//
/**
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
#include  "TGraph.h"

#include  "TSpatialPosition.h"
#include  "TPositionStatus.h"
#include  "TSpatialPositionError.h"

#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TModifiedLocalGeodeticRF.h"
////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TAModifiedLocalAstronomicalRF::TAModifiedLocalAstronomicalRF( const string& name )
	: TA3DEuclideanRefFrame(name),
	fFalseOrigin(TCoordSysFactory::k3DCartesian)
{//constructor
}



TAModifiedLocalAstronomicalRF::~TAModifiedLocalAstronomicalRF()
{//destructor
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
// return the geodetic ref frame
TGeodeticRefFrame* TAModifiedLocalAstronomicalRF::getGeodeticRF() const
{
	return TGeodeticRefFrame::instance();
}

TSpatialOrientation TAModifiedLocalAstronomicalRF::getOrientation() const
{
	TAReferenceFrame* rf = new TAModifiedLocalAstronomicalRF("NULL");
	TSpatialOrientation orie(rf);
	delete rf;
	return orie;
}


////////////////////
// get the eta value
////////////////////
TAngle TAModifiedLocalAstronomicalRF::getEta() const
{	// default function : return a null value
	TAngle eta;
	return eta;
}

////////////////////
// get the xsi value
////////////////////
TAngle TAModifiedLocalAstronomicalRF::getXsi() const
{	// default function : return a null value
	TAngle xsi;
	return xsi;
}

///////////////////////
// get the dAlpha value
///////////////////////
TAngle TAModifiedLocalAstronomicalRF::getDAlpha() const
{	// default function : return a null value
	TAngle dAlpha;
	return dAlpha;
}


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////

