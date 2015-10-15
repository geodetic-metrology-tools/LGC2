// TWPS2DFilter.h
//
// Concrete Class of the TWPSFilter PABC.
// Handles the IO of an TWPSMeasurement object for the case
// where there are both transverse and vertical offsets
//
// Patterns:
// This class is a Singleton.
// Works as a Strategy of a TLGCTStream object. 
//
// Copyright 2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations
#include "TLGCTStream.h"
#include "TWPSMeasurement.h"

#include "TWPS2DFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TWPS2DFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TWPS2DFilter *TWPS2DFilter::fFilter = 0;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TWPS2DFilter::TWPS2DFilter()
{}

TWPS2DFilter::~TWPS2DFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////
// returns the single instance of this class
////////////////////////////////////////////
TWPS2DFilter *TWPS2DFilter::instance()
{
	if( fFilter == 0 )
	{
		fFilter = new TWPS2DFilter();
	}

	return fFilter;
}


////////////////////////////////////////////////
// inputs a TWPSMeasurement from the QTextStream
////////////////////////////////////////////////
void TWPS2DFilter::input(TLGCTStream &iStream, TWPSMeasurement &wps) const{
	

	TSpatialPointName end1Name, end2Name, setupName;
	TLength obsTOffset(LITERAL(0.0)), obsVOffset(LITERAL(0.0)), tSigma, vSigma(LITERAL(0.0));
	
	//get the wire's first end point's name
	iStream >> end1Name;

	//get the wps's setup point's name
	iStream >> setupName;

	//get the wire's second end point's name
	iStream >> end2Name;

	//get the transverse offset
	iStream >> obsTOffset;

	//get the transverse offset's precision
	iStream >> tSigma;
	tSigma *= LITERAL(0.001);

	//possibly read the vertical offset
	if (!iStream.atEnd()){
		iStream >> obsVOffset;
	}

	//possibly read the vertical offset's precision
	if (!iStream.atEnd()){
		iStream >> vSigma;
		vSigma *= LITERAL(0.001);
	}

	TWPSMeasurement* newWPS = new TWPSMeasurement(end1Name, setupName, end2Name, obsTOffset, tSigma, obsVOffset, vSigma);
	newWPS->activateTOffset();
	newWPS->activateVOffset();

	wps = *newWPS;

	delete newWPS;

	return;
}




