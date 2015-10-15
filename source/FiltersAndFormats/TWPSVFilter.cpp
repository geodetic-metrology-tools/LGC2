// TWPSVFilter.h
//
// Concrete Class of the TWPSFilter PABC.
// Handles the IO of an TWPSMeasurement object for the case
// where there is only a transverse offset
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

#include "TWPSVFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TWPSVFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TWPSVFilter *TWPSVFilter::fFilter = 0;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TWPSVFilter::TWPSVFilter()
{}

TWPSVFilter::~TWPSVFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////
// returns the single instance of this class
////////////////////////////////////////////
TWPSVFilter *TWPSVFilter::instance()
{
	if( fFilter == 0 )
	{
		fFilter = new TWPSVFilter();
	}

	return fFilter;
}


////////////////////////////////////////////////
// inputs a TWPSMeasurement from the QTextStream
////////////////////////////////////////////////
void TWPSVFilter::input(TLGCTStream &iStream, TWPSMeasurement &wps) const{
	

	TSpatialPointName end1Name, end2Name, setupName;
	TLength obsTOffset(LITERAL(0.0)), tSigma(LITERAL(0.0)), obsVOffset(LITERAL(0.0)), vSigma(LITERAL(0.0));
	
	//get the wire's first end point's name
	iStream >> end1Name;

	//get the wps's setup point's name
	iStream >> setupName;

	//get the wire's second end point's name
	iStream >> end2Name;

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
	newWPS->disactivateTOffset();
	newWPS->activateVOffset();

	wps = *newWPS;

	delete newWPS;

	return;
}
