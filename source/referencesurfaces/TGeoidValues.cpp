// TGeoidValues.cpp
//
// Class for geoidal values as defined for survey purposes
//
// Copyright 1999,2000, Mark Jones, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////


//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"

// other forward declarations

#include	"TGeoidValues.h"
#include	"TAngle.h"
////////////////////////////////////////////////////////////////



//ClassImp(TGeoidValues)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


TGeoidValues::TGeoidValues()
{	// default constructor 
}


TGeoidValues::TGeoidValues(TReal fNValue, TReal fXiValue, TReal fEtaValue, TReal fDAlphaValue)
{
	setN(fNValue);
	setXi(fXiValue);
	setEta(fEtaValue);
	setDAlpha(fDAlphaValue);
}


TGeoidValues::TGeoidValues(const TGeoidValues& tgv)
{
	fNValue=tgv.fNValue;
	fXiValue=tgv.fXiValue;
	fEtaValue=tgv.fEtaValue;
	fDAlphaValue=tgv.fDAlphaValue;
}

TGeoidValues::~TGeoidValues()
{
}

//////////////////////////////////////////////////////////////////////
// Operator =
//////////////////////////////////////////////////////////////////////
TGeoidValues& TGeoidValues::operator=(const TGeoidValues& tgv)
{
		if (this != &tgv)
	{
		fNValue=tgv.fNValue;
		fXiValue=tgv.fXiValue;
		fEtaValue=tgv.fEtaValue;
		fDAlphaValue=tgv.fDAlphaValue;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TReal	TGeoidValues::getXiAzimuth(TAngle azimuth)
{//
	TReal  az = azimuth.getRadiansValue();
	TReal	XiAzimuthValue;
	XiAzimuthValue = fXiValue*cosq(az) + fEtaValue*sinq(az);
	return XiAzimuthValue;
}


TReal	TGeoidValues::getEtaAzimuth(TAngle azimuth)
{//
	TReal  az = azimuth.getRadiansValue();
	TReal	EtaAzimuthValue;
	EtaAzimuthValue = fEtaValue*cosq(az) - fXiValue*sinq(az);
	return EtaAzimuthValue;
}
